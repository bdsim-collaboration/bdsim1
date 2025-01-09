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

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class G4Colour;
class G4Material;

/**
 * @brief A class for a beam mask collimator.
 *
 * @author Marin Deniaud based on BDSCollimatorRectangular
 */

class BDSCollimatorBeamMask: public BDSAcceleratorComponent
{
public:
    BDSCollimatorBeamMask(const G4String&  name,
                          G4double         length,
                          BDSBeamPipeInfo* beamPipeInfoIn,
                          G4double         horizontalWidth,
                          G4Material*      collimatorMaterial,
                          G4Material*      vacuumMaterial,
                          G4double         xApertureIn     = 0,
                          G4double         yApertureIn     = 0,
                          G4double         xApertureSlitIn = 0,
                          G4double         yApertureSlitIn = 0,
                          G4double         xOffsetIn       = 0,
                          G4double         yOffsetIn       = 0,
                          G4double         xOffsetSlitIn   = 0,
                          G4double         yOffsetSlitIn   = 0,
                          G4double         tiltSlitIn      = 0,
                          G4Colour*        colourIn       = nullptr,
                          G4bool           circularOuterIn = false);
  virtual ~BDSCollimatorBeamMask(){;};

  /// @{ Accessor.
  virtual G4String Material() const;
  virtual G4double XAperture()  const {return xAperture;}
  virtual G4double XApertureSlit() const {return xApertureSlit;}
  virtual G4double YAperture()  const {return yAperture;}
  virtual G4double YApertureSlit() const {return yApertureSlit;}
  /// @}

protected:
  /// Check and update parameters before construction. Called at the start of Build() as
  /// we can't call a virtual function in a constructor.
  virtual void CheckParameters();

  virtual void BuildContainerLogicalVolume();
  virtual void BuildInner();
  virtual void Build();

  ///@{ Geometrical objects:
  G4VSolid* collimatorSolid;
  G4VSolid* innerSolid;
  G4VSolid* vacuumSolid;
  ///@}

  G4double    horizontalWidth;    ///< Horizontal width.
  G4Material* collimatorMaterial; ///< Material.
  G4Material* vacuumMaterial;     ///< Vacuum material.
  G4double    xAperture;          ///< Aperture of main slit in x dimension.
  G4double    yAperture;          ///< Aperture of main slit in y dimension.
  G4double    xApertureSlit;      ///< Aperture of side slit in x dimension.
  G4double    yApertureSlit;      ///< Aperture of side slit in y dimension.
  G4double    xOffset;            ///< Offset of the mask in the pipe in x.
  G4double    yOffset;            ///< Offset of the mask in the pipe in y.
  G4double    xOffsetSlit;        ///< Offset of the side slit wrt the main slit in x.
  G4double    yOffsetSlit;        ///< Offset of the side slit wrt the main slit in y.
  G4double    tiltSlit;           ///< Angle of the side slit.
  G4Colour*   colour;             ///< Colour of beam mask.
  G4bool      circularOuter;      ///< Shape of the outer solid




private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorBeamMask();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorBeamMask& operator=(const BDSCollimatorBeamMask&) = delete;
    BDSCollimatorBeamMask(BDSCollimatorBeamMask&) = delete;
  ///@}
};

#endif
