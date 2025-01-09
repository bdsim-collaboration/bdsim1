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
#include "G4Version.hh"

#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleDefinition.hh"
#if G4VERSION_NUMBER > 1039
#include "BDSPhysicsChannelling.hh"
#endif
#include "BDSPhysicsCutsAndLimits.hh"
#include "BDSPhysicsEMDissociation.hh"
#include "BDSPhysicsMuonSplitting.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"
#include "BDSEmStandardPhysicsOp4Channelling.hh" // included with bdsim

#include "FTFP_BERT.hh"
#include "globals.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4Electron.hh"
#include "G4EmParameters.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysicsSS.hh"
#include "G4DynamicParticle.hh"
#include "G4Gamma.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4GenericIon.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonTable.hh"
#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonZero.hh"
#include "G4KaonZeroLong.hh"
#include "G4KaonZeroShort.hh"
#include "G4LeptonConstructor.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4Neutron.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTableIterator.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionZero.hh"
#include "G4Positron.hh"
#include "G4ProductionCutsTable.hh"
#include "G4Proton.hh"
#include "G4PhysListFactory.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4Proton.hh"
#include "G4String.hh"
#include "G4UImanager.hh"
#if G4VERSION_NUMBER > 1049
#include "G4ParticleDefinition.hh"
#include "G4CoupledTransportation.hh"
#include "G4Transportation.hh"
#include <utility>
#endif

#if G4VERSION_NUMBER > 1069
#include "G4HadronicParameters.hh"
#endif

#include "parser/beam.h"
#include "parser/fastlist.h"
#include "parser/physicsbiasing.h"

#include <iomanip>
#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <string> // for stoi

G4bool BDS::IsIon(const G4ParticleDefinition* particle)
{
  return G4IonTable::IsIon(particle) && particle!=G4Proton::Definition();
}

G4bool BDS::IsIon(const G4DynamicParticle* particle)
{
  return BDS::IsIon(particle->GetDefinition()) || particle->GetTotalOccupancy()>0;
}

