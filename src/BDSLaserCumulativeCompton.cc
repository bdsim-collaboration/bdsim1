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
#include "BDSLaserCumulativeCompton.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSStep.hh"
#include "BDSParticleDefinition.hh"
#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessType.hh"
#include "G4Proton.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"
#include "G4RandomTools.hh"
#include "BDSUserTrackInformation.hh"
#include "BDSPolarizationState.hh"


#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <limits>

BDSLaserCumulativeCompton::BDSLaserCumulativeCompton(const G4String& processName):
        G4VDiscreteProcess(processName, G4ProcessType::fElectromagnetic),
        auxNavigator(new BDSAuxiliaryNavigator()),
        comptonEngine(new BDSComptonScatteringEngine())
{;}

BDSLaserCumulativeCompton::~BDSLaserCumulativeCompton()
{
  delete auxNavigator;
}

G4double BDSLaserCumulativeCompton::GetMeanFreePath(const G4Track& track,
                                                    G4double /*previousStepSize*/,
                                                    G4ForceCondition* forceCondition)
{


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

G4VParticleChange* BDSLaserCumulativeCompton::PostStepDoIt(const G4Track& track,
                                                           const G4Step& step)
{
  aParticleChange.Initialize(track);
  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended()) // not extended so can't be a laser logical volume
  {return pParticleChange;}
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv)// it's an extended volume but not ours (could be a crystal)
  {return pParticleChange;}
  // else proceed

  /////////////////////////////// Get Particle Info //////////////////////////
  BDSUserTrackInformation* trackInfo = dynamic_cast<BDSUserTrackInformation*>(track.GetUserInformation());
  G4ThreeVector particlePolarization = trackInfo->GetPolarizationState()->GetPolarization();
  const G4DynamicParticle* particle = track.GetDynamicParticle();
  G4double particleEnergy = particle->GetTotalEnergy();
  G4ThreeVector particleMomentum = particle->GetMomentum();
  G4ThreeVector particleBeta = particleMomentum/particleEnergy;
  G4double particleGamma = particleEnergy/particle->GetMass();
  G4double particleVelocity = particleBeta.mag()*CLHEP::c_light;
  G4LorentzVector particle4Vector = particle->Get4Momentum();
  particle4Vector.boost(-particleBeta);

  G4double particleGlobalTimePostStep = track.GetGlobalTime();
  G4int partID = particle->GetParticleDefinition()->GetPDGEncoding();

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
  const BDSLaser* laser = lvv->Laser();
  //######################## Get/Create photon information ##############################
  G4ThreeVector photonPolarization = laser->Polarization();
  G4ThreeVector photonUnit(0,0,1);
  photonPolarization.transform(*rot);
  particlePolarization.transform(*rot);
  photonUnit.transform(*rot);
  G4double photonE = (CLHEP::h_Planck*CLHEP::c_light)/laser->Wavelength();
  G4ThreeVector photonVector = photonUnit*photonE;
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector,photonE);
  photonLorentz.boost(-1.0*particleBeta);
  G4double photonEnergy = photonLorentz.e();
  G4double photonFluxSum = 0;
  G4double particleGlobalTimePreStep = particleGlobalTimePostStep-(stepVector.mag()/particleVelocity);
  std::vector<G4double> fluxArray;
  std::vector<G4LorentzVector> trajectoryPositions;
  for(G4int i = 0;i<=99;i++)
    {
    G4ThreeVector stepPositionGlobal = particlePositionGlobal+float(i)*(stepMagnitude/100.)*particleDirectionMomentumGlobal;
    G4ThreeVector stepPositionLocal = transform.TransformPoint(stepPositionGlobal);
    G4double particleStepGlobalTime = particleGlobalTimePreStep+((float(i)*(stepMagnitude/100.))/particleVelocity);
    G4double stepIntensity  = ((laser->Intensity(stepPositionLocal,0)/photonEnergy)
                             * laser->TemporalProfileGaussian(particleStepGlobalTime,stepPositionLocal.z()));
    photonFluxSum = photonFluxSum + stepIntensity;
    fluxArray.push_back(stepIntensity);
    }

  G4double crossSection = comptonEngine->CrossSection(photonEnergy,partID);

  G4double stepTime = stepMagnitude/particleVelocity;
  G4double cumulativeProbability = 1.0 - std::exp(-1.0*crossSection*photonFluxSum*(stepTime/100.)*particleGamma);
  G4double secondaryStepPosition;

  if(photonFluxSum == 0)
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
  comptonEngine->setIncomingElectron(particle4Vector);
  comptonEngine->setIncomingGamma(photonLorentz);
  comptonEngine->SetIncomingGammaPolarization(G4StokesVector(photonPolarization));
  comptonEngine->SetIncomingElectronPolarization(G4StokesVector(particlePolarization));
  comptonEngine->PerformCompton(particleBeta,partID);
  G4LorentzVector scatteredGamma = comptonEngine->GetScatteredGamma();
  G4DynamicParticle* gamma = new G4DynamicParticle(G4Gamma::Gamma(),
                                                   scatteredGamma.vect().unit(),// direction
                                                   scatteredGamma.e());

  G4LorentzVector scatteredParticle = comptonEngine->GetScatteredElectron();
  G4LorentzVector particleLorentz = G4LorentzVector(scatteredParticle.vect().unit(),scatteredParticle.e());
  aParticleChange.AddSecondary(gamma,proposedTime);
  aParticleChange.ProposeEnergy(particleLorentz.e());
  aParticleChange.ProposeMomentumDirection(particleLorentz.getX(),particleLorentz.getY(),particleLorentz.getZ());
  G4ThreeVector proposedStep = currentParticlePositionGlobal-proposedPositionGlobal;
  aParticleChange.ProposePosition(particleLorentz.vect()*proposedStep.mag());

  return G4VDiscreteProcess::PostStepDoIt(track, step);

}
