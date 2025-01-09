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
#ifndef BDSPOLARIZATIONSTATE_H
#define BDSPOLARIZATIONSTATE_H
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "parser/beam.h"

/**
 * @brief Polarization state to pass to processes
 *
 * @author Siobhan Alden
 */


class BDSPolarizationState
{
public:
    BDSPolarizationState(const GMAD::Beam& beamDefinition);
    ~BDSPolarizationState();
    inline void SetPol1(G4double pol1In){pol1=pol1In;}
    inline void SetPol2(G4double pol2In){pol1=pol2In;}
    inline void SetPol3(G4double pol3In){pol1=pol3In;}
    inline G4double GetPol1() {return pol1;}
    inline G4double GetPol2() {return pol2;}
    inline G4double GetPol3() {return pol3;}
    inline void SetPolarization(G4ThreeVector polarizationIn){polarization=polarizationIn;}
    inline G4ThreeVector GetPolarization() {return polarization;}

private:
    G4double pol1=0;
    G4double pol2=0;
    G4double pol3=0;
    G4ThreeVector polarization;

};



#endif
