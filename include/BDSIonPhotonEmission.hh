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
#ifndef BDSIONPHOTONEMISSION_H
#define BDSIONPHOTONEMISSION_H

#include "globals.hh"
#include "G4VDiscreteProcess.hh"

/**
 * @brief Spontaneous emission of photon by relativistic ion.
 *
 * @author Siobhan Alden, Laurie Nevay
 */

class BDSIonPhotonEmission: public G4VDiscreteProcess
{
public:
  explicit BDSIonPhotonEmission(const G4String& processName = "ionPhotonEmission");
  virtual ~BDSIonPhotonEmission();

  virtual G4VParticleChange* PostStepDoIt(const G4Track& track,
			   const G4Step&  step) override;

  virtual G4double GetMeanFreePath(const G4Track& aTrack,
				   G4double previousStepSize,
				   G4ForceCondition* condition) override;
    
};

#endif
