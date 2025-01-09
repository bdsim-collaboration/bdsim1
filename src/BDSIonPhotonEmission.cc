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
#include "BDSElectronOccupancy.hh"
#include "BDSIonPhotonEmission.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSLaser.hh"
#include "BDSIonExcitationEngine.hh"
#include "BDSStep.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUserTrackInformation.hh"

#include "globals.hh"
#include "G4IonTable.hh"
#include "G4Track.hh"

#include "G4AffineTransform.hh"
#include "G4Electron.hh"
#include "G4DecayProducts.hh"
#include "G4Gamma.hh"
#include "G4Hydrogen.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessType.hh"
#include "G4Proton.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"

#include "G4VTouchable.hh"
#include "Randomize.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIonPhotonEmission::BDSIonPhotonEmission(const G4String& processName):
  G4VDiscreteProcess(processName, G4ProcessType::fUserDefined)
{;}

BDSIonPhotonEmission::~BDSIonPhotonEmission()
{;}

G4double BDSIonPhotonEmission::GetMeanFreePath(const G4Track& track,
					       G4double /*previousStepSize*/,
					       G4ForceCondition* /*forceCondition*/)
{
  const G4DynamicParticle*    particle    = track.GetDynamicParticle();
  //const G4ParticleDefinition* particleDef = particle->GetDefinition();

  BDSUserTrackInformation* trackInfo = dynamic_cast<BDSUserTrackInformation*>(track.GetUserInformation());
  G4bool excited = trackInfo->GetElectronOccupancy()->StatePopulated(2,1,0.5);

  // can only apply to ions
  if (!excited)
    {return DBL_MAX;}
  
  // else proceed with lifetime calculation
  G4double aMass = particle->GetMass();
  G4double currentProperTime = particle->GetProperTime();
  G4double excitedTime = trackInfo->GetElectronOccupancy()->GetTimeOfExcitation(2,1,0.5);
  G4double timeOfFlight = currentProperTime-excitedTime;
  //G4double lifeTime = trackInfo->GetElectronOccupancy()->GetStateLifetime(2,1,(1/2));


    // returns the mean free path in GEANT4 internal units
  G4double pathlength = DBL_MIN;
  G4double aCtau = CLHEP::c_light * timeOfFlight;

  const G4double highestValue = 20; // copied from G4Decay
  if (aCtau < DBL_MIN)
    {pathlength =  DBL_MIN;}
  else
    {
      // calculate the mean free path 
      // by using normalized kinetic energy (= Ekin/mass)
      G4double rKineticEnergy = particle->GetKineticEnergy()/aMass; 
      if (rKineticEnergy > highestValue)
	{
	  // gamma >>  1
	  pathlength = (rKineticEnergy + 1.0) * aCtau;
	}
      else if (rKineticEnergy < DBL_MIN)
	{
	  // too slow particle
	  pathlength = DBL_MIN;
	}
      else
	{
	  // beta <1 
	  pathlength = (particle->GetTotalMomentum())/aMass*aCtau ;
	}
    }

  return  pathlength;
}

G4VParticleChange* BDSIonPhotonEmission::PostStepDoIt(const G4Track& track,
						      const G4Step&  step)
{
  aParticleChange.Initialize(track);
  G4DynamicParticle* particle = const_cast<G4DynamicParticle*>(track.GetDynamicParticle());

  //G4double parentEnergy  = particle->GetTotalEnergy();
 // G4ThreeVector parentDirection = track.GetMomentumDirection();
  BDSUserTrackInformation* trackInfo = dynamic_cast<BDSUserTrackInformation*>(track.GetUserInformation());
  trackInfo->GetElectronOccupancy()->RemoveElectrons(2,1,0.5,1);
  trackInfo->GetElectronOccupancy()->AddElectrons(2,0,0.5,1);


  G4ThreeVector ionBeta = particle->GetMomentum()/particle->GetTotalEnergy();
  G4LorentzVector ion4Vector = particle->Get4Momentum();
  ion4Vector.boost(-ionBeta);
  BDSIonExcitationEngine* excitationEngine = new BDSIonExcitationEngine();
  excitationEngine->setIncomingIon(ion4Vector);
  excitationEngine->PhotonEmission(ionBeta);
  G4LorentzVector gammaLorentz = excitationEngine->GetEmittedGamma();
  G4LorentzVector ionLorentz = excitationEngine->GetScatteredIonEmission();

  G4DynamicParticle* gamma = new G4DynamicParticle(G4Gamma::Gamma(),
                                                        gammaLorentz.vect().unit(),
                                                        gammaLorentz.e());
  aParticleChange.AddSecondary(gamma);
  aParticleChange.ProposeEnergy(ionLorentz.e());
  aParticleChange.ProposeMomentumDirection(ionLorentz.vect().unit());
  /*
  G4double ek = generalElectron->GetKineticEnergy();
  G4DecayProducts* newProducts = new G4DecayProducts();
  G4DynamicParticle* daughterparticle = new G4DynamicParticle(G4Gamma::Definition(),
							      direction,
							      ek);

  newProducts->PushProducts(daughterparticle);
  newProducts->SetParentParticle(*(track.GetDynamicParticle()));
  newProducts->Boost(parentEnergy, parentDirection);

  G4double finalGlobalTime = track.GetGlobalTime();

  const G4TouchableHandle thand = track.GetTouchableHandle();
  G4ThreeVector currentPosition = track.GetPosition();
  // create a new track object
  G4Track* secondary = new G4Track(newProducts->PopProducts(),
				   finalGlobalTime,
				   currentPosition);
  // switch on good for tracking flag
  secondary->SetGoodForTrackingFlag();
  secondary->SetTouchableHandle(thand);
  // add the secondary track in the List
  aParticleChange.AddSecondary(secondary);
  
  aParticleChange.SetNumberOfSecondaries(1);
*/
  delete excitationEngine;
  return G4VDiscreteProcess::PostStepDoIt(track, step);
}

/*
G4double BDSIonPhotonEmission::Pmx(G4double e, G4double p1, G4double p2)
{
   // calcurate momentum of daughter particles in two-body decay
   G4double ppp = (e+p1+p2)*(e+p1-p2)*(e-p1+p2)*(e-p1-p2)/(4.0*e*e);
   if (ppp>0) return std::sqrt(ppp);
   else       return -1.;
}

G4double BDSIonPhotonEmission::PostStepGetPhysicalInteractionLength(const G4Track& track,
								    G4double previousStepSize,
								    G4ForceCondition* condition)
{
  G4double result = G4Decay::PostStepGetPhysicalInteractionLength(track, previousStepSize, condition);
  return result;
}


G4double BDSIonPhotonEmission::GetMeanFreePath(const G4Track& aTrack,
					       G4double previousStepSize,
					       G4ForceCondition* condition)
{
  G4double result = G4Decay::GetMeanFreePath(aTrack, previousStepSize, condition);
  return result;
}
*/
