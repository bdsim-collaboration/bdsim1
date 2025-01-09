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
#ifndef BDSCOMPTONENGINE_H
#define BDSCOMPTONENGINE_H

#include "globals.hh"

#include "G4LorentzVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"

/**
 * @brief Engine to calculate product of compton scattering process.
 *
 * @author Grahame Blair.
 */

class BDSComptonEngine
{ 
public: 
  BDSComptonEngine();
  
  BDSComptonEngine(G4LorentzVector InGam_FourVec, 
		   G4LorentzVector InEl_FourVec );
  
  ~BDSComptonEngine();

  void PerformCompton();
  void SetIncomingPhoton4Vec(G4LorentzVector inGam);
  inline void SetIncomingElectron4Vec(G4LorentzVector inEl) {itsIncomingEl=inEl;}

  G4LorentzVector GetScatteredElectron() const {return itsScatteredEl;}
  G4LorentzVector GetScatteredGamma() const {return itsScatteredGam;}

private:
  G4LorentzVector itsScatteredEl;
  G4LorentzVector itsScatteredGam;
  G4LorentzVector itsIncomingEl;
  G4LorentzVector itsIncomingGam;

  static const G4int ntryMax = 10000000;
};

inline void BDSComptonEngine::SetIncomingPhoton4Vec(G4LorentzVector inGam)
{itsIncomingGam=inGam;
 if(itsIncomingEl.e()<CLHEP::electron_mass_c2)
      {G4Exception("BDSComptonEngine: Invalid Electron Energy", "-1", FatalException, "");}

}
#endif
