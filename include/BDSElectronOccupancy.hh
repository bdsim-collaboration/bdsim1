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
#ifndef BDSELECTRONOCCUPANCY_H
#define BDSELECTRONOCCUPANCY_H
#include "globals.hh"
#include "G4ElectronOccupancy.hh"

#include <tuple>
#include <vector>

class BDSElectronQuantumLevel;

/**
 * @brief Electron Occupancy for more than just n quantum number
 *
 * @author Siobhan Alden
 */

class BDSElectronOccupancy
{
public:
  BDSElectronOccupancy(G4int maxn);
  ~BDSElectronOccupancy();
  void CreateNewLevel(G4int n, G4int l, G4int j);
  static G4bool CompareEnergy(BDSElectronQuantumLevel* level1, BDSElectronQuantumLevel* level2);
  void PopulateLevels();
  void AddElectrons(G4int number);
  void AddElectrons(G4int n, G4int l,G4double j, G4int number);
  void RemoveElectrons(G4int number);
  void RemoveElectrons(G4int n, G4int l,G4double j, G4int number);
  void SetStateLifetime(G4int n, G4int l, G4double lifetime);
  G4double GetStateLifetime(G4int n, G4int l, G4double j);
  G4bool StatePopulated(G4int n, G4int l, G4double j);
  void SetTimeOfExciation(G4double timeOfExcitationIn, G4int n, G4int l, G4double j);
  G4double GetTimeOfExcitation(G4int n, G4int l, G4double j);
  
  // setters
  inline void SetTotalElectrons(G4int totalElectronsIn)   {totalElectrons=totalElectronsIn;}
  
private:
  std::vector<BDSElectronQuantumLevel*> stateList;
  G4int totalElectrons;
};

#endif
