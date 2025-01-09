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
#ifndef BDSLASERWIRENEW_H
#define BDSLASERWIRENEW_H
#include "BDSAcceleratorComponent.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class BDSBeamPipeInfo;
class BDSLaser;

class G4Colour;
class G4LogicalVolume;
class G4Material;
class G4VSolid;

/**
 * @brief Laserwire in beam pipe.
 * 
 * @author Laurie Nevay, Siobhan Alden
 */

class BDSLaserWireNew: public BDSAcceleratorComponent
{
public:
  BDSLaserWireNew(G4String         nameIn,
		  G4double         lengthIn,
		  BDSBeamPipeInfo* beamPipeInfoIn,
		  BDSLaser*        laserIn,
		  G4double         wireDiameterIn,
		  G4double         wireLengthIn,
		  G4double         wireAngleIn             = 0,
		  G4double         wireLongitudinalAngleIn = 0,
		  G4ThreeVector    wireOffsetIn            = G4ThreeVector(),
		  G4Colour*        wireColourIn            = nullptr);
  virtual ~BDSLaserWireNew();

protected:
  virtual G4LogicalVolume* BuildWireLV(G4VSolid* solid);
  virtual void             Build();
  virtual void             BuildContainerLogicalVolume();
  virtual G4VSolid* 	   BuildHyperbolicWireSolid();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSLaserWireNew() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSLaserWireNew& operator=(const BDSLaserWireNew&) = delete;
  BDSLaserWireNew(BDSLaserWireNew&) = delete;
  ///@}
  
  const BDSLaser* laser;
  G4Material*   wireMaterial;
  G4double      wireDiameter;
  G4double      wireLength;
  G4double      wireAngle;
  G4double      wireLongitudinalAngle;
  G4ThreeVector wireOffset;
  G4Colour*     wireColour;
};

#endif
