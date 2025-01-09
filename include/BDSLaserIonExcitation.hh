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
#ifndef BDSLASERIONEXCITATION_H
#define BDSLASERIONEXCITATION_H
#include "globals.hh"
#include "G4VDiscreteProcess.hh"

class BDSAuxiliaryNavigator;

/**
 * @brief Laser excitation of ion.
 *
 * @author Siobhan Alden
 */

class BDSLaserIonExcitation: public G4VDiscreteProcess
{
public:
  explicit BDSLaserIonExcitation(const G4String& processName = "laserIonExcitation");
  virtual ~BDSLaserIonExcitation();
  virtual G4double GetMeanFreePath(const G4Track& track,
				   G4double previousStepSize,
				   G4ForceCondition* forceCondition);
  G4VParticleChange* PostStepDoIt(const G4Track& track ,
				  const G4Step&  step);
private:
  BDSAuxiliaryNavigator* auxNavigator;
  //G4DynamicParticle* hydrogen;
  //G4Hydrogen* ahydrogen;
};

#endif
