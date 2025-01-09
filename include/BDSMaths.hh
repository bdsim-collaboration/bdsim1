/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSMATHS_H
#define BDSMATHS_H

#include "G4Types.hh"

namespace BDS
{
  /// Generalised Complete Elliptical Integral. This uses the algorithm for the
  /// generalised complete elliptical integral as described in (rewritten in C++ here):
  /// Cylindrical Magnets and ideal Solenoids, N. Derby and S. Olbert, American Journal of
  /// Physics **78**, 229 (2010); https://doi.org/10.1119/1.3256157 and also at
  /// https://arxiv.org/abs/0909.3880.
  G4double CEL(G4double kc,
               G4double p,
               G4double c,
               G4double s,
               G4int nIterationLimit = 1000);
};

#endif
