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
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldInfoExtra.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMaterials.hh"
#include "BDSMuonCooler.hh"
#include "BDSMuonCoolerBuilder.hh"
#include "BDSMuonCoolerStructs.hh"
#include "BDSParser.hh"

#include "parser/coolingchannel.h"
#include "parser/element.h"

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <list>
#include <set>
#include <string>
#include <vector>


BDSMuonCooler* BDS::BuildMuonCooler(const G4String& elementName,
                                    G4double chordLength,
                                    G4double horizontalWidth,
                                    const GMAD::CoolingChannel& definition,
                                    BDSBeamPipeInfo* beamPipeInfo,
                                    G4double designRigidity)
{
  G4double elementRadius = horizontalWidth * 0.5;

  G4String definitionName = definition.name;

  // build recipes for coils
  std::vector<BDS::MuonCoolerCoilInfo> coilInfos = BDS::BuildMuonCoolerCoilInfos(definition);
  // check potential overlaps
  // boundary squares in 2D - keep them to reuse for various checks
  std::vector<BDS::SquareCheck> coilSquares = BDS::MuonCoolerSquaresFromCoils(coilInfos);
  BDS::CheckMuonCoolerCoilInfosForOverlaps(definitionName,
                                           elementName,
                                           coilSquares,
                                           chordLength,
                                           elementRadius);

  // build recipes for rf cavities
  std::vector<BDS::MuonCoolerCavityInfo> cavityInfos = BDS::BuildMuonCoolerCavityInfos(definition);
  // check potential overlaps
  // boundary squares in 2D - keep them to reuse for various checks
  std::vector<BDS::SquareCheck> cavitySquares = BDS::MuonCoolerSquaresFromCavities(cavityInfos);
  BDS::CheckMuonCoolerCavityInfosForOverlaps(definitionName,
                                             elementName,
                                             cavitySquares,
                                             coilSquares,
                                             chordLength,
                                             elementRadius);

  // build recipes for absorbers
  std::vector<BDS::MuonCoolerAbsorberInfo> absorberInfos = BDS::BuildMuonCoolerAbsorberInfo(definition);
  // check potential overlaps
  BDS::CheckMuonCoolerAbsorberInfoForOverlaps(definitionName,
                                              elementName,
                                              absorberInfos,
                                              cavitySquares,
                                              coilSquares,
                                              chordLength,
                                              elementRadius);

  // build combined field recipe
  BDSFieldInfo* outerFieldRecipe = BDS::BuildMuonCoolerFieldRecipe(definitionName,
                                                     designRigidity,
                                                     definition.integrator,
                                                     definition.magneticFieldModel,
                                                     definition.electricFieldModel,
                                                     coilInfos,
                                                     cavityInfos);



  // build final object for beam line
  G4Material* surroundingMaterial = BDSMaterials::Instance()->GetMaterial(definition.surroundingMaterial);
  auto result = new BDSMuonCooler(elementName,
                                  chordLength,
                                  elementRadius,
                                  surroundingMaterial,
                                  coilInfos,
                                  cavityInfos,
                                  absorberInfos,
                                  beamPipeInfo,
                                  outerFieldRecipe);
  return result;
}

