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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSPhysicsAnnihiToMuMu.hh"
#include "BDSPhysicsCherenkov.hh"
#include "BDSPhysicsCutsAndLimits.hh"
#include "BDSPhysicsEMDissociation.hh"
#include "BDSPhysicsGammaToMuMu.hh"
#include "BDSPhysicsIonisation.hh"
#include "BDSPhysicsLaserWire.hh"
#include "BDSPhysicsMuon.hh"
#include "BDSPhysicsMuonInelastic.hh"
#include "BDSPhysicsSynchRad.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSUtilities.hh"
#include "BDSPhysicsLaserPhotoDetachment.hh"
#include "BDSPhysicsLaserIonExcitation.hh"
#include "BDSPhysicsLaserComptonScattering.hh"
#include "BDSPhysicsLaserCumulativePhotodetachment.hh"
#include "BDSPhysicsLaserCumulativeCompton.hh"
#include "BDSPhysicsLaserWire.hh"
#include "BDSPhysicsMuon.hh"
#include "BDSPhysicsSynchRad.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSUtilities.hh"

#include "parser/fastlist.h"
#include "parser/physicsbiasing.h"

// general geant4
#include "globals.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4String.hh"
#include "G4Version.hh"

// physics processes / builders (assumed Geant4.10.0 and upwards)
#include "G4ChargeExchangePhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmLivermorePolarizedPhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysicsSS.hh"
#include "G4EmStandardPhysicsWVI.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4HadronDElasticPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronElasticPhysicsLEND.hh"
#include "G4HadronElasticPhysicsXS.hh"
#include "G4HadronHElasticPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonINCLXXPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4IonQMDPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4OpticalPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4SynchrotronRadiation.hh"

// version specific physics lists
#if G4VERSION_NUMBER > 1009
#include "G4HadronElasticPhysicsPHP.hh"
#endif

#if G4VERSION_NUMBER > 1019
#include "G4EmStandardPhysicsGS.hh"
#endif

#if G4VERSION_NUMBER > 1020
#include "G4SpinDecayPhysics.hh"
#endif

#if G4VERSION_NUMBER > 1022
#include "G4IonPhysicsPHP.hh"
#endif

#if G4VERSION_NUMBER > 1029
#include "G4MuonicAtomDecayPhysics.hh"
#endif

#if G4VERSION_NUMBER > 1039
#include "BDSPhysicsChannelling.hh"
#include "BDSPhysicsRadioactivation.hh"
#include "G4EmDNAPhysics.hh"
#include "G4EmDNAPhysics_option1.hh"
#include "G4EmDNAPhysics_option2.hh"
#include "G4EmDNAPhysics_option3.hh"
#include "G4EmDNAPhysics_option4.hh"
#include "G4EmDNAPhysics_option5.hh"
#include "G4EmDNAPhysics_option6.hh"
#include "G4EmDNAPhysics_option7.hh"
#include "G4HadronPhysicsShieldingLEND.hh"
#endif

#if G4VERSION_NUMBER < 1070
#include "G4OpticalProcessIndex.hh"
#else
#include "G4OpticalParameters.hh"
#endif

#if G4VERSION_NUMBER > 1119
#include "BDSPhysicsXrayReflection.hh"
#endif

// particles
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4BaryonConstructor.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4NeutrinoE.hh"
#include "G4Neutron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4ShortLivedConstructor.hh"

#include <iomanip>
#include <iterator>
#include <limits>
#include <map>
#include <ostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

