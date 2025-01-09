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
#include "BDSElectronQuantumLevel.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonExcitationEngine.hh"
#include "BDSLaserIonExcitation.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSLaser.hh"
#include "BDSStep.hh"
#include "BDSUserTrackInformation.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Electron.hh"
#include "G4DecayProducts.hh"
#include "G4Hydrogen.hh"
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
#include "BDSElectronOccupancy.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <limits>

BDSLaserIonExcitation::BDSLaserIonExcitation(const G4String& processName):
  G4VDiscreteProcess(processName, G4ProcessType::fUserDefined),
  auxNavigator(new BDSAuxiliaryNavigator())
{;}

BDSLaserIonExcitation::~BDSLaserIonExcitation()
{
  delete auxNavigator;
}

G4double BDSLaserIonExcitation::GetMeanFreePath(const G4Track& track,
						G4double /*previousStepSize*/,
						G4ForceCondition* forceCondition)
{
  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  BDSUserTrackInformation* trackInfo = dynamic_cast<BDSUserTrackInformation*>(track.GetUserInformation());

  if (!lv->IsExtended())
    {// not extended so can't be a laser logical volume
      return std::numeric_limits<double>::max();
    }
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv)
    {// it's an extended volume but not ours (could be a crystal)
      return std::numeric_limits<double>::max();
    }
  G4bool excited = trackInfo->GetElectronOccupancy()->StatePopulated(2,1,0.5);
  if(excited)
  {// its already in the excited state cannot be excited again
    return std::numeric_limits<double>::max();
  }

  const BDSLaser* laser = lvv->Laser();
  aParticleChange.Initialize(track);

  *forceCondition = Forced;
  return laser->Sigma0()/10;
}

G4VParticleChange* BDSLaserIonExcitation::PostStepDoIt(const G4Track& track,
						       const G4Step&  step)
{
  // get coordinates for photon desity calculations
  aParticleChange.Initialize(track);
  //copied from mfp to access laser instance is clearly incorrect!
  
  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended())// not extended so can't be a laser logical volume
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

  BDSIonExcitationEngine* ionExcitationEngine = new BDSIonExcitationEngine();
  // create photon
  // photon vector in laser frame coordinates
  G4ThreeVector photonUnit(0, 0, 1);
  //translate into lab frame coordinates
  G4double photonE = (CLHEP::h_Planck * CLHEP::c_light) / laser->Wavelength();
  G4ThreeVector photonVector = photonUnit * photonE;
  photonVector.transform(*rot);
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector, photonE);
  
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4ThreeVector ionBeta = ionMomentum / ionEnergy;
  G4double ionVelocity = ionBeta.mag() * CLHEP::c_light;
  photonLorentz.boost(-ionBeta);
  G4double photonEnergy = photonLorentz.e();
  G4double crossSection = ionExcitationEngine->CrossSection(photonEnergy);
  G4double particleGlobalTime = track.GetGlobalTime();
  G4double photonFlux = (laser->Intensity(particlePositionLocal,0)*laser->TemporalProfileGaussian(particleGlobalTime,particlePositionLocal.z()))/photonEnergy;
  G4LorentzVector ion4Vector = ion->Get4Momentum();
  ion4Vector.boost(-ionBeta);

  G4double ionTime = (stepLength / ionVelocity);
  G4double excitationProbability = 1.0 - std::exp(-crossSection * photonFlux * ionTime);
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  G4double scaleFactor = g->ScaleFactorLaser();
  G4double randomNumber = G4UniformRand();
  
  if ((excitationProbability * scaleFactor) > randomNumber)
    {
      BDSUserTrackInformation* userInfo = dynamic_cast<BDSUserTrackInformation*>(track.GetUserInformation());
      userInfo->GetElectronOccupancy()->RemoveElectrons(2,0,0.5,1);
      userInfo->GetElectronOccupancy()->AddElectrons(2,1,0.5,1);
      userInfo->GetElectronOccupancy()->SetTimeOfExciation(ion->GetProperTime(),2,1,(1/2));
      // Kinematics
      ionExcitationEngine->setIncomingGamma(photonLorentz);
      ionExcitationEngine->setIncomingIon(ion4Vector);
      ionExcitationEngine->PhotonAbsorption(ionBeta);
      G4LorentzVector scatteredIon = ionExcitationEngine->GetScatteredIonAbsorption();
      
      G4LorentzVector IonLorentz = G4LorentzVector(scatteredIon.vect().unit(),scatteredIon.e());
      
      aParticleChange.ProposeEnergy(scatteredIon.e());
      aParticleChange.ProposeMomentumDirection(IonLorentz.getX(),IonLorentz.getY(),IonLorentz.getZ());
      aParticleChange.ProposeWeight(scaleFactor);
    }

  delete ionExcitationEngine;
  
  return G4VDiscreteProcess::PostStepDoIt(track,step);
}
