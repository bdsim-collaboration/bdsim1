
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
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "G4AssemblyVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"

#include <algorithm>
#include <cmath>
#include <string>

G4Material* BDSAcceleratorComponent::emptyMaterial   = nullptr;
G4Material* BDSAcceleratorComponent::worldMaterial   = nullptr;
G4double    BDSAcceleratorComponent::lengthSafety    = -1;
G4bool      BDSAcceleratorComponent::checkOverlaps   = false;
G4bool      BDSAcceleratorComponent::sensitiveOuter  = true;
G4bool      BDSAcceleratorComponent::sensitiveVacuum = false;
G4VisAttributes* BDSAcceleratorComponent::containerVisAttr = nullptr;
G4double    BDSAcceleratorComponent::lengthSafetyLarge = 0;

BDSAcceleratorComponent::BDSAcceleratorComponent(const G4String&      nameIn,
						 G4double             arcLengthIn,
						 G4double             angleIn,
						 const G4String&      typeIn,
						 BDSBeamPipeInfo*     beamPipeInfoIn,
						 const G4ThreeVector& inputFaceNormalIn,
						 const G4ThreeVector& outputFaceNormalIn,
						 BDSFieldInfo*        fieldInfoIn):
  BDSGeometryComponent(nullptr,nullptr),
  name(nameIn),
  arcLength(arcLengthIn),
  type(typeIn),
  angle(angleIn),
  beamPipeInfo(beamPipeInfoIn),
  endPieceBefore(nullptr),
  endPieceAfter(nullptr),
  userLimits(nullptr),
  fieldInfo(fieldInfoIn),
  copyNumber(-1), // -1 initialisation since it will be incremented when placed
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn)
{
  // initialise static members
  if (!emptyMaterial)
    {
      const auto globals = BDSGlobalConstants::Instance(); // shortcut
      emptyMaterial      = BDSMaterials::Instance()->GetMaterial(globals->EmptyMaterial());
      worldMaterial      = BDSMaterials::Instance()->GetMaterial(globals->WorldMaterial());
      lengthSafety       = globals->LengthSafety();
      lengthSafetyLarge  = globals->LengthSafetyLarge();
      checkOverlaps      = globals->CheckOverlaps();
      sensitiveOuter     = globals->SensitiveOuter();
      sensitiveVacuum    = globals->StoreELossVacuum();
      containerVisAttr   = BDSGlobalConstants::Instance()->ContainerVisAttr();
    }

  // Prevent negative length components.
  if (arcLength < 0)
    {
      G4String message = "Negative length for component named \"" + name + "\" with length " + std::to_string(arcLength);
      throw BDSException(__METHOD_NAME__, message);
    }
  
  // calculate the chord length if the angle is finite
  if (BDS::IsFinite(angleIn))
    {chordLength = 2.0 * arcLengthIn * std::sin(0.5*angleIn) / angleIn;}
  else
    {chordLength = arcLengthIn;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "angle:        " << angleIn     << G4endl;
  G4cout << __METHOD_NAME__ << "arc length:   " << arcLengthIn << G4endl;
  G4cout << __METHOD_NAME__ << "chord length: " << chordLength << G4endl; 
#endif

  initialised = false;
}

BDSAcceleratorComponent::~BDSAcceleratorComponent()
{
  delete beamPipeInfo;
  // Don't delete usersLimits as could be globals one. If not it'll be registered
  // with BDSGeometryComponent
}

void BDSAcceleratorComponent::Initialise()
{
  if (initialised)
    {return;} // protect against duplicated initialisation and memory leaks

  Build();

  // field construction must be done after all the geometry is constructed if the
  // field is to propagate to the daughter volumes correctly.
  if (fieldInfo)
    {
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(fieldInfo,
								containerLogicalVolume,
								true);
    }
  initialised = true; // record that this component has been initialised
}

void BDSAcceleratorComponent::Build()
{
  BuildContainerLogicalVolume(); // pure virtual provided by derived class
  BuildUserLimits();
  AttachUserLimits();
  if (containerLogicalVolume)
    {containerLogicalVolume->SetVisAttributes(containerVisAttr);}
}

void BDSAcceleratorComponent::SetField(BDSFieldInfo* fieldInfoIn)
{
  fieldInfo = fieldInfoIn;
}

G4double BDSAcceleratorComponent::Sagitta() const
{
  if (!BDS::IsFinite(angle))
    {return 0;}
  G4double bendingRadius = arcLength / angle;
  G4double sagitta = bendingRadius - std::sqrt(std::pow(bendingRadius,2) - std::pow(chordLength, 2));
  return sagitta;
}

G4bool BDSAcceleratorComponent::AngledInputFace() const
{
  G4ThreeVector zeroAngle = G4ThreeVector(0,0,-1);
  G4ThreeVector cross = inputFaceNormal.cross(zeroAngle);
  G4double det = cross.mag2();
  return BDS::IsFinite(det); // if finite, there is an angle and if not, no angle
}

G4bool BDSAcceleratorComponent::AngledOutputFace() const
{
  G4ThreeVector zeroAngle = G4ThreeVector(0,0,1);
  G4ThreeVector cross = outputFaceNormal.cross(zeroAngle);
  G4double det = cross.mag2();
  return BDS::IsFinite(det);
}

void BDSAcceleratorComponent::BuildUserLimits()
{
  // user limits
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, std::max(chordLength, arcLength));
  if (ul != defaultUL) // if it's not the default register it
    {RegisterUserLimits(ul);}
  userLimits = ul; // assign to member
}

void BDSAcceleratorComponent::AttachUserLimits() const
{
  if (!userLimits)
    {return;}
  if (containerLogicalVolume || containerAssembly)
    {
      if (containerIsAssembly && containerAssembly)
        {AttachUserLimitsToAssembly(containerAssembly, userLimits);}
      else if (containerLogicalVolume)
        {containerLogicalVolume->SetUserLimits(userLimits);}
    }
}

std::set<G4LogicalVolume*> BDSAcceleratorComponent::GetAcceleratorMaterialLogicalVolumes() const
{
  // get full set minus ones marked to be excluded completely from biasing
  std::set<G4LogicalVolume*> result = GetAllBiasingVolumes();

  for (auto lv : acceleratorVacuumLV)
    {result.erase(lv);}
  return result;
}

void BDSAcceleratorComponent::SetFieldUsePlacementWorldTransform()
{
  if (fieldInfo)
    {fieldInfo->SetUsePlacementWorldTransform(true);}
}
