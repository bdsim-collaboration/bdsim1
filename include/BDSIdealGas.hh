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

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = (pressure*averageMolarMass)/(R*temperature);

      G4cout << "density : " << density/1000 << G4endl;
      return density/1000; //convert kg.m-3 to g.cm-3
    }

    template <typename Type>
    G4double CalculateTemperatureFromPressureDensity(const std::list<G4String>& components,
                                                     const std::list<Type>& componentFractions,
                                                     G4double pressure,
                                                     G4double density) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double temperature = (pressure*averageMolarMass)/(R*density);

      return temperature;
    }

    template <typename Type>
    G4double CalculatePressureFromTemperatureDensity(const std::list<G4String>& components,
                                                     const std::list<Type>& componentFractions,
                                                     G4double temperature,
                                                     G4double density) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double pressure = (density*R*temperature)/(averageMolarMass);

      return pressure;
    }

    template <typename Type>
    G4double CalculateDensityFromNumberDensity(const std::list<G4String>& components,
                                               const std::list<Type>& componentFractions,
                                               G4double numberDensity) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = numberDensity*averageMolarMass/Avogadro;

      return density;
    }

    template <typename Type>
    G4double CalculateDensityFromMolarDensity(const std::list<G4String>& components,
                                              const std::list<Type>& componentFractions,
                                              G4double molarDensity) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = molarDensity*averageMolarMass;

      return density;
    }

    template <typename Type>
    G4double CalculateAverageMolarMass(const std::list<G4String>& components,
                                       const std::list<Type>& componentFractions){

      std::vector<G4String> componentsVector{ components.begin(), components.end() };
      std::vector<Type> componentFractionsVector{ componentFractions.begin(), componentFractions.end() };
      std::map<G4String, Type> componentsTable;


      G4double averageMolarMass = 0;
      G4double fracSum = 0;
      for (size_t i=0; i < componentsVector.size(); i++)
      {
        // TODO - Remove couts
        G4cout << "componentName : " << componentsVector[i] << " | componentFraction : " << componentFractionsVector[i] << G4endl;
        auto component = BDSMaterials::Instance()->GetMaterial(componentsVector[i]);

        size_t nbelement = component->GetNumberOfElements();
        G4cout << "GetNumberOfElements : " << nbelement << G4endl;
        if (nbelement == 1)
        {
          auto element = component->GetElement(0);
          auto molarMass = element->GetN();
          G4cout << "Element Name : " << element->GetName() << " | molarMass : " << molarMass << G4endl;
          //G4cout << "GetAtomsVector : "  << component->GetAtomsVector()[i] << G4endl;
          averageMolarMass = averageMolarMass + componentFractionsVector[i] * molarMass;
          fracSum = fracSum + componentFractionsVector[i];
        }
        else
        {
          G4cout << "=======================================" << G4endl;
          auto elementVector = component->GetElementVector();
          //G4cout << "GetAtomsVector : "  << component->GetAtomsVector()[i] << G4endl;
          std::list<G4String> elementNames;
          std::list<Type> elementFractions;
          for (const auto element: *elementVector)
          {
            elementNames.push_back(element->GetName());
          }
          for (size_t ii=0; ii < elementVector->size(); ii++)
          {
            elementFractions.push_back(component->GetFractionVector()[ii]);
          }
          averageMolarMass = averageMolarMass + componentFractionsVector[i] * CalculateAverageMolarMass(elementNames, elementFractions);
          fracSum = fracSum + componentFractionsVector[i];
          G4cout << "=======================================" << G4endl;
        }
      }

      G4cout << "averageMolarMass : " << averageMolarMass/fracSum << G4endl;
      return averageMolarMass/fracSum;
    }

    static void CheckGasLaw(G4double &temperature, G4double &pressure, G4double &density) {
      G4cout << temperature << " " << pressure << " " << density << G4endl;

      temperature = 300;

      G4cout << temperature << " " << pressure << " " << density << G4endl;
    }


private:
    ///constants
    G4double Avogadro = 6.022e23;
    G4double R = 8.314;
};

#endif
