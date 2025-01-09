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
#include "BDSLaserCumulativePhotodetachment.hh"
#include "BDSLogicalVolumeLaser.hh"
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

BDSLaserCumulativePhotodetachment::BDSLaserCumulativePhotodetachment(const G4String& processName):
        G4VDiscreteProcess(processName, G4ProcessType::fElectromagnetic),
        auxNavigator(new BDSAuxiliaryNavigator()),
        photoDetachmentEngine(new BDSPhotoDetachmentEngine())
{;}

BDSLaserCumulativePhotodetachment::~BDSLaserCumulativePhotodetachment()
{
  delete auxNavigator;
  delete photoDetachmentEngine;
}

G4double BDSLaserCumulativePhotodetachment::GetMeanFreePath(const G4Track& track,
                                                  G4double /*previousStepSize*/,
                                                  G4ForceCondition* forceCondition)
{  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //########### Typical volume check here for process active or not ########################

  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended()) // not extended so can't be a laser logical volume
  {return std::numeric_limits<double>::max();}

  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv) // it's an extended volume but not ours (could be a crystal)
  {return std::numeric_limits<double>::max();}

  *forceCondition = Forced;

  return std::numeric_limits<double>::max();
}

G4VParticleChange* BDSLaserCumulativePhotodetachment::PostStepDoIt(const G4Track& track,
                                                         const G4Step& step)
{
  // get coordinates for photon density calculations
  aParticleChange.Initialize(track);

  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended()) // not extended so can't be a laser logical volume
  {return pParticleChange;}

  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser *>(lv);
  if (!lvv) // it's an extended volume but not ours (could be a crystal)
  {return pParticleChange;}

  // else proceed
  const BDSLaser* laser = lvv->Laser();

  G4DynamicParticle* ion = const_cast<G4DynamicParticle*>(track.GetDynamicParticle());
  G4int ionCharge=ion->GetCharge();
  if(ionCharge==0){  return G4VDiscreteProcess::PostStepDoIt(track, step); }
  G4double particleEnergy = ion->GetTotalEnergy();
  G4ThreeVector particleMomentum = ion->GetMomentum();
  G4ThreeVector particleBeta = particleMomentum/particleEnergy;
  G4double particleGamma = particleEnergy/ion->GetMass();
  G4double particleVelocity = particleBeta.mag()*CLHEP::c_light;
  G4LorentzVector particle4Vector = ion->Get4Momentum();
  particle4Vector.boost(-particleBeta);
  G4double particleGlobalTimePostStep = track.GetGlobalTime();

  //######### Get particle position and momentum direction ###############################
  G4ThreeVector particlePositionGlobalPostStep = track.GetPosition();
  G4ThreeVector particlePositionGlobal = track.GetStep()->GetPreStepPoint()->GetPosition();
  G4ThreeVector particleDirectionMomentumGlobal = track.GetStep()->GetPreStepPoint()->GetMomentumDirection();
  G4ThreeVector currentParticlePositionGlobal = track.GetPosition();

  G4ThreeVector stepVector = currentParticlePositionGlobal-particlePositionGlobal;
  G4double  stepMagnitude = stepVector.mag();

  //#################### Get rotation and translation matrices for global to laser coordinates ############################

  const G4RotationMatrix* rot = track.GetTouchable()->GetRotation();
  const G4AffineTransform transform = track.GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector particlePositionLocal = transform.TransformPoint(particlePositionGlobal);
  G4ThreeVector particleDirectionMomentumLocal = transform.TransformPoint(particleDirectionMomentumGlobal).unit();
  //########################## Get Laser #####################################

  //######################## Get/Create photon information ##############################

  G4ThreeVector photonUnit(0,0,1);
  photonUnit.transform(*rot);
  G4double photonE = (CLHEP::h_Planck*CLHEP::c_light)/laser->Wavelength();
  G4ThreeVector photonVector = photonUnit*photonE;
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector,photonE);

  photonLorentz.boost(particleBeta);
  G4double photonEnergy = photonLorentz.e();

  G4double photonFluxSum = 0;

  G4double particleGlobalTimePreStep = particleGlobalTimePostStep-(stepVector.mag()/particleVelocity);

  std::vector<G4double> fluxArray;
  std::vector<G4LorentzVector> trajectoryPositions;

  G4int nSteps = 100;
  G4double nStepsD = (G4double)nSteps;

  for (G4int i = 0; i < nSteps; i++)
  {
        G4ThreeVector stepPositionGlobal = particlePositionGlobal+float(i)*(stepMagnitude/100.)*particleDirectionMomentumGlobal;
        G4ThreeVector stepPositionLocal = transform.TransformPoint(stepPositionGlobal);
        G4double particleStepGlobalTime = particleGlobalTimePreStep+((float(i)*(stepMagnitude/100.))/particleVelocity);
        G4double stepIntensity  = ((laser->Intensity(stepPositionLocal,0)/photonEnergy)
                                   * laser->TemporalProfileGaussian(particleStepGlobalTime,stepPositionLocal.z()));
        photonFluxSum = photonFluxSum + stepIntensity;
        fluxArray.push_back(stepIntensity);
  }

  G4double crossSection = photoDetachmentEngine->CrossSection(photonEnergy);

  G4double stepTime = stepMagnitude/particleVelocity;
  G4double cumulativeProbability = 1.0 - std::exp(-1.0*crossSection*photonFluxSum*(stepTime/100.)*particleGamma);

  G4double secondaryStepPosition;
  
  if (photonFluxSum == 0)
    {secondaryStepPosition = G4UniformRand();}
  else
  {
    G4RandGeneral trajectoryPDFRandom = CLHEP::RandGeneral(*CLHEP::HepRandom::getTheEngine(),
                                                             fluxArray.data(),100,0);
    secondaryStepPosition = trajectoryPDFRandom.shoot();
  }

  G4ThreeVector proposedPositionGlobal = particlePositionGlobal + (stepMagnitude*secondaryStepPosition*particleDirectionMomentumGlobal);
  G4double proposedTime = particleGlobalTimePreStep + (stepMagnitude*secondaryStepPosition/particleVelocity);

  aParticleChange.ProposePosition(proposedPositionGlobal);
  G4double initialWeight=aParticleChange.GetParentWeight();
  aParticleChange.ProposeParentWeight(initialWeight*cumulativeProbability);
  aParticleChange.SetNumberOfSecondaries(1);

  // electron kinematics
  G4ThreeVector randomDirection = G4RandomDirection();
  G4double hydrogenMass = (CLHEP::electron_mass_c2 + CLHEP::proton_mass_c2);
  G4double outgoingElectronEnergy = CLHEP::electron_mass_c2;
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4ThreeVector ionBeta = ionMomentum/ionEnergy;

  G4LorentzVector outgoingElectron;
  outgoingElectron.setE(outgoingElectronEnergy);

  G4double ionMass = ion->GetMass();
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
  aParticleChange.AddSecondary(electron,proposedTime);
  aParticleChange.ProposePosition(particlePositionGlobalPostStep);

  aParticleChange.ProposeParentWeight(initialWeight*cumulativeProbability);
  return G4VDiscreteProcess::PostStepDoIt(track, step);
}

