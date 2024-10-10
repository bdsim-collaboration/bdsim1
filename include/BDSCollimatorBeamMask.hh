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
 *
 * @author Marin Deniaud based on BDSCollimatorRectangular
 */

class BDSCollimatorBeamMask: public BDSCollimator
{
public:
    BDSCollimatorBeamMask(const G4String& name,
			   G4double    length,
			   G4double    horizontalWidth,
			   G4Material* collimatorMaterial,
			   G4Material* vacuumMaterial,
			   G4double    xApertureIn    = 0,
			   G4double    yApertureIn    = 0,
			   G4double    xApertureSlitIn = 0,
			   G4double    yApertureSlitIn = 0,
               G4double    xOffsetSlitIn = 0,
               G4double    yOffsetSlitIn = 0,
               G4double    tiltSlitIn = 0,
			   G4Colour*   colourIn       = nullptr,
			   G4bool      circularOuterIn = false);
  virtual ~BDSCollimatorBeamMask(){;};

  virtual void BuildInnerCollimator();

protected:
  G4double xApertureSlit;
  G4double yApertureSlit;
  G4double xOffsetSlit;
  G4double yOffsetSlit;
  G4double tiltSlit;

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorBeamMask();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorBeamMask& operator=(const BDSCollimatorBeamMask&) = delete;
    BDSCollimatorBeamMask(BDSCollimatorBeamMask&) = delete;
  ///@}
};

#endif
