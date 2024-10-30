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

#include "BDSMaterials.hh"

#include "globals.hh" // geant4 globals / types

#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <vector>

class BDSIdealGas{
public:
    template <typename Type>
    G4double CalculateDensityFromPressureTemperature(const std::list<G4String>& components,
                                                     const std::list<Type>& componentFractions,
                                                     G4double pressure,
                                                     G4double temperature) {

      G4double averageMass = CalculateAverageMass(components, componentFractions);
      G4double density = (pressure*Avogadro*averageMass)/(R*temperature);

      return density;
    }

    G4double CalculateTemperatureFromPressureDensity(const std::list<G4String>& components,
                                                     const std::list<G4double>& componentFractions,
                                                     G4double pressure,
                                                     G4double density);

    G4double CalculatePressureFromTemperatureDensity(const std::list<G4String>& components,
                                                     const std::list<G4double>& componentFractions,
                                                     G4double temperature,
                                                     G4double density);

    G4double CalculateDensityFromNumberDensity(const std::list<G4String>& components,
                                               const std::list<G4double>& componentFractions,
                                               G4double numberDensity);

    G4double CalculateDensityFromMolarDensity(const std::list<G4String>& components,
                                              const std::list<G4double>& componentFractions,
                                              G4double molarDensity);

    template <typename Type>
    G4double CalculateAverageMass(const std::list<G4String>& components,
                                  const std::list<Type>& componentFractions){

      std::vector<G4String> componentsVector{ components.begin(), components.end() };
      std::vector<G4double> componentFractionsVector{ componentFractions.begin(), componentFractions.end() };

      G4double averageMass = 0;
      for (int i=0; i < componentsVector.size(); i++)
      {
        auto component = BDSMaterials::Instance()->GetMaterial(componentsVector[i]);
        // GET THE MASS NUMBER OR MASS FOR EACH ELEMENT
        G4double mass = componentFractionsVector[i] * 1;
        averageMass = averageMass + mass;
      }

      return averageMass;
    }

    ///constants
    G4double Avogadro = 6.022e23;
    G4double R = 8.314;
};

#endif
