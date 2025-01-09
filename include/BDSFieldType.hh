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
#ifndef BDSFIELDTYPE_H
#define BDSFIELDTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for field formats - 
 * used for comparison in factory methods.
 *
 * dipole   - a uniform field assumed along local unit Y
 * dipole3d - a uniform field specified by a unit vector and field magnitude
 * multipoleouterdipole3d - orientates itself to follow dipole3d
 * 
 * @author Laurie Nevay
 */

struct fieldtypes_def
{
  // we use "bfieldzero" etc instead of "bzero" because "bzero" exists
  // in strings.h in the macosx system SDK globals
  enum type {none,
	     bfieldzero, efieldzero, ebfieldzero,
	     teleporter,
	     bmap1d,  bmap2d,  bmap3d,  bmap4d,
	     emap1d,  emap2d,  emap3d,  emap4d,
	     ebmap1d, ebmap2d, ebmap3d, ebmap4d,
	     mokka,
	     solenoid, solenoidsheet, solenoidloop, solenoidblock,
	     dipole, quadrupole, dipolequadrupole, sextupole,
	     octupole, decapole, multipole, muonspoiler, muoncooler,
             skewquadrupole, skewsextupole, skewoctupole, skewdecapole,
	     rfpillbox, rfconstantinx, rfconstantiny, rfconstantinz, cavityfringe,
	     rmatrix, paralleltransporter, undulator,
             dipole3d,
	     multipoleouterdipole, multipoleouterquadrupole,
	     multipoleoutersextupole, multipoleouteroctupole,
	     multipoleouterdecapole,
	     skewmultipoleouterquadrupole, skewmultipoleoutersextupole,
	     skewmultipoleouteroctupole, skewmultipoleouterdecapole,
	     multipoleouterdipole3d,
	     multipoleouterdipolelhc, multipoleouterquadrupolelhc,
	     multipoleoutersextupolelhc
  };
};

// NOTE - when adding a new field type:
//  - BDSFieldClassType should also be updated
//  - BDSIntegratorSet::Integrator() should also be updated

// NOTE - also add to manual/source/dev_fields.rst -> Field Names and Parameters

typedef BDSTypeSafeEnum<fieldtypes_def,int> BDSFieldType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSFieldType DetermineFieldType(G4String fieldType);
}

#endif