BDSModularPhysicsList::BDSModularPhysicsList(const G4String& physicsList):
  constructedAllLeptons(false),
  constructedAllShortLived(false),
  constructedAllMesons(false),
  constructedAllBaryons(false),
  constructedAllIons(false),
  temporaryName(""),
  opticalPhysics(nullptr),
  emWillBeUsed(false),
  usingIons(false)
{
  globals = BDSGlobalConstants::Instance();
  
  SetVerboseLevel(1);

  physicsConstructors.insert(std::make_pair("all_particles",          &BDSModularPhysicsList::AllParticles));
  physicsConstructors.insert(std::make_pair("annihi_to_mumu",         &BDSModularPhysicsList::AnnihiToMuMu));
  physicsConstructors.insert(std::make_pair("charge_exchange",        &BDSModularPhysicsList::ChargeExchange));
  physicsConstructors.insert(std::make_pair("cherenkov",              &BDSModularPhysicsList::Cherenkov));
  physicsConstructors.insert(std::make_pair("cuts_and_limits",        &BDSModularPhysicsList::CutsAndLimits));
  physicsConstructors.insert(std::make_pair("decay",                  &BDSModularPhysicsList::Decay));
  physicsConstructors.insert(std::make_pair("decay_radioactive",      &BDSModularPhysicsList::DecayRadioactive));
  physicsConstructors.insert(std::make_pair("em",                     &BDSModularPhysicsList::Em));
  physicsConstructors.insert(std::make_pair("em_extra",               &BDSModularPhysicsList::EmExtra));
  physicsConstructors.insert(std::make_pair("em_livermore",           &BDSModularPhysicsList::EmLivermore));
  physicsConstructors.insert(std::make_pair("em_livermore_polarised", &BDSModularPhysicsList::EmLivermorePolarised));
  physicsConstructors.insert(std::make_pair("em_low_ep",              &BDSModularPhysicsList::EmLowEP));
  physicsConstructors.insert(std::make_pair("em_penelope",            &BDSModularPhysicsList::EmPenelope));
  physicsConstructors.insert(std::make_pair("em_ss",                  &BDSModularPhysicsList::EmSS));
  physicsConstructors.insert(std::make_pair("em_wvi",                 &BDSModularPhysicsList::EmWVI));
  physicsConstructors.insert(std::make_pair("em_1",                   &BDSModularPhysicsList::Em1));
  physicsConstructors.insert(std::make_pair("em_2",                   &BDSModularPhysicsList::Em2));
  physicsConstructors.insert(std::make_pair("em_3",                   &BDSModularPhysicsList::Em3));
  physicsConstructors.insert(std::make_pair("em_4",                   &BDSModularPhysicsList::Em4));
  physicsConstructors.insert(std::make_pair("ftfp_bert",              &BDSModularPhysicsList::FTFPBERT));
  physicsConstructors.insert(std::make_pair("ftfp_bert_hp",           &BDSModularPhysicsList::FTFPBERTHP));
  physicsConstructors.insert(std::make_pair("gamma_to_mumu",          &BDSModularPhysicsList::GammaToMuMu));
  physicsConstructors.insert(std::make_pair("hadronic_elastic",       &BDSModularPhysicsList::HadronicElastic));
  physicsConstructors.insert(std::make_pair("hadronic_elastic_d",     &BDSModularPhysicsList::HadronicElasticD));
  physicsConstructors.insert(std::make_pair("hadronic_elastic_h",     &BDSModularPhysicsList::HadronicElasticH));
  physicsConstructors.insert(std::make_pair("hadronic_elastic_hp",    &BDSModularPhysicsList::HadronicElasticHP));
  physicsConstructors.insert(std::make_pair("hadronic_elastic_lend",  &BDSModularPhysicsList::HadronicElasticLEND));
  physicsConstructors.insert(std::make_pair("hadronic_elastic_xs",    &BDSModularPhysicsList::HadronicElasticXS));
  physicsConstructors.insert(std::make_pair("ion",                    &BDSModularPhysicsList::Ion));
  physicsConstructors.insert(std::make_pair("ion_binary",             &BDSModularPhysicsList::IonBinary));
  physicsConstructors.insert(std::make_pair("ion_elastic",            &BDSModularPhysicsList::IonElastic));
  physicsConstructors.insert(std::make_pair("ion_elastic_qmd",        &BDSModularPhysicsList::IonElasticQMD));
  physicsConstructors.insert(std::make_pair("ion_em_dissociation",    &BDSModularPhysicsList::IonEMDissociation));
  physicsConstructors.insert(std::make_pair("ion_inclxx",             &BDSModularPhysicsList::IonINCLXX));
  physicsConstructors.insert(std::make_pair("ionisation",             &BDSModularPhysicsList::Ionisation));
  physicsConstructors.insert(std::make_pair("lw",                     &BDSModularPhysicsList::LaserWire));
  physicsConstructors.insert(std::make_pair("laser_photo_detachment", &BDSModularPhysicsList::LaserPhotoDetachment));
  physicsConstructors.insert(std::make_pair("laser_compton_scattering", &BDSModularPhysicsList::LaserComptonScattering));
  physicsConstructors.insert(std::make_pair("laser_cumulative_photo_detachment", &BDSModularPhysicsList::LaserCumulativePhotoDetachment));
  physicsConstructors.insert(std::make_pair("laser_cumulative_compton_scattering", &BDSModularPhysicsList::LaserCumulativeCompton));
  physicsConstructors.insert(std::make_pair("laser_ion_excitation",   &BDSModularPhysicsList::LaserIonExcitation));
  physicsConstructors.insert(std::make_pair("muon",                   &BDSModularPhysicsList::Muon));
  physicsConstructors.insert(std::make_pair("muon_inelastic",         &BDSModularPhysicsList::MuonInelastic));
  physicsConstructors.insert(std::make_pair("neutron_tracking_cut",   &BDSModularPhysicsList::NeutronTrackingCut));
  physicsConstructors.insert(std::make_pair("optical",                &BDSModularPhysicsList::Optical));
  physicsConstructors.insert(std::make_pair("qgsp_bert",              &BDSModularPhysicsList::QGSPBERT));
  physicsConstructors.insert(std::make_pair("qgsp_bert_hp",           &BDSModularPhysicsList::QGSPBERTHP));
  physicsConstructors.insert(std::make_pair("qgsp_bic",               &BDSModularPhysicsList::QGSPBIC));
  physicsConstructors.insert(std::make_pair("qgsp_bic_hp",            &BDSModularPhysicsList::QGSPBICHP));
  physicsConstructors.insert(std::make_pair("shielding",              &BDSModularPhysicsList::Shielding));
  physicsConstructors.insert(std::make_pair("stopping",               &BDSModularPhysicsList::Stopping));
  physicsConstructors.insert(std::make_pair("synch_rad",              &BDSModularPhysicsList::SynchRad));
#if G4VERSION_NUMBER > 1019
  physicsConstructors.insert(std::make_pair("em_gs",                  &BDSModularPhysicsList::EmGS));
#endif
#if G4VERSION_NUMBER > 1020
  physicsConstructors.insert(std::make_pair("decay_spin",             &BDSModularPhysicsList::DecaySpin));
#endif
#if G4VERSION_NUMBER > 1022
  physicsConstructors.insert(std::make_pair("ion_php",                &BDSModularPhysicsList::IonPHP));
#endif
#if G4VERSION_NUMBER > 1029
  physicsConstructors.insert(std::make_pair("decay_muonic_atom",      &BDSModularPhysicsList::DecayMuonicAtom));
#endif
#if G4VERSION_NUMBER > 1039
  physicsConstructors.insert(std::make_pair("channelling",            &BDSModularPhysicsList::Channelling));
  physicsConstructors.insert(std::make_pair("dna",                    &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("dna_1",                  &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("dna_2",                  &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("dna_3",                  &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("dna_4",                  &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("dna_5",                  &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("dna_6",                  &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("dna_7",                  &BDSModularPhysicsList::DNA));
  physicsConstructors.insert(std::make_pair("radioactivation",        &BDSModularPhysicsList::Radioactivation));
  physicsConstructors.insert(std::make_pair("shielding_lend",         &BDSModularPhysicsList::ShieldingLEND));
#endif
#if G4VERSION_NUMBER > 1119
  physicsConstructors.insert(std::make_pair("xray_reflection",        &BDSModularPhysicsList::XrayReflection));
#endif

  // old names and aliases
  aliasToOriginal["cerenkov"]      = "cherenkov";
  aliasToOriginal["cutsandlimits"] = "cuts_and_limits";
  aliasToOriginal["em_low"]        = "em_penelope";
  aliasToOriginal["hadronic"]      = "ftfp_bert";
  aliasToOriginal["hadronic_hp"]   = "ftfp_bert_hp";
  aliasToOriginal["ionbinary"]     = "ion_binary";
  aliasToOriginal["ioninclxx"]     = "ion_inclxx";
  aliasToOriginal["ionphp"]        = "ion_php";
  aliasToOriginal["spindecay"]     = "decay_spin";
  aliasToOriginal["synchrad"]      = "synch_rad";
#if G4VERSION_NUMBER > 1039
  aliasToOriginal["channeling"]    = "channelling";
#endif
  
  // prepare vector of valid names for searching when parsing physics list string
  for (const auto& constructor : physicsConstructors)
    {
      physicsLists.emplace_back(constructor.first);
      physicsActivated[constructor.first] = false;
    }

  // setup a list of incompatible physics lists for each one - mostly based on experience
  // initialise all to empty vectors and specify only ones that have some incompatible physics lists
  for (const auto& kv : physicsConstructors)
    {incompatible.insert(std::make_pair(kv.first, std::vector<G4String>()));}
  incompatible["annihi_to_mumu"] = {"em_extra"};
  incompatible["muon"] = {"em_extra"};
  incompatible["muon_inelastic"] = {"em_extra", "muon"};
  incompatible["em"]     = {"em_ss", "em_wvi", "em_1",   "em_2", "em_3", "em_4"};
  incompatible["em_ss"]  = {"em",    "em_wvi", "em_1",   "em_2", "em_3", "em_4"};
  incompatible["em_wvi"] = {"em",    "em_ss",  "em_1",   "em_2", "em_3", "em_4"};
  incompatible["em_1"]   = {"em",    "em_ss",  "em_wvi", "em_2", "em_3", "em_4"};
  incompatible["em_2"]   = {"em",    "em_ss",  "em_wvi", "em_1", "em_3", "em_4"};
  incompatible["em_3"]   = {"em",    "em_ss",  "em_wvi", "em_1", "em_2", "em_4"};
  incompatible["em_4"]   = {"em",    "em_ss",  "em_wvi", "em_1", "em_2", "em_3"};
  incompatible["em_livermore"] = {"em_livermore_polarised"};
  incompatible["em_extra"] = {"muon", "muon_inelastic"};
  incompatible["ftfp_bert"]    = {"ftfp_bert_hp", "qgsp_bert", "qgsp_bert_hp", "qgsp_bic", "qgsp_bic_hp"};
  incompatible["ftfp_bert_hp"] = {"ftfp_bert",    "qgsp_bert", "qgsp_bert_hp", "qgsp_bic", "qgsp_bic_hp"};
  incompatible["gamma_to_mumu"] = {"em_extra"};
  incompatible["hadronic_elastic"]      = {"hadronic_elastic_d", "hadronic_elastic_h", "hadronic_elastic_hp", "hadronic_elastic_lend", "hadronic_elastic_xs"};
  incompatible["hadronic_elastic_d"]    = {"hadronic_elastic",   "hadronic_elastic_h", "hadronic_elastic_hp", "hadronic_elastic_lend", "hadronic_elastic_xs"};
  incompatible["hadronic_elastic_h"]    = {"hadronic_elastic",   "hadronic_elastic_d", "hadronic_elastic_hp", "hadronic_elastic_lend", "hadronic_elastic_xs"};
  incompatible["hadronic_elastic_hp"]   = {"hadronic_elastic",   "hadronic_elastic_d", "hadronic_elastic_h",  "hadronic_elastic_lend", "hadronic_elastic_xs"};
  incompatible["hadronic_elastic_lend"] = {"hadronic_elastic",   "hadronic_elastic_d", "hadronic_elastic_h",  "hadronic_elastic_hp",   "hadronic_elastic_xs"};
  incompatible["hadronic_elastic_xs"]   = {"hadronic_elastic",   "hadronic_elastic_d", "hadronic_elastic_h",  "hadronic_elastic_hp",   "hadronic_elastic_lend"};
  incompatible["ion_elastic"] = {"ion_elastic_qmd"};
  incompatible["ionisation"] = {"em", "em_ss", "em_1", "em_2", "em_3", "em_4", "em_livermore"};
  incompatible["qgsp_bert"]    = {"ftfp_bert", "ftfp_bert_hp", "qgsp_bert_hp", "qgsp_bic",     "qgsp_bic_hp"};
  incompatible["qgsp_bert_hp"] = {"ftfp_bert", "ftfp_bert_hp", "qgsp_bert",    "qgsp_bic",     "qgsp_bic_hp"};
  incompatible["qgsp_bic"]     = {"ftfp_bert", "ftfp_bert_hp", "qgsp_bert",    "qgsp_bert_hp", "qgsp_bic_hp"};
  incompatible["qgsp_bic_hp"]  = {"ftfp_bert", "ftfp_bert_hp", "qgsp_bert",    "qgsp_bert_hp", "qgsp_bic"};

#if G4VERSION_NUMBER > 1019
  for (const auto& name : {"em", "em_ss", "em_wvi", "em_1", "em_2", "em_3", "em_4"})
    {incompatible[name].emplace_back("em_gs");}
  incompatible["em_gs"] = {"em", "em_ss", "em_wvi", "em_1", "em_2", "em_3", "em_4"};
#endif
#if G4VERSION_NUMBER > 1020
  incompatible["decay"].emplace_back("decay_spin"); // append for safety in future
  incompatible["decay_spin"] = {"decay"};
#endif
#if G4VERSION_NUMBER > 1039
  incompatible["shielding"].emplace_back("shielding_lend");
  incompatible["shielding_lend"] = {"shielding"};
#endif
  
  ParsePhysicsList(physicsList);
  ConfigurePhysics();

  // register the physics constructors with base class mechanics.
  for (auto physics : constructors)
    {RegisterPhysics(physics);}
  
#ifdef BDSDEBUG
  if (true)
#else
  if (globals->Verbose())
#endif
    {Print();}
}

BDSModularPhysicsList::~BDSModularPhysicsList()
{;}

void BDSModularPhysicsList::ConstructParticle()
{
  BDS::ConstructMinimumParticleSet();
  G4VModularPhysicsList::ConstructParticle();
}

void BDSModularPhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
  DumpCutValuesTable(100);
}

void BDSModularPhysicsList::Print()
{
  for (const auto& physics : physicsActivated)
    {
      G4String result = (physics.second ? "active" : "inactive");
      G4cout << std::setw(27) << ("\"" + physics.first + "\": ") << result << G4endl;
    }
}

void BDSModularPhysicsList::ParsePhysicsList(const G4String& physListName)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Physics list string: \"" << physListName << "\"" << G4endl;
#endif
  // string stream to vector will take a single string that contains words
  // delimited by whitespace and split them on the whitespace
  std::stringstream ss(physListName);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> physicsListNamesS(begin, end);

  // convert to G4String for lower case convenience
  std::vector<G4String> physicsListNames;
  for (const auto& physicsListName : physicsListNamesS)
    {
      G4String name = G4String(physicsListName); // convert string to G4String.
      name = BDS::LowerCase(name);
      temporaryName = name; // copy to temporary variable
      
      // search aliases
      auto result = aliasToOriginal.find(name);
      if (result != aliasToOriginal.end())
        {
          G4cout << __METHOD_NAME__ << "alias \"" << name << "\" forwarding to \""
                 << result->second << "\"" << G4endl;
          name = result->second; // overwrite name with the correct one
        }
      physicsListNames.push_back(name);
    }

  // search for em physics (could be any order) - needed for different construction of muon phyiscs
  if (std::find(physicsListNames.begin(), physicsListNames.end(), "em") != physicsListNames.end())
    {emWillBeUsed = true;}

  for (const auto& name : physicsListNames)
    {
      auto result = physicsConstructors.find(name);
      if (result != physicsConstructors.end())
        {
          G4cout << __METHOD_NAME__ << "Constructing \"" << result->first << "\" physics list" << G4endl;
          CheckIncompatiblePhysics(name);
          auto mem = result->second;
          (this->*mem)(); // call the function pointer in this instance of the class
        }
      else
        {
          G4cout << "\"" << name << "\" is not a valid physics list. Available ones are: " << G4endl;
          for (const auto& listName : physicsLists)
            {G4cout << "\"" << listName << "\"" << G4endl;}
          throw BDSException(__METHOD_NAME__, "Invalid physics list.");
        }
    }

  //Always load cuts and limits.
  CutsAndLimits();
}

void BDSModularPhysicsList::ConstructAllLeptons()
{
  if (constructedAllLeptons)
    {return;}
  G4LeptonConstructor::ConstructParticle();
  constructedAllLeptons = true;
}

void BDSModularPhysicsList::ConstructAllShortLived()
{
  if (constructedAllShortLived)
    {return;}
  G4ShortLivedConstructor::ConstructParticle();
  constructedAllShortLived = true;
}

void BDSModularPhysicsList::ConstructAllMesons()
{
  if (constructedAllMesons)
    {return;}
  G4MesonConstructor::ConstructParticle();
  constructedAllMesons = true;
}

void BDSModularPhysicsList::ConstructAllBaryons()
{
  if (constructedAllBaryons)
    {return;}
  G4BaryonConstructor::ConstructParticle();
  constructedAllBaryons = true;
}

void BDSModularPhysicsList::ConstructAllIons()
{
  if (constructedAllIons)
    {return;}
  usingIons = true; // all physics lists that use ions call this function so put this here
  G4GenericIon::GenericIonDefinition();
  G4IonConstructor::ConstructParticle();
  constructedAllIons = true;
}

void BDSModularPhysicsList::ConfigurePhysics()
{
  if (opticalPhysics)
    {ConfigureOptical();}
}

void BDSModularPhysicsList::ConfigureOptical()
{
  G4long maxPhotonsPerStep = globals->MaximumPhotonsPerStep();
#if G4VERSION_NUMBER < 1079
  // cherenkov turned on with optical even if it's not on as separate list
  opticalPhysics->Configure(G4OpticalProcessIndex::kCerenkov, true);
  opticalPhysics->Configure(G4OpticalProcessIndex::kScintillation, true);                                ///< Scintillation process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kAbsorption,    globals->TurnOnOpticalAbsorption());  ///< Absorption process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kRayleigh,      globals->TurnOnRayleighScattering()); ///< Rayleigh scattering process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kMieHG,         globals->TurnOnMieScattering());      ///< Mie scattering process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kBoundary,      globals->TurnOnOpticalSurface());     ///< Boundary process index
  opticalPhysics->Configure(G4OpticalProcessIndex::kWLS,           true);                                ///< Wave Length Shifting process index
  opticalPhysics->SetScintillationYieldFactor(globals->ScintYieldFactor());
  if (maxPhotonsPerStep >= 0)
    {opticalPhysics->SetMaxNumPhotonsPerStep(maxPhotonsPerStep);}
#else
  G4OpticalParameters* opticalParameters = G4OpticalParameters::Instance();
  opticalParameters->SetProcessActivation(G4OpticalProcessName(G4OpticalProcessIndex::kCerenkov), true);
  opticalParameters->SetProcessActivation(G4OpticalProcessName(G4OpticalProcessIndex::kScintillation), true);
  opticalParameters->SetProcessActivation(G4OpticalProcessName(G4OpticalProcessIndex::kAbsorption), globals->TurnOnOpticalAbsorption());
  opticalParameters->SetProcessActivation(G4OpticalProcessName(G4OpticalProcessIndex::kRayleigh), globals->TurnOnRayleighScattering());
  opticalParameters->SetProcessActivation(G4OpticalProcessName(G4OpticalProcessIndex::kMieHG), globals->TurnOnMieScattering());
  opticalParameters->SetProcessActivation(G4OpticalProcessName(G4OpticalProcessIndex::kBoundary), globals->TurnOnOpticalSurface());
  opticalParameters->SetProcessActivation(G4OpticalProcessName(G4OpticalProcessIndex::kWLS), true);
  if (maxPhotonsPerStep >= 0)
    {opticalParameters->SetCerenkovMaxPhotonsPerStep((G4int)maxPhotonsPerStep);}
#endif
}

void BDSModularPhysicsList::CheckIncompatiblePhysics(const G4String& singlePhysicsIn) const
{
  // no need to check if key is present as we control both maps in the constructor
  const std::vector<G4String>& forbidden = incompatible.at(singlePhysicsIn);
  
  for (const auto& key : forbidden)
    {// for each forbidden physics list, check if it's activated
      if (physicsActivated.at(key))
        {
          G4cerr << __METHOD_NAME__ << "Incompatible physics list \"" << singlePhysicsIn
                 << "\" being used with already used \"" << key << "\"" << G4endl;
          G4cout << "\"" << singlePhysicsIn << "\" cannot be used with the following:" << G4endl;
          for (const auto& v : forbidden)
            {G4cout << "\"" << v << "\"" << G4endl;}
          throw BDSException(__METHOD_NAME__, "Incompatible physics list.");
        }
    }
}

void BDSModularPhysicsList::AllParticles()
{
  ConstructAllLeptons();
  ConstructAllShortLived();
  ConstructAllMesons();
  ConstructAllBaryons();
  ConstructAllIons();
}

void BDSModularPhysicsList::AnnihiToMuMu()
{
  if (!physicsActivated["annihi_to_mumu"])
    {
      constructors.push_back(new BDSPhysicsAnnihiToMuMu());
      physicsActivated["annihi_to_mumu"] = true;
    }
}

void BDSModularPhysicsList::ChargeExchange()
{
  ConstructAllIons();
  if (!physicsActivated["charge_exchange"])
    {
      constructors.push_back(new G4ChargeExchangePhysics());
      physicsActivated["charge_exchange"] = true;
    }
}

void BDSModularPhysicsList::Cherenkov()
{
  if (!physicsActivated["cherenkov"])
    {
      constructors.push_back(new BDSPhysicsCherenkov(BDSGlobalConstants::Instance()->MaximumPhotonsPerStep(),
                                                     BDSGlobalConstants::Instance()->MaximumBetaChangePerStep()));
      physicsActivated["cherenkov"] = true;
      if (!physicsActivated["em"])
        {Em();} // requires em physics to work (found empirically)
    }
}

void BDSModularPhysicsList::CutsAndLimits()
{
  if (!physicsActivated["cuts_and_limits"])
    {
      constructors.push_back(new BDSPhysicsCutsAndLimits(BDSGlobalConstants::Instance()->ParticlesToExcludeFromCutsAsSet()));
      physicsActivated["cuts_and_limits"] = true;
    }
}

void BDSModularPhysicsList::Decay()
{
  if (!physicsActivated["decay"])
    {
      constructors.push_back(new G4DecayPhysics());
      physicsActivated["decay"] = true;
    }
}

void BDSModularPhysicsList::DecayRadioactive()
{
  if (!physicsActivated["decay_radioactive"])
    {
      constructors.push_back(new G4RadioactiveDecayPhysics());
      physicsActivated["decay_radioactive"] = true;
    }
}

void BDSModularPhysicsList::Em()
{
  ConstructAllLeptons();
  if (!physicsActivated["em"])
    {
      constructors.push_back(new G4EmStandardPhysics());
      physicsActivated["em"] = true;
    }
}

void BDSModularPhysicsList::EmExtra()
{
  ConstructAllLeptons();

  // These are required by GammaNuclear and MuonNuclear which
  // are activated by default in G4EmExtraPhysics.
  ConstructAllShortLived();
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllMesons();

  if (!physicsActivated["em_extra"])
    {
      auto constructor = new G4EmExtraPhysics();
#if G4VERSION_NUMBER > 1019
      G4bool useMuonNuclear = BDSGlobalConstants::Instance()->UseMuonNuclear();
      constructor->MuonNuclear(useMuonNuclear);
      G4cout << __METHOD_NAME__ << "G4EmExtraPhysics> muon nuclear processes : " << BDS::BoolToString(useMuonNuclear) << G4endl;
#endif
#if G4VERSION_NUMBER > 1029
      G4bool useGammaToMuMu       = BDSGlobalConstants::Instance()->UseGammaToMuMu();
      constructor->GammaToMuMu(useGammaToMuMu);
      G4cout << __METHOD_NAME__ << "G4EmExtraPhysics> gamma to mu mu : " << BDS::BoolToString(useGammaToMuMu) << G4endl;
      G4bool usePositronToMuMu    = BDSGlobalConstants::Instance()->UsePositronToMuMu();
      constructor->PositronToMuMu(usePositronToMuMu);
      G4cout << __METHOD_NAME__ << "G4EmExtraPhysics> e+ to mu mu : " << BDS::BoolToString(usePositronToMuMu) << G4endl;
      G4bool usePositronToHadrons = BDSGlobalConstants::Instance()->UsePositronToHadrons();
      constructor->PositronToHadrons(usePositronToHadrons);
      G4cout << __METHOD_NAME__ << "G4EmExtraPhysics> e+ to hadrons : " << BDS::BoolToString(usePositronToHadrons) << G4endl;
#endif
#if G4VERSION_NUMBER > 1039
      G4bool useLENDGammaNuclear = BDSGlobalConstants::Instance()->UseLENDGammaNuclear();
      if (useLENDGammaNuclear)
        {
          BDS::CheckLowEnergyNeutronDataExists("em_extra");
          constructor->LENDGammaNuclear(true);
          G4cout << __METHOD_NAME__ << "G4EmExtraPhysics> LEND gamma nuclear : " << BDS::BoolToString(useMuonNuclear) << G4endl;
        }
      G4bool useElectroNuclear = BDSGlobalConstants::Instance()->UseElectroNuclear();
      constructor->ElectroNuclear(useElectroNuclear);
#endif
      constructors.push_back(constructor);
      physicsActivated["em_extra"] = true;
    }
}

void BDSModularPhysicsList::EmLivermore()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_livermore"])
    {
      constructors.push_back(new G4EmLivermorePhysics());
      physicsActivated["em_livermore"] = true;
    }
}

void BDSModularPhysicsList::EmLivermorePolarised()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_livermore_polarised"])
    {
      constructors.push_back(new G4EmLivermorePolarizedPhysics());
      physicsActivated["em_livermore_polarised"] = true;
    }
}

void BDSModularPhysicsList::EmLowEP()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_low_ep"])
    {
      constructors.push_back(new G4EmLowEPPhysics());
      physicsActivated["em_low_ep"] = true;
    }
}

void BDSModularPhysicsList::EmPenelope()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_penelope"])
    {
      constructors.push_back(new G4EmPenelopePhysics());
      physicsActivated["em_penelope"] = true;
    }
}

void BDSModularPhysicsList::EmSS()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_ss"])
    {
      constructors.push_back(new G4EmStandardPhysicsSS());
      physicsActivated["em_ss"] = true;
    }
}