G4VModularPhysicsList* BDS::BuildPhysics(const G4String& physicsList, G4int verbosity)
{
  // this must be done BEFORE any physics processes are constructed
  // set the upper and lower energy levels applicable for all physics processes
  BDS::CheckAndSetEnergyValidityRange();
  
  G4VModularPhysicsList* result = nullptr;

  BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  
  BDS::ConstructMinimumParticleSet();
  G4String physicsListNameLower = BDS::LowerCase(physicsList);
  G4bool useGeant4Physics = BDS::StrContains(physicsListNameLower, "g4");
  G4bool completePhysics  = BDS::StrContains(physicsListNameLower, "complete");
  if (useGeant4Physics)
    {
      // strip off G4_ prefix - from original as G4 factory case sensitive
      G4String geant4PhysicsList = physicsList.substr(2);
      G4PhysListFactory factory;
      if (!factory.IsReferencePhysList(geant4PhysicsList))
        {
          G4cerr << "Unknown Geant4 physics list \"" << geant4PhysicsList << "\"" << G4endl;
          G4cout << "Available Geant4 hadronic physics lists:" << G4endl;
          for (const auto &name : factory.AvailablePhysLists())
            {G4cout << "\"" << name << "\"" << G4endl;}
          G4cout << "Available Geant4 EM physics lists:" << G4endl;
          for (const auto &name : factory.AvailablePhysListsEM())
            {G4cout << "\"" << name << "\"" << G4endl;}
          throw BDSException(__METHOD_NAME__, "Unknown Geant4 physics list \"" + geant4PhysicsList + "\"");
        }
      else
        {
          result = factory.GetReferencePhysList(geant4PhysicsList);
          if (g->G4PhysicsUseBDSIMRangeCuts())
            {BDS::SetRangeCuts(result);}
          if (g->MinimumKineticEnergy() > 0 || g->G4PhysicsUseBDSIMCutsAndLimits())
            {
              G4cout << "\nAdding cuts and limits physics process to Geant4 reference physics list" << G4endl;
              G4cout << "This is to enforce BDSIM range cuts and the minimumKinetic energy option.\n";
              G4cout << "This is done by default for the functionality of BDSIM tracking and should not affect the physics greatly.\n";
              G4cout << "See the BDSIM manual about Geant4 reference physics lists for details." << G4endl;
              result->RegisterPhysics(new BDSPhysicsCutsAndLimits(g->ParticlesToExcludeFromCutsAsSet()));
            }
          else if (!g->G4PhysicsUseBDSIMCutsAndLimits() && g->Circular())
            {
              G4String message = "g4PhysicsUseBDSIMCutsAndLimits turned off but using a circular machine - circular mechanics will be broken";
              BDS::Warning(__METHOD_NAME__, message);
            }
        }
    }
  else if (completePhysics)
    {// we test one by one for the exact name of very specific physics lists
      if (BDS::StrContains(physicsListNameLower, "channelling"))
        {
          G4cout << "Constructing \"" << physicsListNameLower << "\" complete physics list" << G4endl;
#if G4VERSION_NUMBER > 1039
          G4bool useEMD  = BDS::StrContains(physicsListNameLower, "emd");
          G4bool regular = BDS::StrContains(physicsListNameLower, "regular");
          G4bool em4     = BDS::StrContains(physicsListNameLower, "em4");
          G4bool emss    = BDS::StrContains(physicsListNameLower, "emss");
          // we don't assign 'result' variable or proceed as that would result in the
          // range cuts being set for a complete physics list that we wouldn't use
          auto r = BDS::ChannellingPhysicsComplete(useEMD, regular, em4, emss);
          r->SetVerboseLevel(verbosity);
          return r;
#else
          throw BDSException(__METHOD_NAME__, "Channel physics is not supported with Geant4 versions less than 10.4");
#endif
        }
      else
        {throw BDSException(__METHOD_NAME__, "Unknown 'complete' physics list \"" + physicsList + "\"");}
    }
  else
    {
      result = new BDSModularPhysicsList(physicsList);
      BDS::SetRangeCuts(result, verbosity); // always set our range cuts for our physics list
    }
  
  // force construction of the particles - does no harm and helps with
  // usage of exotic particle beams
  result->ConstructParticle();
  result->SetVerboseLevel(verbosity);
  
  // apply any user-supplied macro to adjust geant4 physics parameters
  G4UImanager* UIManager = G4UImanager::GetUIpointer();
  G4String physicsMacro = BDSGlobalConstants::Instance()->Geant4PhysicsMacroFileName();
  if (!physicsMacro.empty())
    {
      G4bool setInExecOptions = BDSGlobalConstants::Instance()->Geant4PhysicsMacroFileNameFromExecOptions();
      G4String physicsMacroFull = BDS::GetFullPath(physicsMacro, false, setInExecOptions);
      G4cout << "Applying geant4 physics macro file: " << physicsMacroFull << G4endl;
      UIManager->ApplyCommand("/control/execute " + physicsMacroFull);
    }
  
  G4VUserPhysicsList* resultAsUserPhysicsList = dynamic_cast<G4VUserPhysicsList*>(result);
  if (resultAsUserPhysicsList)
    {// have to cast as they shadow functions and aren't virtual :(
      resultAsUserPhysicsList->DumpCutValuesTable(verbosity);
      resultAsUserPhysicsList->SetVerboseLevel(verbosity);
    }
  return result;
}

G4int BDS::NBeamParametersSet(const GMAD::Beam&            beamDefinition,
                              const std::set<std::string>& keys)
{
  G4int nSet = 0;
  for (const auto& k : keys)
    {nSet += beamDefinition.HasBeenSet(k) ? 1 : 0;}
  return nSet;
}

void BDS::ConflictingParametersSet(const GMAD::Beam&            beamDefinition,
                                   const std::set<std::string>& keys,
                                   G4int                        nSet,
                                   G4bool                       warnZeroParamsSet,
                                   const G4String&              unitString)
{
  if (nSet > 1)
    {
      G4cerr << "Beam> More than one parameter set - there should only be one" << G4endl;
      for (const auto& k : keys)
        {G4cerr << std::setw(14) << std::left << k << ": " << std::setw(7) << std::right << beamDefinition.get_value(k) << " " << unitString << G4endl;}
      throw BDSException(__METHOD_NAME__, "conflicting parameters set");
    }
  else if (nSet == 0 && warnZeroParamsSet)
    {
      G4cerr << "Beam> One of the following required to be set" << G4endl;
      for (const auto &k : keys)
        {G4cerr << std::setw(14) << std::left << k << ": " << std::setw(7) << std::right << beamDefinition.get_value(k) << " " << unitString << G4endl;}
      throw BDSException(__METHOD_NAME__, "insufficient parameters set");
    }
}

