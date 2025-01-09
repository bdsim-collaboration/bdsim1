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
#ifndef BDSGABORLENS_H
#define BDSGABORLENS_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class G4Colour;
class G4Material;

/**
 * @brief Gabor lens based on prototype design at Imperial College (https://doi.org/10.3390/app11104357)
 * 
 * @author Will Shields
 */

class BDSGaborLens: public BDSAcceleratorComponent
{
public:
    BDSGaborLens(const G4String& nameIn,
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
          BDSBeamPipeInfo* beamPipeInfoIn = nullptr,
          BDSFieldInfo* vacuumFieldInfoIn = nullptr);
    virtual ~BDSGaborLens();
  
protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();

  G4double      horizontalWidth;
  G4double      anodeRadius;
  G4double      anodeLength;
  G4Material*   anodeMaterial;
  G4double      anodeThickness;
  G4double      electrodeRadius;
  G4double      electrodeLength;
  G4Material*   electrodeMaterial;
  G4double      electrodeThickness;
  G4Material*   outerMaterial;
  G4Colour*     colour;
  BDSFieldInfo* vacuumFieldInfo;
  G4double      vacuumLength;
  G4double      endcapsLength;   /// total length of both end caps

private:
  /// Private default constructor to force the use of the supplied one.
  BDSGaborLens() = delete;

  /// Build the beam pipe
  void BuildBeamPipe();

  /// Build the outer volume
  void BuildOuter();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSGaborLens& operator=(const BDSGaborLens&) = delete;
  BDSGaborLens(BDSGaborLens&) = delete;
  ///@}

};

#endif
