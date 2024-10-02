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
#ifndef BDSBEAMMASK_H
#define BDSBEAMMASK_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class G4Colour;

/**
 * @brief An arrangement of slits.
 *
 * @author Marin Deniaud
 */

class BDSBeamMask: public BDSAcceleratorComponent
{
public:
    BDSBeamMask(G4String nameIn,
	            G4double lengthIn,
	            G4double horizontalWidthIn,
                G4Material* solidMaterial,
                G4Material* vacuumMaterial,
                G4double    xAperture1   = 0,
                G4double    xAperture2   = 0,
                G4double    xGap         = 0,
                G4Colour*   colour       = nullptr);
  
  virtual ~BDSBeamMask();

    /// @{ Accessor.
    virtual G4String Material() const;
    /// @}

protected:
  /// Call default build then override visualisation attributes.
  virtual void Build() override;

  /// Build a simple box or cylinder.
  virtual void BuildContainerLogicalVolume() override;

  /// Customised user limits.
  virtual void BuildUserLimits() override;

private:
  /// No default constructor.
  BDSBeamMask() = delete;

  ///@{ Geometrical objects:
  G4VSolid* collimatorSolid;
  G4VSolid* innerSolid;
  G4VSolid* vacuumSolid;
  ///@}

  G4double    horizontalWidth;    ///< Horizontal width.
  G4Material* solidMaterial;      ///< Material.
  G4Material* vacuumMaterial;     ///< Vacuum material.
  G4double    xAperture1;         ///< First aperture on beam axis.
  G4double    xAperture2;         ///< Second aperture off beam axis.
  G4double    xGap;               ///< Gap between the two apertures.
  G4Colour*   colour;             ///< Colour of beam mask.
  
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBeamMask& operator=(const BDSBeamMask&) = delete;
  BDSBeamMask(BDSBeamMask&) = delete;
  /// @}
};

#endif
