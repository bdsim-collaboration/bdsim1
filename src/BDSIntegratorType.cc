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
#include "BDSIntegratorType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSIntegratorType for reflexivity
template<>
std::map<BDSIntegratorType, std::string>* BDSIntegratorType::dictionary =
  new std::map<BDSIntegratorType, std::string> ({
      {BDSIntegratorType::none,                 "none"},
      {BDSIntegratorType::teleporter,           "teleporter"},
      {BDSIntegratorType::solenoid,             "solenoid"},
      {BDSIntegratorType::dipolerodrigues,      "dipolerodrigues"},
      {BDSIntegratorType::dipolerodrigues2,     "dipolerodrigues2"},
      {BDSIntegratorType::dipolematrix,         "dipolematrix"},
      {BDSIntegratorType::quadrupole,           "quadrupole"},
      {BDSIntegratorType::sextupole,            "sextupole"},
      {BDSIntegratorType::multipolethin,        "multipolethin"},
      {BDSIntegratorType::octupole,             "octupole"},
      {BDSIntegratorType::decapole,             "decapole"},
      {BDSIntegratorType::dipolefringe,         "dipolefringe"},
      {BDSIntegratorType::dipolefringescaling,  "dipolefringescaling"},
      {BDSIntegratorType::euler,                "euler"},
      {BDSIntegratorType::kickerthin,           "kickerthin"},
      {BDSIntegratorType::rmatrixthin,          "rmatrixthin"},
      {BDSIntegratorType::g4rk4minimumstep,     "g4rk4minimumstep"},
      {BDSIntegratorType::g4cashkarprkf45,      "g4cashkarprkf45"},
      {BDSIntegratorType::g4classicalrk4,       "g4classicalrk4"},
      {BDSIntegratorType::g4constrk4,           "g4constrk4"},
      {BDSIntegratorType::g4expliciteuler,      "g4expliciteuler"},
      {BDSIntegratorType::g4impliciteuler,      "g4impliciteuler"},
      {BDSIntegratorType::g4simpleheum,         "g4simpleheum"},
      {BDSIntegratorType::g4simplerunge,        "g4simplerunge"},
      {BDSIntegratorType::g4exacthelixstepper,  "g4exacthelixstepper"},
      {BDSIntegratorType::g4helixexpliciteuler, "g4helixexpliciteuler"},
      {BDSIntegratorType::g4helixheum,          "g4helixheum"},
      {BDSIntegratorType::g4heliximpliciteuler, "g4heliximpliciteuler"},
      {BDSIntegratorType::g4helixmixedstepper,  "g4helixmixedstepper"},
      {BDSIntegratorType::g4helixsimplerunge,   "g4helixsimplerunge"},
      {BDSIntegratorType::g4nystromrk4,         "g4nystromrk4"},
      {BDSIntegratorType::g4rkg3stepper,        "g4rkg3stepper"},
      {BDSIntegratorType::rmatrixthin,          "rmatrixthin"},
      {BDSIntegratorType::paralleltransport,    "paralleltransport"},
      {BDSIntegratorType::cavityfringe,         "cavityfringe"},
      {BDSIntegratorType::gaborlens,            "g4classicalrk4"}
#if G4VERSION_NUMBER > 1029
      // introduced in version 10.3
      ,
      {BDSIntegratorType::g4bogackishampine23,  "g4bogackishampine23"},
      {BDSIntegratorType::g4bogackishampine45,  "g4bogackishampine45"},
      {BDSIntegratorType::g4dolomcprirk34,      "g4dolomcprirk34"},
      {BDSIntegratorType::g4dormandprince745,   "g4dormandprince745"},
      {BDSIntegratorType::g4dormandprincerk56,  "g4dormandprincerk56"},
      {BDSIntegratorType::g4tsitourasrk45,      "g4tsitourasrk45"}
#endif
#if G4VERSION_NUMBER > 1039
      // introduced in version 10.4
      // g4dormandprincerk78 is broken in 10.3 but fixed in 10.4 - safe to use now
      ,
      {BDSIntegratorType::g4dormandprincerk78,  "g4dormandprincerk78"},
      {BDSIntegratorType::g4rk547feq1,          "g4rk547feq1"},
      {BDSIntegratorType::g4rk547feq2,          "g4rk547feq2"},
      {BDSIntegratorType::g4rk547feq3,          "g4rk547feq3"}
#endif
    });