void BDSModularPhysicsList::EmWVI()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_wvi"])
    {
      constructors.push_back(new G4EmStandardPhysicsWVI());
      physicsActivated["em_wvi"] = true;
    }
}

void BDSModularPhysicsList::Em1()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_1"])
    {
      constructors.push_back(new G4EmStandardPhysics_option1());
      physicsActivated["em_1"] = true;
    }
}

void BDSModularPhysicsList::Em2()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_2"])
    {
      constructors.push_back(new G4EmStandardPhysics_option2());
      physicsActivated["em_2"] = true;
    }
}

void BDSModularPhysicsList::Em3()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_3"])
    {
      constructors.push_back(new G4EmStandardPhysics_option3());
      physicsActivated["em_3"] = true;
    }
}

void BDSModularPhysicsList::Em4()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_4"])
    {
      constructors.push_back(new G4EmStandardPhysics_option4());
      physicsActivated["em_4"] = true;
    }
}

void BDSModularPhysicsList::FTFPBERT()
{
  ConstructAllLeptons();
  HadronicElastic(); // has to be here to prevent G4 segfault
  if (!physicsActivated["ftfp_bert"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT());
      physicsActivated["ftfp_bert"] = true;
    }
}

void BDSModularPhysicsList::FTFPBERTHP()
{
  ConstructAllLeptons();
  HadronicElastic(); // has to be here to prevent G4 segfault
  if (!physicsActivated["ftfp_bert_hp"])
    {
      constructors.push_back(new G4HadronPhysicsFTFP_BERT_HP());
      physicsActivated["ftfp_bert_hp"] = true;
    }
}

