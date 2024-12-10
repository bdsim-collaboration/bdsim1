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

#ifndef BDSBUNCHHDF5_H
#define BDSBUNCHHDF5_H

#include "BDSBunchFileBased.hh"

#include <array>
#include <vector>

#include "globals.hh" // geant4 types / globals

/**
 * @brief A bunch distribution that reads a HDF5 input file.
 *
 * @author Marin Deniaud based on Stewart Boogert
 */

class BDSBunchHDF5: public BDSBunchFileBased
{
public:
    BDSBunchHDF5();
    virtual ~BDSBunchHDF5();
    virtual void SetOptions(const BDSParticleDefinition* beamParticle,
                            const GMAD::Beam& beam,
                            const BDSBunchType& distrType,
                            G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
                            const G4double beamlineS = 0);

    virtual void Initialise();

    virtual BDSParticleCoordsFull GetNextParticleLocal();

    virtual void RecreateAdvanceToEvent(G4int eventOffset);

private:
    void LoadHDF5Data();

    G4bool   matchDistrFileLength; ///< Whether to only run the number of particles in the file.
    G4int    iRay;      ///< Iterator counter for current ray.
    G4int    nRays;     ///< Number of rays in file (1 counting).
    G4String fileName;  ///< File name.
    std::vector<std::array<double, 6>> HDF5Data; ///< Data.
    G4double mass;
};

#endif //BDSBUNCHHDF5_H
