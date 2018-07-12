/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef BDSPHYSICSTRANSITIONRADIATION_H
#define BDSPHYSICSTRANSITIONRADIATION_H

#include "BDSSingleUse.hh"

#include "globals.hh"
#include "G4VPhysicsConstructor.hh"

/**
 * @brief Cherenkov physics process constructor.
 *
 * @author Laurie Nevay
 */

class BDSPhysicsTransitionRadiation: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsTransitionRadiation(G4double maxPhotonsPerStepIn,
		      G4double maxBetaChangePerStepIn);
  virtual ~BDSPhysicsTransitionRadiation();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

private:

  BDSPhysicsTransitionRadiation() = delete;
  
  /// @{ Parameter from global constants to control cherenkov physics.
  G4double maxPhotonsPerStep;
  G4double maxBetaChangePerStep;
  /// @}
};

#endif