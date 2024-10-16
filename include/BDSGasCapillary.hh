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
#ifndef BDSGASCAPILARY_H
#define BDSGASCAPILARY_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class G4Colour;
class G4Material;
class G4VSolid;

/**
 * @brief Base class for gas capillary.
 *
 * @author Marin Deniaud
 */

class BDSGasCapillary: public BDSAcceleratorComponent
{
public:
    BDSGasCapillary(const G4String&  name,
                    G4double         length,
                    BDSBeamPipeInfo* beamPipeInfo,
                    G4double         horizontalWidth,
                    G4Material*      capillaryOuterMaterial,
                    G4Material*      capillaryGasMaterial,
                    G4Material*      electrodeMaterial,
                    G4double         capillaryGasDiameter = 0,
                    G4double         electrodeThickness = 0,
                    G4bool           circularOuterIn = false);
    virtual ~BDSGasCapillary();

    /// @{ Accessor.
    virtual G4String CapillaryOuterMaterial() const;
    virtual G4String CapillaryGasMaterial() const;
    virtual G4String ElectrodeMaterial() const;
    /// @}


protected:
    /// Check and update parameters before construction. Called at the start of Build() as
    /// we can't call a virtual function in a constructor.
    virtual void CheckParameters();

    virtual void BuildContainerLogicalVolumeOld();
    virtual void BuildContainerLogicalVolume();

    virtual void BuildUncutSolid();
    virtual void BuildHolesSolid();
    virtual void BuildCutSolid();

    virtual void Build();

    virtual void SetupLV(G4LogicalVolume* lv, G4Colour* colour);

    G4double    horizontalWidth;      ///< Horizontal width.
    G4double    capillaryGasDiameter; ///< Aperture diameter of the gas.
    G4double    electrodeThickness;   ///< Thickness of the two electrodes
    G4bool      circularOuter;        ///< Aperture type of the outer wall

    ///@{ Solids:
    G4VSolid* capillaryOuterSolid;
    G4VSolid* capillaryGasSolid;
    G4VSolid* electrodeSolid;

    G4VSolid* capillaryOuterUncutSolid;
    G4VSolid* electrodeUncutSolid;

    G4VSolid* capillaryHoleSolid;
    G4VSolid* electrodeHoleSolid;
    ///@}

    ///@{ Materials:
    G4Material* capillaryOuterMaterial;
    G4Material* capillaryGasMaterial;
    G4Material* electrodeMaterial;
    ///@}

    ///@{ Logical Volumes:
    G4LogicalVolume* capillaryOuterLogicalVolume;
    G4LogicalVolume* capillaryGasLogicalVolume;
    G4LogicalVolume* electrodeLogicalVolume;
    ///@}

private:
    /// Private default constructor to force the use of the supplied one.
    BDSGasCapillary() = delete;

    /// @{ Assignment and copy constructor not implemented nor used
    BDSGasCapillary& operator=(const BDSGasCapillary&) = delete;
    BDSGasCapillary(BDSGasCapillary&) = delete;
    /// @}
};

#endif
