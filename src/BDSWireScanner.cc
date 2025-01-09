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
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSUtilities.hh"
#include "BDSWireScanner.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>

BDSWireScanner::BDSWireScanner(G4String         nameIn,
			       G4double         lengthIn,
			       BDSBeamPipeInfo* beamPipeInfoIn,
			       G4Material*      wireMaterialIn,
			       G4double         wireDiameterIn,
			       G4double         wireLengthIn,
			       G4double         wireAngleIn,
			       G4ThreeVector    wireOffsetIn,
			       G4Colour*        wireColourIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "wirescanner", beamPipeInfoIn),
  wireMaterial(wireMaterialIn),
  wireDiameter(wireDiameterIn),
  wireLength(wireLengthIn),
  wireAngle(wireAngleIn),
  wireOffset(wireOffsetIn),
  wireColour(wireColourIn)
{
  if (wireDiameter <= 0)
    {throw BDSException(__METHOD_NAME__, "Error: wireDiameter for \"" + name + "\" is not defined or must be greater than 0");}

  if (wireLength <= 0)
    {throw BDSException(__METHOD_NAME__, "Error: wire for \"" + name + "\" must be > 0.");}

  // check whether the beam pipe will fit transversely (ignores presumably very small
  // wire diameter). work out end points off wire including length and offset in x,y.
  G4TwoVector offsetXY = G4TwoVector(wireOffset.x(), wireOffset.y());
  G4TwoVector tipTop = G4TwoVector(0, 0.5*wireLength);
  tipTop.rotate(wireAngle);
  G4TwoVector tipBot = G4TwoVector(tipTop);
  tipBot.rotate(CLHEP::pi);
  tipTop += offsetXY;
  tipBot += offsetXY;
  G4double innerRadius = beamPipeInfo->IndicativeRadiusInner();
  if (tipTop.mag() > innerRadius || tipBot.mag() > innerRadius)
    {throw BDSException(__METHOD_NAME__, "wire for \"" + name + "\" is too big to fit in beam pipe give offsets.");}
  if (!wireColour)
    {wireColour = BDSColours::Instance()->GetColour("wirescanner");}
}

void BDSWireScanner::BuildContainerLogicalVolume()
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

void BDSWireScanner::Build()
{
  BDSAcceleratorComponent::Build();
  
  G4VSolid*        wire   = BuildWireSolid();
  G4LogicalVolume* wireLV = BuildWireLV(wire);
  
  // placement rotation
  G4RotationMatrix* wireRot = new G4RotationMatrix();
  wireRot->rotateX(CLHEP::halfpi);
  // want to rotate about unit Z but this has now changed
  wireRot->rotateY(wireAngle);
  RegisterRotationMatrix(wireRot);
  
  // visualisation attributes
  G4VisAttributes* wireVisAttr = new G4VisAttributes(*wireColour);
  wireLV->SetVisAttributes(wireVisAttr);
  RegisterVisAttributes(wireVisAttr);
  
  // placement
  G4LogicalVolume* vac = *(GetAcceleratorVacuumLogicalVolumes().begin()); // take the first one
  G4PVPlacement* wirePV = new G4PVPlacement(wireRot,           // rotation
					    wireOffset,        // position
					    wireLV,            // its logical volume
					    name + "_wire_pv", // its name
					    vac,
					    false,                  // no boolean operation
					    0,                      // copy number
					    checkOverlaps);
  RegisterPhysicalVolume(wirePV);
}

G4VSolid* BDSWireScanner::BuildWireSolid()
{
  G4Tubs* wire = new G4Tubs(name + "_wire_solid", // name
			    0,                    // inner radius
			    wireDiameter*0.5,     // outer radius
			    wireLength*0.5,       // half length
			    0, CLHEP::twopi);     // start and finish angle
  RegisterSolid(wire);
  return wire;
}


G4LogicalVolume* BDSWireScanner::BuildWireLV(G4VSolid* solid)
{
  G4LogicalVolume* wireLV = new G4LogicalVolume(solid,
						wireMaterial,
						name + "_wire_lv");
  RegisterLogicalVolume(wireLV);
  return wireLV;
}
