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
#ifndef BDSIDEALGAS_H
#define BDSIDEALGAS_H

#include "globals.hh" // geant4 globals / types

#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <vector>

class BDSIdealGas{
public:
    G4double CalculateDensityFromPressureTemperature(const std::list<G4String>& components,
                                                     const std::list<G4double>& componentFractions,
                                                     G4double pressure,
                                                     G4double temperature);
};

#endif