std::vector<BDS::MuonCoolerCoilInfo> BDS::BuildMuonCoolerCoilInfos(const GMAD::CoolingChannel& definition)
{
  std::vector<BDS::MuonCoolerCoilInfo> result;
  
  // Check we have matching coil parameter sizes or tolerate 1 variable for all
  G4int nCoils = definition.nCoils;
  std::vector<std::string> coilParamNames = {"coilInnerRadius",
                                             "coilRadialThickness",
                                             "coilLengthZ",
                                             "coilCurrent",
                                             "coilOffsetZ"};
  std::vector<const std::list<double>*> coilVars = {&(definition.coilInnerRadius),
                                                    &(definition.coilRadialThickness),
                                                    &(definition.coilLengthZ),
                                                    &(definition.coilCurrent),
                                                    &(definition.coilOffsetZ)};
  std::vector<std::vector<double> > coilVarsV;
  BDS::MuonParamsToVector(definition.name,
                          coilVars,
                          coilParamNames,
                          nCoils,
                          coilVarsV);

  std::vector<G4Material*> coilMaterials;
  BDS::MuonParamsToMaterials(definition.name,
                             "coilMaterial",
                             definition.coilMaterial,
                             nCoils,
                             coilMaterials);
  
  // build coil infos
  for (G4int i = 0; i < nCoils; i++)
    {
      BDS::MuonCoolerCoilInfo info = {coilVarsV[0][i] * CLHEP::m,      // innerRadius
                                      coilVarsV[1][i] * CLHEP::m,      // radialThickness
                                      coilVarsV[2][i] * CLHEP::m,      // lengthZ
                                      coilVarsV[3][i] * CLHEP::ampere, // current
                                      coilVarsV[4][i] * CLHEP::m,      // offsetZ
                                      coilMaterials[i],        // no material for now
                                      definition.onAxisTolerance,
                                      definition.nSheets

      };
      result.push_back(info);
    }
  
  if (definition.mirrorCoils)
    {
      result.reserve(2*result.size());
      std::copy(result.rbegin(), result.rend(), std::back_inserter(result));
      for (G4int j = (G4int)result.size() - 1; j > nCoils - 1; j--)
        {result[j].offsetZ *= -1;}
    }
  
  return result;
}

void BDS::CheckMuonCoolerCoilInfosForOverlaps(const G4String& definitionName,
                                              const G4String& elementName,
                                              const std::vector<BDS::SquareCheck>& coilSquares,
                                              G4double elementChordLength,
                                              G4double elementRadius)
{
  // To check the overlaps, we will do it only in 2D in a cross-section considering the square
  // polygons of the coil blocks. We will do a very simple point in square check - doesn't work
  // if squares are rotated for example.
  
  // check each against each other
  for (G4int i = 0; i < (G4int)coilSquares.size(); i++)
    {
      for (G4int j = i+1; j < (G4int)coilSquares.size(); j++)
        {
          if (i == j)
            {continue;}
          if (coilSquares[i].Overlaps(coilSquares[j]))
            {
              G4String msg = "error in definition \"" + definitionName + "\": coil #" + std::to_string(i);
              msg += " overlaps with coil #" + std::to_string(j) + "\n" + coilSquares[i].Str() + "\n" + coilSquares[j].Str();
              throw BDSException(__METHOD_NAME__,msg);
            }
        }
    }
  
  // check all lie inside the container length and radius
  G4double halfLength = 0.5*elementChordLength;
  for (G4int i = 0; i < (G4int)coilSquares.size(); i++)
    {
      if (std::abs(coilSquares[i].z1) > halfLength || std::abs(coilSquares[i].z2) > halfLength)
        {
          G4String msg = "error in definition \"" + definitionName + "\": coil #" + std::to_string(i);
          msg += " lies outside +- the half length (" + std::to_string(halfLength) + " mm) of the element \"" + elementName + "\"\n";
          msg += coilSquares[i].Str();
          throw BDSException(__METHOD_NAME__,msg);
        }
      if (coilSquares[i].r2 >= elementRadius)
        {
          G4String msg = "error in definition \"" + definitionName + "\": coil #" + std::to_string(i);
          msg += " outer radius (" + std::to_string(coilSquares[i].r2) + " mm) is greater than 1/2 the horizontalWidth of the element \"" + elementName + "\"\n";
          msg += elementName + ": horizontalWidth = " + std::to_string(elementRadius) + " mm\n";
          msg += "Coil cross-section: " + coilSquares[i].Str();
          throw BDSException(__METHOD_NAME__,msg);
        }
    }
}

