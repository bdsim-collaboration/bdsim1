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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"


BDSCollimatorBeamMask::BDSCollimatorBeamMask(const G4String& nameIn,
                                             G4double    lengthIn,
                                             BDSBeamPipeInfo* beamPipeInfoIn,
                                             G4double    horizontalWidthIn,
                                             G4Material* collimatorMaterialIn,
                                             G4Material* vacuumMaterialIn,
                                             G4double    xApertureIn,
                                             G4double    yApertureIn,
                                             G4double    xApertureSlitIn,
                                             G4double    yApertureSlitIn,
                                             G4double    xOffsetIn,
                                             G4double    yOffsetIn,
                                             G4double    xOffsetSlitIn,
                                             G4double    yOffsetSlitIn,
                                             G4double    tiltSlitIn,
                                             G4Colour*   colourIn,
                                             G4bool      circularOuterIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "bmcol", beamPipeInfoIn),
  collimatorSolid(nullptr),
  innerSolid(nullptr),
  vacuumSolid(nullptr),
  horizontalWidth(horizontalWidthIn),
  collimatorMaterial(collimatorMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  xAperture(xApertureIn),
  yAperture(yApertureIn),
  xApertureSlit(xApertureSlitIn),
  yApertureSlit(yApertureSlitIn),
  xOffset(xOffsetIn),
  yOffset(yOffsetIn),
  xOffsetSlit(xOffsetSlitIn),
  yOffsetSlit(yOffsetSlitIn),
  tiltSlit(tiltSlitIn),
  colour(colourIn),
  circularOuter(circularOuterIn)
{;}

void BDSCollimatorBeamMask::CheckParameters()
{
  if ((xAperture > 0.5 * horizontalWidth) || (yAperture > 0.5 * horizontalWidth))
  {
    G4cerr << __METHOD_NAME__ << "half aperture bigger than width or height!" << G4endl;
    G4cerr << "Full horizontal width is " << horizontalWidth << " mm for component named: \""
           << name << "\"" << G4endl;
    G4cerr << "x (half) aperture " << xAperture << " mm, y (half) aperture " << yAperture << " mm" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in beam mask");
  }

  if (BDS::IsFinite((xAperture <= 0) || (yAperture <= 0)))
  {
    G4cerr << __METHOD_NAME__ << "no aperture set for the main slit!" << G4endl;
    G4cerr << "In element named "  << name << " we have a x (half) aperture " << xAperture <<
    " mm, y (half) aperture " << yAperture << " mm" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in beam mask");
  }

  if (BDS::IsFinite((xApertureSlit <= 0) || (yApertureSlit <= 0)))
  {
    G4cerr << __METHOD_NAME__ << "no aperture set for the side slit!" << G4endl;
    G4cerr << "In element named "  << name << " we have a x (half) aperture " << xApertureSlit <<
           " mm, y (half) aperture " << yApertureSlit << " mm" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in beam mask");
  }

  if (!colour)
  {colour = BDSColours::Instance()->GetColour("collimator");}
}

G4String BDSCollimatorBeamMask::Material() const
{
  if (collimatorMaterial)
  {return collimatorMaterial->GetName();}
  else
  {return "none";}
}

void BDSCollimatorBeamMask::BuildContainerLogicalVolume()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name + "_beampipe",
                                              chordLength,
                                              beamPipeInfo);
  RegisterDaughter(pipe);

  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());
}

void BDSCollimatorBeamMask::BuildInner()
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

void BDSCollimatorBeamMask::Build()
{
  CheckParameters();
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  G4RotationMatrix* Rot = new G4RotationMatrix;
  G4ThreeVector Trans(xOffset, yOffset, 0);

  G4VSolid* outerSolid;
  if (circularOuter)
  {
    outerSolid = new G4Tubs(name + "_outer_solid",
                            0,
                            horizontalWidth * 0.5 - lengthSafety,
                            chordLength * 0.5 - lengthSafety,
                            0,
                            CLHEP::twopi);
  }
  else
  {
    outerSolid = new G4Box(name + "_outer_solid",
                           horizontalWidth * 0.5 - lengthSafety,
                           horizontalWidth * 0.5 - lengthSafety,
                           chordLength * 0.5 - lengthSafety);
  }
  RegisterSolid(outerSolid);

  G4bool buildVacuumAndAperture = (BDS::IsFinite(xAperture) && BDS::IsFinite(yAperture));

  // only do subtraction if aperture actually set
  if (buildVacuumAndAperture)
  {
    BuildInner();

    collimatorSolid = new G4SubtractionSolid(name + "_collimator_solid", // name
                                             outerSolid,                 // solid 1
                                             innerSolid);                // minus solid 2
    RegisterSolid(collimatorSolid);
  }
  else
  {collimatorSolid = outerSolid;}

  G4LogicalVolume* collimatorLV = new G4LogicalVolume(collimatorSolid,          // solid
                                                      collimatorMaterial,       // material
                                                      name + "_collimator_lv"); // name


  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  collimatorLV->SetVisAttributes(collimatorVisAttr);
  RegisterVisAttributes(collimatorVisAttr);

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(collimatorLV);
  if (sensitiveOuter)
  {RegisterSensitiveVolume(collimatorLV, BDSSDType::collimatorcomplete);}

  G4PVPlacement* collPV = new G4PVPlacement(Rot,               // rotation
                                            Trans,         // position
                                            collimatorLV,            // its logical volume
                                            name + "_collimator_pv", // its name
                                            containerLogicalVolume,  // its mother  volume
                                            false,		     // no boolean operation
                                            0,		             // copy number
                                            checkOverlaps);

  RegisterPhysicalVolume(collPV);

  if (buildVacuumAndAperture)
  {
    G4LogicalVolume* vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
                                                    vacuumMaterial,       // material
                                                    name + "_vacuum_lv"); // name

    vacuumLV->SetVisAttributes(containerVisAttr);
    // user limits - provided by BDSAcceleratorComponent
    vacuumLV->SetUserLimits(userLimits);
    SetAcceleratorVacuumLogicalVolume(vacuumLV);
    RegisterLogicalVolume(vacuumLV);
    if (sensitiveVacuum)
    {RegisterSensitiveVolume(vacuumLV, BDSSDType::energydepvacuum);}

    G4PVPlacement* vacPV = new G4PVPlacement(Rot,               // rotation
                                             Trans,       // position
                                             vacuumLV,                // its logical volume
                                             name + "_vacuum_pv",     // its name
                                             containerLogicalVolume,  // its mother  volume
                                             false,                   // no boolean operation
                                             0,                       // copy number
                                             checkOverlaps);

    RegisterPhysicalVolume(vacPV);
  }
}