void BDSModularPhysicsList::GammaToMuMu()
{
  if (!physicsActivated["gamma_to_mumu"])
    {
      constructors.push_back(new BDSPhysicsGammaToMuMu());
      physicsActivated["gamma_to_mumu"] = true;
    }
}

void BDSModularPhysicsList::HadronicElastic()
{
  ConstructAllLeptons();
  if (!physicsActivated["hadronic_elastic"])
    {
      constructors.push_back(new G4HadronElasticPhysics());
      physicsActivated["hadronic_elastic"] = true;
    }
}

void BDSModularPhysicsList::HadronicElasticD()
{
  ConstructAllLeptons();
  if (!physicsActivated["hadronic_elastic_d"])
    {
      constructors.push_back(new G4HadronDElasticPhysics());
      physicsActivated["hadronic_elastic_d"] = true;
    }
}

void BDSModularPhysicsList::HadronicElasticH()
{
  ConstructAllLeptons();
  if (!physicsActivated["hadronic_elastic_h"])
    {
      constructors.push_back(new G4HadronHElasticPhysics());
      physicsActivated["hadronic_elastic_h"] = true;
    }
}

void BDSModularPhysicsList::HadronicElasticHP()
{
  ConstructAllLeptons();
  if (!physicsActivated["hadronic_elastic_hp"])
    {
      constructors.push_back(new G4HadronElasticPhysicsHP());
      physicsActivated["hadronic_elastic_hp"] = true;
    }
}

