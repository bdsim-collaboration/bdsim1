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
#ifndef BDSPLASMAHDF5_H
#define BDSPLASMAHDF5_H

#include "H5Cpp.h"

#include <array>
#include <vector>

#include "globals.hh" // geant4 types / globals

/**
 * @brief A class to read plasma acceleration data from an HDF5 file format
 *
 * @author Marin Deniaud
 */

class BDSPlasmaHDF5 {
public:
    BDSPlasmaHDF5(G4String mainPath = "/data/0",
                  G4String particlePath = "/particles/elec_bunch_0",
                  G4String fieldPath = "/fields");
    virtual ~BDSPlasmaHDF5();

    void LoadHDF5(G4String filename);

    void CheckHDF5(G4String filename);

    std::vector<double> Get1DVector(H5::Group group, G4String setName);
    std::vector<double> Get3DVector(H5::Group group, G4String setName);

    void TestPrintFirstX();

    std::vector<double> GetX();
    std::vector<double> GetY();
    std::vector<double> GetZ();

    std::vector<double> GetPX();
    std::vector<double> GetPY();
    std::vector<double> GetPZ();

    std::vector<double> GetEr();
    std::vector<double> GetEz();
    std::vector<double> GetBt();

    G4String mainPath;
    G4String particlePath;
    G4String fieldPath;

    H5::Group particles;
    H5::Group fields;
};

#endif //BDSPLASMAHDF5_H
