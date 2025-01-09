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
#ifndef BDSElectronQuantumLevel_H
#define BDSElectronQuantumLevel_H

#include "G4ElectronOccupancy.hh"
#include <vector>
#include <tuple>

/**
 * @brief Electron Quantum levels for more than just n quantum number
 *
 * @author Siobhan Alden
 */

class BDSElectronQuantumLevel{
public:
	BDSElectronQuantumLevel(G4int n, G4int l, G4double j);
	~BDSElectronQuantumLevel();
    G4double DiracEnergyNJ(G4int n, G4int j);
	void AddElectrons(G4int number);
	void RemoveElectrons(G4int number);
	// setters
	inline void SetnPrincipleNumber(G4int nPrincipleNumberIn)		{nPrincipleNumber=nPrincipleNumberIn;}
	inline void SetlAngularNumber(G4int lAngularNumberIn)			{lAngularNumber=lAngularNumberIn;}
	// getters
	inline G4int		GetnPrincipleNumnber() 								{return nPrincipleNumber;}
	inline G4int 		GetlAngularNumber()									{return lAngularNumber;}
	inline G4double 	GetjSpinOrbitCoupling()								{return jSpinOrbitCoupling;}
	inline G4int 		GetMaxOccupancy()									{return maxOccupancy;}
	inline G4int 		GetCurrentOccupancy()								{return currentOccupancy;}
	inline G4double 	GetLevelEnergy()									{return levelEnergy;}
	inline void 		SetExcitedLifetime(G4double excitedLifetimeIn)		{excitedLifetime=excitedLifetimeIn;}
	inline void 		SetTimeOfExcitement(G4double timeOfExcitementIn)	{timeOfExcitement=timeOfExcitementIn;}
	inline G4double 	GetExcitedLifetime()								{return excitedLifetime;}
	inline G4double 	GetTimeOfExcitement()								{return timeOfExcitement;}

private:
	G4int		nPrincipleNumber;
	G4int		lAngularNumber;
	G4double 	jSpinOrbitCoupling;
	G4double	levelEnergy;
	G4int 		maxOccupancy;
	G4int		currentOccupancy;
	G4double 	excitedLifetime;
	G4double 	timeOfExcitement;

};

#endif
