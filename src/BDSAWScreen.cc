/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSAWScreen.hh"

#include "BDSMultilayerScreen.hh"
#include "BDSScreenLayer.hh"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <list>
#include <sstream>

BDSAWScreen::BDSAWScreen(G4String         nameIn,
		     G4double         chordLengthIn,
		     BDSBeamPipeInfo* beamPipeInfoIn,
		     G4TwoVector      sizeIn, //X Y dimensions of screen
		     G4double         screenAngleIn):
  BDSDrift(nameIn,chordLengthIn,beamPipeInfoIn),
  size(sizeIn), 
  screenAngle(screenAngleIn),
  screenPos(G4ThreeVector()),
  nLayers(0)
{
  mlScreen = new BDSMultilayerScreen(size, nameIn+"_mlscreen");
  
  screenRot = new G4RotationMatrix();
  screenRot->rotateY(screenAngle);
}

BDSAWScreen::~BDSAWScreen()
{
  delete screenRot;
  delete mlScreen;
}

void BDSAWScreen::Build()
{
  BDSDrift::Build(); //Build the beam pipe geometry.
  
  // Make the beam pipe wireframe
  G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4, 0.3));
  VisAtt1->SetForceWireframe(true);
  VisAtt1->SetVisibility(true);
  containerLogicalVolume->SetVisAttributes(VisAtt1);

  PlaceAWScreen(); //Place the screen in the beam pipe
}

void BDSAWScreen::AddScreenLayer(G4double thickness, G4String material, G4int isSampler)
{
  std::stringstream ss;
  ss << nLayers;
  G4String lNum = ss.str();
  G4String lName = name+"_"+lNum;
  mlScreen->AddScreenLayer(thickness,material,lName, isSampler);
  if(!isSampler)
    {RegisterSensitiveVolume(mlScreen->LastLayer()->GetLog());}
  nLayers++;
}

void BDSAWScreen::PlaceAWScreen()
{
  mlScreen->Build();//Build the screen.
  G4LogicalVolume* vacuumLV = GetAcceleratorVacuumLogicalVolume();
  mlScreen->Place(screenRot, screenPos, vacuumLV); //Place the screen in the beampipe centre.
}