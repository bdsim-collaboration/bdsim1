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
#ifndef BDSBUNCHTYPE_H
#define BDSBUNCHTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for bunch distributions - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay
 */

struct bunchtypes_def
{
  enum type {reference, gaussmatrix, gauss, gausstwiss, circle, square, ring, eshell,
	     halo, composite, userfile, ptc, sixtrack, eventgeneratorfile, sphere,
	     compositesde, box, bdsimsampler, halosigma, hdf5};
};

typedef BDSTypeSafeEnum<bunchtypes_def,int> BDSBunchType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive).
  BDSBunchType DetermineBunchType(G4String distrType);
}

#endif
