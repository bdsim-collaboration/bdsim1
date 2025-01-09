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
#ifndef BDSTRACKINGACTION_H
#define BDSTRACKINGACTION_H

#include "BDSTrajectoryOptions.hh"

#include "G4Types.hh"
#include "globals.hh" // geant4 types / globals
#include "G4UserTrackingAction.hh"

class BDSEventAction;
class G4Track;

/**
 * @brief Action to decide whether or not to store trajectory information.
 */

class BDSTrackingAction: public G4UserTrackingAction
{
public:
  BDSTrackingAction(G4bool batchMode,
		    G4bool storeTrajectoryIn,
		    const BDS::TrajectoryOptions& storeTrajectoryOptionsIn,
		    BDSEventAction* eventActionIn,
		    G4int  verboseSteppingEventStartIn,
		    G4int  verboseSteppingEventStopIn,
		    G4bool verboseSteppingPrimaryOnlyIn,
		    G4int  verboseSteppingLevelIn);
  
  virtual ~BDSTrackingAction(){;}

  /// Used to decide whether or not to store trajectories.
  virtual void PreUserTrackingAction(const G4Track* track);

  /// Detect whether track is a primary and if so whether it ended in a collimator.
  virtual void PostUserTrackingAction(const G4Track* track);

private:
  /// No default constructor required.
  BDSTrackingAction() = delete;
  
  /// Whether we're using the visualiser - in which case always store
  /// trajectories for visualisation.
  const G4bool interactive;

  const G4bool storeTrajectory; /// Cache of flag from global constants to control storing all trajectories.
  const BDS::TrajectoryOptions storeTrajectoryOptions; /// Cache of trajectory options.

  /// Cache of event action to communicate whether a primary stopped in a collimator or not.
  BDSEventAction* eventAction;

  G4int  verboseSteppingEventStart;
  G4int  verboseSteppingEventStop;
  G4bool verboseSteppingPrimaryOnly;
  G4int  verboseSteppingLevel;
};

#endif
