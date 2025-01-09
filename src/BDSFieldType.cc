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
#include "BDSFieldType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSFieldType for reflexivity
template<>
std::map<BDSFieldType, std::string>* BDSFieldType::dictionary =
  new std::map<BDSFieldType, std::string> ({
      {BDSFieldType::none,             	           "none"},
      {BDSFieldType::bfieldzero,                   "bfieldzero"},
      {BDSFieldType::efieldzero,                   "efieldzero"},
      {BDSFieldType::ebfieldzero,                  "ebfieldzero"},
      {BDSFieldType::teleporter,       	           "teleporter"},
      {BDSFieldType::bmap1d,           	           "bmap1d"},
      {BDSFieldType::bmap2d,           	           "bmap2d"},
      {BDSFieldType::bmap3d,           	           "bmap3d"},
      {BDSFieldType::bmap4d,           	           "bmap4d"},
      {BDSFieldType::emap1d,           	           "emap1d"},
      {BDSFieldType::emap2d,           	           "emap2d"},
      {BDSFieldType::emap3d,           	           "emap3d"},
      {BDSFieldType::emap4d,           	           "emap4d"},
      {BDSFieldType::ebmap1d,          	           "ebmap1d"},
      {BDSFieldType::ebmap2d,          	           "ebmap2d"},
      {BDSFieldType::ebmap3d,          	           "ebmap3d"},
      {BDSFieldType::ebmap4d,          	           "ebmap4d"},
      {BDSFieldType::mokka,            	           "mokka"},
      {BDSFieldType::solenoid,         	           "solenoid"},
      {BDSFieldType::solenoidsheet,                "solenoidsheet"},
      {BDSFieldType::gaborlens,                    "gaborlens"},
      {BDSFieldType::dipole,           	           "dipole"},
      {BDSFieldType::quadrupole,       	           "quadrupole"},
      {BDSFieldType::dipolequadrupole,             "dipolequadrupole"},
      {BDSFieldType::sextupole,                    "sextupole"},
      {BDSFieldType::octupole,                     "octupole"},
      {BDSFieldType::decapole,                     "decapole"},
      {BDSFieldType::multipole,                    "multipole"},
      {BDSFieldType::muonspoiler,                  "muonspoiler"},
      {BDSFieldType::skewquadrupole,               "skewquadrupole"},
      {BDSFieldType::skewsextupole,                "skewsextupole"},
      {BDSFieldType::skewoctupole,                 "skewoctupole"},
      {BDSFieldType::skewdecapole,                 "skewdecapole"},
      {BDSFieldType::rfpillbox,                    "rfpillbox"},
      {BDSFieldType::rfconstantinx,                "rfconstantinx"},
      {BDSFieldType::rfconstantiny,                "rfconstantiny"},
      {BDSFieldType::rfconstantinz,                "rfconstantinz"},
      {BDSFieldType::undulator,                    "undulator"},
      {BDSFieldType::rmatrix,                      "rmatrix"},
      {BDSFieldType::paralleltransporter,          "paralleltransporter"},
      {BDSFieldType::cavityfringe,                 "cavityfringe"},
      {BDSFieldType::dipole3d,                     "dipole3d"},
      {BDSFieldType::multipoleouterdipole,         "multipoleouterdipole"},
      {BDSFieldType::multipoleouterquadrupole,     "multipoleouterquadrupole"},
      {BDSFieldType::multipoleoutersextupole,      "multipoleoutersextupole"},
      {BDSFieldType::multipoleouteroctupole,       "multipoleouteroctupole"},
      {BDSFieldType::multipoleouterdecapole,       "multipoleouterdecapole"},
      {BDSFieldType::skewmultipoleouterquadrupole, "skewmultipoleouterquadrupole"},
      {BDSFieldType::skewmultipoleoutersextupole,  "skewmultipoleoutersextupole"},
      {BDSFieldType::skewmultipoleouteroctupole,   "skewmultipoleouteroctupole"},
      {BDSFieldType::skewmultipoleouterdecapole,   "skewmultipoleouterdecapole"},
      {BDSFieldType::multipoleouterdipole3d,       "multipoleouterdipole3d"},
      {BDSFieldType::multipoleouterdipolelhc,      "multipoleouterdipolelhc"},
      {BDSFieldType::multipoleouterquadrupolelhc,  "multipoleouterquadrupolelhc"},
      {BDSFieldType::multipoleoutersextupolelhc,   "multipoleoutersextupolelhc"}
});	

