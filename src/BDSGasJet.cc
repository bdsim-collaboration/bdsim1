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
#include "BDSGasJet.hh"
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
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"


BDSGasJet::BDSGasJet(const G4String& nameIn,
                     G4double    lengthIn,
                     BDSBeamPipeInfo* beamPipeInfoIn,
                     G4Material* gasMaterialIn,
                     G4double    xSizeIn,
                     G4double    ySizeIn,
                     G4double    zSizeIn,
                     G4double    xAngleIn,
                     G4double    yAngleIn,
                     G4double    zAngleIn,
                     G4double    xOffsetIn,
                     G4double    yOffsetIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "gasjet", beamPipeInfoIn),
  gasSolid(nullptr),
  gasMaterial(gasMaterialIn),
  xSize(xSizeIn),
  ySize(ySizeIn),
  zSize(zSizeIn),
  xAngle(xAngleIn),
  yAngle(yAngleIn),
  zAngle(zAngleIn),
  xOffset(xOffsetIn),
  yOffset(yOffsetIn)
{;}

void BDSGasJet::CheckParameters()
{
  if (BDS::IsFinite(xSize <= 0))
  {
    G4cerr << __METHOD_NAME__ << "must provide positive non-zero dimension to gas jet!" << G4endl;
    G4cerr << "x size : " << xSize << " mm" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in beam mask");
  }

  if (BDS::IsFinite(ySize <= 0))
  {
    G4cerr << __METHOD_NAME__ << "must provide positive non-zero dimension to gas jet!" << G4endl;
    G4cerr << "y size : " << ySize << " mm" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in beam mask");
  }

  if (BDS::IsFinite(zSize <= 0))
  {
    G4cerr << __METHOD_NAME__ << "must provide positive non-zero dimension to gas jet!" << G4endl;
    G4cerr << "z size : " << zSize << " mm" << G4endl;
    throw BDSException(__METHOD_NAME__, "Error in beam mask");
  }
}

G4String BDSGasJet::Material() const
{
  if (gasMaterial)
  {return gasMaterial->GetName();}
  else
  {return "none";}
}

void BDSGasJet::BuildContainerLogicalVolume()
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

void BDSGasJet::Build()
{
  CheckParameters();
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  G4RotationMatrix* Rot = new G4RotationMatrix(xAngle, yAngle, zAngle);
  G4ThreeVector Trans(xOffset, yOffset, 0);

  gasSolid = new G4Box(name + "_gas_solid",
                       xSize * 0.5 - lengthSafety,
                       ySize * 0.5 - lengthSafety,
                       zSize * 0.5 - lengthSafety);
  RegisterSolid(gasSolid);

  G4LogicalVolume* gasLV = new G4LogicalVolume(gasSolid,          // solid
                                               gasMaterial,       // material
                                               name + "_gas_lv"); // name


  G4VisAttributes* gasVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("capillarygas"));
  gasLV->SetVisAttributes(gasVisAttr);
  RegisterVisAttributes(gasVisAttr);

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(gasLV);

  G4LogicalVolume* vac = *(GetAcceleratorVacuumLogicalVolumes().begin()); // take the first one

  G4PVPlacement* gasPV = new G4PVPlacement(Rot,              // rotation
                                           Trans,            // position
                                           gasLV,            // its logical volume
                                           name + "_gas_pv", // its name
                                           vac,              // its mother  volume
                                           false,		     // no boolean operation
                                           0,		         // copy number
                                           checkOverlaps);

  RegisterPhysicalVolume(gasPV);
}