void BDSModularPhysicsList::HadronicElasticLEND()
{
  BDS::CheckLowEnergyNeutronDataExists("hadronic_elastic_lend");
  ConstructAllLeptons();
  if (!physicsActivated["hadronic_elastic_lend"])
    {
      constructors.push_back(new G4HadronElasticPhysicsLEND());
      physicsActivated["hadronic_elastic_lend"] = true;
    }
}

void BDSModularPhysicsList::HadronicElasticXS()
{
  ConstructAllLeptons();
  if (!physicsActivated["hadronic_elastic_xs"])
    {
      constructors.push_back(new G4HadronElasticPhysicsXS());
      physicsActivated["hadronic_elastic_xs"] = true;
    }
}

void BDSModularPhysicsList::Ion()
{
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();

  if (!physicsActivated["ion"])
    {
      constructors.push_back(new G4IonPhysics());
      physicsActivated["ion"] = true;
    }
}

void BDSModularPhysicsList::IonBinary()
{
  BDS::CheckHighPrecisionDataExists("ion_binary");
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ion_binary"])
    {
      constructors.push_back(new G4IonBinaryCascadePhysics());
      physicsActivated["ion_binary"] = true;
    }
}


void BDSModularPhysicsList::IonElastic()
{
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ion_elastic"])
    {
      constructors.push_back(new G4IonElasticPhysics());
      physicsActivated["ion_elastic"] = true;
    }
}


