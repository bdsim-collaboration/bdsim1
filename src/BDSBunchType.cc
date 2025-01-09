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
#include "BDSBunchType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSBunchType
template<>
std::map<BDSBunchType, std::string>* BDSBunchType::dictionary =
  new std::map<BDSBunchType, std::string> ({
      {BDSBunchType::reference,   "reference"},
      {BDSBunchType::gaussmatrix, "gaussmatrix"},
      {BDSBunchType::gauss,       "gauss"},
      {BDSBunchType::gausstwiss,  "gausstwiss"},
      {BDSBunchType::circle,      "circle"},
      {BDSBunchType::square,      "square"},
      {BDSBunchType::ring,        "ring"},
      {BDSBunchType::eshell,      "eshell"},
      {BDSBunchType::halo,        "halo"},
      {BDSBunchType::composite,   "composite"},
      {BDSBunchType::userfile,    "userfile"},
      {BDSBunchType::ptc,         "ptc"},
      {BDSBunchType::sixtrack,    "sixtrack"},
      {BDSBunchType::eventgeneratorfile, "eventgeneratorfile"},
      {BDSBunchType::sphere,      "sphere"},
      {BDSBunchType::compositesde,"compositespacedirectionenergy"},
      {BDSBunchType::box,         "box"},
      {BDSBunchType::halosigma,   "halosigma"},
      {BDSBunchType::bdsimsampler, "bdsimsampler"},
      {BDSBunchType::hdf5, "hdf5"}
});

BDSBunchType BDS::DetermineBunchType(G4String distrType)
{
  std::map<G4String, BDSBunchType> types;

  types["reference"]      = BDSBunchType::reference;
  types["gaussmatrix"]    = BDSBunchType::gaussmatrix;
  types["gauss"]          = BDSBunchType::gauss;
  types["gausstwiss"]     = BDSBunchType::gausstwiss;
  types["circle"]         = BDSBunchType::circle;
  types["square"]         = BDSBunchType::square;
  types["ring"]           = BDSBunchType::ring;
  types["eshell"]         = BDSBunchType::eshell;
  types["halo"]           = BDSBunchType::halo;
  types["composite"]      = BDSBunchType::composite;
  types["userfile"]       = BDSBunchType::userfile;
  types["ptc"]            = BDSBunchType::ptc;
  types["sixtrack"]       = BDSBunchType::sixtrack;
  types["eventgeneratorfile"] = BDSBunchType::eventgeneratorfile;
  types["sphere"]         = BDSBunchType::sphere;
  types["compositespacedirectionenergy"] = BDSBunchType::compositesde;
  types["compositesde"]   = BDSBunchType::compositesde;
  types["box"]            = BDSBunchType::box;
  types["halosigma"]      = BDSBunchType::halosigma;
  types["bdsimsampler"]   = BDSBunchType::bdsimsampler;
  types["hdf5"]   = BDSBunchType::hdf5;

  distrType = BDS::LowerCase(distrType);

  auto result = types.find(distrType);
  if (result == types.end())
    {// it's not a valid key
      G4String message = "\"" + distrType + "\" is not a valid distribution\n";
      message += "Available distributions are:\n";
      for (const auto& it : types)
        {message += it.first + "\n";}
      throw BDSException(__METHOD_NAME__, message);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined distribution to be " << result->second  << G4endl;
#endif
  return result->second;
}
