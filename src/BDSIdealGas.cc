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

#include "BDSIdealGas.hh"


G4double BDSIdealGas::CalculateTemperatureFromPressureDensity(const std::list<G4String>& components,
                                                              const std::list<G4double>& componentFractions,
                                                              G4double pressure,
                                                              G4double density) {

  G4double averageMass = CalculateAverageMass(components, componentFractions);
  G4double temperature = (pressure*Avogadro*averageMass)/(R*density);

  return temperature;
}

G4double BDSIdealGas::CalculatePressureFromTemperatureDensity(const std::list<G4String>& components,
                                                              const std::list<G4double>& componentFractions,
                                                              G4double temperature,
                                                              G4double density) {

  G4double averageMass = CalculateAverageMass(components, componentFractions);
  G4double pressure = (density*R*temperature)/(Avogadro*averageMass);

  return pressure;
}



G4double BDSIdealGas::CalculateDensityFromNumberDensity(const std::list<G4String>& components,
                                                        const std::list<G4double>& componentFractions,
                                                        G4double numberDensity) {

  G4double averageMass = CalculateAverageMass(components, componentFractions);
  G4double density = numberDensity*averageMass;

  return density;
}

G4double BDSIdealGas::CalculateDensityFromMolarDensity(const std::list<G4String>& components,
                                                       const std::list<G4double>& componentFractions,
                                                       G4double molarDensity) {

  G4double averageMass = CalculateAverageMass(components, componentFractions);
  G4double density = molarDensity*Avogadro*averageMass;

  return density;
}

