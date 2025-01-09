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
#ifndef BDSWIRESCANNER_H
#define BDSWIRESCANNER_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"

class BDSBeamPipeInfo;
class G4Colour;
class G4LogicalVolume;
class G4Material;
class G4VSolid;

/**
 * @brief Single cylindrical wire inside beam pipe.
 * 
 * @author Max Porter
 */

class BDSWireScanner: public BDSAcceleratorComponent
{
public:
  BDSWireScanner(G4String         nameIn,
		 G4double         lengthIn,
		 BDSBeamPipeInfo* beamPipeInfoIn,
		 G4Material*      wireMaterialIn,
		 G4double         wireDiameterIn,
		 G4double         wireLengthIn,
		 G4double         wireAngleIn  = 0,
		 G4ThreeVector    wireOffsetIn = G4ThreeVector(),
		 G4Colour*        wireColourIn = nullptr

  );

  virtual ~BDSWireScanner(){;}

  /// Return the name of a material - in this case the wire is the most relevant.
  virtual G4String Material() const {return wireMaterial->GetName();}
  
protected:
  virtual void Build();  
  virtual void BuildContainerLogicalVolume();

  /// @{ Allow overriding of certain bits of construction.
  virtual G4VSolid*        BuildWireSolid();
  virtual G4LogicalVolume* BuildWireLV(G4VSolid* solid);
  /// @}

  G4Material*   wireMaterial;
  G4double      wireDiameter;
  G4double      wireLength;
  G4double      wireAngle;
  G4ThreeVector wireOffset;
  G4Colour*     wireColour;

private:
  /// Private default constructor to force the use of the supplied one.
  BDSWireScanner() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSWireScanner& operator=(const BDSWireScanner&) = delete;
  BDSWireScanner(BDSWireScanner&) = delete;
  ///@}
};

#endif
