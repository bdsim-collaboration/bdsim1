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
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 globals / types

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <iomanip>
#include <map>
#include <vector>

const std::vector<G4String> BDSMagnetStrength::keys = {
  "beta0",           // relativistic beta for the primary particle - used in some integrators
  "field",           // constant field in G4units - magnitude of field only - use bx,by,bz to get direction
  "efield",          // electric field in G4units - magnitude of field only - use ex,ey,ez to get direction
  "bx","by","bz",    // (assumed) unit vector components for field direction
  "ex","ey","ez",    // (assumed) unit vector components for field direction
  "e1",              // entrance poleface rotation angle
  "e2",              // entrance poleface rotation angle
  "h1",              // poleface curvature for entrance face
  "h2",              // poleface curvature for exit face
  "angle", "length", // (rad, mm)
  "fint",            // fringe field integral value for entrance face
  "fintx",           // fringe field integral value for exit face
  "fintk2",          // second fringe field integral value for entrance face
  "fintxk2",         // second fringe field integral value for exit face
  "hgap",            // fringe field vertical half-gap
  "hkick", "vkick",  // fractional horizontal and vertical dPx (w.r.t. rigidity)
  "ks",              // not in G4 units
  "k1", "k1s",
  "k2", "k2s",
  "k3", "k3s",
  "k4", "k4s",
  "k5", "k5s",
  "k6", "k6s",
  "k7", "k7s",
  "k8", "k8s",
  "k9", "k9s",
  "k10", "k10s",
  "k11", "k11s",
  "k12", "k12s",
  "kg",              // gabor lens focusing strength
  "plasmaEfield",    // magnitude of electric strength of confined Gabor lens plasma
  "frequency",       // frequency for time varying field (presumably em)
  "phase",           // phase for time varying field
  "synchronousT0",   // global T0 for the synchronous particle at the centre of the object
  "equatorradius",   // radius from axis at which field goes to 0
  "nominalenergy",   // nominal beam energy needed by some integrators
  "scaling",         // field scaling factor needed by dipolequadrupole integrator
  "scalingOuter",    // arbitrary scaling for yoke fields - kept as a separate scaling number
  "isentrance",      // bool to determine is integrator is for entrance (1) or exit (0) face
  "kick1",
  "kick2",
  "kick3",
  "kick4",
  "rmat11",
  "rmat12",
  "rmat13",
  "rmat14",
  "rmat21",
  "rmat22",
  "rmat23",
  "rmat24",
  "rmat31",
  "rmat32",
  "rmat33",
  "rmat34",
  "rmat41",
  "rmat42",
  "rmat43",
  "rmat44"
};

const std::map<G4String, BDSMagnetStrength::unitsFactors> BDSMagnetStrength::unitsFactorsMap = {
    {"beta0"         , {"",    1.0}},
    {"field"         , {"T",   CLHEP::tesla}},
    {"efield"        , {"",    CLHEP::megavolt}},
    {"bx"            , {"",    1.0}},
    {"by"            , {"",    1.0}},
    {"bz"            , {"",    1.0}},
    {"ex"            , {"",    1.0}},
    {"ey"            , {"",    1.0}},
    {"ez"            , {"",    1.0}},
    {"e1"            , {"rad", CLHEP::rad}},
    {"e2"            , {"rad", CLHEP::rad}},
    {"h1"            , {"rad", CLHEP::rad}},
    {"h2"            , {"rad", CLHEP::rad}},
    {"angle"         , {"rad", CLHEP::rad}},
    {"length"        , {"m",   CLHEP::m}},
    {"fint"          , {"",    1.0}},
    {"fintx"         , {"",    1.0}},
    {"fintk2"        , {"",    1.0}},
    {"fintxk2"       , {"",    1.0}},
    {"hgap"          , {"m",   CLHEP::m}},
    {"hkick"         , {"",    1.0}},
    {"vkick"         , {"",    1.0}},
    {"ks"            , {"",    1.0}},
    {"k1"            , {"",    1.0}},
    {"k2"            , {"",    1.0}},
    {"k3"            , {"",    1.0}},
    {"k4"            , {"",    1.0}},
    {"k5"            , {"",    1.0}},
    {"k6"            , {"",    1.0}},
    {"k7"            , {"",    1.0}},
    {"k8"            , {"",    1.0}},
    {"k9"            , {"",    1.0}},
    {"k10"           , {"",    1.0}},
    {"k11"           , {"",    1.0}},
    {"k12"           , {"",    1.0}},
    {"k1s"           , {"",    1.0}},
    {"k2s"           , {"",    1.0}},
    {"k3s"           , {"",    1.0}},
    {"k4s"           , {"",    1.0}},
    {"k5s"           , {"",    1.0}},
    {"k6s"           , {"",    1.0}},
    {"k7s"           , {"",    1.0}},
    {"k8s"           , {"",    1.0}},
    {"k9s"           , {"",    1.0}},
    {"k10s"          , {"",    1.0}},
    {"k11s"          , {"",    1.0}},
    {"k12s"          , {"",    1.0}},
    {"frequency"     , {"",    CLHEP::megahertz}},
    {"phase"         , {"rad", CLHEP::rad}},
    {"synchronousT0" , {"s",   CLHEP::s}},
    {"equatorradius" , {"m",   CLHEP::m}},
    {"kg"            , {"",    1.0}},
    {"plasmaEfield"  , {"",    1.0}},
    {"nominalenergy" , {"GeV", CLHEP::GeV}},
    {"scaling"       , {"",    1.0}},
    {"scalingOuter"  , {"",    1.0}},
    {"isentrance"    , {"",    1.0}},
    {"kick1"         , {"",    1.0}},
    {"kick2"         , {"",    1.0}},
    {"kick3"         , {"",    1.0}},
    {"kick4"         , {"",    1.0}},
    {"rmat11"        , {"",    1.0}},
    {"rmat12"        , {"",    1.0}},
    {"rmat13"        , {"",    1.0}},
    {"rmat14"        , {"",    1.0}},
    {"rmat21"        , {"",    1.0}},
    {"rmat22"        , {"",    1.0}},
    {"rmat23"        , {"",    1.0}},
    {"rmat24"        , {"",    1.0}},
    {"rmat31"        , {"",    1.0}},
    {"rmat32"        , {"",    1.0}},
    {"rmat33"        , {"",    1.0}},
    {"rmat34"        , {"",    1.0}},
    {"rmat41"        , {"",    1.0}},
    {"rmat42"        , {"",    1.0}},
    {"rmat43"        , {"",    1.0}},
    {"rmat44"        , {"",    1.0}}
};