BDSFieldType BDS::DetermineFieldType(G4String bType)
{
  std::map<G4String, BDSFieldType> types;
  types["none"]             = BDSFieldType::none;
  types["bfieldzero"]       = BDSFieldType::bfieldzero;
  types["efieldzero"]       = BDSFieldType::efieldzero;
  types["ebfieldzero"]      = BDSFieldType::ebfieldzero;
  types["teleporter"]       = BDSFieldType::teleporter;
  types["bmap1d"]           = BDSFieldType::bmap1d;
  types["bmap2d"]           = BDSFieldType::bmap2d;
  types["bmap3d"]           = BDSFieldType::bmap3d;
  types["bmap4d"]           = BDSFieldType::bmap4d;
  types["emap1d"]           = BDSFieldType::emap1d;
  types["emap2d"]           = BDSFieldType::emap2d;
  types["emap3d"]           = BDSFieldType::emap3d;
  types["emap4d"]           = BDSFieldType::emap4d;
  types["ebmap1d"]          = BDSFieldType::ebmap1d;
  types["ebmap2d"]          = BDSFieldType::ebmap2d;
  types["ebmap3d"]          = BDSFieldType::ebmap3d;
  types["ebmap4d"]          = BDSFieldType::ebmap4d;
  types["mokka"]            = BDSFieldType::mokka;
  types["solenoid"]         = BDSFieldType::solenoid;
  types["solenoidsheet"]    = BDSFieldType::solenoidsheet;
  types["gaborlens"]        = BDSFieldType::gaborlens;
  types["dipole"]           = BDSFieldType::dipole;
  types["quadrupole"]       = BDSFieldType::quadrupole;
  types["dipolequadrupole"] = BDSFieldType::dipolequadrupole;
  types["sextupole"]        = BDSFieldType::sextupole;
  types["octupole"]         = BDSFieldType::octupole;
  types["decapole"]         = BDSFieldType::decapole;
  types["multipole"]        = BDSFieldType::multipole;
  types["muonspoiler"]      = BDSFieldType::muonspoiler;
  types["skewquadrupole"]   = BDSFieldType::skewquadrupole;
  types["skewsextupole"]    = BDSFieldType::skewsextupole;
  types["skewoctupole"]     = BDSFieldType::skewoctupole;
  types["skewdecapole"]     = BDSFieldType::skewdecapole;
  types["rfpillbox"]        = BDSFieldType::rfpillbox;
  types["rfconstantinx"]    = BDSFieldType::rfconstantinx;
  types["rfconstantiny"]    = BDSFieldType::rfconstantiny;
  types["rfconstantinz"]    = BDSFieldType::rfconstantinz;
  types["undulator"]        = BDSFieldType::undulator;
  types["rmatrix"]          = BDSFieldType::rmatrix;
  types["paralleltransporter"] = BDSFieldType::paralleltransporter;
  types["cavityfringe"]     = BDSFieldType::cavityfringe;
  types["dipole3d"]         = BDSFieldType::dipole3d;
  types["multipoleouterdipole"]     = BDSFieldType::multipoleouterdipole;
  types["multipoleouterquadrupole"] = BDSFieldType::multipoleouterquadrupole;
  types["multipoleoutersextupole"]  = BDSFieldType::multipoleoutersextupole;
  types["multipoleouteroctupole"]   = BDSFieldType::multipoleouteroctupole;
  types["multipoleouterdecapole"]   = BDSFieldType::multipoleouterdecapole;
  types["skewmultipoleouterquadrupole"] = BDSFieldType::skewmultipoleouterquadrupole;
  types["skewmultipoleoutersextupole"]  = BDSFieldType::skewmultipoleoutersextupole;
  types["skewmultipoleouteroctupole"]   = BDSFieldType::skewmultipoleouteroctupole;
  types["skewmultipoleouterdecapole"]   = BDSFieldType::skewmultipoleouterdecapole;
  types["multipoleouterdipole3d"]       = BDSFieldType::multipoleouterdipole3d;
  types["multipoleouterdipolelhc"]      = BDSFieldType::multipoleouterdipolelhc;
  types["multipoleouterquadrupolelhc"]  = BDSFieldType::multipoleouterquadrupolelhc;
  types["multipoleoutersextupolelhc"]   = BDSFieldType::multipoleoutersextupolelhc;

  bType = BDS::LowerCase(bType);

  auto result = types.find(bType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + bType + "\" is not a valid field type\n";
      msg += "Available field types are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined field type to be " << result->second << G4endl;
#endif
  return result->second;
}