void BDS::ConstructDesignAndBeamParticle(const GMAD::Beam& beamDefinition,
                                         G4double ffact,
                                         BDSParticleDefinition*& designParticle,
                                         BDSParticleDefinition*& beamParticle,
                                         G4bool& beamDifferentFromDesignParticle)
{
  if (beamDefinition.particle.empty())
    {throw BDSException("Beam> no \"particle\" specified (required).");}

  // check only one of the following has been set - ie no conflicting information
  std::set<std::string> keysDesign = {"energy", "momentum", "kineticEnergy"};
  G4int nSetDesign = BDS::NBeamParametersSet(beamDefinition, keysDesign);
  BDS::ConflictingParametersSet(beamDefinition, keysDesign, nSetDesign);
  std::set<std::string> keysParticle = {"E0", "P0", "Ek0"};
  G4int nSetParticle = BDS::NBeamParametersSet(beamDefinition, keysParticle);
  
  designParticle = BDS::ConstructParticleDefinition(beamDefinition.particle,
                                                    beamDefinition.beamEnergy        * CLHEP::GeV,
                                                    beamDefinition.beamKineticEnergy * CLHEP::GeV,
                                                    beamDefinition.beamMomentum      * CLHEP::GeV,
                                                    ffact);

  // ensure a default
  std::string beamParticleName = beamDefinition.beamParticleName.empty() ? beamDefinition.particle : beamDefinition.beamParticleName;
  beamDifferentFromDesignParticle = nSetParticle > 0 || beamParticleName != beamDefinition.particle;
  if (nSetParticle > 0)
    {// at least one specified so use all of the beam particle ones
      BDS::ConflictingParametersSet(beamDefinition, keysParticle, nSetParticle, false);
      beamParticle = BDS::ConstructParticleDefinition(beamParticleName,
                                                      beamDefinition.E0  * CLHEP::GeV,
                                                      beamDefinition.Ek0 * CLHEP::GeV,
                                                      beamDefinition.P0  * CLHEP::GeV,
                                                      ffact);
    }
  else if (beamDefinition.beamParticleName != beamDefinition.particle && !beamDefinition.beamParticleName.empty())
    {// different particle name but no extra E0, Ek0 or P0 -> therefore use general beam defaults
      beamParticle = BDS::ConstructParticleDefinition(beamParticleName,
                                                      beamDefinition.beamEnergy        * CLHEP::GeV,
                                                      beamDefinition.beamKineticEnergy * CLHEP::GeV,
                                                      beamDefinition.beamMomentum      * CLHEP::GeV,
                                                      ffact);
    }
  else
    {
      beamParticle = new BDSParticleDefinition(*designParticle);
    }
}

