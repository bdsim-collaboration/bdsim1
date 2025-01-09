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
#ifndef BDSMUONCOOLERSTRUCTS_H
#define BDSMUONCOOLERSTRUCTS_H

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <vector>

class BDSFieldInfo;
class G4Material;

namespace BDS
{
  struct MuonCoolerCoilInfo
  {
    G4double innerRadius;
    G4double radialThickness;
    G4double fullLengthZ;
    G4double current;
    G4double offsetZ;
    G4Material* material;
    G4double onAxisTolerance;
    G4int   nSheets;
  };

  struct MuonCoolerCavityInfo
  {
    G4double    offsetZ;
    G4double    lengthZ;
    G4double    peakEField;
    G4double    phaseOffset;
    G4double    frequency;
    G4Material* vacuumMaterial;
    G4double    windowThickness;
    G4Material* windowMaterial;
    G4double    windowRadius;
    G4Material* cavityMaterial;
    G4double    cavityRadius;
    G4double    cavityThickness;
    G4double    globalTimeOffset;
  };

  struct MuonCoolerAbsorberInfo
  {
    G4String absorberType;
    G4double cylinderLength;
    G4double cylinderRadius;
    G4double wedgeOpeningAngle;
    G4double wedgeHeight;
    G4double wedgeRotationAngle;
    G4ThreeVector placement;
    G4double wedgeApexToBase;
    G4Material* material;
  };
}

#endif
