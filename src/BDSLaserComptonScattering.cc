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
#include "BDSComptonEngine.hh"
#include "BDSComptonScatteringEngine.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLaserComptonScattering.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSLaser.hh"
#include "BDSStep.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Gamma.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <limits>

BDSLaserComptonScattering::BDSLaserComptonScattering(const G4String& processName):
  G4VDiscreteProcess(processName, G4ProcessType::fElectromagnetic),
  auxNavigator(new BDSAuxiliaryNavigator()),
  comptonEngine(new BDSComptonScatteringEngine())
{;}

BDSLaserComptonScattering::~BDSLaserComptonScattering()
{
  delete auxNavigator;
}

G4double BDSLaserComptonScattering::GetMeanFreePath(const G4Track& track,
						    G4double /*previousStepSize*/,
						    G4ForceCondition* forceCondition)
{

  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended()) // not extended so can't be a laser logical volume
    {return std::numeric_limits<double>::max();}
  
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv) // it's an extended volume but not ours (could be a crystal)
    {return std::numeric_limits<double>::max();}

  // else proceed
  const BDSLaser* laser = lvv->Laser();

  *forceCondition = Forced;
  return laser->Sigma0()/10;
}

G4VParticleChange* BDSLaserComptonScattering::PostStepDoIt(const G4Track& track,
							   const G4Step& step)
{
  //G4RandomDirection.hh
  // get coordinates for photon desity calculations
  aParticleChange.Initialize(track);

  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended()) // not extended so can't be a laser logical volume
    {return pParticleChange;}
  
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv)// it's an extended volume but not ours (could be a crystal)
    {return pParticleChange;}
  // else proceed
  
  const BDSLaser* laser = lvv->Laser();
  
  G4double stepLength = step.GetStepLength();

  const G4DynamicParticle* electron = track.GetDynamicParticle();

  G4ThreeVector particlePositionGlobal = track.GetPosition();
  G4ThreeVector particleDirectionMomentumGlobal = track.GetMomentumDirection();
  const G4RotationMatrix* rot = track.GetTouchable()->GetRotation();
  const G4AffineTransform transform = track.GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector particlePositionLocal = transform.TransformPoint(particlePositionGlobal);
  G4ThreeVector particleDirectionMomentumLocal = transform.TransformPoint(particleDirectionMomentumGlobal).unit();

  G4int partID = electron->GetParticleDefinition()->GetPDGEncoding();
  // create photon
  G4ThreeVector photonUnit(0,0,1);
  photonUnit.transform(*rot);
  G4double photonE = (CLHEP::h_Planck*CLHEP::c_light)/laser->Wavelength();
  G4ThreeVector photonVector = photonUnit*photonE;
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector,photonE);

  G4double electronEnergy = electron->GetTotalEnergy();
  G4ThreeVector electronMomentum = electron->GetMomentum();
  G4ThreeVector electronBeta = electronMomentum/electronEnergy;
  G4double electronGamma = electronEnergy/CLHEP::electron_mass_c2;
  G4double electronVelocity = electronBeta.mag()*CLHEP::c_light;
  photonLorentz.boost(electronBeta);
  G4double photonEnergy = photonLorentz.e();
  G4LorentzVector electron4Vector = electron->Get4Momentum();
  electron4Vector.boost(-electronBeta);
  G4double crossSection = comptonEngine->CrossSection(photonEnergy,partID);

  G4double particleGlobalTime = track.GetGlobalTime();
  G4double photonFlux = ((laser->Intensity(particlePositionLocal,0)/photonEnergy) //photon density
                         * laser->TemporalProfileGaussian(particleGlobalTime,particlePositionLocal.z())); // temporal intensity


  G4double ionTime = (stepLength/electronVelocity)*electronGamma;
  G4double scatteringProb = 1.0-std::exp(-crossSection*photonFlux*ionTime);
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  G4double scaleFactor = g->ScaleFactorLaser();
  G4double randomNumber = G4UniformRand();


  if((scaleFactor*scatteringProb)>randomNumber)
    {
      G4double initialWeight=aParticleChange.GetParentWeight();
      aParticleChange.ProposeParentWeight(initialWeight*1.0/scaleFactor);
      aParticleChange.SetNumberOfSecondaries(1);
      comptonEngine->setIncomingElectron(electron4Vector);
      comptonEngine->setIncomingGamma(photonLorentz);
      comptonEngine->PerformCompton(electronBeta,partID);
      G4LorentzVector scatteredGamma = comptonEngine->GetScatteredGamma();
      G4DynamicParticle* gamma = new G4DynamicParticle(G4Gamma::Gamma(),
						       scatteredGamma.vect().unit(),// direction
						       scatteredGamma.e());
      G4LorentzVector scatteredElectron = comptonEngine->GetScatteredElectron();
      G4LorentzVector electronLorentz = G4LorentzVector(scatteredElectron.vect().unit(),scatteredElectron.e());
      aParticleChange.AddSecondary(gamma);
      aParticleChange.ProposeEnergy(electronLorentz.e());
      aParticleChange.ProposeMomentumDirection(electronLorentz.getX(),electronLorentz.getY(),electronLorentz.getZ());
      aParticleChange.ProposeParentWeight(initialWeight);
      return G4VDiscreteProcess::PostStepDoIt(track, step);
    }
  else
    {return G4VDiscreteProcess::PostStepDoIt(track, step);}
}

