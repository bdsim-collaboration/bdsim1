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
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGaborLens.hh"
#include "BDSExtent.hh"
#include "BDSException.hh"
#include "BDSMaterials.hh"
#include "BDSSDType.hh"

#include "G4Box.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include "globals.hh" // geant4 globals / types
#include <string>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

BDSGaborLens::BDSGaborLens(const G4String& nameIn,
                           G4double lengthIn,
                           G4double horizontalWidthIn,
                           G4double anodeLengthIn,
                           G4Material* anodeMaterialIn,
                           G4double anodeRadiusIn,
                           G4double anodeThicknessIn,
                           G4double electrodeLengthIn,
                           G4Material* electrodeMaterialIn,
                           G4double electrodeRadiusIn,
                           G4double electrodeThicknessIn,
                           G4Material* outerMaterialIn,
                           G4Colour* colourIn,
                           BDSBeamPipeInfo* beamPipeInfoIn,
                           BDSFieldInfo* vacuumFieldInfoIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "gaborlens", beamPipeInfoIn),
  horizontalWidth(horizontalWidthIn),
  anodeRadius(anodeRadiusIn),
  anodeLength(anodeLengthIn),
  anodeMaterial(anodeMaterialIn),
  anodeThickness(anodeThicknessIn),
  electrodeRadius(electrodeRadiusIn),
  electrodeLength(electrodeLengthIn),
  electrodeMaterial(electrodeMaterialIn),
  electrodeThickness(electrodeThicknessIn),
  outerMaterial(outerMaterialIn),
  colour(colourIn),
  vacuumFieldInfo(vacuumFieldInfoIn)
{
  if (vacuumFieldInfo)
    {vacuumFieldInfo->SetBeamPipeRadius(beamPipeInfoIn->IndicativeRadius());}

  endcapsLength = 10;

  if (chordLength <= endcapsLength)
    {throw BDSException(__METHOD_NAME__,"the element length must be greater than 20 mm for element \"" + name + "\"");}

  vacuumLength = chordLength - 2*endcapsLength;

  // Input Checks
  if (horizontalWidth <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"horizontalWidth\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if (anodeLength <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"anodeLength\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if (anodeRadius <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"anodeRadius\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if (anodeThickness <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"anodeThickness\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if (electrodeLength <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"electrodeLength\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if (electrodeRadius <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"electrodeRadius\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if (electrodeThickness <= 0)
    {throw BDSException(__METHOD_NAME__,"option \"electrodeThickness\" is not defined or must be greater than 0 for element \"" + name + "\"");}

  if ((anodeRadius + anodeThickness) > beamPipeInfo->ExtentInner().MinimumAbsTransverse())
    {throw BDSException(__METHOD_NAME__,"\"anodeRadius\" must be smaller than the beam pipe aperture for element \"" + name + "\"");}

  if (anodeLength > vacuumLength)
    {throw BDSException(__METHOD_NAME__,"\"anodeLength\" must be shorter than the element length minus 20mm for element \"" + name + "\"");}

  if ((electrodeRadius + electrodeThickness) > anodeRadius)
    {throw BDSException(__METHOD_NAME__,"\"anodeRadius\" cannot be smaller than \"electrodeRadius\" + \"electrodeThickness\" for element \"" + name + "\"");}

  if (electrodeLength > 0.5*vacuumLength)
    {throw BDSException(__METHOD_NAME__,"\"electrodeLength\" must be smaller than half element length minus 20mm for element \"" + name + "\"");}

}

BDSGaborLens::~BDSGaborLens()
{;}

void BDSGaborLens::BuildContainerLogicalVolume()
{
  containerSolid = new G4Tubs(name + "_container_solid",
                              0, horizontalWidth + lengthSafety,
                              chordLength*0.5,
                              0, CLHEP::twopi);

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               beamPipeInfo->vacuumMaterial,
                                               name + "_container_lv");
}

void BDSGaborLens::Build()
{
  BDSAcceleratorComponent::Build();
  BuildBeamPipe();
  BuildOuter();
}

void BDSGaborLens::BuildOuter()
{
  BDSExtent extent = beamPipeInfo->Extent();
  G4double bpMaxExtent = extent.MaximumAbsTransverse();

  G4double coilOuterRadius = horizontalWidth*0.5 *0.9;
  G4Tubs* coils = new G4Tubs(name + "_coils_solid",  // name
                             bpMaxExtent + lengthSafetyLarge*2,          // inner radius
                             coilOuterRadius,        // outer radius
                             0.5*(vacuumLength- lengthSafetyLarge*10),   // half length
                             0, CLHEP::twopi);       // start and finish angle
  RegisterSolid(coils);
  G4Material* coilMaterial = BDSMaterials::Instance()->GetMaterial("copper");
  G4LogicalVolume* coilsLV = new G4LogicalVolume(coils,                // solid
                                                 coilMaterial,         // material
                                                 name + "_coils_lv");  // name
  G4Colour* coil = BDSColours::Instance()->GetColour("coil");
  G4VisAttributes* coilVisAttr = new G4VisAttributes(*coil);
  coilVisAttr->SetVisibility(true);
  coilsLV->SetVisAttributes(coilVisAttr);

  RegisterLogicalVolume(coilsLV);

  // coils placement
  G4PVPlacement* coilsPV = new G4PVPlacement(nullptr,              // rotation
                                             G4ThreeVector(0,0,0), // position
                                             coilsLV,              // its logical volume
                                             name + "_coils_pv",   // its name
                                             containerLogicalVolume,
                                             false,                // no boolean operation
                                             0,                    // copy number
                                             checkOverlaps);
  RegisterPhysicalVolume(coilsPV);

  G4Tubs* outer = new G4Tubs(name + "_outer_solid",                   // name
                             coilOuterRadius + lengthSafetyLarge*10,  // inner radius
                             horizontalWidth*0.5 - lengthSafety,      // outer radius
                             vacuumLength*0.5,                        // half length
                             0, CLHEP::twopi);                        // start and finish angle
  RegisterSolid(outer);
  G4LogicalVolume* outerLV = new G4LogicalVolume(outer,                // solid
                                                 outerMaterial,        // material
                                                 name + "_outer_lv");  // name
  G4Colour* outercolour = BDSColours::Instance()->GetColour("gaborlens");
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*outercolour);
  outerVisAttr->SetVisibility(true);
  outerLV->SetVisAttributes(outerVisAttr);

  RegisterLogicalVolume(outerLV);

  // outer placement
  G4PVPlacement* outerPV = new G4PVPlacement(nullptr,             // rotation
                                             G4ThreeVector(0,0,0),// position
                                             outerLV,             // its logical volume
                                             name + "_outer_pv",  // its name
                                             containerLogicalVolume,
                                             false,               // no boolean operation
                                             0,                   // copy number
                                             checkOverlaps);
  RegisterPhysicalVolume(outerPV);

  G4Tubs* endcap = new G4Tubs(name + "_endcap_solid",              // name
                              electrodeRadius,                     // inner radius
                              horizontalWidth*0.5 - lengthSafety,  // outer radius
                              endcapsLength*0.5 - lengthSafetyLarge, // half length
                              0, CLHEP::twopi);                    // start and finish angle
  RegisterSolid(endcap);
  G4Material* endcapMaterial = BDSMaterials::Instance()->GetMaterial("stainlesssteel");
  G4LogicalVolume* endcapLV = new G4LogicalVolume(endcap,                // solid
                                                  endcapMaterial,        // material
                                                  name + "_endcap_lv");  // name
  G4Colour* endcapcolour = BDSColours::Instance()->GetColour("beampipe");
  G4VisAttributes* endcapVisAttr = new G4VisAttributes(*endcapcolour);
  endcapVisAttr->SetVisibility(true);
  endcapLV->SetVisAttributes(endcapVisAttr);

  RegisterLogicalVolume(endcapLV);

  // end cap placement
  G4double endcapZ = (chordLength-endcapsLength)*0.5;
  G4PVPlacement* endcapInPV = new G4PVPlacement(nullptr,                     // rotation
                                                G4ThreeVector(0,0,-endcapZ), // position
                                                endcapLV,                    // its logical volume
                                                name + "_endcapIn_pv",       // its name
                                                containerLogicalVolume,
                                                false,                       // no boolean operation
                                                0,                           // copy number
                                                checkOverlaps);
  RegisterPhysicalVolume(endcapInPV);

  G4PVPlacement* endcapOutPV = new G4PVPlacement(nullptr,                    // rotation
                                                 G4ThreeVector(0,0,endcapZ), // position
                                                 endcapLV,                   // its logical volume
                                                 name + "_endcapOut_pv",     // its name
                                                 containerLogicalVolume,
                                                 false,                      // no boolean operation
                                                 0,                          // copy number
                                                 checkOverlaps);
  RegisterPhysicalVolume(endcapOutPV);
}

void BDSGaborLens::BuildBeamPipe()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name + "_beampipe",
                                              vacuumLength,
                                              beamPipeInfo);
  RegisterDaughter(pipe);

  G4PVPlacement* bpPV = new G4PVPlacement(nullptr,
                                          G4ThreeVector(),
                                          pipe->GetContainerLogicalVolume(),
                                          name+"_beampipe_pv",
                                          containerLogicalVolume,
                                          false,
                                          0,
                                          checkOverlaps);

  RegisterPhysicalVolume(bpPV);

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumFieldInfo,
                                                            pipe->GetVacuumLogicalVolume(),
                                                            true);

  G4Tubs* anode = new G4Tubs(name + "_anode_solid",  // name
                             anodeRadius,            // inner radius
                             anodeRadius + anodeThickness,   // outer radius
                             anodeLength*0.5,        // half length
                             0, CLHEP::twopi);       // start and finish angle
  RegisterSolid(anode);

  G4LogicalVolume* anodeLV = new G4LogicalVolume(anode,                // solid
                                                 anodeMaterial,        // material
                                                 name + "_anode_lv");  // name
  G4Colour* anodecolour = BDSColours::Instance()->GetColour("coil");
  G4VisAttributes* anodeVisAttr = new G4VisAttributes(*anodecolour);
  anodeVisAttr->SetVisibility(true);
  anodeLV->SetVisAttributes(anodeVisAttr);
  RegisterLogicalVolume(anodeLV);

  // anode placement
  G4LogicalVolume* vac = *(GetAcceleratorVacuumLogicalVolumes().begin()); // take the first one
  G4PVPlacement* anodePV = new G4PVPlacement(nullptr,               // rotation
                                             G4ThreeVector(0,0,0),  // position
                                             anodeLV,               // its logical volume
                                             name + "_anode_pv",    // its name
                                             vac,
                                             false,                 // no boolean operation
                                             0,                     // copy number
                                             checkOverlaps);
  RegisterPhysicalVolume(anodePV);

  G4Tubs* electrode = new G4Tubs(name + "_electrode_solid",  // name
                                 electrodeRadius,            // inner radius
                                 electrodeRadius + electrodeThickness,      // outer radius
                                 electrodeLength*0.5 - lengthSafetyLarge,   // half length
                                 0, CLHEP::twopi);           // start and finish angle
  RegisterSolid(electrode);

  G4LogicalVolume* electrodeLV = new G4LogicalVolume(electrode,                // solid
                                                     electrodeMaterial,        // material
                                                     name + "_electrode_lv");  // name
  G4Colour* electrodecolour = BDSColours::Instance()->GetColour("copper");
  G4VisAttributes* electrodeVisAttr = new G4VisAttributes(*electrodecolour);
  electrodeVisAttr->SetVisibility(true);
  electrodeLV->SetVisAttributes(electrodeVisAttr);
  RegisterLogicalVolume(electrodeLV);

  G4double elecZ = chordLength*0.5 - endcapsLength - electrodeLength*0.5 - lengthSafety;  // abs(z) electrode position

  // electrode placement
  G4PVPlacement* electrodeInPV = new G4PVPlacement(nullptr,                    // rotation
                                                   G4ThreeVector(0,0,-elecZ),  // position
                                                   electrodeLV,                // its logical volume
                                                   name + "_electrodeIn_pv",   // its name
                                                   vac,
                                                   false,                      // no boolean operation
                                                   0,                          // copy number
                                                   checkOverlaps);
  RegisterPhysicalVolume(electrodeInPV);

  G4PVPlacement* electrodeOutPV = new G4PVPlacement(nullptr,                    // rotation
                                                    G4ThreeVector(0,0,elecZ),   // position
                                                    electrodeLV,                // its logical volume
                                                    name + "_electrodeOut_pv",  // its name
                                                    vac,
                                                    false,                      // no boolean operation
                                                    0,                          // copy number
                                                    checkOverlaps);
  RegisterPhysicalVolume(electrodeOutPV);
}