void BDSModularPhysicsList::IonElasticQMD()
{
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ion_elastic_qmd"])
    {
      constructors.push_back(new G4IonQMDPhysics());
      physicsActivated["ion_elastic_qmd"] = true;
    }
}

void BDSModularPhysicsList::IonEMDissociation()
{
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  if (!physicsActivated["ion_em_dissociation"])
    {
      constructors.push_back(new BDSPhysicsEMDissociation());
      physicsActivated["ion_em_dissociation"] = true;
    }
}

void BDSModularPhysicsList::IonINCLXX()
{
  BDS::CheckHighPrecisionDataExists("ion_inclxx");
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ion_inclxx"])
    {
      constructors.push_back(new G4IonINCLXXPhysics());
      physicsActivated["ion_inclxx"] = true;
    }
}

void BDSModularPhysicsList::Ionisation()
{
  if (!physicsActivated["ionisation"])
    {
      constructors.push_back(new BDSPhysicsIonisation());
      physicsActivated["ionisation"] = true;
    }
}

void BDSModularPhysicsList::LaserWire()
{
  if (!physicsActivated["lw"])
    {
      constructors.push_back(new BDSPhysicsLaserWire());
      physicsActivated["lw"] = true;
    }
}

void BDSModularPhysicsList::LaserPhotoDetachment()
{
  if(!physicsActivated["laser_photo_detachment"])
  {
    constructors.push_back(new BDSPhysicsLaserPhotoDetachment());
    physicsActivated["laser_photo_detachment"] = true;
  }
}

