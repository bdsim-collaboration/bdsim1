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
#include "BDSCollimatorBeamMask.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4UnionSolid.hh"


BDSCollimatorBeamMask::BDSCollimatorBeamMask(const G4String& nameIn,
                                                   G4double    lengthIn,
                                                   G4double    horizontalWidthIn,
                                                   G4Material* collimatorMaterialIn,
                                                   G4Material* vacuumMaterialIn,
                                                   G4double    xApertureIn,
                                                   G4double    yApertureIn,
                                                   G4double    xApertureSlitIn,
                                                   G4double    yApertureSlitIn,
                                                   G4double    xOffsetSlitIn,
                                                   G4double    yOffsetSlitIn,
                                                   G4double    tiltSlitIn,
                                                   G4Colour*   colourIn,
                                                   G4bool      circularOuterIn):
  xApertureSlit(xApertureSlitIn),
  yApertureSlit(yApertureSlitIn),
  xOffsetSlit(xOffsetSlitIn),
  yOffsetSlit(yOffsetSlitIn),
  tiltSlit(tiltSlitIn),
  BDSCollimator(nameIn, lengthIn, horizontalWidthIn, "bmcol",
                collimatorMaterialIn, vacuumMaterialIn, xApertureIn,
                yApertureIn, 0, 0, colourIn, circularOuterIn)
{;}

void BDSCollimatorBeamMask::BuildInnerCollimator()
{
  G4Box* inner1 = new G4Box(name + "_inner_solid_1",    // name
                            xAperture,                // x half width
                            yAperture,                // y half width
                            chordLength);             // z half length
  // z half length long for unambiguous subtraction

  G4Box* inner2 = new G4Box(name + "_inner_solid_2",    // name
                            xApertureSlit,                // x half width
                            yApertureSlit,                // y half width
                            chordLength);             // z half length
  // z half length long for unambiguous subtraction

  G4ThreeVector Trans(xOffsetSlit, yOffsetSlit, 0);
  G4RotationMatrix* Rot = new G4RotationMatrix;
  Rot->rotateZ(tiltSlit);

  innerSolid = new G4UnionSolid(name + "_inner_solid_union", inner1, inner2, Rot, Trans);

  G4Box* vacuum1 = new G4Box(name + "_vacuum_solid_1",   // name
                             xAperture - lengthSafety, // x half width
                             yAperture - lengthSafety, // y half width
                             chordLength*0.5);         // z half length

  G4Box* vacuum2 = new G4Box(name + "_vacuum_solid_2",   // name
                             xApertureSlit - lengthSafety, // x half width
                             yApertureSlit - lengthSafety, // y half width
                             chordLength*0.5);         // z half length

  vacuumSolid = new G4UnionSolid(name + "_vacuum_solid_union", vacuum1, vacuum2, Rot, Trans);
    
  RegisterSolid(innerSolid);
  RegisterSolid(vacuumSolid);
}