BDSParticleDefinition* BDS::ConstructParticleDefinition(const G4String& particleNameIn,
                                                        G4double totalEnergyIn,
                                                        G4double kineticEnergyIn,
                                                        G4double momentumIn,
                                                        G4double ffact)
{
  BDSParticleDefinition* particleDefB = nullptr; // result
  G4String particleName = BDS::LowerCase(particleNameIn);

  std::map<G4String, G4String> commonSubstitutions = { {"photon", "gamma"},
                                                       {"electron", "e-"},
                                                       {"positron", "e+"},
                                                       {"pion+", "pi+"},
                                                       {"pion-", "pi-"},
                                                       {"pion0", "pi0"},
                                                       {"antiproton", "anti_proton"},
                                                       {"anti-proton", "anti_proton"} };

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  std::regex ionParticle("(ion\\s).*");
  if (std::regex_match(particleName, ionParticle))
    {
      G4GenericIon::GenericIonDefinition(); // construct general ion particle
      auto ionDef = new BDSIonDefinition(particleName); // parse the ion definition

      G4IonTable* ionTable = particleTable->GetIonTable();
      /// cache this here in case the particle definition isn't available until during a run
      G4int ionPDGID  = G4IonTable::GetNucleusEncoding(ionDef->Z(), ionDef->A());
      G4double mass   = ionTable->GetIonMass(ionDef->Z(), ionDef->A());
      mass += ionDef->NElectrons()*G4Electron::Definition()->GetPDGMass();
      G4double charge = ionDef->Charge(); // correct even if overridden
      particleDefB = new BDSParticleDefinition(particleName, mass, charge,
                                               totalEnergyIn, kineticEnergyIn, momentumIn, ffact, ionDef, ionPDGID);
      delete ionDef;
    }
  else
    {
      // swap out some common name substitutions for Geant4 ones
      auto searchName = commonSubstitutions.find(particleName);
      if (searchName != commonSubstitutions.end())
        {
          G4cout << "Substituting particle name \"" << particleName << "\" for the Geant4 name: \"" << searchName->second << "\"" << G4endl;
          particleName = searchName->second;
        }

      BDS::ConstructBeamParticleG4(particleName); // enforce construction of some basic particles
      G4ParticleDefinition* particleDef = nullptr;
      // try and see if it's an integer and therefore PDG ID, if not search by string
      try
        {
          // we try this because std::stoi can throw a std::invalid_argument or
          // std::out_of_range exception, both of which inherit std::logic_error
          int particleID = std::stoi(particleName);
          // we don't use the G4ParticleTable->FindParticle(int) because it unnecessarily
          // checks for physics readiness and throws an exception. here we just inspect
          // the encoding dictionary ourselves. it's all typedeffed but it's std::map<G4int, G4ParticleDefinition*>
          G4ParticleTable::G4PTblEncodingDictionary* encoding = G4ParticleTable::fEncodingDictionary;
          auto search = encoding->find(particleID);
          if (search != encoding->end())
            {particleDef = search->second;}
          else
            {throw BDSException(__METHOD_NAME__,"PDG ID \"" + particleName + "not found in particle table");}
        }
      catch (const std::logic_error&) // else, usual way by string search
        {
          particleDef = particleTable->FindParticle(particleName);
          if (!particleDef)
            {particleDef = particleTable->FindParticle(particleNameIn);} // try with original case
        }
      if (!particleDef)
        {
          BDS::PrintDefinedParticles();
          throw BDSException(__METHOD_NAME__, "Particle \"" + particleName + "\" not found.");
        }
      particleDefB = new BDSParticleDefinition(particleDef, totalEnergyIn, kineticEnergyIn, momentumIn, ffact);
    }
  return particleDefB;
}

void BDS::ConstructBeamParticleG4(const G4String& name)
{
  // note, we compare to the lower case name here, not the Geant4 one as
  // we will already have converted to lower case when this is used
  if (name == "proton")
    {G4Proton::ProtonDefinition();}
  else if (name == "anti_proton")
    {G4AntiProton::AntiProtonDefinition();}
  else if (name == "e-")
    {G4Electron::ElectronDefinition();}
  else if (name == "e+")
    {G4Positron::PositronDefinition();}
  else if (name == "pi-")
    {G4PionMinus::PionMinusDefinition();}
  else if (name == "pi+")
    {G4PionPlus::PionPlusDefinition();}
  else if (name == "pi0")
    {G4PionZero::PionZeroDefinition();}
  else if (name == "neutron")
    {G4Neutron::NeutronDefinition();}
  else if (name == "photon" || name == "gamma")
    {G4Gamma::Gamma();}
  else if (name == "mu-")
    {G4MuonMinus::MuonMinusDefinition();}
  else if (name == "mu+")
    {G4MuonPlus::MuonPlusDefinition();}
  else if (name == "kaon-")
    {G4KaonMinus::KaonMinusDefinition();}
  else if (name == "kaon+")
    {G4KaonPlus::KaonPlusDefinition();}
  else if (name == "kaon0")
    {G4KaonZero::KaonZeroDefinition();}
  else if (name == "kaon0l")
    {G4KaonZeroLong::KaonZeroLongDefinition();}
  else if (name == "kaon0s")
    {G4KaonZeroShort::KaonZeroShortDefinition();}
  else if (name == "nu_e")
    {G4NeutrinoE::NeutrinoEDefinition();}
  else if (name == "anti_nu_e")
    {G4AntiNeutrinoE::AntiNeutrinoEDefinition();}
  else if (name == "nu_mu")
    {G4NeutrinoMu::NeutrinoMuDefinition();}
  else if (name == "anti_nu_mu")
    {G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();}
  else if (name == "nu_tau")
    {G4NeutrinoTau::NeutrinoTauDefinition();}
  else if (name == "anti_nu_tau")
    {G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();}
  else
    {
      G4String msg = "Unknown common beam particle type \"" + name;
      msg += "\" - if it doesn't work, include all \"all_particles\" in the physicsList option.";
      BDS::Warning(__METHOD_NAME__, msg);
    }
}

