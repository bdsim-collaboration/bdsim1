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
#ifndef BDSMUONCOOLERBUILDER_H
#define BDSMUONCOOLERBUILDER_H

#include <vector>

class BDSBeamPipeInfo;
class BDSMuonCooler;
namespace GMAD
{
  class CoolingChannel;
  struct Element;
}

namespace BDS
{
  struct MuonCoolerCoilInfo; // forward declaration
  struct MuonCoolerCavityInfo;
  struct MuonCoolerAbsorberInfo;
  struct SquareCheck;
  
  /// Build a muon cooler beam line element interpreting and checking
  /// all the relevant parameters from the parser cooling definition. This also
  /// handles units from the parser to Geant4. Materials are upgraded
  /// from strings to real materials as well as some other information refined.
  BDSMuonCooler* BuildMuonCooler(const G4String& elementName,
                                 G4double chordLength,
                                 G4double horizontalWidth,
                                 const GMAD::CoolingChannel& definition,
                                 BDSBeamPipeInfo* beamPipeInfo,
                                 G4double designRigidity);
  
  /// Loop over parser definition variables and construct a set of infos. Tolerate
  /// if some variables are single-valued with the intention that these are therefore
  /// the same of all the infos.
  std::vector<BDS::MuonCoolerCoilInfo> BuildMuonCoolerCoilInfos(const GMAD::CoolingChannel& definition);
  
  /// Check if any of the definitions will cause overlaps. Do this by using the square
  /// cross-section of each coil in 2D and perform a simple un-rotated square overlap
  /// calculation. Check they will also fit inside the container length, radius.
  void CheckMuonCoolerCoilInfosForOverlaps(const G4String& definitionName,
                                           const G4String& elementName,
                                           const std::vector<BDS::SquareCheck>& coilSquares,
                                           G4double elementChordLength,
                                           G4double elementRadius);
  
  std::vector<BDS::MuonCoolerAbsorberInfo> BuildMuonCoolerAbsorberInfo(const GMAD::CoolingChannel& definition);
  void CheckMuonCoolerAbsorberInfoForOverlaps(const G4String& definitionName,
                                              const G4String& elementName,
                                              const std::vector<BDS::MuonCoolerAbsorberInfo>& absorberInfos,
                                              const std::vector<BDS::SquareCheck>& cavitySquares,
                                              const std::vector<BDS::SquareCheck>& coilSquares,
                                              G4double elementChordLength,
                                              G4double elementRadius);
  
  std::vector<BDS::MuonCoolerCavityInfo> BuildMuonCoolerCavityInfos(const GMAD::CoolingChannel& definition);
  void CheckMuonCoolerCavityInfosForOverlaps(const G4String& definitionName,
                                             const G4String& elementName,
                                             const std::vector<BDS::SquareCheck>& cavitySquares,
                                             const std::vector<BDS::SquareCheck>& coilSquares,
                                             G4double elementChordLength,
                                             G4double elementRadius);
  
  /**
   *  @brief A very simple struct to permit checking un-rotated (i.e. axis-aligned) square overlaps.
   *  @author Laurie Nevay
   */
  struct SquareCheck
  {
    G4double z1;
    G4double z2;
    G4double r1;
    G4double r2;
    
    G4bool Overlaps(const SquareCheck& o) const
    {
      G4bool zOK = (o.z1 > z2 || o.z1 < z1) && (o.z2 > z2 || o.z2 < z1) && (o.z2 > o.z1);
      G4bool rOK = (o.r1 > r2 || o.r1 < r1) && (o.r2 > r2 || o.r2 < r1) && (o.r2 > o.r1);
      return !zOK && !rOK;
    }
    G4String Str() const {return "z1: " + std::to_string(z1) + ", z2: " + std::to_string(z2) + ", r1: " + std::to_string(r1) + ", r2: " + std::to_string(r2) + " (mm)";}
  };
  
  /// Utility function to make SquareCheck instances from coil info.
  std::vector<BDS::SquareCheck> MuonCoolerSquaresFromCoils(const std::vector<BDS::MuonCoolerCoilInfo>& coilInfos);
  /// Utility function to make SquareCheck instances from cavity info.
  std::vector<BDS::SquareCheck> MuonCoolerSquaresFromCavities(const std::vector<BDS::MuonCoolerCavityInfo>& cavityInfos);
  /// Utility function to convert list of doubles to vector, build vector of whether single valued and throw an expection
  /// if not nExpectedParams or 1.
  void MuonParamsToVector(const G4String&                              definitionName,
                          const std::vector<const std::list<double>*>& params,
                          const std::vector<std::string>&              paramNames,
                          G4int                                        nExpectedParams,
                          std::vector<std::vector<double>>&            paramsV);

  /// Utility function that returns (ie modifies by reference) a vector of
  /// G4Materials from either 1 or multiple materials in a list. Throws an
  /// exception if the number isn't 1 or nExpectedParams.
  void MuonParamsToMaterials(const G4String&               definitionName,
                             const G4String&               variableName,
                             const std::list<std::string>& materialNames,
                             G4int                         nExpectedParams,
                             std::vector<G4Material*>&     materials);
  
  BDSFieldInfo* BuildMuonCoolerFieldRecipe(const G4String& definitionName,
                                           G4double        designRigidity,
                                           const G4String& integrator,
                                           const G4String& magneticFieldModel,
                                           const G4String& electricFieldModel,
                                           const std::vector<BDS::MuonCoolerCoilInfo>& coilInfos,
                                           const std::vector<BDS::MuonCoolerCavityInfo>& cavityInfos);
}

#endif
