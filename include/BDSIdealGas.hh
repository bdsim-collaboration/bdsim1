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
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 globals / types

#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <vector>

/**
 * @brief A class to perform ideal gas calculations.
 *
 * @author Marin Deniaud
 */

class BDSIdealGas{
public:
    template <typename Type>
    static G4double CalculateDensityFromPressureTemperature(const std::list<G4String>& components,
                                                            const std::list<Type>& componentFractions,
                                                            G4double pressure,
                                                            G4double temperature) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = (pressure*averageMolarMass)/(CLHEP::Avogadro*CLHEP::k_Boltzmann*temperature);

      return density/1000; //convert kg.m-3 to g.cm-3
    }

    template <typename Type>
    static G4double CalculateTemperatureFromPressureDensity(const std::list<G4String>& components,
                                                            const std::list<Type>& componentFractions,
                                                            G4double pressure,
                                                            G4double density) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double temperature = (pressure*averageMolarMass)/(CLHEP::Avogadro*CLHEP::k_Boltzmann*(density/1000));

      return temperature;
    }

    template <typename Type>
    static G4double CalculatePressureFromTemperatureDensity(const std::list<G4String>& components,
                                                     const std::list<Type>& componentFractions,
                                                     G4double temperature,
                                                     G4double density) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double pressure = ((density/1000)*CLHEP::Avogadro*CLHEP::k_Boltzmann*temperature)/(averageMolarMass);

      return pressure;
    }

    template <typename Type>
    static G4double CalculateDensityFromNumberDensity(const std::list<G4String>& components,
                                               const std::list<Type>& componentFractions,
                                               G4double numberDensity) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = numberDensity*averageMolarMass/CLHEP::Avogadro;

      return density/1000;
    }

    template <typename Type>
    static G4double CalculateDensityFromMolarDensity(const std::list<G4String>& components,
                                              const std::list<Type>& componentFractions,
                                              G4double molarDensity) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = molarDensity*averageMolarMass;

      return density/1000;
    }

    template <typename Type>
    static G4double CalculateAverageMolarMass(const std::list<G4String>& components,
                                              const std::list<Type>& componentFractions){

      std::vector<G4String> componentsVector{ components.begin(), components.end() };
      std::vector<Type> componentFractionsVector{ componentFractions.begin(), componentFractions.end() };
      std::map<G4String, Type> componentsTable;

      G4double averageMolarMass = 0;
      G4double fracSum = 0;
      for (size_t i=0; i < componentsVector.size(); i++)
      {
        const G4String& componentName = componentsVector[i];
        auto component = BDSMaterials::Instance()->GetMaterial(componentName);

        size_t nbelement = component->GetNumberOfElements();
        if (nbelement == 1)
        {
          auto element = component->GetElement(0);
          auto molarMass = element->GetN();
          averageMolarMass = averageMolarMass + componentFractionsVector[i] * molarMass;
          fracSum = fracSum + componentFractionsVector[i];
        }
        else
        {
          auto elementVector = component->GetElementVector();
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
        }
      }

      return averageMolarMass/fracSum;
    }


    template <typename Type>
    static void CheckGasLaw(G4double &temperature, G4double &pressure, G4double &density,
                            const std::list<G4String>& components,
                            const std::list<Type>& componentFractions) {

#ifdef BDSDEBUG
      G4cout << "BDSIdealGas::CheckGasLaw: " << G4endl;
#endif
      G4double calcDensity = CalculateDensityFromPressureTemperature(components, componentFractions, pressure, temperature);
      if(density != calcDensity)
      {
        G4String msg = "Ideal gas density calculated from pressure and temperature doesn't match given density\n";
        msg += "Assuming temperature of 300K and computing correct pressure for this density";
        BDS::Warning(msg);
        temperature = 300;
        pressure = CalculatePressureFromTemperatureDensity(components, componentFractions, temperature, density);
      }
    }
};

#endif
