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
#ifndef BDSINTEGRATORTYPE_H
#define BDSINTEGRATORTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Version.hh"

/**
 * @brief Type definition for integrators.
 * 
 * @author Laurie Nevay
 */

struct integratortypes_def
{

  enum type {none, teleporter, solenoid, dipolerodrigues, dipolerodrigues2,
	     dipolematrix, quadrupole, sextupole,
	     multipolethin, octupole, decapole, dipolefringe, dipolefringescaling,
	     euler, kickerthin, g4rk4minimumstep,
	     g4cashkarprkf45, g4classicalrk4, g4constrk4, g4expliciteuler,
	     g4impliciteuler, g4simpleheum, g4simplerunge,
	     g4exacthelixstepper, g4helixexpliciteuler, g4helixheum,
	     g4heliximpliciteuler, g4helixmixedstepper, g4helixsimplerunge,
	     g4nystromrk4, g4rkg3stepper, rmatrixthin, paralleltransport,
	     cavityfringe, gaborlens
#if G4VERSION_NUMBER > 1029
	     // introduced in version 10.3
	     , g4bogackishampine23, g4bogackishampine45, g4dolomcprirk34,
	     g4dormandprince745, g4dormandprincerk56, g4tsitourasrk45
#endif
#if G4VERSION_NUMBER > 1039
	     // introduced in version 10.4
	     , g4dormandprincerk78, g4rk547feq1, g4rk547feq2, g4rk547feq3
#endif
  };
};

typedef BDSTypeSafeEnum<integratortypes_def,int> BDSIntegratorType;

namespace BDS
{
  /// Function that determines enum from string (case-insensitive).
  BDSIntegratorType DetermineIntegratorType(G4String integratorType);
}

#endif