void BDSModularPhysicsList::LaserComptonScattering()
{
  if(!physicsActivated["laser_compton_scattering"])
  {
    constructors.push_back(new BDSPhysicsLaserComptonScattering());
    physicsActivated["laser_compton_scattering"] = true;
  }
}

void BDSModularPhysicsList::LaserCumulativePhotoDetachment()
{
  if(!physicsActivated["laser_cumulative_photo_detachment"])
  {
    constructors.push_back(new BDSPhysicsLaserCumulativePhotodetachment());
    physicsActivated["laser_cumulative_photo_detachment"] = true;
  }
}

void BDSModularPhysicsList::LaserCumulativeCompton()
{
  if(!physicsActivated["laser_cumulative_compton_scattering"])
  {
    constructors.push_back(new BDSPhysicsLaserCumulativeCompton());
    physicsActivated["laser_cumulative_compton_scattering"] = true;
  }
}

void BDSModularPhysicsList::LaserIonExcitation()
{
  if(!physicsActivated["laser_ion_excitation"])
  {
    constructors.push_back(new BDSPhysicsLaserIonExcitation());
    physicsActivated["laser_ion_excitation"] = true;
  }
}

void BDSModularPhysicsList::Muon()
{
  if (!physicsActivated["muon"])
    {
      constructors.push_back(new BDSPhysicsMuon(emWillBeUsed));
      physicsActivated["muon"] = true;
    }
}

void BDSModularPhysicsList::MuonInelastic()
{
  if (!physicsActivated["muon_inelastic"])
    {
      constructors.push_back(new BDSPhysicsMuonInelastic());
      physicsActivated["muon_inelastic"] = true;
    }
}

void BDSModularPhysicsList::NeutronTrackingCut()
{
  if (!physicsActivated["neutron_tracking_cut"])
    {
      auto ntc = new G4NeutronTrackingCut();
      G4double timeLimit = BDSGlobalConstants::Instance()->NeutronTimeLimit();
      G4double eKinLimit = BDSGlobalConstants::Instance()->NeutronKineticEnergyLimit();
      G4cout << __METHOD_NAME__ << "Neutron time limit: " << timeLimit / CLHEP::s << " s" << G4endl;
      G4cout << __METHOD_NAME__ << "Neutron kinetic energy limit: " << eKinLimit / CLHEP::MeV << G4endl;
      ntc->SetTimeLimit(timeLimit);
      ntc->SetKineticEnergyLimit(eKinLimit);
      constructors.push_back(ntc);
      physicsActivated["neutron_tracking_cut"] = true;
    }
}

void BDSModularPhysicsList::Optical()
{
  if (!physicsActivated["optical"])
    {
      opticalPhysics = new G4OpticalPhysics();
      constructors.push_back(opticalPhysics);
      physicsActivated["optical"] = true;
    }
}

void BDSModularPhysicsList::QGSPBERT()
{
  ConstructAllLeptons();
  if (!physicsActivated["qgsp_bert"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BERT());
      physicsActivated["qgsp_bert"] = true;
    }
}

