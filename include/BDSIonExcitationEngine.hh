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
#ifndef BDSIONEXCITATIONENGINE_H
#define BDSIONEXCITATIONENGINE_H
#include "globals.hh" // geant4 types / globals
#include "G4LorentzVector.hh"
#include "G4ThreeVector.hh"

class BDSIonExcitationEngine
{
public:
    BDSIonExcitationEngine();
    ~BDSIonExcitationEngine();
    G4double CrossSection(G4double photonEnergy);
    inline void setIncomingGamma(G4LorentzVector incomingGammaIn) {incomingGamma=incomingGammaIn;}
    inline void setIncomingIon(G4LorentzVector incomingIonIn) {incomingIon=incomingIonIn;}
    inline G4LorentzVector GetEmittedGamma()     {return emittedGamma;};
    inline G4LorentzVector GetScatteredIonAbsorption()  {return scatteredIonAbsorbed;};
    inline G4LorentzVector GetScatteredIonEmission()  {return scatteredIonEmission;};
    void PhotonAbsorption(G4ThreeVector boost);
    void PhotonEmission(G4ThreeVector boost);

private:
    G4LorentzVector incomingGamma;
    G4LorentzVector incomingIon;
    G4LorentzVector scatteredIonAbsorbed;
    G4LorentzVector scatteredIonEmission;
    G4LorentzVector emittedGamma;
};

#endif
