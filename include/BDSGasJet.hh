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
#ifndef BDSGASJET_H
#define BDSGASJET_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class G4Colour;
class G4Material;
class G4VSolid;

/**
 * @brief A class for a gas jet.
 *
 * @author Marin Deniaud
 */

class BDSGasJet: public BDSAcceleratorComponent
{
public:
    BDSGasJet(const G4String&  name,
              G4double         length,
              BDSBeamPipeInfo* beamPipeInfoIn,
              G4Material*      gasMaterial,
              G4double         xSize     = 0,
              G4double         ySize     = 0,
              G4double         zSize     = 0,
              G4double         xAngle    = 0,
              G4double         yAngle    = 0,
              G4double         zAngle    = 0,
              G4double         xOffset   = 0,
              G4double         yOffset   = 0);
  virtual ~BDSGasJet(){;};

  /// @{ Accessor.
  virtual G4String Material() const;
  /// @}

protected:
  /// Check and update parameters before construction. Called at the start of Build() as
  /// we can't call a virtual function in a constructor.
  virtual void CheckParameters();
  virtual void BuildContainerLogicalVolume();
  virtual void Build();

  ///@{ Geometrical objects:
  G4VSolid* gasSolid;
  ///@}

  G4Material* gasMaterial;     ///< Gas material.
  G4double    xSize;           ///< Aperture of main slit in x dimension.
  G4double    ySize;           ///< Aperture of main slit in y dimension.
  G4double    zSize;           ///< Aperture of side slit in x dimension.
  G4double    xAngle;          ///< Aperture of side slit in y dimension.
  G4double    yAngle;          ///< Offset of the mask in the pipe in x.
  G4double    zAngle;          ///< Offset of the mask in the pipe in y.
  G4double    xOffset;     ///< Offset of the side slit wrt the main slit in x.
  G4double    yOffset;     ///< Offset of the side slit wrt the main slit in y.





private:
  /// Private default constructor to force the use of the supplied one.
  BDSGasJet() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSGasJet& operator=(const BDSGasJet&) = delete;
  BDSGasJet(BDSGasJet&) = delete;
  ///@}
};

#endif
