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
#include "BDSCavityFieldType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSCavityFieldType
template<>
std::map<BDSCavityFieldType, std::string>* BDSCavityFieldType::dictionary =
  new std::map<BDSCavityFieldType, std::string> ({
  {BDSCavityFieldType::constantinz,        "constantinz"},
  {BDSCavityFieldType::pillbox,            "pillbox"},
  {BDSCavityFieldType::transversemagnetic, "transversemagnetic"},
  {BDSCavityFieldType::axialstandingapprox,"axialstandingapprox"},
  {BDSCavityFieldType::axialfloquetapprox, "axialfloquetapprox"}

});

BDSCavityFieldType BDS::DetermineCavityFieldType(G4String cavityFieldType)
{
  std::map<G4String, BDSCavityFieldType> types;
  types["constantinz"]         = BDSCavityFieldType::constantinz;
  types["pillbox"]             = BDSCavityFieldType::pillbox;
  types["transversemagnetic"]  = BDSCavityFieldType ::transversemagnetic;
  types["axialstandingapprox"] = BDSCavityFieldType ::axialstandingapprox;
  types["axialfloquetapprox"]  = BDSCavityFieldType ::axialfloquetapprox;

  cavityFieldType = BDS::LowerCase(cavityFieldType);

  auto result = types.find(cavityFieldType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + cavityFieldType + "\" is not a valid cavity field type\n";
      msg += "Available cavity field types are:\n";
      for (const auto& it : types)
        {msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined cavity field type to be " << result->second << G4endl;
#endif
  return result->second;
}

BDSFieldType BDS::FieldTypeFromCavityFieldType(BDSCavityFieldType cavityFieldType)
{
  BDSFieldType result;
  switch (cavityFieldType.underlying())
    {
    case BDSCavityFieldType::constantinz:
      {result = BDSFieldType::rfconstantinz; break;}
    case BDSCavityFieldType::pillbox:
      {result = BDSFieldType::rfpillbox; break;}
    case BDSCavityFieldType::transversemagnetic:
      {result = BDSFieldType::transversemagnetic; break;}
    case BDSCavityFieldType::axialstandingapprox:
      {result = BDSFieldType::axialstandingapprox; break;}
    case BDSCavityFieldType::axialfloquetapprox:
      {result = BDSFieldType::axialfloquetapprox; break;}
    }
  return result;
}