const std::vector<G4String> BDSMagnetStrength::normalComponentKeys = {
  "k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9", "k10", "k11", "k12"};

const std::vector<G4String> BDSMagnetStrength::skewComponentKeys = {
  "k1s", "k2s", "k3s", "k4s", "k5s", "k6s", "k7s", "k8s", "k9s", "k10s", "k11s", "k12s"};

const G4double BDSMagnetStrength::zero     = 0.0;
G4double       BDSMagnetStrength::variable = 0.0;

BDSMagnetStrength::BDSMagnetStrength(const std::map<G4String, G4double>& sts)
{
  for (const auto&  keyValue : sts)
    {
      if (ValidKey(keyValue.first))
	{(*this)[keyValue.first] = keyValue.second;}
    }
}

std::ostream& operator<<(std::ostream& out, BDSMagnetStrength const &st)
{
  for (const auto& key : BDSMagnetStrength::keys)
    {out << key << ": " << st.GetValue(key) << ", ";}
  return out;
}

std::ostream& BDSMagnetStrength::WriteValuesInSIUnitsForSuvey(std::ostream& out,
							      const G4int precision) const
{
  for (auto& key : keys)
    {out << " " << std::setw(precision) << GetValue(key) / unitsFactorsMap.at(key).factor;}
  return out;
}

G4String BDSMagnetStrength::UnitName(const G4String& key)
{
  if (ValidKey(key))
    {return unitsFactorsMap.at(key).unit;}
  else
    {throw BDSException(__METHOD_NAME__, "Invalid key \"" + key + "\"");}
}

G4double BDSMagnetStrength::Unit(const G4String& key)
{
  if (ValidKey(key))
    {return unitsFactorsMap.at(key).factor;}
  else
    {throw BDSException(__METHOD_NAME__, "Invalid key \"" + key + "\"");}
}

G4double& BDSMagnetStrength::operator[](const G4String& key)
{
  if (ValidKey(key))
    {
      // check if this key is initialised (non const version of this operator)
      // if it's not, initialise it so assignment will work
      auto it = strengths.find(key);
      if (it != strengths.end())
	{return it->second;}
      else
	{
	  strengths[key] = 0.0;
	  return strengths[key];
	}
    }
  else
    {throw BDSException(__METHOD_NAME__, "Invalid key \"" + key + "\"");}
}

const G4double& BDSMagnetStrength::operator[](const G4String& key) const
{
  if (ValidKey(key))
    {return GetValue(key);}
  else
    {throw BDSException(__METHOD_NAME__, "Invalid key \"" + key + "\"");}
}

std::vector<G4double> BDSMagnetStrength::NormalComponents() const
{
  std::vector<G4double> result;
  result.reserve(normalComponentKeys.size());
  for (const auto& key : normalComponentKeys)
    {result.push_back(GetValue(key));}
  return result;
}

std::vector<G4double> BDSMagnetStrength::SkewComponents() const
{
  std::vector<G4double> result;
  result.reserve(skewComponentKeys.size());
  for (const auto& key : skewComponentKeys)
    {result.push_back(GetValue(key));}
  return result;
}

G4bool BDSMagnetStrength::ValidKey(const G4String& key)
{
  if (std::find(keys.begin(), keys.end(), key) != keys.end())
    {return true;}
  else
    {return false;}
}

const G4double& BDSMagnetStrength::GetValue(const G4String& key) const
{
  auto it = strengths.find(key);
  if (it != strengths.end())
    {return it->second;}
  else
    {return zero;}
}

G4bool BDSMagnetStrength::KeyHasBeenSet(const G4String& key) const
{
  return strengths.find(key) != strengths.end();
}