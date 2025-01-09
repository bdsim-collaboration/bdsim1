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
#ifndef BDSLASERCUMULATIVECOMPTON_H
#define BDSLASERCUMULATIVECOMPTON_H
#include "globals.hh"
#include "BDSLaser.hh"
#include "G4VDiscreteProcess.hh"
#include "G4AffineTransform.hh"

class BDSAuxiliaryNavigator;
class BDSComptonScatteringEngine;

/**
 * @brief Laser excitation of charged particle.
 *
 * @author Siobhan Alden
 */

extern G4bool FireLaserCompton;


class BDSLaserCumulativeCompton: public G4VDiscreteProcess
{
public:
	explicit BDSLaserCumulativeCompton(const G4String& processName = "laserCumulativeCompton");
	virtual ~BDSLaserCumulativeCompton();
	virtual G4double GetMeanFreePath(const G4Track& track,
									 G4double previousStepSize,
									 G4ForceCondition* forceCondition);
	G4VParticleChange* PostStepDoIt(const G4Track& track ,
									const G4Step&  step);

private:
	BDSAuxiliaryNavigator* auxNavigator;
	BDSComptonScatteringEngine* comptonEngine;
};

#endif
