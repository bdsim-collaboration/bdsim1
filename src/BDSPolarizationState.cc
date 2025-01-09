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
#include "BDSPolarizationState.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

BDSPolarizationState::BDSPolarizationState(const GMAD::Beam& beamDefinition)
{
    pol1=beamDefinition.polarization1;
    pol2=beamDefinition.polarization2;
    pol3=beamDefinition.polarization3;
    polarization.set(pol1,pol2,pol3);
}

BDSPolarizationState::~BDSPolarizationState()
{;}
