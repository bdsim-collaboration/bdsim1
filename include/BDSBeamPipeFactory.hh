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
#ifndef BDSBEAMPIPEFACTORY_H
#define BDSBEAMPIPEFACTORY_H

#include "BDSBeamPipeType.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4ThreeVector.hh"

class G4Material;

class BDSBeamPipe;
class BDSBeamPipeInfo;
class BDSBeamPipeFactoryBase;

/**
 * @brief The main interface for using the beam pipe factories. 
 * 
 * Each function in this class mirrors one in BDSBeamPipeFactoryBase 
 * but with the addition of a BDSBeamPipeType enum as an argument. This
 * factory dispatches the call to the correct factory and should be 
 * the only one used by the caller.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactory
{
public:
  static BDSBeamPipeFactory* Instance(); ///< Singleton accessor.
  
  ~BDSBeamPipeFactory();

  BDSBeamPipe* CreateBeamPipe(const G4String&  name,
			      G4double         length,
			      BDSBeamPipeInfo* bpi);

  /// Create a circular vacuum volume that's smaller than the aperture definition
  /// by length safety large suitable for intersections with something that should
  /// fit in the vacuum of a beam pipe that would be created with the same recipe.
  BDSBeamPipe* CreateBeamPipeForVacuumIntersection(const G4String&  name,
						   G4double         length,
						   BDSBeamPipeInfo* info);
  
  BDSBeamPipe* CreateBeamPipe(BDSBeamPipeType beamPipeTypeIn,            // aperture type
			      const G4String& nameIn,                    // name
			      G4double        lengthIn,                  // length [mm]
			      G4double        aper1 = 0,                 // aperture parameter 1
			      G4double        aper2 = 0,                 // aperture parameter 2
			      G4double        aper3 = 0,                 // aperture parameter 3
			      G4double        aper4 = 0,                 // aperture parameter 4
			      G4Material*     vacuumMaterialIn = nullptr,// vacuum material
			      G4double        beamPipeThicknessIn = 0,   // beampipe thickness [mm]
			      G4Material*     beamPipeMaterialIn = nullptr, // beampipe material
			      const G4String& pointsFileIn       = "",
			      const G4String& pointsUnitIn       = "");

  BDSBeamPipe* CreateBeamPipe(BDSBeamPipeType beamPipeType,
			      const G4String& name,
			      G4double        length,
			      const G4ThreeVector& inputFaceNormal,
			      const G4ThreeVector& outputFaceNormal,
			      G4double        aper1,
			      G4double        aper2,
			      G4double        aper3,
			      G4double        aper4,
			      G4Material*     vacuumMaterial,
			      G4double        beamPipeThickness,
			      G4Material*     beamPipeMaterial,
			      const G4String& pointsFileIn      = "",
			      const G4String& pointsUnitIn      = "");

private:
  BDSBeamPipeFactory(); ///< Private constructor as singleton pattern.
  static BDSBeamPipeFactory* instance; ///< Singleton instance pointer.

  /// Return the appropriate factory singleton pointer given a type.
  BDSBeamPipeFactoryBase* GetAppropriateFactory(BDSBeamPipeType beamPipeTypeIn);

  /// @{ Factory instance.
  BDSBeamPipeFactoryBase* circular;
  BDSBeamPipeFactoryBase* elliptical;
  BDSBeamPipeFactoryBase* rectangular;
  BDSBeamPipeFactoryBase* lhc;
  BDSBeamPipeFactoryBase* lhcdetailed;
  BDSBeamPipeFactoryBase* rectellipse;
  BDSBeamPipeFactoryBase* racetrack;
  BDSBeamPipeFactoryBase* rhombus;
  BDSBeamPipeFactoryBase* octagonal;
  BDSBeamPipeFactoryBase* circularvacuum;
  BDSBeamPipeFactoryBase* clicpcl;
  BDSBeamPipeFactoryBase* pointsfile;
  /// @}
};



#endif
