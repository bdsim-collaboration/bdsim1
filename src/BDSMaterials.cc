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
#include "BDSIdealGas.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "G4MaterialTable.hh"
#include "G4String.hh"
#include "G4NistManager.hh"
#include "G4Version.hh"

#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <vector>

BDSMaterials* BDSMaterials::instance = nullptr;

BDSMaterials* BDSMaterials::Instance()
{
  if (!instance)
    {instance = new BDSMaterials();}
  return instance;
}

BDSMaterials::BDSMaterials()
{
#ifdef BDSDEBUG
  G4cout << "BDSMaterials: G4 predefined units: " << G4endl;
  G4cout << "g= "      << CLHEP::g << G4endl;
  G4cout << "m= "      << CLHEP::m << G4endl;
  G4cout << "mole= "   << CLHEP::mole << G4endl;
  G4cout << "kelvin= " << CLHEP::kelvin << G4endl;
#endif

  DefineMetals();
  DefineSuperconductors();
  DefineNonMetalSolids();
  DefineScintillators();
  DefineLHCComponents();
  DefineLiquids();
  DefineGases();
  DefinePlasmas();
  DefineVacuums();
}

void BDSMaterials::DefineMetals()
{
  //default Geant4 temperature = 293.15 K (NTP_Temperature)
  //default Geant4 pressure = 1atm
#if G4VERSION_NUMBER < 1011
  G4double NTP_Temperature = 293.15;
#endif

  // solid materials
  // metals
  // standard single element metals
  AddExistingMaterialAlias("Al", "aluminium");
  AddExistingMaterialAlias("Be", "beryllium");
  AddExistingMaterialAlias("C",  "carbon");
  AddExistingMaterialAlias("Cr", "chromium");
  AddExistingMaterialAlias("Cu", "copper");
  AddExistingMaterialAlias("Fe", "iron");
  AddExistingMaterialAlias("Pb", "lead");
  AddExistingMaterialAlias("Mg", "magnesium");
  AddExistingMaterialAlias("Ni", "nickel");
  AddExistingMaterialAlias("Si", "silicon");
  AddExistingMaterialAlias("Ti", "titanium");
  AddExistingMaterialAlias("W",  "tungsten");
  AddExistingMaterialAlias("U",  "uranium");
  AddExistingMaterialAlias("V",  "vanadium");
  AddExistingMaterialAlias("Zn", "zinc");

  // special forms
  std::list<int> singleElement = {1};
  AddMaterial("graphite",     2.265, kStateSolid, NTP_Temperature, 1, {"C"}, singleElement);
  AddMaterial("graphitefoam", 0.61,  kStateSolid, NTP_Temperature, 1, {"C"}, singleElement);
  AddMaterial("solidhydrogen",8.96,  kStateSolid, NTP_Temperature, 1, {"H"}, singleElement);
  AddMaterial("solidnitrogen",8.96,  kStateSolid, NTP_Temperature, 1, {"N"}, singleElement);
  AddMaterial("solidoxygen",  8.96,  kStateSolid, NTP_Temperature, 1, {"O"}, singleElement);
  AddMaterial("weightiron",   7.87,  kStateSolid, NTP_Temperature, 1, {"Fe"}, singleElement);

  // composites and alloys
  AddMaterial("leadtungstate" ,
	      8.27,
	      kStateSolid, NTP_Temperature, 1,
              {"Pb","W" ,"O" },
	      std::list<int>{1, 1, 4});
  
  AddMaterial("smco",
	      8.4,
	      kStateSolid, 300, 1,
              {"Sm","Co"},
	      std::list<double>{0.338, 0.662});

  // Titanium alloy (BDS collimator material SLAC-TN-03-069 p25) deacon 15 Jun 2007
  AddMaterial("titaniumalloy",
	      4.48,
	      kStateSolid, 300, 1,
              {"V" ,"Al","Ti"},
	      std::list<double>{0.025, 0.03, 0.945});

  // Carbon Steel (shell of cryomodule). LDeacon 21 Feb 2006
  AddMaterial("carbonsteel",
	      7.87,
	      kStateSolid, 100, 1,
              {"C","Mn","P","S","Fe"},
	      std::list<double>{0.0017, 0.0045, 0.0004, 0.0005, 0.9929});

  // Copper Alloy 17410 (C17410) "Beryllium Copper"
  AddMaterial("berylliumcopper",
	      8.8,
	      kStateSolid, 300, 1,
	      {"Cu", "Be", "Co", "Al", "Fe", "Ni"},
	      std::list<double>{0.991, 0.0031, 0.00500, 0.0004, 0.0003, 0.0002});

  // Stainless Steel 316L
  AddMaterial("stainlesssteel",
	      8.0,
	      kStateSolid, 295, 1,
              {"C","Mn","Si","P","S","Cr","Mo","Ni","N","Fe"},
	      std::list<double>{0.0003, 0.02, 0.0075, 0.00045, 0.0003, 0.17, 0.025, 0.12, 0.001, 0.65545});

  // Stainless Steel AISI code 304L (low-carbon) @ 300K
  AddMaterial("stainless_steel_304L",
	      8.02,
	      kStateSolid, 300, 1,
	      {"Fe", "Cr", "Ni", "Mn", "Si", "P", "S", "C"},
	      std::list<double>{0.67145, 0.185, 0.1125, 0.02, 0.01, 0.00045, 0.0003, 0.0003});

  // Stainless Steel AISI code 304L (low-carbon) @ 87K
  AddMaterial("stainless_steel_304L_87K",
	      8.02,
	      kStateSolid, 87, 1,
	      {"Fe", "Cr", "Ni", "Mn", "Si", "P", "S", "C"},
	      std::list<double>{0.67145, 0.185, 0.1125, 0.02, 0.01, 0.00045, 0.0003, 0.0003});
  
  // Stainless Steel AISI code 304L (low-carbon) @ 2K
  AddMaterial("stainless_steel_304L_2K",
              8.02,
              kStateSolid, 2, 1,
              {"Fe", "Cr", "Ni", "Mn", "Si", "P", "S", "C"},
              std::list<double>{0.67145, 0.185, 0.1125, 0.02, 0.01, 0.00045, 0.0003, 0.0003});

  // Stainless Steel AISI code 316LN
  // (Type 316, low carbon, nitrogen-enhanced) @ 300K
  AddMaterial("stainless_steel_316LN",
	      8.03,
	      kStateSolid, 300, 1,
	      {"Fe", "Cr", "Ni", "Mo", "Mn", "Si", "Ti", "N", "Nb",
	       "Cu", "Co", "P", "C", "S", "Ta", "B"},
	      std::list<double>{0.65093, 0.1700, 0.12000, 0.02500, 0.0200, 0.00750,
				  0.00150, 0.0014, 0.00100, 0.00100, 0.0005, 0.00045,
				  0.00030, 0.0003, 0.00010, 0.00002});

 // Stainless Steel AISI code 316LN
 // (Type 316, low-carbon nitrogen-enhanced) @ 87K
  AddMaterial("stainless_steel_316LN_87K",
	      8.03,
	      kStateSolid, 87, 1,
	      {"Fe", "Cr", "Ni", "Mo", "Mn", "Si", "Ti", "N",
	       "Nb", "Cu", "Co", "P", "C", "S", "Ta", "B"},
	      std::list<double>{0.65093, 0.1700, 0.12000, 0.02500, 0.0200,
				  0.00750, 0.00150, 0.0014, 0.00100, 0.00100,
				  0.0005, 0.00045, 0.00030, 0.0003, 0.00010,
				  0.00002});
  
  // Stainless Steel AISI code 316LN
  // (Type 316, low-carbon nitrogen-enhanced) @ 2K
  AddMaterial("stainless_steel_316LN_2K",
              8.03,
              kStateSolid, 2, 1,
              {"Fe", "Cr", "Ni", "Mo", "Mn", "Si", "Ti", "N",
               "Nb", "Cu", "Co", "P", "C", "S", "Ta", "B"},
              std::list<double>{0.65093, 0.1700, 0.12000, 0.02500, 0.0200,
                                0.00750, 0.00150, 0.0014, 0.00100, 0.00100,
                                0.0005, 0.00045, 0.00030, 0.0003, 0.00010,
                                0.00002});
  
  
  // Mild Steel
  AddMaterial("mild_steel",   8.000, kStateSolid, 295, 1,
	      {"C", "Mn", "Si", "Fe"},
	      std::list<double>{0.002, 0.005, 0.0015, 0.99150});
  
  // Pure tungsten is not typically used, but instead part of "heavy
  // alloy."  I think "heavy" in the sense that the tungsten makes up almost
  // all of the composition, and tungsten is a very dense metal.
  AddMaterial("tungsten_heavy_alloy",
	      18.5,
	      kStateSolid, 87, 1,
      {"W", "Ni", "Fe"},
	      std::list<double>{0.97, 0.02, 0.01});
  
  // For HL-LHC Collimation - Inermet170
  AddMaterial("inermet170",
              17.0,
              kStateSolid, 300, 1,
              {"W", "Ni", "Cu"},
              std::list<double>{0.90, 0.05, 0.05});
  
  // For HL-LHC Collimation - Inermet176
  AddMaterial("inermet176",
              17.6,
              kStateSolid, 300, 1,
              {"W", "Ni", "Cu"},
              std::list<double>{0.925, 0.0375, 0.0375});
  
  // For HL-LHC Collimation - Inermet180
  AddMaterial("inermet180",
              18.0,
              kStateSolid, 300, 1,
              {"W", "Ni", "Cu"},
              std::list<double>{0.95, 0.025, 0.025});
}

