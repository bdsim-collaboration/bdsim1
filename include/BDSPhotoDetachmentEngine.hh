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
#ifndef BDSPHOTODETACHMENTENGINE_H
#define BDSPHOTODETACHMENTENGINE_H
#include "globals.hh" // geant4 types / globals

class BDSPhotoDetachmentEngine
{
public:
  BDSPhotoDetachmentEngine();
  ~BDSPhotoDetachmentEngine();
  G4double CrossSection(G4double photonEnergy);
  
private:
  // DO NOT CHANGE THESE NUMBERS THEY ARE FROM A FIT OF THE CROSS SECTION DATA
  const G4double fitCoefficient1 = 2.79945909e-12;
  const G4double fitCoefficient2 = 1.24982046e-8;
  const G4double fitCoefficient3 = 1.00207573e-5;
  const G4double fitCoefficient4 = 3.64205996e-3;
  const G4double fitCoefficient5 = 7.30751085e-2;
};

#endif