std::vector<BDS::MuonCoolerAbsorberInfo> BDS::BuildMuonCoolerAbsorberInfo(const GMAD::CoolingChannel& definition)
{
  std::vector<BDS::MuonCoolerAbsorberInfo> result;
  
  G4int nAbsorbers = definition.nAbsorbers;
  std::vector<std::string> absParamNames = {"absorberOffsetZ",
                                            "absorberCylinderLength",
                                            "absorberCylinderRadius",
                                            "absorberWedgeOpeningAngle",
                                            "absorberWedgeHeight",
                                            "absorberWedgeRotationAngle",
                                            "absorberWedgeOffsetX",
                                            "absorberWedgeOffsetY",
                                            "absorberWedgeApexToBase"};
  std::vector<const std::list<double>*> absVars = {&(definition.absorberOffsetZ),
                                                   &(definition.absorberCylinderLength),
                                                   &(definition.absorberCylinderRadius),
                                                   &(definition.absorberWedgeOpeningAngle),
                                                   &(definition.absorberWedgeHeight),
                                                   &(definition.absorberWedgeRotationAngle),
                                                   &(definition.absorberWedgeOffsetX),
                                                   &(definition.absorberWedgeOffsetY),
                                                   &(definition.absorberWedgeApexToBase)};
  std::vector<std::vector<double> > absVarsV;
  BDS::MuonParamsToVector(definition.name,
                          absVars,
                          absParamNames,
                          nAbsorbers,
                          absVarsV);
  absVarsV.reserve(absVars.size());
  
  // check the absorber type list
  const auto typeListSize = definition.absorberType.size();
  if (definition.absorberType.empty() || (typeListSize != 1 && (G4int)typeListSize != nAbsorbers))
    {
      G4String msg = "error in coolingchannel definition \"" + definition.name + "\"\n";
      msg += "number of \"absorberType\" doesn't match nAbsorbers (" + std::to_string(nAbsorbers) + ") or isn't 1";
      throw BDSException(__METHOD_NAME__, msg);
    }
  
  // check contents of absorber type list
  const std::set<std::string> absorberTypes = {"wedge", "cylinder"};
  std::vector<std::string> absorberTypeV = {definition.absorberType.begin(), definition.absorberType.end()};
  if (absorberTypeV.size() == 1)
    {
      for (G4int i = 1; i < nAbsorbers; i++)
        {absorberTypeV.push_back(absorberTypeV[0]);}
    }
  for (G4int i = 0; i < (G4int)absorberTypeV.size(); i++)
    {
      auto search = absorberTypes.find(absorberTypeV[i]);
      if (search == absorberTypes.end())
        {
          G4String msg = "unknown type of absorber: \"" + absorberTypeV[i] + "\" at index " + std::to_string(i);
          msg += "\nin definition \"" + definition.name + "\"";
          throw BDSException(__METHOD_NAME__, msg);
        }
    }
  
  std::vector<G4Material*> absorberMaterials;
  BDS::MuonParamsToMaterials(definition.name,
                             "absorberMaterial",
                             definition.absorberMaterial,
                             nAbsorbers,
                             absorberMaterials);
  
  // build absorber infos
  for (G4int i = 0; i < nAbsorbers; i++)
    {
      G4double dx = absVarsV[6][i] * CLHEP::m; // wedgeOffsetX
      G4double dy = absVarsV[7][i] * CLHEP::m; // wedgeOffsetY
      G4double dz = absVarsV[0][i] * CLHEP::m;
      BDS::MuonCoolerAbsorberInfo info = {absorberTypeV[i],            // absorberType
                                          absVarsV[1][i] * CLHEP::m,   // cylinderLength
                                          absVarsV[2][i] * CLHEP::m,   // cylinderRadius
                                          absVarsV[3][i] * CLHEP::rad, // wedgeOpeningAngle
                                          absVarsV[4][i] * CLHEP::m,   // wedgeHeight
                                          absVarsV[5][i] * CLHEP::rad, // wedgeRotationAngle
                                          G4ThreeVector(dx,dy,dz),     // wedgeOffset
                                          absVarsV[8][i] * CLHEP::m,   // wedgeApexToBase
                                          absorberMaterials[i]
      };
      result.push_back(info);
    }
  
  return result;
}

void BDS::CheckMuonCoolerAbsorberInfoForOverlaps(const G4String& /*definitionName*/,
                                                 const G4String& /*elementName*/,
                                                 const std::vector<BDS::MuonCoolerAbsorberInfo>& /*absorberInfos*/,
                                                 const std::vector<BDS::SquareCheck>& /*coilInfos*/,
                                                 const std::vector<BDS::SquareCheck>& /*cavityInfos*/,
                                                 G4double /*elementChordLength*/,
                                                 G4double /*elementRadius*/)
{;}