void BDS::ConstructMinimumParticleSet()
{
  // e-, e+, v_e, v_e(bar)
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();

  // p, pbar, neutron, anti-neutron
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();

  // photon
  G4Gamma::Gamma();
}

void BDS::ConstructExtendedParticleSet()
{
  G4LeptonConstructor::ConstructParticle();
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();
  G4GenericIon::GenericIonDefinition();
}

void BDS::PrintPrimaryParticleProcesses(const G4String& primaryParticleName)
{
  G4cout << "Register physics processes by name for the primary particle \""
         << primaryParticleName << "\":" << G4endl;

  auto particle = G4ParticleTable::GetParticleTable()->FindParticle(primaryParticleName);
  if (!particle)
    {// could be ion that isn't defined
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "primary particle not defined yet - could be ion" << G4endl;
#endif
      return;
    }
  auto pl = particle->GetProcessManager()->GetProcessList();
  for (G4int i = 0; i < (G4int)pl->length(); i++)
    {G4cout << "\"" << (*pl)[i]->GetProcessName() << "\"" << G4endl;}
}


G4GenericBiasingPhysics* BDS::BuildAndAttachBiasWrapper(const GMAD::FastList<GMAD::PhysicsBiasing>& biases)
{
  std::set<const G4ParticleDefinition*> particlesToBias;
  for (const auto& b : biases)
    {
      const G4ParticleDefinition* particle = nullptr;
      G4String particleName = G4String(b.particle);
      // this works for "GenericIon" too
      particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
      
      if (particle)
        {particlesToBias.insert(particle);}
      else
        {throw BDSException(__METHOD_NAME__, "Unknown particle type for biasing: \"" + particleName + "\"");}
    }
  
  if (particlesToBias.empty()) // nothing valid to bias
    {return nullptr;}
  
  G4GenericBiasingPhysics* physBias = new G4GenericBiasingPhysics();
  for (auto part : particlesToBias)
    {
      const G4String& particleName = part->GetParticleName();
      G4cout << __METHOD_NAME__ << "wrapping \"" << particleName << "\" for biasing" << G4endl;
      physBias->Bias(particleName);
    }
  return physBias;
}

void BDS::BuildMuonBiasing(G4VModularPhysicsList* physicsList)
{
  auto globals = BDSGlobalConstants::Instance();
  G4int muonSplittingFactor = globals->MuonSplittingFactor();
  if (muonSplittingFactor > 1)
    {
      G4int muonSplittingFactor2 = globals->MuonSplittingFactor2();
      G4double muonSplittingThresholdParentEk = globals->MuonSplittingThresholdParentEk();
      G4double muonSplittingThresholdParentEk2 = globals->MuonSplittingThresholdParentEk2();
      G4cout << "BDSPhysicsMuonSplitting -> using muon splitting wrapper -> factor of: " << muonSplittingFactor << G4endl;
      if (muonSplittingThresholdParentEk > 0)
        {G4cout << "BDSPhysicsMuonSplitting -> minimum parent kinetic energy: " << muonSplittingThresholdParentEk / CLHEP::GeV << " GeV" << G4endl;}
      if (muonSplittingFactor2 > 1)
        {
          G4cout << "BDSPhysicsMuonSplitting -> factor #2: " << muonSplittingFactor2 << " for muons above "
                 << muonSplittingThresholdParentEk / CLHEP::GeV << " GeV" << G4endl;
        }
      G4bool excludeW1P = globals->MuonSplittingExcludeWeight1Particles();
      physicsList->RegisterPhysics(new BDSPhysicsMuonSplitting(muonSplittingFactor,  muonSplittingThresholdParentEk,
                                                               muonSplittingFactor2, muonSplittingThresholdParentEk2,
                                                               excludeW1P, globals->MuonSplittingExclusionWeight()));
    }
}

void BDS::PrintDefinedParticles()
{
  G4cout << __METHOD_NAME__ << "Defined particles: " << G4endl;
  auto it = G4ParticleTable::GetParticleTable()->GetIterator();
  it->reset(); // because there's only 1 iterator due to geant4 design
  while ((*it)())
    {G4cout <<  it->value()->GetParticleName() << " ";}
  G4cout << G4endl;
}

