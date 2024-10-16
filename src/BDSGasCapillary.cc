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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSGasCapillary.hh"
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
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include <map>

BDSGasCapillary::BDSGasCapillary(const G4String&  nameIn,
                                 G4double         lengthIn,
                                 BDSBeamPipeInfo* beamPipeInfoIn,
                                 G4double         horizontalWidthIn,
                                 G4Material*      capillaryOuterMaterialIn,
                                 G4Material*      capillaryGasMaterialIn,
                                 G4Material*      electrodesMaterialIn,
                                 G4double         capillaryGasDiameterIn,
                                 G4double         electrodeThicknessIn,
                                 G4bool           circularOuterIn):
        BDSAcceleratorComponent(nameIn, lengthIn, 0, "gascap", beamPipeInfoIn),
        horizontalWidth(horizontalWidthIn),
        capillaryGasDiameter(capillaryGasDiameterIn),
        electrodeThickness(electrodeThicknessIn),
        circularOuter(circularOuterIn),

        capillaryOuterSolid(nullptr),
        capillaryGasSolid(nullptr),
        electrodeSolid(nullptr),

        capillaryOuterUncutSolid(nullptr),
        electrodeUncutSolid(nullptr),

        capillaryHoleSolid(nullptr),
        electrodeHoleSolid(nullptr),

        capillaryOuterMaterial(capillaryOuterMaterialIn),
        capillaryGasMaterial(capillaryGasMaterialIn),
        electrodeMaterial(electrodesMaterialIn)
{;}

BDSGasCapillary::~BDSGasCapillary()
{;}

void BDSGasCapillary::CheckParameters()
{
  if (capillaryGasDiameter > horizontalWidth)
  {
    G4cerr << __METHOD_NAME__ << "aperture bigger than width!" << G4endl;
    G4cerr << "Full horizontal width is " << horizontalWidth << " mm for component named: \""
           << name << "\"" << G4endl;
    G4cerr << "gas aperture " << capillaryGasDiameter << " mm" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in gas capillary");
  }
  if (!BDS::IsFinite(capillaryGasDiameter))
  {
    G4cerr << __METHOD_NAME__ << "element need a non null aperture!" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in gas capillary");
  }
}

G4String BDSGasCapillary::CapillaryOuterMaterial() const
{if (capillaryOuterMaterial) {return capillaryOuterMaterial->GetName();}
  else {return "none";}}

G4String BDSGasCapillary::CapillaryGasMaterial() const
{if (capillaryGasMaterial) {return capillaryGasMaterial->GetName();}
  else{return "none";}}

G4String BDSGasCapillary::ElectrodeMaterial() const
{if (electrodeMaterial) {return electrodeMaterial->GetName();}
  else {return "none";}}


void BDSGasCapillary::BuildContainerLogicalVolumeOld()
{
  if (circularOuter) {
    containerSolid = new G4Tubs(name + "_container_solid",
                                0, horizontalWidth * 0.5, // rMin rMax
                                chordLength * 0.5,        // length
                                0, CLHEP::twopi);         // phiMin phiMax
  }
  else
  {
    containerSolid = new G4Box(name + "_container_solid",
                               horizontalWidth * 0.5,   // pX
                               horizontalWidth * 0.5,   // pY
                               chordLength * 0.5);      // pZ
  }
  containerLogicalVolume = new G4LogicalVolume(containerSolid, emptyMaterial, name + "_container_lv");
  BDSExtent ext(horizontalWidth * 0.5, horizontalWidth * 0.5, chordLength * 0.5);
  SetExtent(ext);
}

void BDSGasCapillary::BuildContainerLogicalVolume()
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

void BDSGasCapillary::BuildUncutSolid() {
  if (circularOuter)
  {
    capillaryOuterUncutSolid = new G4Tubs(name + "_capillaryOuterUncut_solid",
                                          0, horizontalWidth * 0.5 - lengthSafety, // rMin rMax
                                          chordLength * 0.5 - lengthSafety,        // length
                                          0, CLHEP::twopi);                        // phiMin phiMax
  }
  else {
    capillaryOuterUncutSolid = new G4Box(name + "_capillaryOuterUncut_solid",
                                         horizontalWidth * 0.5 - lengthSafety,  // pX
                                         horizontalWidth * 0.5 - lengthSafety,  // pY
                                         chordLength * 0.5 - lengthSafety);     // pZ
  }

  electrodeUncutSolid = new G4Box(name + "_electrodeUncut_solid",
                                  horizontalWidth * 0.5 - lengthSafety,       // pX
                                  horizontalWidth * 0.5 - lengthSafety,       // pY
                                  electrodeThickness * 0.5 - lengthSafety);   // pZ

  capillaryGasSolid = new G4Tubs(name + "_capillaryGas_solid",
                                 0, capillaryGasDiameter * 0.5 - lengthSafety, // rMin rMax
                                 chordLength * 0.5 - lengthSafety,             // length
                                 0, CLHEP::twopi);                             // phiMin phiMax

  RegisterSolid(capillaryOuterSolid);
  RegisterSolid(electrodeUncutSolid);
  RegisterSolid(capillaryGasSolid);
}

