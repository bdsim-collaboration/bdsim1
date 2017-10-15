#include "BDSBunch.hh"
#include "BDSDebug.hh"
#include "BDSEventInfo.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSOutputLoader.hh"
#include "BDSParticle.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRandom.hh"

#include "CLHEP/Random/Random.h"

#include "globals.hh" // geant4 types / globals
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"

BDSPrimaryGeneratorAction::BDSPrimaryGeneratorAction(BDSBunch* bdsBunchIn):
  G4VUserPrimaryGeneratorAction(),
  weight(1),
  bdsBunch(bdsBunchIn),
  recreateFile(nullptr),
  eventOffset(0),
  ionDefinition(nullptr),
  overrideCharge(false),
  particleCharge(0)
{
  particleGun  = new G4ParticleGun(1); // 1-particle gun

  writeASCIISeedState = BDSGlobalConstants::Instance()->WriteSeedState();
  recreate            = BDSGlobalConstants::Instance()->Recreate();
  useASCIISeedState   = BDSGlobalConstants::Instance()->UseASCIISeedState();
  
  if (recreate)
    {
      recreateFile = new BDSOutputLoader(BDSGlobalConstants::Instance()->RecreateFileName());
      eventOffset  = BDSGlobalConstants::Instance()->StartFromEvent();
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Primary particle is "
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName() << G4endl;
#endif
  ionPrimary = BDSGlobalConstants::Instance()->IonPrimary();
  if (ionPrimary)
    {
      ionDefinition  = BDSGlobalConstants::Instance()->IonDefinition();
      overrideCharge = ionDefinition->OverrideCharge();
      if (overrideCharge)
	{particleCharge = ionDefinition->Charge();}
    }
  
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector(0.*CLHEP::cm,0.*CLHEP::cm,0.*CLHEP::cm));
  particleGun->SetParticleEnergy(BDSGlobalConstants::Instance()->BeamKineticEnergy());
  particleGun->SetParticleTime(0);
}

BDSPrimaryGeneratorAction::~BDSPrimaryGeneratorAction()
{
  delete particleGun;
  delete recreateFile;
}

void BDSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Load seed state if recreating.
  if (recreate)
    {BDSRandom::SetSeedState(recreateFile->SeedState(anEvent->GetEventID() + eventOffset));}
  
  // Save the seed state in a file to recover potentially unrecoverable events
  if (writeASCIISeedState)
    {BDSRandom::WriteSeedState();}

  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  if (useASCIISeedState)
    {
      G4String fileName = globals->SeedStateFileName();
      BDSRandom::LoadSeedState(fileName);
    }

  // Always save seed state in output
  BDSEventInfo* eventInfo = new BDSEventInfo();
  anEvent->SetUserInformation(eventInfo);
  eventInfo->SetSeedStateAtStart(BDSRandom::GetSeedState());

  G4double mass = particleGun->GetParticleDefinition()->GetPDGMass();
  
  G4double x0=0.0, y0=0.0, z0=0.0, xp=0.0, yp=0.0, zp=0.0, t=0.0, E=0.0;
  // continue generating particles until positive finite kinetic energy.
  G4int n = 0;
  while (n < 100) // prevent infinite loops
    {
      ++n;
      bdsBunch->GetNextParticle(x0, y0, z0, xp, yp, zp, t, E, weight); // get next starting point

      if ((E - mass) > 0)
        {break;}
    }

  // set particle definition
  // either from input bunch file, an ion, or regular beam particle
  if (bdsBunch->ParticleCanBeDifferentFromBeam())
    {
      particleGun->SetParticleDefinition(bdsBunch->ParticleDefinition()->ParticleDefinition());
    }
  else
    {particleGun->SetParticleDefinition(globals->GetParticleDefinition()->ParticleDefinition());}

  // In the case of ions we should override the default charge of 0
  if (overrideCharge)
    {particleGun->SetParticleCharge(particleCharge);}

  // check that kinetic energy is positive and finite anyway and abort if not.
  G4double EK = E - mass;
  if(EK <= 0)
  {
    G4cout << __METHOD_NAME__ << "Particle kinetic energy smaller than 0! This will not be tracked." << G4endl;
    anEvent->SetEventAborted();
  }

  /// Write initial particle position and momentum
  if (writeASCIISeedState) {
    std::ofstream ofstr("output.primary.txt");
    ofstr << x0 << " " << y0 << " " << z0 << " "
          << xp << " " << yp << " " << zp << " "
          << t << " " << E << " " << weight << std::endl;
    ofstr.close();
  }

  /// Check the coordinates are valid
  if (!worldExtent.Encompasses(x0,y0,z0))
    {
      G4cerr << __METHOD_NAME__ << "point (" << x0 << ", " << y0 << ", " << z0
	     << ") mm lies outside the world volume with extent ("
	     << worldExtent << ") - event aborted!" << G4endl << G4endl;
      anEvent->SetEventAborted();
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ 
	 << x0 << " " << y0 << " " << z0 << " " 
	 << xp << " " << yp << " " << zp << " " 
	 << t  << " " << E  << " " << weight << G4endl;
#endif
  
  G4ThreeVector PartMomDir(xp,yp,zp);
  G4ThreeVector PartPosition(x0,y0,z0);
  
  particleGun->SetParticlePosition(PartPosition);
  particleGun->SetParticleEnergy(EK);
  particleGun->SetParticleMomentumDirection(PartMomDir);
  particleGun->SetParticleTime(t);
  
  particleGun->GeneratePrimaryVertex(anEvent);

  //Set the weight
#ifdef BDSDEBUG
  G4cout << "BDSPrimaryGeneratorAction: setting weight = " << weight << G4endl;
#endif
  anEvent->GetPrimaryVertex()->SetWeight(weight);
  
#ifdef BDSDEBUG
  G4cout
    << "BDSPrimaryGeneratorAction: " << G4endl
    << "  position= " << particleGun->GetParticlePosition()/CLHEP::m<<" m"<<G4endl
    << "  total energy= " << E/CLHEP::GeV << " GeV" << G4endl
    << "  kinetic energy= " << EK/CLHEP::GeV << " GeV" << G4endl
    << "  momentum direction= " << PartMomDir << G4endl
    << "  weight= " << anEvent->GetPrimaryVertex()->GetWeight() << G4endl;
#endif

  // save initial values outside scope for entry into the samplers:
  BDSParticle initialPoint(x0,y0,z0,xp,yp,zp,E,t,weight);
  BDSGlobalConstants::Instance()->SetInitialPoint(initialPoint);
}