#if G4VERSION_NUMBER > 1039
G4VModularPhysicsList* BDS::ChannellingPhysicsComplete(G4bool useEMD,
                                                       G4bool regular,
                                                       G4bool em4,
                                                       G4bool emss)
{
  G4VModularPhysicsList* physlist = new FTFP_BERT();
  physlist->RegisterPhysics(new G4IonElasticPhysics()); // not included by default in FTFP_BERT
  G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();
  physlist->RegisterPhysics(new BDSPhysicsChannelling());
  if (!regular)
    {
      // replace the EM physics in the Geant4 provided FTFP_BERT composite physics list
      physlist->ReplacePhysics(new BDSEmStandardPhysicsOp4Channelling());
    }
  else if (em4)
    {
      physlist->ReplacePhysics(new G4EmStandardPhysics_option4());
    }
  else if (emss)
    {
      physlist->ReplacePhysics(new G4EmStandardPhysicsSS());
    }

  // optional electromagnetic dissociation that isn't in FTFP_BERT by default
  if (useEMD)
    {
      G4cout << "Adding EM Dissocation to crystal channelling physics list" << G4endl;
      physlist->RegisterPhysics(new BDSPhysicsEMDissociation());
    }

  biasingPhysics->PhysicsBiasAllCharged();
  physlist->RegisterPhysics(biasingPhysics);
  if (BDSGlobalConstants::Instance()->MinimumKineticEnergy() > 0 &&
      BDSGlobalConstants::Instance()->G4PhysicsUseBDSIMCutsAndLimits())
    {
      G4cout << "\nWARNING - adding cuts and limits physics process to \"COMPLETE\" physics list" << G4endl;
      G4cout << "This is to enforce BDSIM range cuts and the minimumKinetic energy option.\n";
      G4cout << "This can be turned off by setting option, g4PhysicsUseBDSIMCutsAndLimits=0;\n" << G4endl;
      physlist->RegisterPhysics(new BDSPhysicsCutsAndLimits(BDSGlobalConstants::Instance()->ParticlesToExcludeFromCutsAsSet()));
    }
  return physlist;
}
#endif

void BDS::SetRangeCuts(G4VModularPhysicsList* physicsList, G4int verbosity)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();

  // overwrite when explicitly set in options
  if (globals->DefaultRangeCutsSet())
    {
      G4cout << __METHOD_NAME__ << "Default production range cut  " << physicsList->GetDefaultCutValue()  << " mm" << G4endl;
      physicsList->SetDefaultCutValue(globals->DefaultRangeCut());
    }
  if (globals->ProdCutPhotonsSet())
    {
      G4cout << __METHOD_NAME__ << "Photon production range cut   " << physicsList->GetCutValue("gamma")  << " mm" << G4endl;
      physicsList->SetCutValue(globals->ProdCutPhotons(),  "gamma");
    }
  if (globals->ProdCutElectronsSet())
    {
      G4cout << __METHOD_NAME__ << "Electron production range cut " << physicsList->GetCutValue("e-")     << " mm" << G4endl;
      physicsList->SetCutValue(globals->ProdCutElectrons(),"e-");
    }
  if (globals->ProdCutPositronsSet())
    {
      G4cout << __METHOD_NAME__ << "Positron production range cut " << physicsList->GetCutValue("e+")     << " mm" << G4endl;
      physicsList->SetCutValue(globals->ProdCutPositrons(),"e+");
    }
  if (globals->ProdCutProtonsSet())
    {
      G4cout << __METHOD_NAME__ << "Proton production range cut   " << physicsList->GetCutValue("proton") << " mm" << G4endl;
      physicsList->SetCutValue(globals->ProdCutProtons(),  "proton");
    }

  // inspection of the physics list doesn't work at this point and seems to always return 0 apart from the default
  G4cout << __METHOD_NAME__ << "Range cuts from inspection of the physics list" << G4endl;
  G4cout << __METHOD_NAME__ << "Default production range cut  " << physicsList->GetDefaultCutValue()  << " mm" << G4endl;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "List of all constructed particles by physics lists" << G4endl;
  for (auto particle : *G4ParticleTable::fDictionary)
    {G4cout << particle.second->GetParticleName() << ", ";}
  G4cout << G4endl;
#endif

  physicsList->DumpCutValuesTable(verbosity);
}

