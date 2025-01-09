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
#include "BDSElectronQuantumLevel.hh"

#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

BDSElectronQuantumLevel::BDSElectronQuantumLevel(G4int    nPrincipleNumberIn,
						 G4int    lAngularNumberIn,
						 G4double jSpinOrbitCouplingIn):
  nPrincipleNumber(nPrincipleNumberIn),
  lAngularNumber(lAngularNumberIn),
  jSpinOrbitCoupling(jSpinOrbitCouplingIn),
  maxOccupancy(0),
  currentOccupancy(0),
  excitedLifetime(0),
  timeOfExcitement(0)
{
  maxOccupancy=(jSpinOrbitCoupling*2)+1;
  levelEnergy=DiracEnergyNJ(nPrincipleNumberIn,jSpinOrbitCouplingIn);
}

BDSElectronQuantumLevel::~BDSElectronQuantumLevel()
{;}

G4double BDSElectronQuantumLevel::DiracEnergyNJ(G4int n, G4int j)
{
  G4double a = CLHEP::fine_structure_const;
  G4double c = CLHEP::c_light;
  return ((c*c*a*a)/(n*n))*(1+((a*a)/(n*n))*((n/(j+0.5))+0.75));
}

void BDSElectronQuantumLevel::AddElectrons(G4int number)
{
  if(maxOccupancy - currentOccupancy >= number)
    {currentOccupancy +=number;}
  //else{error}
}


void BDSElectronQuantumLevel::RemoveElectrons(G4int number)
{
  if (currentOccupancy >= number)
    {currentOccupancy = currentOccupancy - number;}
  //else error
}
