/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSCavity.hh"
#include "BDSCavityFactory.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSColours.hh"
#include "BDSColourFromMaterial.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSMuonCooler.hh"

#include "G4Colour.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Types.hh"
#include "G4VisAttributes.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

class G4Material;

BDSMuonCooler::BDSMuonCooler(const G4String& nameIn,
                             G4double        lengthIn,
                             G4double        containerRadiusIn,
                             G4Material*     surroundingMaterialIn,
                             const std::vector<BDS::MuonCoolerCoilInfo>&     coilInfosIn,
                             const std::vector<BDS::MuonCoolerCavityInfo>&   cavityInfosIn,
                             const std::vector<BDS::MuonCoolerAbsorberInfo>& absorberInfosIn,
                             BDSBeamPipeInfo* beamPipeTemplateIn,
                             BDSFieldInfo*    outerFieldRecipeIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "muoncooler", nullptr),
  containerRadius(containerRadiusIn),
  surroundingMaterial(surroundingMaterialIn),
  coilInfos(coilInfosIn),
  cavityInfos(cavityInfosIn),
  absorberInfos(absorberInfosIn),
  beamPipeTemplate(beamPipeTemplateIn),
  outerFieldRecipe(outerFieldRecipeIn)
{;}

BDSMuonCooler::~BDSMuonCooler()
{;}

void BDSMuonCooler::Build()
{
  BuildContainerLogicalVolume();
  BuildCoils();
  BuildAbsorbers();
  BuildCavities();
  BuildField();
}

void BDSMuonCooler::BuildContainerLogicalVolume()
{
  containerSolid = new G4Tubs(name + "_container_solid",
                              0,
                              containerRadius,
                              0.5*chordLength,
                              0,
                              CLHEP::twopi);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
                                               surroundingMaterial,
                                               name + "_container_lv");

  BDSExtent ext(containerRadius, containerRadius, chordLength * 0.5);
  SetExtent(ext);
  
  BuildUserLimits();
  containerLogicalVolume->SetUserLimits(userLimits);

  auto containerVis = new G4VisAttributes(*BDSColours::Instance()->GetColour("opaquebox"));
  containerVis->SetVisibility(true);
  containerVis->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
  containerLogicalVolume->SetVisAttributes(containerVis);
  RegisterVisAttributes(containerVis);
}

void BDSMuonCooler::BuildCoils()
{
  // make a unique set of coil visualisations - only what we need
  std::set<G4Material*> coilMaterials;
  for (const auto& info : coilInfos)
    {coilMaterials.insert(info.material);}
  std::map<G4Material*, G4VisAttributes*> coilVises;
  for (const auto& material : coilMaterials)
    {
      auto coilVis = new G4VisAttributes(*BDSColourFromMaterial::Instance()->GetColour(material));
      coilVis->SetVisibility(true);
      coilVis->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
      RegisterVisAttributes(coilVis);
      coilVises[material] = coilVis;
    }

  // loop over coils and build and place them
  G4int i = 0;
  for (const auto& info : coilInfos)
    {
      G4String iStr = std::to_string(i);
      G4String baseName = name + "_coil_" + iStr;
      auto coilSolid = new G4Tubs(baseName + "_solid",
                                  info.innerRadius,
                                  info.innerRadius + info.radialThickness,
                                  0.5*info.fullLengthZ,
                                  0,
                                  CLHEP::twopi);
      RegisterSolid(coilSolid);
      auto coilLV = new G4LogicalVolume(coilSolid, info.material, baseName + "_lv");
      RegisterLogicalVolume(coilLV);
      coilLV->SetVisAttributes(coilVises[info.material]);
      
      auto coilPV = new G4PVPlacement(nullptr,
                                      G4ThreeVector(0,0,info.offsetZ),
                                      coilLV,
                                      baseName + "_pv",
                                      containerLogicalVolume,
                                      false,
                                      0,
                                      checkOverlaps);
      RegisterPhysicalVolume(coilPV);
      i++;
    }
}