void BDS::CheckAndSetEnergyValidityRange()
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  G4double energyLimitLow     = globals->PhysicsEnergyLimitLow();
  G4double energyLimitHigh    = globals->PhysicsEnergyLimitHigh();
  G4bool   setEnergyLimitLow  = BDS::IsFinite(energyLimitLow);
  G4bool   setEnergyLimitHigh = BDS::IsFinite(energyLimitHigh);
  
  if (setEnergyLimitLow || setEnergyLimitHigh)
    {
      auto table = G4ProductionCutsTable::GetProductionCutsTable();
      G4double defaultEnergyLimitLow  = table->GetLowEdgeEnergy();
      G4double defaultEnergyLimitHigh = table->GetHighEdgeEnergy();
      G4double elLow  = setEnergyLimitLow  ? energyLimitLow  : defaultEnergyLimitLow;
      G4double elHigh = setEnergyLimitHigh ? energyLimitHigh : defaultEnergyLimitHigh;
      table->SetEnergyRange(elLow, elHigh);
      if (setEnergyLimitLow)
        {
          G4cout << __METHOD_NAME__ << "set EM physics low energy limit:  "
                 << elLow/CLHEP::MeV  << " MeV" << G4endl;
        }
      if (setEnergyLimitHigh)
        {
          G4cout << __METHOD_NAME__ << "set high energy limit: "
                 << elHigh/CLHEP::TeV << " TeV" << G4endl;
      if (elHigh > G4EmParameters::Instance()->MaxKinEnergy())
        {
          G4cout << __METHOD_NAME__ << "set EM physics Ek limit to " << elHigh/CLHEP::TeV << " TeV" << G4endl;
          G4EmParameters::Instance()->SetMaxEnergy(elHigh);
        }
#if G4VERSION_NUMBER > 1069
      // this was in a patch of our own before 10.7 and compensates for ion physics
      if (elHigh > G4HadronicParameters::Instance()->GetMaxEnergy())
        {
          G4cout << __METHOD_NAME__ << "set hadronic physics Ek limit to " << elHigh/CLHEP::TeV << " TeV" << G4endl;
          G4HadronicParameters::Instance()->SetMaxEnergy(elHigh);
        }
#endif
        }
    }
}

#if G4VERSION_NUMBER > 1049
void BDS::FixGeant105ThreshholdsForBeamParticle(const BDSParticleDefinition* particleDefinition)
{
  G4ParticleDefinition* particleDef = particleDefinition->ParticleDefinition();
  BDS::FixGeant105ThreshholdsForParticle(particleDef);
}

void BDS::FixGeant105ThreshholdsForParticle(const G4ParticleDefinition* particleDef)
{
  // in the case of ions the particle definition isn't available early on so protect
  // against this
  if (!particleDef)
    {return;}
  // taken from the Geant4.10.5 field01 example
  // used to compensate for aggressive killing in Geant4.10.5
  G4double warningEnergy   =   1.0 * CLHEP::kiloelectronvolt;  // Arbitrary
  G4double importantEnergy =  10.0 * CLHEP::kiloelectronvolt;  // Arbitrary
  G4double numberOfTrials  =  1500;                            // Arbitrary
  auto transportPair    = BDS::FindTransportation(particleDef);
  auto transport        = transportPair.first;
  auto coupledTransport = transportPair.second;

  if (transport)
    {
      // Change the values of the looping particle parameters of Transportation
      transport->SetThresholdWarningEnergy(warningEnergy);
      transport->SetThresholdImportantEnergy(importantEnergy);
      transport->SetThresholdTrials(numberOfTrials);
    }
  else if(coupledTransport)
    {
      // Change the values for Coupled Transport
      coupledTransport->SetThresholdWarningEnergy(warningEnergy);
      coupledTransport->SetThresholdImportantEnergy(importantEnergy);
      coupledTransport->SetThresholdTrials(numberOfTrials);
    }
}

std::pair<G4Transportation*, G4CoupledTransportation*> BDS::FindTransportation(const G4ParticleDefinition* particleDef)
{
  const auto* partPM = particleDef->GetProcessManager();

  G4VProcess* partTransport = partPM->GetProcess("Transportation");
  auto transport = dynamic_cast<G4Transportation*>(partTransport);

  partTransport = partPM->GetProcess("CoupledTransportation");
  auto coupledTransport = dynamic_cast<G4CoupledTransportation*>(partTransport);

  return std::make_pair(transport, coupledTransport);
}
#endif
