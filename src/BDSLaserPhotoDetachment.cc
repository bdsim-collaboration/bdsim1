/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSAuxiliaryNavigator.hh"
#include "BDSLaserPhotoDetachment.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSLaser.hh"
#include "BDSPhotoDetachmentEngine.hh"
#include "BDSStep.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Electron.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessType.hh"
#include "G4Proton.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RandomDirection.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserPhotoDetachment::BDSLaserPhotoDetachment(const G4String& processName):
  G4VDiscreteProcess(processName, G4ProcessType::fElectromagnetic),
  auxNavigator(new BDSAuxiliaryNavigator()),
  photoDetachmentEngine(new BDSPhotoDetachmentEngine())
{;}

BDSLaserPhotoDetachment::~BDSLaserPhotoDetachment()
{
  delete auxNavigator;
  delete photoDetachmentEngine;
}

G4double BDSLaserPhotoDetachment::GetMeanFreePath(const G4Track& track,
                                                  G4double /*previousStepSize*/,
						  G4ForceCondition* forceCondition)
{
  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended())
    {// not extended so can't be a laser logical volume
      return DBL_MAX;
    }
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv)
    {// it's an extended volume but not ours (could be a crystal)
      return DBL_MAX;
    }

  // else proceed
  const BDSLaser* laser = lvv->Laser();

  aParticleChange.Initialize(track);
  const G4DynamicParticle* ion = track.GetDynamicParticle();

  if (ion->GetCharge()==-1)
    {
      *forceCondition = Forced;
      return laser->Sigma0()/10;
    }
  else
    {return DBL_MAX;}
}

G4VParticleChange* BDSLaserPhotoDetachment::PostStepDoIt(const G4Track& track,
							 const G4Step& step)
{
  // get coordinates for photon desity calculations
  aParticleChange.Initialize(track);

  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended()) // not extended so can't be a laser logical volume
    {return pParticleChange;}
  
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser *>(lv);
  if (!lvv) // it's an extended volume but not ours (could be a crystal)
    {return pParticleChange;}
  
  // else proceed
  const BDSLaser* laser = lvv->Laser();
  
  G4double stepLength = step.GetStepLength();

  G4DynamicParticle* ion = const_cast<G4DynamicParticle*>(track.GetDynamicParticle());

  G4ThreeVector particlePositionGlobal = track.GetPosition();
  G4ThreeVector particleDirectionMomentumGlobal = track.GetMomentumDirection();
  const G4RotationMatrix* rot = track.GetTouchable()->GetRotation();
  const G4AffineTransform transform = track.GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector particlePositionLocal = transform.TransformPoint(particlePositionGlobal);
  G4ThreeVector particleDirectionMomentumLocal = transform.TransformPoint(particleDirectionMomentumGlobal).unit();

  // create photon
  G4ThreeVector photonUnit(0,0,1);
  photonUnit.transform(*rot);
  G4double photonE = (CLHEP::h_Planck*CLHEP::c_light)/laser->Wavelength();
  G4ThreeVector photonVector = photonUnit*photonE;
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector,photonE);

  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  G4ThreeVector ionBeta = ionMomentum/ionEnergy;
  G4double ionGamma = ionEnergy/ionMass;
  G4double ionVelocity = ionBeta.mag()*CLHEP::c_light;
  photonLorentz.boost(-ionBeta);
  G4double photonEnergy = photonLorentz.e();
  G4double crossSection = photoDetachmentEngine->CrossSection(photonEnergy);

  G4double particleGlobalTime = track.GetGlobalTime();
  G4double intensity =laser->Intensity(particlePositionLocal,0);
  G4double timeProfile=laser->TemporalProfileGaussian(particleGlobalTime,particlePositionLocal.z());
  G4double photonFlux = (intensity/photonEnergy)*timeProfile;

  G4double ionTime = (stepLength/ionVelocity)*ionGamma;
  G4double NeutralisationProbability = 1.0-std::exp(-crossSection*photonFlux*ionTime);
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  G4double scaleFactor = g->ScaleFactorLaser();
  G4double randomNumber = G4UniformRand();

      if((NeutralisationProbability*scaleFactor)>randomNumber)
    {
    // electron kinematics
      G4ThreeVector randomDirection = G4RandomDirection();
      G4double hydrogenMass = (CLHEP::electron_mass_c2 + CLHEP::proton_mass_c2);
      G4double outgoingElectronEnergy = CLHEP::electron_mass_c2;

      G4LorentzVector outgoingElectron;
      outgoingElectron.setE(outgoingElectronEnergy);

      // hydrogen kinematics

      G4double outgoingH0Energy = ionMass+photonEnergy-outgoingElectronEnergy;
      G4double outgoingH0Momentum = std::sqrt(outgoingH0Energy*outgoingH0Energy-hydrogenMass*hydrogenMass);
      G4LorentzVector outgoingH0;
      outgoingH0.setPx(-1.0*randomDirection.x()*outgoingH0Momentum);
      outgoingH0.setPy(-1.0*randomDirection.y()*outgoingH0Momentum);
      outgoingH0.setPz(-1.0*randomDirection.z()*outgoingH0Momentum);
      outgoingH0.setE(outgoingH0Energy);

      outgoingElectron.boost(ionBeta);
      outgoingH0.boost(ionBeta);


      // changes to ion to make H0
      aParticleChange.SetNumberOfSecondaries(1);
      aParticleChange.ProposeMass(hydrogenMass);
      G4ThreeVector H0Momentum;
      H0Momentum.set(outgoingH0.px(),outgoingH0.py(),outgoingH0.pz());
      aParticleChange.ProposeMomentumDirection(H0Momentum.unit());
      aParticleChange.ProposeCharge(0);
      ion->RemoveElectron(1);
      ion->SetCharge(0);

      // create outgoing electron
      G4DynamicParticle* electron = new G4DynamicParticle(G4Electron::ElectronDefinition(),
                                                          outgoingElectron.vect().unit(),
                                                          outgoingElectron.e());
      aParticleChange.AddSecondary(electron);

      aParticleChange.ProposeWeight(1.0/scaleFactor);
      
      return G4VDiscreteProcess::PostStepDoIt(track, step);
    }
  else
    {return G4VDiscreteProcess::PostStepDoIt(track, step);}
}

