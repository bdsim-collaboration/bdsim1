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
#ifndef BDSCOLLIMATORBEAMMASK_H
#define BDSCOLLIMATORBEAMMASK_H

#include "BDSCollimator.hh"

class G4Colour;
class G4Material;

/**
 * @brief A class for a beam mask collimator.
 */

class BDSCollimatorBeamMask: public BDSCollimator
{
public:
    BDSCollimatorBeamMask(const G4String& name,
			   G4double    length,
			   G4double    horizontalWidth,
			   G4Material* collimatorMaterial,
			   G4Material* vacuumMaterial,
			   G4double    xAperture    = 0,
			   G4double    yAperture    = 0,
			   G4double    xOutAperture = 0,
			   G4double    yOutAperture = 0,
			   G4Colour*   colour       = nullptr,
			   G4bool      circularOuter = false);
  virtual ~BDSCollimatorBeamMask(){;};

  virtual void BuildInnerCollimator();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorBeamMask();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorBeamMask& operator=(const BDSCollimatorBeamMask&) = delete;
    BDSCollimatorBeamMask(BDSCollimatorBeamMask&) = delete;
  ///@}
};

#endif