void BDSMaterials::DefineSuperconductors()
{
  // niobium at 2K
  AddMaterial("niobium_2k", 8.57 , kStateSolid, 2, 1, {"Nb"}, std::list<int>{1});
  AddExistingMaterialAlias("niobium_2k", "nb_2k"); // alias
  // niobium titanium at 4K
  AddMaterial("nbti_4k", 5.6  , kStateSolid, 4, 1, {"Nb","Ti"}, std::list<int>{1,1});
}

void BDSMaterials::DefineNonMetalSolids()
{
  // boron Nickel (absorber)
  AddMaterial("bn5000",
	      1.925,
	      kStateSolid, 300, 1,
	      {"B","Ni","O","Ca","Si"}, 
	      std::list<double>{0.383249242, 0.472071387, 0.0366276887, 0.0228923054, 0.0851593762});

  // calcium carbonate (calcite)
  AddMaterial("calciumCarbonate",
	      2.711,
	      kStateSolid, 300, 1,
	      {"Ca","O","C"},
	      std::list<int>{1,3,1});

  // clay
  AddMaterial("clay",
	      1.746,
	      kStateSolid, 300, 1,
	      {"Al","O","Si","H"},
	      std::list<int>{1,9,2,4});

  AddMaterial("concrete",
	      2.3,
	      kStateSolid, 300, 1, {"Si","O","H","Ca","Al","Fe"},
	      std::list<double>{0.227915, 0.60541, 0.09972, 0.04986, 0.014245, 0.00285});

  // we make the material manually here so we have a pointer to it. note density units must
  // be used here.
  G4Material* tmpMaterial = new G4Material("fusedsilica",
					   1.032*CLHEP::g/CLHEP::cm3, // density
					   2,
					   kStateSolid);
  tmpMaterial->AddElement(GetElement("O") , 2);
  tmpMaterial->AddElement(GetElement("Si"), 1);
  const G4int FusedSilica_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double FusedSilica_RIND[FusedSilica_NUMENTRIES]={1.49,1.49,1.49};
  G4double FusedSilica_AbsLength[FusedSilica_NUMENTRIES]={420.*CLHEP::cm,420.*CLHEP::cm,420.*CLHEP::cm};
  G4double FusedSilica_Energy[FusedSilica_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  G4MaterialPropertiesTable* fsMaterialPropertiesTable = CreatePropertiesTable();
  fsMaterialPropertiesTable->AddProperty("ABSLENGTH",FusedSilica_Energy,FusedSilica_AbsLength,FusedSilica_NUMENTRIES);
  fsMaterialPropertiesTable->AddProperty("RINDEX",FusedSilica_Energy,FusedSilica_RIND,FusedSilica_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(fsMaterialPropertiesTable);
  AddMaterial(tmpMaterial, "fusedsilica");

  // perspex
  AddMaterial("perspex",
	      1.18,
	      kStateSolid,
	      300,
	      1,
	      {"C","O","H"}, 
	      std::list<double>{0.59984,0.31961,0.08055});

  // invar - Temperature 2 kelvin. LDeacon 6th Feburary 2006
  AddMaterial("invar" ,
	      8.1 ,
	      kStateSolid, 2, 1,
	      {"Ni","Fe"},
	      std::list<double>{0.35,0.65});

  // kapton polyimide film
  AddMaterial("kapton",
	      1.42,
	      kStateSolid, 295, 1,
	      {"H","C","N","O"}, 
	      std::list<double>{0.026362,0.691133,0.073270,0.209235});

  // n-bk7
  tmpMaterial = new G4Material("n-bk7", 1.032*CLHEP::g/CLHEP::cm3, 2, kStateSolid);
  tmpMaterial->AddElement(GetElement("O") , 2);
  tmpMaterial->AddElement(GetElement("Si"), 1);
  const G4int N_Bk7_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double N_Bk7_RIND[N_Bk7_NUMENTRIES]={1.51680,1.51680,1.51680};
  G4double N_Bk7_AbsLength[N_Bk7_NUMENTRIES]={420.*CLHEP::cm,420.*CLHEP::cm,420.*CLHEP::cm};
  G4double N_Bk7_Energy[N_Bk7_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  G4MaterialPropertiesTable* nbk7MaterialPropertiesTable = CreatePropertiesTable();
  nbk7MaterialPropertiesTable->AddProperty("ABSLENGTH",N_Bk7_Energy,N_Bk7_AbsLength,N_Bk7_NUMENTRIES);
  nbk7MaterialPropertiesTable->AddProperty("RINDEX",N_Bk7_Energy,N_Bk7_RIND,N_Bk7_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(nbk7MaterialPropertiesTable);
  AddMaterial(tmpMaterial, "n-bk7");

  // quartz
  AddMaterial("quartz", 2.655, kStateSolid, 300, 1, {"Si","O"}, std::list<int>{1,2});

  // marl
  AddMaterial("marl",
	      1.474,
	      kStateSolid, 300, 1,
	      {"clay","calciumCarbonate"},
	      std::list<double>{0.5,0.5});

  // clayousMarl
  AddMaterial("clayousMarl",
	      1.555,
	      kStateSolid, 300, 1,
	      {"clay","calciumCarbonate"},
	      std::list<double>{0.65,0.35});

  // limousMarl
  AddMaterial("limousMarl",
	      1.392,
	      kStateSolid, 300, 1,
	      {"clay", "calciumCarbonate"},
	      std::list<double>{0.35,0.65});

  // "standard" soil (dry)
  AddMaterial("soil",
	      1.9,
	      kStateSolid, 300, 1,
	      {"Si","O","H","Al"},
	      std::list<double>{0.33377483443708611, 0.57218543046357617, 0.022516556291390728, 0.071523178807947022});

  // epoxy Resin components
  // the main component of epoxy resin commonly used to insulate magnet coils.
  AddMaterial("aralditef", 1.175, kStateSolid, 300, 1, {"C","H","O"},std::list<int>{12,18,4});

  // a hardener for the epoxy resin
  AddMaterial("hy906", 1.225, kStateSolid, 300, 1, {"C","H","O"},std::list<int>{10,5,3});

  // an accelerator for epoxy resin
  AddMaterial("dy061", 1.025, kStateSolid, 300, 1, {"C","H","O","N"},std::list<int>{15,25,1,3});

  // material type 3 from CERN 81-05, "The Selection and Properties of Epoxide Resins
  // Used for the Insulation of Magnet Systems in Radiation Environments".
  AddMaterial("epoxyresin3",
	      1.20,
	      kStateSolid, 300, 1,
	      {"aralditef","hy906","dy061"},
	      std::list<double>{0.497512,0.497512,0.004976});

  // cellulose
  tmpMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_CELLULOSE_CELLOPHANE",true,true);
  const G4int Cellulose_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Cellulose_RIND[Cellulose_NUMENTRIES] = {1.532,1.532,1.532};//Assume constant refractive index.
  G4double Cellulose_Energy[Cellulose_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4MaterialPropertiesTable* celluloseMaterialPropertiesTable = CreatePropertiesTable();
  celluloseMaterialPropertiesTable->AddProperty("RINDEX",Cellulose_Energy, Cellulose_RIND, Cellulose_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(celluloseMaterialPropertiesTable);
  AddMaterial(tmpMaterial, "cellulose");

  // polyurethane
  AddMaterial("polyurethane",
	      1.05,
	      kStateSolid, NTP_Temperature, 1,
	      {"C","H","N","O"},
	      std::list<int>{6,10,2,4});
  
  // RCH 1000 - Ultra high molecular weight polyethylene [PE-UHMW]
  // at 4K for LHC dipoles
  AddMaterial("rch1000_4k",
              0.925,
              kStateSolid, 4, 1,
              {"C", "H"},
              std::list<int>{2,4});
}

void BDSMaterials::DefineScintillators()
{
  //YAG
  G4Material* tmpMaterial = new G4Material("yag", 4.56*CLHEP::g/CLHEP::cm3, 3);
  tmpMaterial->AddElement(GetElement("Y") , 3);
  tmpMaterial->AddElement(GetElement("Al"), 5);
  tmpMaterial->AddElement(GetElement("O") , 12);
  G4double birks = 0.08*CLHEP::mm/CLHEP::MeV; 
  tmpMaterial->GetIonisation()->SetBirksConstant(birks);
  G4MaterialPropertiesTable* mpt_YAG = CreatePropertiesTable();
  //  const int nEntries3=60;
  const G4int nEntries = 9;
  G4double PhotonEnergyYAG[nEntries];
  G4double dNEntries2=(G4double)nEntries;
  G4double energyMin=1.91*CLHEP::eV;
  G4double energyMax=2.76*CLHEP::eV;
  G4double deltaEnergy=(energyMax-energyMin)/(dNEntries2-1.0);
  G4double energy=energyMin;
  for (G4int i=0; i<nEntries; energy += deltaEnergy, i++)
    {PhotonEnergyYAG[i]=energy;}
    
  G4double RefractiveIndexYAG[nEntries] = //Approximately correct, but check for different wavelengths
    { 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82,
      1.82, 1.82 };
  
  mpt_YAG->AddProperty("RINDEX",PhotonEnergyYAG, RefractiveIndexYAG, nEntries);
#if G4VERSION_NUMBER < 1079
  G4double scintFastYAG[nEntries] = //Approximately correct
    { 0, 0.25, 2.0, 14.0, 13.0, 7.0, 4.0, 2.0, 0.0 };
  // All of these parameters are deprecated in V11 onwards and will cause a crash
  mpt_YAG->AddProperty("FASTCOMPONENT",PhotonEnergyYAG, scintFastYAG, nEntries)->SetSpline(true);
  mpt_YAG->AddConstProperty("FASTTIMECONSTANT",70.*CLHEP::ns); //Approximately correct
  mpt_YAG->AddConstProperty("YIELDRATIO",1.0);
#endif
  mpt_YAG->AddConstProperty("SCINTILLATIONYIELD",8000./CLHEP::MeV); //Approximately correct
  mpt_YAG->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  tmpMaterial->SetMaterialPropertiesTable(mpt_YAG);
  AddMaterial(tmpMaterial, "yag");
 
  //UPS-923A  - see http://www.amcrys-h.com/
  //Define the material properties (copy from NIST table of materials).
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* polystyrene = nistManager->FindOrBuildMaterial("G4_POLYSTYRENE",true,true);
  tmpMaterial = new G4Material("ups923a",polystyrene->GetDensity(),1);
  tmpMaterial->AddMaterial(polystyrene,1);
  tmpMaterial->SetName("ups923a");
  std::vector<G4double> ups923a_PhotonEnergy = {
    3.35,    3.31,    3.28,    3.26,    3.25,    3.23,    3.23,
    3.22,    3.21,    3.19,    3.18,    3.17,    3.16,    3.15,
    3.14,    3.14,    3.13,    3.11,    3.1,     3.09,    3.09,
    3.08,    3.07,    3.04,    3.02,    3.02,    3.01,    2.99,
    2.98,    2.97,    2.97,    2.95,    2.95,    2.93,    2.93,
    2.92,    2.92,    2.91,    2.89,    2.88,    2.87,    2.86,
    2.85,    2.83,    2.81,    2.8,     2.79,    2.78,    2.76,
    2.74,    2.72,    2.71,    2.68,    2.66,    2.64,    2.62,
    2.61,    2.58,    2.55,    2.53,    2.5,     2.48,    2.46,
    2.44,    2.41,    2.38,    2.35  };
  std::reverse(ups923a_PhotonEnergy.begin(), ups923a_PhotonEnergy.end());
  
  // AUG 21 - these were previously just one number as a const property but they should be non-const
  // which requires vs energy numbers - so just use arrays the same shape
  std::vector<G4double> ups923a_RINDEX(ups923a_PhotonEnergy.size(), 1.52);
  std::vector<G4double> ups923a_ABSLENGTH(ups923a_PhotonEnergy.size(), 1*CLHEP::m);
  
  G4MaterialPropertiesTable* ups923a_mt = CreatePropertiesTable();
#if G4VERSION_NUMBER < 1070
  ups923a_mt->AddProperty("RINDEX",    ups923a_PhotonEnergy.data(), ups923a_RINDEX.data(),    (int)ups923a_PhotonEnergy.size());
  ups923a_mt->AddProperty("ABSLENGTH", ups923a_PhotonEnergy.data(), ups923a_ABSLENGTH.data(), (int)ups923a_PhotonEnergy.size());
#else
  ups923a_mt->AddProperty("RINDEX",    ups923a_PhotonEnergy, ups923a_RINDEX);
  ups923a_mt->AddProperty("ABSLENGTH", ups923a_PhotonEnergy, ups923a_ABSLENGTH);
#endif
  //Birk's constant
  birks = (0.014/1.06)*CLHEP::cm/CLHEP::MeV;
  tmpMaterial->GetIonisation()->SetBirksConstant(birks);
#if G4VERSION_NUMBER < 1079
  const G4int ups923a_numentries = 67;
  G4double ups923a_emission[ups923a_numentries]   = {
    0,       0.04,    0.11,    0.2,     0.3,     0.4,     0.52,
    0.62,    0.67,    0.68,    0.67,    0.62,    0.53,    0.48,
    0.44,    0.42,    0.4,     0.41,    0.42,    0.51,    0.46,
    0.57,    0.67,    0.78,    0.91,    0.93,    0.95,    0.96,
    0.94,    0.91,    0.85,    0.76,    0.67,    0.61,    0.57,
    0.55,    0.52,    0.51,    0.52,    0.54,    0.57,    0.58,
    0.6,     0.6,     0.59,    0.58,    0.55,    0.48,    0.42,
    0.37,    0.33,    0.31,    0.29,    0.28,    0.26,    0.24,
    0.2,     0.17,    0.12,    0.09,    0.08,    0.07,
    0.06,    0.04,    0.02,    0.01,    0.01  };
  ups923a_mt->AddConstProperty("FASTTIMECONSTANT",3.3*CLHEP::ns);
  ups923a_mt->AddProperty("FASTCOMPONENT",ups923a_PhotonEnergy.data(), ups923a_emission, ups923a_numentries)->SetSpline(true);
  ups923a_mt->AddConstProperty("YIELDRATIO",1.0);
#endif
  ups923a_mt->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  G4double scintYieldAnthracene=14200; //Anthracene yield per 1 CLHEP::MeV
  G4double scintYieldUPS923A=scintYieldAnthracene*0.60;//60% of anthracene
  ups923a_mt->AddConstProperty("SCINTILLATIONYIELD",scintYieldUPS923A/CLHEP::MeV);

  tmpMaterial->SetMaterialPropertiesTable(ups923a_mt);
  AddMaterial(tmpMaterial, "ups923a");

  // PET 
  G4double pet_density=1.4*CLHEP::g/CLHEP::cm3;
  G4int pet_nelements=3;
  G4State pet_state=kStateSolid;
  tmpMaterial= new G4Material("pet",
			      pet_density,
			      pet_nelements,
			      pet_state);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("C",true),10);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("H",true),8);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("O",true),4);
  const G4int Pet_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Pet_RIND[Pet_NUMENTRIES] = {1.570,1.570,1.570};//Assume constant refractive index.
  G4double Pet_Energy[Pet_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4MaterialPropertiesTable* petMaterialPropertiesTable = CreatePropertiesTable();
  petMaterialPropertiesTable->AddProperty("RINDEX",Pet_Energy, Pet_RIND, Pet_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(petMaterialPropertiesTable);
  AddMaterial(tmpMaterial, "pet");

  // Opaque PET (Dacron)
  tmpMaterial= new G4Material("pet_opaque",
			      pet_density,
			      pet_nelements,
			      pet_state);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("C",true),10);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("H",true),8);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("O",true),4);
  const G4int Pet_Opaque_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Pet_Opaque_RIND[Pet_Opaque_NUMENTRIES] = {1.570,1.570,1.570};//Assume constant refractive index.
  G4double Pet_Opaque_Energy[Pet_Opaque_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4double Pet_Opaque_abslen[]={1*CLHEP::um, 1*CLHEP::um, 1*CLHEP::um};
  G4MaterialPropertiesTable* pet_opaqueMaterialPropertiesTable = CreatePropertiesTable();
  pet_opaqueMaterialPropertiesTable->AddProperty("RINDEX",Pet_Opaque_Energy, Pet_Opaque_RIND, Pet_Opaque_NUMENTRIES);
  pet_opaqueMaterialPropertiesTable->AddProperty("ABSLENGTH",Pet_Opaque_Energy, Pet_Opaque_abslen, Pet_Opaque_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(pet_opaqueMaterialPropertiesTable);
  AddMaterial(tmpMaterial, "pet_opaque");

  // Gadolinium oxysulphate Gd_2 O_2 S
  G4Material* GOS = nistManager->FindOrBuildMaterial("G4_GADOLINIUM_OXYSULFIDE",true,true);

  // Ganolinium oxysulphate in a polyurethane elastomer (lanex)
  G4double fill_factor=0.5;
  G4double lanex_density=fill_factor*GOS->GetDensity()+(1-fill_factor)*GetMaterial("polyurethane")->GetDensity();
  G4double gos_fraction_by_mass=fill_factor*GOS->GetDensity()/lanex_density;
  G4double pur_fraction_by_mass=1-gos_fraction_by_mass;

  tmpMaterial = new G4Material("lanex", lanex_density, 2);
  tmpMaterial->AddMaterial(GOS, gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), pur_fraction_by_mass);
  
  G4MaterialPropertiesTable* mptLanex = CreatePropertiesTable();
  const G4int nentLanex=2;
  G4double rindex=1.50;//(1.82+1.50)/2.0;
  G4double energytab[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double rindextab[]={rindex, rindex};
  G4double abslen[]={7*CLHEP::mm, 7*CLHEP::mm};
  mptLanex->AddProperty("RINDEX",energytab, rindextab, nentLanex); //Average refractive index of bulk material
  mptLanex->AddProperty("ABSLENGTH", energytab, abslen, nentLanex);
#if G4VERSION_NUMBER < 1079
  mptLanex->AddConstProperty("MIEHG", 60.3e-3*CLHEP::mm); // interface changed in V11
  G4double emitspec[]={1.0, 1.0};
  mptLanex->AddProperty("FASTCOMPONENT",energytab, emitspec, nentLanex);
  mptLanex->AddConstProperty("FASTTIMECONSTANT", 1.*CLHEP::ns);
#endif
  G4double scintScalingFactor=1;
  mptLanex->AddConstProperty("SCINTILLATIONYIELD",7.8e4/CLHEP::MeV);
  mptLanex->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptLanex->AddConstProperty("MIEHG_FORWARD", 0.91);
  mptLanex->AddConstProperty("MIEHG_BACKWARD", 0.91);
  mptLanex->AddConstProperty("MIEHG_FORWARD_RATIO", 1.0);
  tmpMaterial->SetMaterialPropertiesTable(mptLanex);
  AddMaterial(tmpMaterial, "lanex");

  //Ganolinium oxysulphate in a polyurethane elastomer (lanex) - version 2
  tmpMaterial = new G4Material("lanex2", lanex_density, 2);
  tmpMaterial->AddMaterial(GOS, gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptLanex2 = CreatePropertiesTable();
  mptLanex2->AddProperty("RINDEX",energytab, rindextab, nentLanex); //Average refractive index of bulk material
  mptLanex2->AddProperty("ABSLENGTH", energytab, abslen, nentLanex);
#if G4VERSION_NUMBER < 1079
  mptLanex2->AddConstProperty("MIEHG", 60.3e-3*CLHEP::mm);
  mptLanex2->AddProperty("FASTCOMPONENT",energytab, emitspec, nentLanex);
  mptLanex2->AddConstProperty("FASTTIMECONSTANT", 1.*CLHEP::ns);
#endif
  mptLanex2->AddConstProperty("SCINTILLATIONYIELD",8.9e4/CLHEP::MeV);
  mptLanex2->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptLanex2->AddConstProperty("MIEHG_FORWARD", 0.91);
  mptLanex2->AddConstProperty("MIEHG_BACKWARD", 0.91);
  mptLanex2->AddConstProperty("MIEHG_FORWARD_RATIO", 0.5);
  tmpMaterial->SetMaterialPropertiesTable(mptLanex);
  AddMaterial(tmpMaterial, "lanex2");

  //gos_lanex - GOS with the bulk optical transport properties of lanex particles suspended in an elastomer but the atomic, density and scintillation properties of GOS
  G4double gos_lanex_density=GOS->GetDensity();
  tmpMaterial = new G4Material("gos_lanex", gos_lanex_density, 1);
  tmpMaterial->AddMaterial(GOS, 1.0);
  G4MaterialPropertiesTable* mptGOSLanex = CreatePropertiesTable();
  const G4int nentGOSLanex=2;
  G4double rindexGOSLanex=1.50;
  G4double energyGOSLanexTab[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double rindexGOSLanexTab[]={rindexGOSLanex, rindexGOSLanex};
  G4double abslenGOSLanex[]={7*CLHEP::mm, 7*CLHEP::mm};
  G4double gosLanexMiehgForward=0.911;
  G4double gosLanexMiehgBackward=0.911;
  G4double gosLanexMiehgForwardRatio=0.5;
#if G4VERSION_NUMBER < 1079
  G4double mieHgTimeConst=1.0*CLHEP::ns;
  G4double emitspecGOSLanex[]={1.0, 1.0};
  G4double mieScatteringLengthGOSLanex=60.3*CLHEP::um;
  mptGOSLanex->AddProperty("FASTCOMPONENT",energyGOSLanexTab, emitspecGOSLanex, nentGOSLanex);
  mptGOSLanex->AddConstProperty("FASTTIMECONSTANT", mieHgTimeConst);
  mptGOSLanex->AddConstProperty("YIELDRATIO", 1.0);
  mptGOSLanex->AddConstProperty("MIEHG", mieScatteringLengthGOSLanex);
#endif
  mptGOSLanex->AddConstProperty("SCINTILLATIONYIELD",8.9e4/CLHEP::MeV); //Intrinisic scintilation yield of GOS
  mptGOSLanex->AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptGOSLanex->AddConstProperty("MIEHG_FORWARD", gosLanexMiehgForward);
  mptGOSLanex->AddConstProperty("MIEHG_BACKWARD", gosLanexMiehgBackward);
  mptGOSLanex->AddConstProperty("MIEHG_FORWARD_RATIO", gosLanexMiehgForwardRatio);
  mptGOSLanex->AddProperty("RINDEX",energyGOSLanexTab, rindexGOSLanexTab, nentGOSLanex); //Average refractive index of bulk material
  mptGOSLanex->AddProperty("ABSLENGTH", energyGOSLanexTab, abslenGOSLanex, nentGOSLanex);
  tmpMaterial->SetMaterialPropertiesTable(mptGOSLanex);
  AddMaterial(tmpMaterial, "gos_lanex");

  //Same as gos_lanex but refractive index = 1
  tmpMaterial = new G4Material("gos_ri1", gos_lanex_density, 1);
  tmpMaterial->AddMaterial(GOS, 1.0);
  G4MaterialPropertiesTable* mptGOSLanexRi1 = CreatePropertiesTable();
  G4double rindexGOSLanexRi1Tab[]={1.0, 1.0};
#if G4VERSION_NUMBER < 1079
  mptGOSLanexRi1->AddProperty("FASTCOMPONENT",energyGOSLanexTab, emitspecGOSLanex, nentGOSLanex);
  mptGOSLanexRi1->AddConstProperty("FASTTIMECONSTANT", mieHgTimeConst);
  mptGOSLanexRi1->AddConstProperty("YIELDRATIO", 1.0);
  mptGOSLanexRi1->AddConstProperty("MIEHG", mieScatteringLengthGOSLanex);
#endif
  mptGOSLanexRi1->AddConstProperty("SCINTILLATIONYIELD",8.9e4/CLHEP::MeV); //Intrinisic scintilation yield of GOS
  mptGOSLanexRi1->AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptGOSLanexRi1->AddConstProperty("MIEHG_FORWARD", gosLanexMiehgForward);
  mptGOSLanexRi1->AddConstProperty("MIEHG_BACKWARD", gosLanexMiehgBackward);
  mptGOSLanexRi1->AddConstProperty("MIEHG_FORWARD_RATIO", gosLanexMiehgForwardRatio);
  mptGOSLanexRi1->AddProperty("RINDEX",energyGOSLanexTab, rindexGOSLanexRi1Tab, nentGOSLanex); //Average refractive index of bulk material
  mptGOSLanexRi1->AddProperty("ABSLENGTH", energyGOSLanexTab, abslenGOSLanex, nentGOSLanex);
  tmpMaterial->SetMaterialPropertiesTable(mptGOSLanexRi1);
  AddMaterial(tmpMaterial, "gos_ri1");

  //pet_lanex - PET with the bulk optical transport properties of lanex particles suspended in an elastomer but the atomic, density and scintillation properties of PET
  G4double pet_lanex_density=GetMaterial("polyurethane")->GetDensity();
  tmpMaterial = new G4Material("pet_lanex", pet_lanex_density, 1);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), 1.0);
  G4MaterialPropertiesTable* mptPETLanex = CreatePropertiesTable();
  mptPETLanex->AddConstProperty("MIEHG_FORWARD", gosLanexMiehgForward);
  mptPETLanex->AddConstProperty("MIEHG_BACKWARD", gosLanexMiehgBackward);
  mptPETLanex->AddConstProperty("MIEHG_FORWARD_RATIO", gosLanexMiehgForwardRatio);
#if G4VERSION_NUMBER < 1079
  mptPETLanex->AddConstProperty("MIEHG", mieScatteringLengthGOSLanex);
#endif
  mptPETLanex->AddProperty("RINDEX",energyGOSLanexTab, rindexGOSLanexTab, nentGOSLanex); //Average refractive index of bulk material
  mptPETLanex->AddProperty("ABSLENGTH", energyGOSLanexTab, abslenGOSLanex, nentGOSLanex);
  tmpMaterial->SetMaterialPropertiesTable(mptPETLanex);
  AddMaterial(tmpMaterial, "pet_lanex");

  //Medex (larger grained lanex)
  //  G4double medex_fill_factor=0.5;
  G4double medex_density=fill_factor*GOS->GetDensity()+(1-fill_factor)*GetMaterial("polyurethane")->GetDensity();
  G4double medex_gos_fraction_by_mass=fill_factor*GOS->GetDensity()/medex_density;
  G4double medex_pur_fraction_by_mass=1-medex_gos_fraction_by_mass;
  tmpMaterial = new G4Material("medex", medex_density, 2);
  tmpMaterial->AddMaterial(GOS, medex_gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), medex_pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptMedex = CreatePropertiesTable();
  const G4int nentMedex=2;
  //  G4double medexRindex=(1.82+1.50)/2.0;
  //  G4double medexEnergytab[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double medexRindextab[]={rindex, rindex};

  G4double medexAbslen[]={7*CLHEP::mm, 7*CLHEP::mm};
  mptMedex->AddProperty("RINDEX",energytab, medexRindextab, nentMedex); //Average refractive index of bulk material
  mptMedex->AddProperty("ABSLENGTH", energytab, medexAbslen, nentMedex);
#if G4VERSION_NUMBER < 1079
  mptMedex->AddConstProperty("MIEHG", 230e-3*CLHEP::mm);
  G4double medexEmitspec[]={1.0, 1.0};
  mptMedex->AddProperty("FASTCOMPONENT",energytab, medexEmitspec, nentMedex);
  mptMedex->AddConstProperty("FASTTIMECONSTANT", 1.*CLHEP::ns);
#endif
  mptMedex->AddConstProperty("SCINTILLATIONYIELD",scintScalingFactor*2.94e4/CLHEP::MeV);
  mptMedex->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptMedex->AddConstProperty("MIEHG_FORWARD", 0.93);
  mptMedex->AddConstProperty("MIEHG_BACKWARD", 0.93);
  mptMedex->AddConstProperty("MIEHG_FORWARD_RATIO", 1.0);
  tmpMaterial->SetMaterialPropertiesTable(mptMedex);
  AddMaterial(tmpMaterial, "medex");

  // carbon fiber
  G4double frac_graph=0.5;
  G4double frac_poly=0.5;
  G4Material* graph=GetMaterial("G4_GRAPHITE");
  G4Material* poly=GetMaterial("G4_POLYACRYLONITRILE");
  G4double dens_graph=graph->GetDensity();
  G4double dens_poly=poly->GetDensity();

  G4double dens_cf =frac_graph*dens_graph+frac_poly*dens_poly;
  G4double frac_graph_bw=frac_graph*dens_graph/dens_cf;
  G4double frac_poly_bw=frac_poly*dens_poly/dens_cf;
  tmpMaterial = new G4Material("carbonfiber", dens_cf, 2);
  tmpMaterial->AddMaterial(graph, frac_graph_bw);
  tmpMaterial->AddMaterial(poly, frac_poly_bw);
  G4MaterialPropertiesTable* mptCarbonfiber = CreatePropertiesTable();
  const G4int nentCarbonfiber=2;
  G4double energytab_cf[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double carbonfiberRindextab[]={2.6, 2.6};
  G4double carbonfiberAbslen[]={2.1*CLHEP::um, 2.1*CLHEP::um};
  mptCarbonfiber->AddProperty("RINDEX",energytab_cf, carbonfiberRindextab, nentCarbonfiber); //Average refractive index of bulk material
  mptCarbonfiber->AddProperty("ABSLENGTH", energytab_cf, carbonfiberAbslen, nentCarbonfiber);
  tmpMaterial->SetMaterialPropertiesTable(mptCarbonfiber);
  AddMaterial(tmpMaterial, "carbonfiber");
}

void BDSMaterials::DefineLHCComponents()
{
  AddMaterial("lhcconcrete",
	      2.42,
	      kStateSolid, 300, 1, 
	      {"H","C","O","Na","Mg","Al","Si","K","Ca","Fe","P","S",
	       "Ti","Mn","Zn","Zr","Ba","Pb","Sr","Eu"},
	      std::list<double>{0.59785345499811 *CLHEP::perCent,
		  5.59989402848226 *CLHEP::perCent,
		  49.1111702720319 *CLHEP::perCent,
		  0.45137935852357 *CLHEP::perCent,
		  0.66062806777291 *CLHEP::perCent,
		  2.05561946276849 *CLHEP::perCent,
		  18.7995018924154 *CLHEP::perCent,
		  0.65365311079793 *CLHEP::perCent,
		  20.0191229406116 *CLHEP::perCent,
		  1.11400027114647 *CLHEP::perCent,
		  0.04782827639985 *CLHEP::perCent,
		  0.01195706909996 *CLHEP::perCent,
		  0.3457585814739  *CLHEP::perCent,
		  0.03856154784738 *CLHEP::perCent,
		  0.02401378044242 *CLHEP::perCent,
		  0.00737352594498 *CLHEP::perCent,
		  0.01783596140744 *CLHEP::perCent,
		  0.04623400051985 *CLHEP::perCent,
		  0.39757254757374 *CLHEP::perCent,
		  4.184974185E-05  *CLHEP::perCent});

  // LHC Rock
  AddMaterial("lhc_rock",
	      2,
	      kStateSolid, 300, 1,
	      {"H", "Na", "Si", "Fe", "C", "Mg", "K", "O", "Al", "Ca"},
	      std::list<double>{0.006, 0.01, 0.2, 0.014, 0.03, 0.005, 0.01, 0.5, 0.03, 0.195});

  // Superconducting components of LHC magnet elements
  // Definitions taken from FLUKA
  
  std::list<int> singleElement = {1};
  // Liquid helium at 1.9K  
  AddMaterial("lhe_1.9k",
	      0.1472,
	      kStateLiquid, 1.9, 1,
	      {"He"}, singleElement);

  // Niobium @ 87K
  AddMaterial("nb_87k",
	      8.902 ,
	      kStateSolid , 87 , 1,
	      {"Nb"}, singleElement);
  
  // Titanium @ 87K
  AddMaterial("ti_87k",
	      4.54,
	      kStateSolid, 87 , 1,
	      {"Ti"}, singleElement);

  // superconductor NbTi with Ti = 47% by weight
  AddMaterial("nbti_87k",
	      6.0471,
	      kStateSolid , 87 , 1,
	      {"nb_87k","ti_87k"},
	      std::list<double>{0.53,0.47});

  // copper at 4 Kelvin
  AddMaterial("cu_4k",
	      8.96,
	      kStateSolid, 4, 1,
	      {"Cu"}, singleElement);
  
  // copper at 2 Kelvin
  AddMaterial("cu_2k",
              8.96,
              kStateSolid, 2, 1,
              {"Cu"}, singleElement);
  
  // naked superconductor NbTi wire with Cu/SC volume ratio (>= 4.0 and <4.8)
  AddMaterial("nbti.1",
	      8.4206,
	      kStateSolid, 4, 1,
	      {"nbti_87k","cu_4k"},
	      std::list<double>{1.0/5.4, 4.4/5.4});
  
  // For HL-LHC Collimation - Copper Diamond - TBC - component fractions
  AddMaterial("copperdiamond",
              5.3,
              kStateSolid, 300, 1,
              {"Cu", "C"},
              std::list<double>{0.32, 0.68});
  AddExistingMaterialAlias("copperdiamond", "cucd");
  
  // For HL-LHC Collimation - Copper Diamond
  AddMaterial("molybdenumcarbide",
              8.9,
              kStateSolid, 300, 1,
              {"Mo", "C"},
              std::list<int>{1, 1});
  AddExistingMaterialAlias("molybdenumcarbide", "mogr");
}

void BDSMaterials::DefineLiquids()
{
  AddMaterial("liquidhelium", 0.12498, kStateLiquid, 4.15, 1, {"He"}, std::list<int>{1});

  // water
  AddExistingMaterialAlias("G4_WATER", "water"); // use NIST water
  G4Material* water = GetMaterial("water"); // get the material pointer back

  // add refractive index to G4_WATER material for cherenkov radiation to work
  const G4int nEntries = 9;
  G4MaterialPropertiesTable* waterProperties = CreatePropertiesTable();
  G4double photonEnergy[nEntries];
  G4double dNEntries   = (G4double)nEntries;
  G4double energyMin   = 1.*CLHEP::eV;
  G4double energyMax   = 3.*CLHEP::eV;
  G4double deltaEnergy = (energyMax-energyMin)/(dNEntries-1.0);
  G4double energy      = energyMin;
  for (G4int i = 0; i < nEntries; energy += deltaEnergy, i++)
    {photonEnergy[i] = energy;}
  G4double refractiveIndex[nEntries] = {1.325, 1.325, 1.326, 1.327, 1.328, 1.33, 1.333, 1.336, 1.343};
  waterProperties->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries);
  water->SetMaterialPropertiesTable(waterProperties); 
}

void BDSMaterials::DefineGases()
{
  AddExistingMaterialAlias("N", "nitrogen");

  // air
  AddExistingMaterialAlias("G4_AIR", "air");
  G4Material* g4AirMaterial = GetMaterial("air");
  const G4int airNEntries = 3;
  // source: NPL Tables of Physical & Chemical Constants. Refractive indices at
  // different energies.
  G4double airRefractiveIndex[airNEntries] = {1.000292,1.000292,1.000292};
  G4double airEnergy[airNEntries]          = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  G4MaterialPropertiesTable* airProperties = CreatePropertiesTable();
  airProperties->AddProperty("RINDEX", airEnergy, airRefractiveIndex, airNEntries);
  g4AirMaterial->SetMaterialPropertiesTable(airProperties);

  // old way - manually constructed air
  // room temperature temperature and standard pressure
  G4double temperature = 300*CLHEP::kelvin;
  G4double pressure    = 1.0*CLHEP::atmosphere;
  G4double airDensity  = 0.001225; // g/cm3
  G4Material* tmpMaterial = new G4Material("airbdsim",
					   airDensity*CLHEP::g/CLHEP::cm3,
					   2,
					   kStateGas,
					   temperature,
					   pressure);
  tmpMaterial->AddElement(GetElement("O"), 0.2);
  tmpMaterial->AddElement(GetElement("N"), 0.8);
  tmpMaterial->SetMaterialPropertiesTable(airProperties);
  AddMaterial(tmpMaterial, "airbdsim");

  // Carbon monoxide
  G4double coDensity = 0.001145; // g/cm3
  AddMaterial("carbonmonoxide",
	      coDensity,
	      kStateGas,
	      temperature,
	      pressure,
	      {"C","O"},
	      std::list<int>{1,1});

  // Carbon monoxide beam pipe gas
  G4double bp_pressure = 0.0133e-9 * (CLHEP::bar/CLHEP::atmosphere); //10 nTorr pressure
  AddMaterial("bp_carbonmonoxide",
	      coDensity,
	      kStateGas,
	      temperature,
	      bp_pressure,
	      {"C","O"},
	      std::list<int>{1,1});  
}

void BDSMaterials::DefinePlasmas()
{
  //Awake plasma - rubidium at density of 7e14 atoms/cm3 
  //  G4double numberDensity = 7.0e14/CLHEP::cm3;
  G4double a = 85.4678*CLHEP::g/CLHEP::mole;
  G4double density = 1e-7 * CLHEP::g/CLHEP::cm3;
  G4Material* tmpMaterial = new G4Material("awakeplasma", 37., a, density);
  AddMaterial(tmpMaterial, "awakeplasma");
}

void BDSMaterials::DefineVacuums()
{
  // Default vacuum (same composition as residual vacuum in warm sections of LHC).
  G4double vacpressure;
  // check if parser is initialised (in case list materials is asked by command line) 
  if (BDSParser::IsInitialised())
    {vacpressure=BDSParser::Instance()->GetOptions().vacuumPressure*CLHEP::bar;}
  else
    {vacpressure=1e-12*CLHEP::bar;}
  G4double temperature = 300*CLHEP::kelvin;
  G4double density = (CLHEP::STP_Temperature/temperature) * (vacpressure/(1.*CLHEP::atmosphere)) * 29*CLHEP::g/(22.4*1.e-3*CLHEP::m3);
  
  G4Material* tmpMaterial = new G4Material("vacuum",
					   density,
					   3, kStateGas,
					   temperature, vacpressure);
  tmpMaterial->AddElement(GetElement("H"), 0.482);
  tmpMaterial->AddElement(GetElement("C"), 0.221);
  tmpMaterial->AddElement(GetElement("O"), 0.297);
  AddMaterial(tmpMaterial, "vacuum");

  const G4int Vac_NUMENTRIES = 3;
  // Assume refractive index = 1 in a vacuum.
  G4double Vac_RIND[Vac_NUMENTRIES]   = {1.000,1.000,1.000};
  G4double Vac_Energy[Vac_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  G4MaterialPropertiesTable* vacMaterialPropertiesTable = CreatePropertiesTable();
  vacMaterialPropertiesTable->AddProperty("RINDEX",Vac_Energy, Vac_RIND, Vac_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(vacMaterialPropertiesTable);

  // copy regular vacuum as named 'laservac' to detect where laserwire is.
  // G4Material doesn't have a copy constructor but it has a sort of copy
  // constructor.
  G4Material* regularVacuum = GetMaterial("vacuum");
  G4Material* laservac = new G4Material("laservac",
					regularVacuum->GetDensity(),
					regularVacuum,
					kStateGas,
					regularVacuum->GetTemperature(),
					regularVacuum->GetPressure());
  AddMaterial(laservac, "laservac");
}

void BDSMaterials::AddMaterial(G4Material* material, G4String name)
{
  name = BDS::LowerCase(name);
  if (materialNames.count(name) == 0) // not defined already
    {
      materials[name] = material;
      materialNames.insert(name);
    }
  else
    {throw BDSException(__METHOD_NAME__, "Material \"" + name + "\" already exists");}
}

void BDSMaterials::AddExistingMaterialAlias(const G4String &existingMaterialName,
                                            G4String alias)
{
  alias = BDS::LowerCase(alias);
  G4Material* material = GetMaterial(existingMaterialName);
  aliases[alias] = material; // store in lower case as that's how we search
  aliasNames.insert(alias);
  
  // cache aliases for a given material for feedback
  auto search = materialToAliases.find(material);
  if (search == materialToAliases.end())
    {materialToAliases[material] = {};}
  materialToAliases[material].push_back(alias);
}

void BDSMaterials::AddMaterial(G4String name,
			       G4double Z,
			       G4double A,
			       G4double density,
			       G4State  state,
			       G4double temperature,
			       G4double pressure)
{
  // convention: material name in small letters (to be able to find materials regardless of capitalisation)
  name = BDS::LowerCase(name);
  DensityCheck(density, name);
  
  G4Material* tmpMaterial = new G4Material(name,
					   Z,
					   A*CLHEP::g/CLHEP::mole,
					   density*CLHEP::g/CLHEP::cm3,
					   state,
					   temperature*CLHEP::kelvin,
					   pressure*CLHEP::atmosphere);
  AddMaterial(tmpMaterial, name);
}

template <typename Type>
void BDSMaterials::AddMaterial(G4String name,
			       G4double density,
			       G4State  state,
			       G4double temperature,
			       G4double pressure,
			       const std::list<G4String>& components,
			       const std::list<Type>&     componentFractions)
{
  name = BDS::LowerCase(name);
  DensityCheck(density, name);


  if(state == G4State::kStateGas) {
    BDSIdealGas::CheckGasLaw(temperature, pressure, density);
    G4cout << temperature << " " << pressure << " " << density << G4endl;
  }


  // TODO - Remove test of ideal gas functions
  if (name == "air") {
    G4cout << "Material name : " << name << G4endl;
    auto IdealGas = new BDSIdealGas();
    IdealGas->CalculateDensityFromPressureTemperature<Type>(components, componentFractions, pressure, temperature);
  }
  
  G4Material* tmpMaterial = new G4Material(name,
					   density*CLHEP::g/CLHEP::cm3, 
					   (G4int)components.size(),
					   state,
					   temperature*CLHEP::kelvin,
					   pressure*CLHEP::atmosphere);
  std::list<G4String>::const_iterator sIter;
  typename std::list<Type>::const_iterator dIter;
  for (sIter = components.begin(), dIter = componentFractions.begin();
       sIter != components.end();
       ++sIter, ++dIter)
    {
#ifdef BDSDEBUG
      G4cout << "BDSMaterials::AddMaterial: " << *sIter << G4endl;
#endif
      G4Element* element = CheckElement(*sIter);
      if (element)
	{tmpMaterial->AddElement(element, (*dIter));}
      else
	{tmpMaterial->AddMaterial(GetMaterial(*sIter), (*dIter));}
    }
  AddMaterial(tmpMaterial, name);
}

G4Material* BDSMaterials::GetMaterial(G4String material) const
{
  if (material.empty())
    {throw BDSException(__METHOD_NAME__, "empty material name");}
  G4String materialOriginal = material;
  // for short names we assume they're elements so we prefix with G4_ and
  // get them from NIST
  G4String nistString ("G4_");
  if (material.length() <= 2)
#if G4VERSION_NUMBER > 1099
    {material = nistString + material;}
#else
    {material.prepend(nistString);}
#endif

  G4String start = material.substr(0,3);
  if (nistString == start)
    {
#ifdef BDSDEBUG
      G4cout << "Using NIST material " << material << G4endl;
#endif
      G4Material* mat = G4NistManager::Instance()->FindOrBuildMaterial(material, true, true);
      if (!mat)
        {throw BDSException(__METHOD_NAME__, "\"" + material + "\" could not be found by NIST.");}
      return mat;
    }
  else
    {
      // find material regardless of capitalisation
      material = BDS::LowerCase(material);

      // search predefined + parser materials
      if (materialNames.count(material) == 1)
        {return materials.at(material);}
      else if (aliasNames.count(material) == 1)
        {return aliases.at(material);}
      else
        {
          // search aliases
          if (externalMaterialNames.count(material) == 1)
            {return externalMaterials.at(material);}
          else
            {// can't find it -> warn and exit
              ListMaterials();
              throw BDSException(__METHOD_NAME__, "\"" + materialOriginal + "\" is unknown.");
            }
        }
    }
}

void BDSMaterials::CheckForConflictingMaterialsAfterLoad(const G4String& geometryFileName,
                                                         const G4String& componentName) const
{
  G4MaterialTable* table = G4Material::GetMaterialTable();
  
  G4bool check = false;
  std::map<G4String, G4int> nameCount;
  for (const auto& mat : *table)
    {
      G4String name = mat->GetName();
      auto search = nameCount.find(name);
      if (search == nameCount.end())
        {nameCount[name] = 0;}
      nameCount[name] += 1;
      check = check || nameCount[name] > 1; // flag for efficiency - don't loop again if we don't need to
    }
  
  if (check)
    {
      for (const auto& nc: nameCount)
        {
          if (nc.second > 1)
            {
              G4String msg = "the material \"" + nc.first + "\" has been defined more\n";
              msg += "than once now and will cause the wrong material to be used for any future usages.\n";
              msg += "Error caused by GDML file \""+geometryFileName+"\" in component \""+componentName+"\"\n";
              msg += "This is most likely due to a material name in the GDML file conflicting with a predefined one in BDSIM.";
              if (BDS::StartsWith(BDS::LowerCase(nc.first), "g4_"))
                {BDS::Warning(msg);} // only warn for a NIST one as ultimately it won't be that different even if redefined from reloading
              else
                {throw BDSException(__METHOD_NAME__, msg);}
            }
        }
    }
}


void BDSMaterials::CacheMaterialsFromGDML(const std::map<G4String, G4Material*>& materialsGDML)
{
  // this function is really just concerning GDML material caching and not
  // all external format name caching...
  for (const auto& kv : materialsGDML)
    {
      G4String nameLower = BDS::LowerCase(kv.first);
      
      // We have to allow names that are in bdsim materials as the user could have
      // used just a bdsim material by name. However, we can't know if that was one
      // that was constructed uniquely with the same name. For this, we need to check
      // the material table in the above function.
      if (BDS::StartsWith(nameLower, "g4_") || materials.find(nameLower) != materials.end())
        {continue;} // a Geant4 material or a BDSIM one

      if (externalMaterialNames.count(nameLower) == 1)
        {
          G4String msg = "the material \""+kv.first+"\" is already defined and has\n";
          msg += "already been loaded from a previous GDML file(s) and is ambiguous.\n";
          msg += "Please prepend with the BDSIM element used to load the file to be explicit.";
          throw BDSException(__METHOD_NAME__, msg);
        }
      
      // it's ok for a GDML material to have a material name the same as a BDSIM
      // alias as only the real name will conflict
      
      externalMaterials[nameLower] = kv.second;
      externalMaterialNames.insert(nameLower);
    }
}

void BDSMaterials::AddElement(G4Element* element, const G4String& symbol)
{
  if (CheckElement(symbol) != nullptr)
    {throw BDSException(__METHOD_NAME__, "Element  \"" + symbol + "\" already exists.");}

  elements.insert(make_pair(symbol, element));
#ifdef BDSDEBUG
  G4cout << "New element : " << symbol << G4endl;
#endif
}

void BDSMaterials::AddElement(const G4String& name,
                              const G4String& symbol,
                              G4double Z,
                              G4double A)
{
  G4Element* tmpElement = new G4Element(name, symbol, Z, A*CLHEP::g/CLHEP::mole);
  AddElement(tmpElement, symbol);
}

void BDSMaterials::DensityCheck(G4double  density,
                                const G4String& materialName) const
{
  if (density > 1e2)
    {// so greater than 100g / cm3, the densest natural material is around 23g/cm3
      G4String msg = "material \"" + materialName + "\"has a density higher than 100g/cm3! Perhaps check this!\n";
      msg += "Density: " + std::to_string(density) + " g/cm3... proceeding";
      BDS::Warning(__METHOD_NAME__, msg);
    }
}

G4Element* BDSMaterials::CheckElement(const G4String& symbol) const
{
  // first look in defined element list
  auto iter = elements.find(symbol);
  if(iter != elements.end())
    {return (*iter).second;}
  else
    {
      G4Element* element = G4NistManager::Instance()->FindOrBuildElement(symbol, true);
      return element;
    }
}

G4Element* BDSMaterials::GetElement(const G4String& symbol) const
{
  G4Element* element = CheckElement(symbol);
  if (!element)
    {throw BDSException(__METHOD_NAME__, "Element \"" + symbol + "\" could not be found.");}
  return element;
}

void BDSMaterials::PrintBasicMaterialMassFraction(G4Material* material) const
{
  // simpler version of geant4 code
  for (G4int i = 0; i < (G4int)material->GetNumberOfElements(); i++)
    {
      G4cout << (*(material->GetElementVector()))[i]->GetName() << "\t "
             << (material->GetFractionVector()[i])/CLHEP::perCent << " %"  << G4endl;
    }
}

void BDSMaterials::ListMaterials() const
{
  auto flagsCache(G4cout.flags());
  G4cout << __METHOD_NAME__ << G4endl;
  // always print out vacuum composition
  G4Material* vacuum = GetMaterial("vacuum");
  G4cout<< "\"vacuum\" composition: " << G4endl;
  PrintBasicMaterialMassFraction(vacuum);
  G4cout<< "pressure    = " << vacuum->GetPressure()/CLHEP::bar          << " bar"   << G4endl;
  G4cout<< "temperature = " << vacuum->GetTemperature()/CLHEP::kelvin    << " K"     << G4endl;
  G4cout<< "density     = " << vacuum->GetDensity()/(CLHEP::g/CLHEP::cm3)<< " g/cm^3" << G4endl << G4endl;
  
  G4cout << "All elements are available with their 1 or 2 letter chemical symbol. ie C or G4_C" << G4endl << G4endl;

  if (!elements.empty())
    {
      G4cout << "Extra defined elements are:" << G4endl;
      for (const auto& element : elements)
        {G4cout << std::left << std::setw(12) << element.second->GetName() << " - " << element.second->GetSymbol() << G4endl;}
      G4cout << G4endl;
    }
  
  G4cout << "Defined materials are:" << G4endl;
  for (const auto& material : materials)
    {
      G4cout << material.first;
      G4String realName = material.second->GetName();
      if (realName != material.first)
        {G4cout << " (" << material.second->GetName() << ")" << G4endl;}
      else
        {G4cout << G4endl;}

      auto aliasSearch = materialToAliases.find(material.second);
      if (aliasSearch != materialToAliases.end())
        {
          const auto v = aliasSearch->second;
          G4cout << "Aliases: ";
          for (const auto& n : v)
            {G4cout << "\"" << n << "\" ";}
          G4cout << G4endl;
        }
    }
  G4cout << G4endl;
  G4cout << "All aliases: (alias, real name)" << G4endl;
  for (const auto& aliasMaterial : aliases)
    {
      G4cout << aliasMaterial.first << " : " << aliasMaterial.second->GetName() << G4endl;}
  G4cout << G4endl;
  G4cout << "Available NIST materials are:" << G4endl;
  G4NistManager::Instance()->ListMaterials("all");
  G4cout.flags(flagsCache);
}

BDSMaterials::~BDSMaterials()
{
  for (auto& material : materials)
    {delete material.second;}
  materials.clear();
  materialNames.clear();

  // we don't delete externalMaterials as any loaded from GDML will be
  // deleted by Geant4
  externalMaterialNames.clear();

  for (auto& element : elements)
    {delete element.second;}
  elements.clear();

  for (G4MaterialPropertiesTable* table : propertiesTables)
    {delete table;}

  instance = nullptr;
}

void BDSMaterials::PrepareRequiredMaterials(G4bool verbose)
{
  // This function uses the list from the parser and prepares
  // the necessary materials for this run.
#ifdef BDSDEBUG
  G4bool debug = true;
#else
  G4bool debug = false;
#endif
 
  // convert the parsed atom list to list of Geant4 G4Elements  
  if (verbose || debug)
    {G4cout << __METHOD_NAME__ << "parsing the atom list..." << G4endl;}
  for (const auto& it : BDSParser::Instance()->GetAtoms())
    {AddElement(it.name,it.symbol,it.Z,it.A);}
  if (verbose || debug)
    {G4cout << "size of atom list: "<< BDSParser::Instance()->GetAtoms().size() << G4endl;}

  // convert the parsed material list to list of Geant4 G4Materials
  if (verbose || debug)
    {G4cout << "parsing the material list..."<< G4endl;}
  for (auto it : BDSParser::Instance()->GetMaterials())
    {
      G4State itsState;
      if      (it.state=="solid")
        {itsState = kStateSolid;}
      else if (it.state=="liquid")
        {itsState = kStateLiquid;}
      else if (it.state=="gas")
        {itsState = kStateGas;}
      else
        {
          G4cout << "Unknown material state "<< it.state 
                 << ", setting it to default (solid)"
                 << G4endl;
          it.state="solid";
          itsState = kStateSolid;
        }
      
#ifdef BDSDEBUG  
      G4cout << "---->adding Material, ";
      it.print();
#endif
      
      if(it.Z != 0)
        {
          AddMaterial(it.name,
                      it.Z,
                      it.A,
                      it.density,
                      itsState,
                      it.temper,
                      it.pressure);
        }
      else if(!(it.components.empty()))
        {
          std::list<G4String> tempComponents;
          for (const auto& jt : it.components)
            {tempComponents.emplace_back(G4String(jt));}
          
          if(it.componentsWeights.size()==it.components.size())
            {
              AddMaterial(it.name,
                          it.density,
                          itsState,
                          it.temper,
                          it.pressure,
                          tempComponents,
                          it.componentsWeights);
            }
          else if(it.componentsFractions.size()==it.components.size())
            {
              AddMaterial(it.name,
                          it.density,
                          itsState,
                          it.temper,
                          it.pressure,
                          tempComponents,
                          it.componentsFractions);
            }
          else
            {throw BDSException(__METHOD_NAME__, "Badly defined material - number of components is not equal to number of weights or mass fractions!");}
        }
      else
        {throw BDSException(__METHOD_NAME__, "Badly defined material - need more information!");}
    }
  if (verbose || debug)
    {G4cout << "size of material list: "<< BDSParser::Instance()->GetMaterials().size() << G4endl;}
}

G4MaterialPropertiesTable* BDSMaterials::CreatePropertiesTable()
{
  G4MaterialPropertiesTable* table = new G4MaterialPropertiesTable();
  propertiesTables.push_back(table);
  return table;
}
