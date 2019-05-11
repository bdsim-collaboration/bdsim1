/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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
#include "BDSScorerType.hh"

#include <map>
#include <string>

/// Dictionary for BDSScorerType.
template<>
std::map<BDSScorerType, std::string>* BDSScorerType::dictionary =
        new std::map<BDSScorerType, std::string> ({
            {BDSScorerType::depositeddose,   "depositeddose"},
	    {BDSScorerType::depositeddose3d, "depositeddose3d"},
            {BDSScorerType::depositedenergy, "depositedenergy"},
	    {BDSScorerType::depositedenergy, "depositedenergy3d"},
            {BDSScorerType::population,      "population"},
            {BDSScorerType::ambientdose,     "ambientdose"},
            {BDSScorerType::ambientdose,     "activation"}
        });

BDSScorerType BDS::DetermineScorerType(G4String scorerType)
{
  std::map<G4String, BDSScorerType> types;
  types["depositeddose"]     = BDSScorerType::depositeddose;
  types["depositeddose3d"]   = BDSScorerType::depositeddose3d;
  types["depositedenergy"]   = BDSScorerType::depositedenergy;
  types["depositedenergy3d"] = BDSScorerType::depositedenergy3d;
  types["population"]        = BDSScorerType::population;
  types["ambientdose"]       = BDSScorerType::ambientdose;
  types["activation"]        = BDSScorerType::activation;
  
  scorerType.toLower();
  
  auto result = types.find(scorerType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cout << __METHOD_NAME__ << "\"" << scorerType << "\" is not a valid scorer type" << G4endl;
      G4cout << "Available scorer types are:" << G4endl;
      for (auto it : types)
        {G4cout << "\"" << it.first << "\"" << G4endl;}
      throw BDSException(__METHOD_NAME__, "error.");
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined scorer type to be " << result->second << G4endl;
#endif
  return result->second;
}