void BDSGasCapillary::BuildHolesSolid() {

  electrodeHoleSolid = new G4Box(name + "_electrodeHole_solid",
                                 horizontalWidth * 0.5 + lengthSafetyLarge * 5,     // pZ
                                 horizontalWidth * 0.5 + lengthSafetyLarge * 5,     // pZ
                                 electrodeThickness * 0.5 + lengthSafetyLarge * 5); // pZ

  capillaryHoleSolid = new G4Tubs(name + "_capillaryHole_solid",
                                  0, capillaryGasDiameter * 0.5 + lengthSafetyLarge * 5, // rMin rMax
                                  chordLength * 0.5 + lengthSafetyLarge * 5,             // length
                                  0, CLHEP::twopi);                             // phiMin phiMax

  RegisterSolid(electrodeHoleSolid);
  RegisterSolid(capillaryHoleSolid);
}

void BDSGasCapillary::BuildCutSolid()
{
  G4ThreeVector TransFront(0, 0, -chordLength * 0.5 + electrodeThickness * 0.5);
  G4ThreeVector TransBack(0, 0, chordLength * 0.5 - electrodeThickness * 0.5);
  G4RotationMatrix* Rot = new G4RotationMatrix;

  G4VSolid* capillaryOuterCut1Solid = new G4SubtractionSolid(name + "_capillaryOuterCut1_solid",
                                                             capillaryOuterUncutSolid, capillaryHoleSolid);

  G4VSolid* capillaryOuterCut2Solid = new G4SubtractionSolid(name + "_capillaryOuterCut2_solid",
                                                             capillaryOuterCut1Solid, electrodeHoleSolid,
                                                             Rot, TransFront);

  capillaryOuterSolid = new G4SubtractionSolid(name + "_capillaryOuterSolid_solid",
                                               capillaryOuterCut2Solid, electrodeHoleSolid,
                                               Rot, TransBack);

  electrodeSolid = new G4IntersectionSolid(name + "_electrode_solid",
                                           capillaryOuterSolid, electrodeHoleSolid);

  RegisterSolid(capillaryOuterSolid);
  RegisterSolid(electrodeSolid);
}

void BDSGasCapillary::Build()
{
  CheckParameters();
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  G4ThreeVector TransFront(0, 0, -chordLength * 0.5 + electrodeThickness * 0.5);
  G4ThreeVector TransBack(0, 0, chordLength * 0.5 - electrodeThickness * 0.5);
  G4RotationMatrix* Rotate= new G4RotationMatrix;

  BuildUncutSolid();
  BuildHolesSolid();
  BuildCutSolid();

  G4LogicalVolume* capillaryOuterLV = new G4LogicalVolume(capillaryOuterSolid, capillaryOuterMaterial,
                                                          name + "_capillaryOuter_lv");
  G4LogicalVolume* capillaryGasLV = new G4LogicalVolume(capillaryGasSolid, capillaryGasMaterial,
                                                        name + "_capillaryGas_lv");
  G4LogicalVolume* electrodeLV = new G4LogicalVolume(electrodeSolid, electrodeMaterial,
                                                     name + "_electrode_lv");

  // color, user limits, register
  SetupLV(capillaryOuterLV, BDSColours::Instance()->GetColour("capillaryouter"));
  SetupLV(capillaryGasLV, BDSColours::Instance()->GetColour("capillarygas"));
  SetupLV(electrodeLV, BDSColours::Instance()->GetColour("electrode"));

  G4LogicalVolume* vac = *(GetAcceleratorVacuumLogicalVolumes().begin()); // take the first one

  G4PVPlacement* capillaryOuterPV = new G4PVPlacement(Rotate,                      // rotation
                                                      G4ThreeVector(),             // position
                                                      capillaryOuterLV,            // its logical volume
                                                      name + "_capillaryOuter_pv", // its name
                                                      vac,      // its mother  volume
                                                      false,		               // no boolean operation
                                                      0,		                   // copy number
                                                      checkOverlaps);

  G4PVPlacement* capillaryGasPV = new G4PVPlacement(Rotate,                    // rotation
                                                    (G4ThreeVector) 0,      // position
                                                    capillaryGasLV,            // its logical volume
                                                    name + "_capillaryGas_pv", // its name
                                                    vac,    // its mother  volume
                                                    false,                     // no boolean operation
                                                    0,                         // copy number
                                                    checkOverlaps);

  G4PVPlacement* electrodeFrontPV = new G4PVPlacement(Rotate,                      // rotation
                                                      TransFront,                  // position
                                                      electrodeLV,                 // its logical volume
                                                      name + "_electrodeFront_pv", // its name
                                                      vac,      // its mother  volume
                                                      false,		               // no boolean operation
                                                      0,		                   // copy number
                                                      checkOverlaps);

  G4PVPlacement* electrodeBackPV = new G4PVPlacement(Rotate,                     // rotation
                                                     TransBack,                  // position
                                                     electrodeLV,                // its logical volume
                                                     name + "_electrodeBack_pv", // its name
                                                     vac,     // its mother  volume
                                                     false,		                 // no boolean operation
                                                     0,		                     // copy number
                                                     checkOverlaps);

  RegisterPhysicalVolume(capillaryOuterPV);
  RegisterPhysicalVolume(capillaryGasPV);
  RegisterPhysicalVolume(electrodeFrontPV);
  RegisterPhysicalVolume(electrodeBackPV);
}

void BDSGasCapillary::SetupLV(G4LogicalVolume* lv, G4Colour* colour)
{
  // color attribute
  G4VisAttributes* gasVisAttr = new G4VisAttributes(*colour);
  lv->SetVisAttributes(gasVisAttr);
  RegisterVisAttributes(gasVisAttr);

  // user limits - provided by BDSAcceleratorComponent
  lv->SetUserLimits(userLimits);

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(lv);
}