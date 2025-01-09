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
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"

#include "globals.hh"

BDSCavityInfo::BDSCavityInfo(BDSCavityType cavityTypeIn,
			     G4Material*   materialIn,
			     G4double      irisRadiusIn,
			     G4double      thicknessIn,
			     G4double      equatorRadiusIn,
			     G4double      halfCellLengthIn,
			     G4int         numberOfPointsIn,
			     G4int         numberOfCellsIn,
			     G4double      equatorHorizontalAxisIn,
			     G4double      equatorVerticalAxisIn,
			     G4double      irisHorizontalAxisIn,
			     G4double      irisVerticalAxisIn,
			     G4double      tangentLineAngleIn):
  cavityType(cavityTypeIn),
  material(materialIn),
  irisRadius(irisRadiusIn),
  thickness(thicknessIn),
  equatorRadius(equatorRadiusIn),
  halfCellLength(halfCellLengthIn),
  numberOfPoints(numberOfPointsIn),
  numberOfCells(numberOfCellsIn),
  equatorHorizontalAxis(equatorHorizontalAxisIn),
  equatorVerticalAxis(equatorVerticalAxisIn),
  irisHorizontalAxis(irisHorizontalAxisIn),
  irisVerticalAxis(irisVerticalAxisIn),
  tangentLineAngle(tangentLineAngleIn),
  inputWindowMaterial(nullptr),
  inputWindowThickness(0),
  outputWindowMaterial(nullptr),
  outputWindowThickness(0)
{;}