std::vector<BDS::MuonCoolerCavityInfo> BDS::BuildMuonCoolerCavityInfos(const GMAD::CoolingChannel& definition)
{
  std::vector<BDS::MuonCoolerCavityInfo> result;
  
  G4int nRFCavities = definition.nRFCavities;
  std::vector<std::string> rfParamNames = {"rfOffsetZ",
                                           "rfLength",
                                           "rfVoltage",
                                           "rfPhase",
                                           "rfFrequency",
                                           "rfWindowThickness",
                                           "rfWindowRadius",
                                           "rfCavityRadius",
                                           "rfCavityThickness",
                                           "rfTimeOffset"};
  std::vector<const std::list<double>*> rfVars = {&(definition.rfOffsetZ),
                                                  &(definition.rfLength),
                                                  &(definition.rfVoltage),
                                                  &(definition.rfPhase),
                                                  &(definition.rfFrequency),
                                                  &(definition.rfWindowThickness),
                                                  &(definition.rfWindowRadius),
                                                  &(definition.rfCavityRadius),
                                                  &(definition.rfCavityThickness),
                                                  &(definition.rfTimeOffset)};
  std::vector<std::vector<double> > rfVarsV;
  BDS::MuonParamsToVector(definition.name,
                          rfVars,
                          rfParamNames,
                          nRFCavities,
                          rfVarsV);
  std::vector<G4Material*> windowMaterials;
  BDS::MuonParamsToMaterials(definition.name,
                             "rfWindowMaterial",
                             definition.rfWindowMaterial,
                             nRFCavities,
                             windowMaterials);
  std::vector<G4Material*> cavityMaterials;
  BDS::MuonParamsToMaterials(definition.name,
                             "rfCavityMaterial",
                             definition.rfCavityMaterial,
                             nRFCavities,
                             cavityMaterials);
  std::vector<G4Material*> cavityVacuumMaterials;
  BDS::MuonParamsToMaterials(definition.name,
                             "rfCavityVaccumMaterial",
                             definition.rfCavityVacuumMaterial,
                             nRFCavities,
                             cavityVacuumMaterials);

  // build cavity infos
  for (G4int i = 0; i < nRFCavities; i++)
    { // BUG: CHECK system of units
      BDS::MuonCoolerCavityInfo info = {rfVarsV[0][i] * CLHEP::m, // offsetZ
                                        rfVarsV[1][i] * CLHEP::m, // lengthZ
                                        rfVarsV[2][i] * CLHEP::megavolt / CLHEP::meter, // peakEfield
                                        rfVarsV[3][i] * CLHEP::rad,    // phaseOffset
                                        rfVarsV[4][i] * CLHEP::hertz, // frequency
                                        cavityVacuumMaterials[i], // cavity vacuum material
                                        rfVarsV[5][i] * CLHEP::m, // windowThickness
                                        windowMaterials[i],       // window material
                                        rfVarsV[6][i] * CLHEP::m, // windowRadius
                                        cavityMaterials[i],       // cavity material
                                        rfVarsV[7][i] * CLHEP::m, // cavityRadius
                                        rfVarsV[8][i] * CLHEP::m, // cavityThickness
                                        rfVarsV[9][i] * CLHEP::s  // globalTimeOffset
                                        };
      result.push_back(info);
    }
  
  return result;
}

void BDS::CheckMuonCoolerCavityInfosForOverlaps(const G4String& /*definitionName*/,
                                                const G4String& /*elementName*/,
                                                const std::vector<BDS::SquareCheck>& /*cavitySquares*/,
                                                const std::vector<BDS::SquareCheck>& /*coilSquares*/,
                                                G4double /*elementChordLength*/,
                                                G4double /*elementRadius*/)
{;}

std::vector<BDS::SquareCheck> BDS::MuonCoolerSquaresFromCoils(const std::vector<BDS::MuonCoolerCoilInfo>& coilInfos)
{
  std::vector<BDS::SquareCheck> squares;
  squares.reserve(coilInfos.size());
  for (auto& info : coilInfos)
    {
      squares.emplace_back(BDS::SquareCheck{info.offsetZ-0.5*info.fullLengthZ,
                                            info.offsetZ+0.5*info.fullLengthZ,
                                            info.innerRadius,
                                            info.innerRadius+info.radialThickness});
    }
  return squares;
}

