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
#ifndef BDSCAVITYINFO_H
#define BDSCAVITYINFO_H

#include "BDSCavityType.hh"

#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

class G4Material;

/**
 * @brief Holder for all Geometrical information required to create an RF cavity. 
 * 
 * Geant4 units are assumed by classes that use instances of this class.
 * This reduces the number of argument to constructors 
 * plus aggregates common tasks in the component factory.
 *
 * Note, compiler provided copy constructor is sufficient as only pointers
 * to materials owned by BDSMaterials are kept - shallow copy is required then.
 *
 * This class does not specify the vacuum material as ideally we can specify the
 * the 'vacuum' pressure and composition throughout the machine without redefining
 * a cavity model.
 *
 * Optional 'windows' that fit the input / output apertures are included as member
 * variables. Only if the material is not nullptr will they be built and then the
 * corresponding thickness variable needs to be > 0.
 * 
 * @author Laurie Nevay
 */

class BDSCavityInfo
{
public:
  /// No default constructor.
  BDSCavityInfo() = delete;
  /// Constructor to assign all members at once. Default values are based
  /// on those for SRF cavity.
  BDSCavityInfo(BDSCavityType cavityTypeIn,
		G4Material*   materialIn,
		G4double      irisRadiusIn,
		G4double      thicknessIn,
		G4double      equatorRadiusIn,
		G4double      halfCellLengthIn,
		G4int         numberOfPointsIn         = 24,
		G4int         numberOfCellsIn          = 1,
		G4double      equatorHorizontalAxisIn  = 42*CLHEP::mm,
		G4double      equatorVerticalAxisIn    = 42*CLHEP::mm,
		G4double      irisHorizontalAxisIn     = 12*CLHEP::mm,
		G4double      irisVerticalAxisIn       = 19*CLHEP::mm,
		G4double      tangentLineAngleIn       = 13.3*CLHEP::degree);
  ~BDSCavityInfo() = default;
  
  BDSCavityType cavityType; ///< Cavity type.  
  G4Material* material;     ///< Material.
  G4double irisRadius;      ///< Iris radius - half width of narrowest part.
  G4double thickness;       ///< Thickness of wall material.
  G4double equatorRadius;   ///< Equator radius - half width of widest part.
  G4double halfCellLength;  ///< Half cell length.

  /// Number of points that will be used to make the cell shape
  /// more will be more accurate but will also increase tracking time.
  G4int numberOfPoints;
  
  G4int    numberOfCells;         ///< Number of cells per cavity.
  G4double equatorHorizontalAxis; ///< Equator ellipse horizontal semi-axis.
  G4double equatorVerticalAxis;   ///< Equator ellipse vertical semi-axis.
  G4double irisHorizontalAxis;    ///< Iris ellipse horizontal semi-axis.
  G4double irisVerticalAxis;      ///< Iris ellipse vertical semi-axis.
  G4double tangentLineAngle;      ///< Tangent angle.

  /// If the material is not nullptr then the window is built.
  G4Material* inputWindowMaterial;
  G4double    inputWindowThickness;
  G4Material* outputWindowMaterial;
  G4double    outputWindowThickness;
};

#endif
