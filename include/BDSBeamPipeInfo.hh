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
#ifndef BDSBEAMPIPEINFO_H
#define BDSBEAMPIPEINFO_H

#include "BDSBeamPipeType.hh"

#include "globals.hh"         // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSExtent;
class G4Material;

/**
 * @brief Holder class for all information required to describe a beam pipe model. 
 * 
 * This reduces the number of argument
 * to all magnet constructors plus aggregates common tasks
 * in the component factory.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeInfo
{
public:
  /// Deleted default constructor to ensure one of supplied constructors is used.
  BDSBeamPipeInfo() = delete;
  /// extra constructor to assign all members at once.
  BDSBeamPipeInfo(BDSBeamPipeType beamPipeTypeIn,
		  G4double        aper1In,
		  G4double        aper2In,
		  G4double        aper3In,
		  G4double        aper4In,
		  G4Material*     vacuumMaterialIn,
		  G4double        beamPipeThicknessIn,
		  G4Material*     beamPipeMaterialIn,
		  const G4ThreeVector& inputFaceNormalIn  = G4ThreeVector(0,0,-1),
		  const G4ThreeVector& outputFaceNormalIn = G4ThreeVector(0,0,1),
      const G4String& pointsFileNameIn = "",
      const G4String& pointsUnitIn = "mm");

  /// Constructor with string descriptors of materials and type. Automatically determined
  /// using BDSBeamPipeType and BDSMaterials
  BDSBeamPipeInfo(const G4String&      beamPipeTypeIn,
		  G4double             aper1In,
		  G4double             aper2In,
		  G4double             aper3In,
		  G4double             aper4In,
		  const G4String&      vacuumMaterialIn,
		  G4double             beamPipeThicknessIn,
		  const G4String&      beamPipeMaterialIn,
		  const G4ThreeVector& inputFaceNormalIn  = G4ThreeVector(0,0,-1),
		  const G4ThreeVector& outputFaceNormalIn = G4ThreeVector(0,0,1));

  /// Constructor that allows a default model to be used as backup. Checks on parameter
  /// validity are done after substituting unset values by values from defaultInfo.
  BDSBeamPipeInfo(const BDSBeamPipeInfo* defaultInfo,
		  const G4String&      beamPipeTypeIn,
		  G4double             aper1In,
		  G4double             aper2In,
		  G4double             aper3In,
		  G4double             aper4In,
		  const G4String&      vacuumMaterialIn,
		  G4double             beamPipeThicknessIn,
		  const G4String&      beamPipeMaterialIn,
		  const G4ThreeVector& inputFaceNormalIn  = G4ThreeVector(0,0,-1),
		  const G4ThreeVector& outputFaceNormalIn = G4ThreeVector(0,0,1));

  /// Function to check relevant aperture values are set.  This is really a dispatch function
  /// for other aperture specific methods below
  void CheckApertureInfo();

  /// Return a BDSExtent instance for the transverse extents - extent in Z is 0 as this is
  /// only aperture information. Includes beam pipe thickness.
  BDSExtent Extent() const;

  /// Return an extent for just the raw aperture.
  BDSExtent ExtentInner() const;

  /// Return an indicative extent of the beam pipe - typically the maximum of x or y extent.
  G4double IndicativeRadius() const;

  /// Return an indicative inner extent for the beam pipe vacuum.
  G4double IndicativeRadiusInner() const;

  /// Return a copy that's shrunk by value margin.
  BDSBeamPipeInfo ShrinkBy(G4double margin) const;

  ///@{ Public member for direct access
  BDSBeamPipeType beamPipeType;
  G4double        aper1;
  G4double        aper2;
  G4double        aper3;
  G4double        aper4;
  G4double        aperOffsetX;
  G4double        aperOffsetY;
  G4Material*     vacuumMaterial;
  G4double        beamPipeThickness;
  G4Material*     beamPipeMaterial;
  G4ThreeVector   inputFaceNormal;
  G4ThreeVector   outputFaceNormal;
  G4String        pointsFileName;
  G4String        pointsUnit;
  ///@}
  
private:
  /// Parse the type string to extract the file name and the optional units and assign to member variables.
  void CheckAndSetPointsInfo(const G4String& beamPipeTypeIn);
  
  /// Function to check whether a parameter is set (using BDSUtilities BDS::IsFinite() ). If the
  /// accompanying G4bool setAper1 (for example) is true, the parameter will be checked for
  /// being set. All parameters required to be checked are checked and appropriate warnings
  /// printed before exiting if any fail that test.
  void CheckRequiredParametersSet(G4bool setAper1, G4bool setAper2,
				  G4bool setAper3, G4bool setAper4);
  
  /// Aperture info check for circular aperture.
  void InfoOKForCircular();

  /// Aperture info check for elliptical aperture.
  void InfoOKForElliptical();

  /// Aperture info check for rectangular aperture.
  void InfoOKForRectangular();

  /// Aperture info check for lhc aperture.
  void InfoOKForLHC();

  /// Aperture info check for lhc detailed aperture.
  void InfoOKForLHCDetailed();

  /// Aperture info check for rectellipse aperture.
  void InfoOKForRectEllipse();

  /// Aperture info check for racetrack aperture.
  void InfoOKForRaceTrack();

  /// Aperture info check for octagonal aperture.
  void InfoOKForOctagonal();

  /// Aperture info check for CLIC PCL aperture.
  void InfoOKForClicPCL();

  /// Aperture info check for Rhombus aperture.
  void InfoOKForRhombus();
};

#endif