std::vector<BDS::SquareCheck> BDS::MuonCoolerSquaresFromCavities(const std::vector<BDS::MuonCoolerCavityInfo>& cavityInfos)
{
  std::vector<BDS::SquareCheck> squares;
  squares.reserve(cavityInfos.size());
  /*
  for (auto& info : cavityInfos)
    {
      squares.emplace_back(BDS::SquareCheck{info.offsetZ-0.5*info.lengthZ,
                                            info.offsetZ+0.5*info.lengthZ,
                                            info.innerRadius,
                                            info.innerRadius+info.radialThickness});
    }
  */
  return squares;
}

void BDS::MuonParamsToVector(const G4String&                              definitionName,
                             const std::vector<const std::list<double>*>& params,
                             const std::vector<std::string>&              paramNames,
                             G4int                                        nExpectedParams,
                             std::vector<std::vector<double>>&            paramsV)
{
  // convert to vectors from lists
  paramsV.reserve(params.size());
  for (auto l: params)
    {paramsV.emplace_back(std::vector<double>{std::cbegin(*l), std::cend(*l)});}
  
  // check lengths are either 1 or nCoils
  for (G4int i = 0; i < (G4int) paramsV.size(); i++)
    {
      auto& v = paramsV[i];
      if (((G4int) v.size() != nExpectedParams && v.size() != 1) || v.empty())
        {
          G4String msg = "error in coolingchannel definition \"" + definitionName + "\"\n";
          msg += "number of " + paramNames[i] + " doesn't match expected number (" + std::to_string(nExpectedParams) + ") or isn't 1";
          throw BDSException(__METHOD_NAME__, msg);
        }
      // if the vector is single-valued, then we copy the first value up to nExpected
      // values so all are the same size
      if (v.size() == 1)
        {
          for (G4int j = 1; j < nExpectedParams; j++)
            {v.push_back(v[0]);}
        }
    }
}

void BDS::MuonParamsToMaterials(const G4String&               definitionName,
                                const G4String&               variableName,
                                const std::list<std::string>& materialNames,
                                G4int                         nExpectedParams,
                                std::vector<G4Material*>&     materials)
{
  // check size
  if (materialNames.empty() || (materialNames.size() != 1 && (G4int)materialNames.size() != nExpectedParams))
    {
      G4String msg = "error in coolingchannel definition \"" + definitionName + "\"\n";
      msg += "number of \"" + variableName + "\" doesn't match expected number (" + std::to_string(nExpectedParams) + ") or isn't 1";
      throw BDSException(__METHOD_NAME__, msg);
    }
  
  materials.reserve(nExpectedParams);  

  if (materialNames.size() == 1)
    {
      G4String materialName = G4String(materialNames.front());
      G4Material* material = BDSMaterials::Instance()->GetMaterial(materialName);
      for (G4int i = 0; i < nExpectedParams; i++)
        {materials.push_back(material);}
    }
  else
    {
      std::vector<std::string> materialNamesV = {std::begin(materialNames), std::end(materialNames)};
      for (G4int i = 0; i < (G4int)materialNamesV.size(); i++)
        {
          G4Material* material = BDSMaterials::Instance()->GetMaterial(materialNamesV[i]);
          materials[i] = material;
        }
    }
}

BDSFieldInfo* BDS::BuildMuonCoolerFieldRecipe(const G4String& definitionName,
                                              G4double designRigidity,
                                              const G4String& integrator,
                                              const G4String& magneticFieldModel,
                                              const G4String& electricFieldModel,
                                              const std::vector<BDS::MuonCoolerCoilInfo>& coilInfos,
                                              const std::vector<BDS::MuonCoolerCavityInfo>& cavityInfos)
{
  try
    {
      BDSIntegratorType it = BDS::DetermineIntegratorType(integrator);
      BDSFieldType mt = BDS::DetermineFieldType(magneticFieldModel);
      BDSFieldType et = BDS::DetermineFieldType(electricFieldModel);
      auto ei = new BDSFieldInfoExtraMuonCooler(mt, et, coilInfos, cavityInfos);
      
      auto result = new BDSFieldInfo(BDSFieldType::muoncooler, designRigidity, it);
      result->SetNameOfParserDefinition(definitionName);
      result->SetExtraInfo(ei);
      return result;
    }
  catch (BDSException& e)
    {
      e.AppendToMessage("\nProblem with field in coolingchannel definition \"" + definitionName + "\"");
      throw e;
    }
}