void BDSMuonCooler::BuildAbsorbers()
{
  // make a unique set of absorber visualisations - only what we need
  std::set<G4Material*> absorberMaterials;
  for (const auto& info : absorberInfos)
    {absorberMaterials.insert(info.material);}
  std::map<G4Material*, G4VisAttributes*> absorberVises;
  for (const auto& material : absorberMaterials)
    {
      auto absVis = new G4VisAttributes(*BDSColourFromMaterial::Instance()->GetColour(material));
      absVis->SetVisibility(true);
      absVis->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
      RegisterVisAttributes(absVis);
      absorberVises[material] = absVis;
    }
  
  // loop over absorbers and build and place them
  G4int i = 0;
  for (const auto& info : absorberInfos)
    {
      G4String iStr = std::to_string(i);
      G4String baseName = name + "_absorber_" + iStr;
      G4VSolid* absSolid;
      G4RotationMatrix* rm = nullptr;
      if (info.absorberType == "wedge")
        {
          G4double halfHeight = 0.5*info.wedgeApexToBase;
          G4double dy = std::tan(info.wedgeOpeningAngle * 0.5) * info.wedgeApexToBase;
          std::vector<G4TwoVector> polygons = { {-halfHeight,0}, {halfHeight,dy}, {halfHeight,-dy} };
          absSolid = new G4ExtrudedSolid(baseName + "_solid",
                                         polygons,
                                         info.wedgeHeight*0.5,
                                         G4TwoVector(), 1.0,
                                         G4TwoVector(), 1.0);
          rm = new G4RotationMatrix();
          rm->rotate(CLHEP::halfpi, G4ThreeVector(1,0,0));
          rm->rotate(info.wedgeRotationAngle, G4ThreeVector(0,1,0));
          RegisterRotationMatrix(rm);
        }
      else if (info.absorberType == "cylinder")
        {
          absSolid = new G4Tubs(baseName + "_solid",
                                0,
                                info.cylinderRadius,
                                0.5 * info.cylinderLength,
                                0,
                                CLHEP::twopi);
        }
      else
        {throw BDSException(__METHOD_NAME__,"unknown absorber type \"" + info.absorberType + "\"");}
      RegisterSolid(absSolid);
      auto absLV = new G4LogicalVolume(absSolid, info.material, baseName + "_lv");
      RegisterLogicalVolume(absLV);
      absLV->SetVisAttributes(absorberVises[info.material]);
      
      auto coilPV = new G4PVPlacement(rm,
                                      info.placement,
                                      absLV,
                                      baseName + "_pv",
                                      containerLogicalVolume,
                                      false,
                                      0,
                                      checkOverlaps);
      RegisterPhysicalVolume(coilPV);
      i++;
    }
}

void BDSMuonCooler::BuildCavities()
{
  struct DriftSpec
  {
    BDSBeamPipeInfo* bpRecipe;
    G4ThreeVector position;
  };
  std::vector<DriftSpec> beampipeInfos;
  for (G4int i = 0; i < (G4int)cavityInfos.size() - 1; i++)
    {
      G4double zA = cavityInfos[i].offsetZ   + 0.5*cavityInfos[i].lengthZ;
      G4double zB = cavityInfos[i+1].offsetZ - 0.5*cavityInfos[i+1].lengthZ;
      G4double dZ = zB - zA;
      if (dZ >= 1*CLHEP::mm)
        {
          G4ThreeVector position = {0,0,zB-0.5*dZ};
          beampipeInfos.emplace_back(DriftSpec{beamPipeTemplate,position});
        }
    }
  
  std::vector<BDSCavityInfo> recipes;
  for (const auto& info : cavityInfos)
    {
      auto r = BDSCavityInfo(BDSCavityType::pillbox,
                             info.cavityMaterial,
                             info.windowRadius,
                             info.cavityThickness,
                             info.cavityRadius,
                             info.lengthZ * 0.5);
      r.inputWindowMaterial   = info.windowMaterial;
      r.inputWindowThickness  = info.windowThickness;
      r.outputWindowMaterial  = info.windowMaterial;
      r.outputWindowThickness = info.windowThickness;
      recipes.push_back(r);
    }
  
  std::vector<BDSCavity*> cavities;
  for (G4int i = 0; i < (G4int)recipes.size(); i++)
    {
      G4String cavityName = name + "_cavity_" + std::to_string(i);
      auto cavity = BDSCavityFactory::Instance()->CreateCavity(cavityName,
                                                               cavityInfos[i].lengthZ,
                                                               &recipes[i],
                                                               cavityInfos[i].vacuumMaterial);
      
      auto cavityPV = new G4PVPlacement(nullptr,
                                        G4ThreeVector(0,0,cavityInfos[i].offsetZ),
                                        cavity->GetContainerLogicalVolume(),
                                        cavityName + "_pv",
                                        containerLogicalVolume,
                                        false,
                                        0,
                                        checkOverlaps);
      RegisterPhysicalVolume(cavityPV);
      RegisterDaughter(cavity);
    }
}

void BDSMuonCooler::BuildField()
{
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(outerFieldRecipe,
                                                            containerLogicalVolume,
                                                            true);
}