BDSIntegratorType BDS::DetermineIntegratorType(G4String integratorType)
{
  std::map<G4String, BDSIntegratorType> types;
  types["none"]                 = BDSIntegratorType::none;
  types["teleporter"]           = BDSIntegratorType::teleporter;
  types["solenoid"]             = BDSIntegratorType::solenoid;
  types["dipolerodrigues"]      = BDSIntegratorType::dipolerodrigues;
  types["dipolerodrigues2"]     = BDSIntegratorType::dipolerodrigues2;
  types["dipolematrix"]         = BDSIntegratorType::dipolematrix;
  types["quadrupole"]           = BDSIntegratorType::quadrupole;
  types["sextupole"]            = BDSIntegratorType::sextupole;
  types["multipolethin"]        = BDSIntegratorType::multipolethin;
  types["octupole"]             = BDSIntegratorType::octupole;
  types["decapole"]             = BDSIntegratorType::decapole;
  types["dipolefringe"]         = BDSIntegratorType::dipolefringe;
  types["dipolefringescaling"]  = BDSIntegratorType::dipolefringescaling;
  types["euler"]                = BDSIntegratorType::euler;
  types["kickerthin"]           = BDSIntegratorType::kickerthin;
  types["g4rk4minimumstep"]     = BDSIntegratorType::g4rk4minimumstep;
  types["g4cashkarprkf45"]      = BDSIntegratorType::g4cashkarprkf45;
  types["g4classicalrk4"]       = BDSIntegratorType::g4classicalrk4;
  types["g4constrk4"]           = BDSIntegratorType::g4constrk4;
  types["g4expliciteuler"]      = BDSIntegratorType::g4expliciteuler;
  types["g4impliciteuler"]      = BDSIntegratorType::g4impliciteuler;
  types["g4simpleheum"]         = BDSIntegratorType::g4simpleheum;
  types["g4simplerunge"]        = BDSIntegratorType::g4simplerunge;
  types["g4exacthelixstepper"]  = BDSIntegratorType::g4exacthelixstepper;
  types["g4helixexpliciteuler"] = BDSIntegratorType::g4helixexpliciteuler;
  types["g4helixheum"]          = BDSIntegratorType::g4helixheum;
  types["g4heliximpliciteuler"] = BDSIntegratorType::g4heliximpliciteuler;
  types["g4helixmixedstepper"]  = BDSIntegratorType::g4helixmixedstepper;
  types["g4helixsimplerunge"]   = BDSIntegratorType::g4helixsimplerunge;
  types["g4nystromrk4"]         = BDSIntegratorType::g4nystromrk4;
  types["g4rkg3stepper"]        = BDSIntegratorType::g4rkg3stepper;
  types["rmatrixthin"]          = BDSIntegratorType::rmatrixthin;
  types["paralleltransport"]    = BDSIntegratorType::paralleltransport;
  types["cavityfringe"]         = BDSIntegratorType::cavityfringe;
  types["gaborlens"]            = BDSIntegratorType::g4classicalrk4;
#if G4VERSION_NUMBER > 1029
  // introduced in version 10.3
  types["g4bogackishampine23"]  = BDSIntegratorType::g4bogackishampine23;
  types["g4bogackishampine45"]  = BDSIntegratorType::g4bogackishampine45;
  types["g4dolomcprirk34"]      = BDSIntegratorType::g4dolomcprirk34;
  types["g4dormandprince745"]   = BDSIntegratorType::g4dormandprince745;
  types["g4dormandprincerk56"]  = BDSIntegratorType::g4dormandprincerk56;
  types["g4tsitourasrk45"]      = BDSIntegratorType::g4tsitourasrk45;
#endif
#if G4VERSION_NUMBER > 1039
  // introduced in version 10.4
  types["g4dormandprincerk78"]  = BDSIntegratorType::g4dormandprincerk78;
  types["g4rk547feq1"]          = BDSIntegratorType::g4rk547feq1;
  types["g4rk547feq2"]          = BDSIntegratorType::g4rk547feq2;
  types["g4rk547feq3"]          = BDSIntegratorType::g4rk547feq3;
#endif

  integratorType = BDS::LowerCase(integratorType);
  
  auto result = types.find(integratorType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + integratorType + "\" is not a valid integrator type\n";
      msg += "Available integrator types are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined integrator type to be " << result->second << G4endl;
#endif
  return result->second;
}