void BDSModularPhysicsList::QGSPBERTHP()
{
  ConstructAllLeptons();
  if (!physicsActivated["qgsp_bert_hp"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BERT_HP());
      physicsActivated["qgsp_bert_hp"] = true;
    }
}

void BDSModularPhysicsList::QGSPBIC()
{
  ConstructAllLeptons();
  if (!physicsActivated["qgsp_bic"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BIC());
      physicsActivated["qgsp_bic"] = true;
    }
}

void BDSModularPhysicsList::QGSPBICHP()
{
  ConstructAllLeptons();
  if (!physicsActivated["qgsp_bic_hp"])
    {
      constructors.push_back(new G4HadronPhysicsQGSP_BIC_HP());
      physicsActivated["qgsp_bic_hp"] = true;
    }
}

void BDSModularPhysicsList::Shielding()
{
#if G4VERSION_NUMBER > 1059
  AllParticles();
#endif
  if (!physicsActivated["shielding"])
    {
      constructors.push_back(new G4HadronPhysicsShielding());
      physicsActivated["shielding"] = true;
    }
}

void BDSModularPhysicsList::Stopping()
{
  ConstructAllShortLived();
  ConstructAllIons();
  if (!physicsActivated["stopping"])
    {
      constructors.push_back(new G4StoppingPhysics());
      physicsActivated["stopping"] = true;
    }
}

void BDSModularPhysicsList::SynchRad()
{
  ConstructAllLeptons();
  if (!physicsActivated["synch_rad"])
    {
      constructors.push_back(new BDSPhysicsSynchRad());
      physicsActivated["synch_rad"] = true;
    }
}

#if G4VERSION_NUMBER > 1019
void BDSModularPhysicsList::EmGS()
{
  ConstructAllLeptons();
  if (!physicsActivated["em_gs"])
    {
      constructors.push_back(new G4EmStandardPhysicsGS());
      physicsActivated["em_gs"] = true;
    }
}
#endif

#if G4VERSION_NUMBER > 1020
void BDSModularPhysicsList::DecaySpin()
{
  if (!physicsActivated["decay_spin"])
    {// this will replace regular decay for various processes
      constructors.push_back(new G4SpinDecayPhysics());
      physicsActivated["decay_spin"] = true;
    }
}
#endif

#if G4VERSION_NUMBER > 1022
void BDSModularPhysicsList::IonPHP()
{
  BDS::CheckHighPrecisionDataExists("ion_php");
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllShortLived();
  
  if (!physicsActivated["ion_php"])
    {
      constructors.push_back(new G4IonPhysicsPHP());
      physicsActivated["ion_php"] = true;
    }
}
#endif

#if G4VERSION_NUMBER > 1029
void BDSModularPhysicsList::DecayMuonicAtom()
{
  ConstructAllLeptons();
#if G4VERSION_NUMBER > 1059
  ConstructAllIons();
#endif
  if (!physicsActivated["decay_muonic_atom"])
    {
      constructors.push_back(new G4MuonicAtomDecayPhysics());
      physicsActivated["decay_muonic_atom"] = true;
    }
}
#endif

#if G4VERSION_NUMBER > 1039
void BDSModularPhysicsList::DNA()
{
  if (!physicsActivated["dna"])
    {
      // only one DNA physics list possible
      if (BDS::StrContains(temporaryName, "option"))
        {
          if (BDS::StrContains(temporaryName, "1"))
            {constructors.push_back(new G4EmDNAPhysics_option1());}
          if (BDS::StrContains(temporaryName, "2"))
            {constructors.push_back(new G4EmDNAPhysics_option2());}
          if (BDS::StrContains(temporaryName, "3"))
            {constructors.push_back(new G4EmDNAPhysics_option3());}
          if (BDS::StrContains(temporaryName, "4"))
            {constructors.push_back(new G4EmDNAPhysics_option4());}
          if (BDS::StrContains(temporaryName, "5"))
            {constructors.push_back(new G4EmDNAPhysics_option5());}
          if (BDS::StrContains(temporaryName, "6"))
            {constructors.push_back(new G4EmDNAPhysics_option6());}
          if (BDS::StrContains(temporaryName, "7"))
            {constructors.push_back(new G4EmDNAPhysics_option7());}
        }
      else
        {constructors.push_back(new G4EmDNAPhysics());}
      
      physicsActivated["dna"] = true;
    }
}


void BDSModularPhysicsList::Channelling()
{
  if (!physicsActivated["channelling"])
    {
      G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();
      biasingPhysics->PhysicsBiasAllCharged();
      RegisterPhysics(biasingPhysics);
      constructors.push_back(new BDSPhysicsChannelling());
      physicsActivated["channelling"] = true;
    }
}

void BDSModularPhysicsList::Radioactivation()
{
  if (!physicsActivated["radioactivation"])
  {
    constructors.push_back(new BDSPhysicsRadioactivation());
    physicsActivated["radioactivation"] = true;
  }
}

void BDSModularPhysicsList::ShieldingLEND()
{
  BDS::CheckLowEnergyNeutronDataExists("shielding_lend");
  if (!physicsActivated["shielding_lend"])
    {
      constructors.push_back(new G4HadronPhysicsShieldingLEND());
      physicsActivated["shielding_lend"] = true;
    }
}
#endif

#if G4VERSION_NUMBER > 1119
void BDSModularPhysicsList::XrayReflection()
{
  if (!physicsActivated["xray_relfection"])
    {
      G4double sr = BDSGlobalConstants::Instance()->XrayAllSurfaceRoughness();
      constructors.push_back(new BDSPhysicsXrayReflection(sr));
      physicsActivated["xray_relfection"] = true;
    }
}
#endif
