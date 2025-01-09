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
#include "BDSAcceleratorModel.hh"
#include "BDSDebug.hh"
#include "BDSEventAction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorMag.hh"
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPrimary.hh"
#include "BDSUserTrackInformation.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

#include <set>

class G4LogicalVolume;

BDSTrackingAction::BDSTrackingAction(G4bool batchMode,
				     G4bool storeTrajectoryIn,
				     const BDS::TrajectoryOptions& storeTrajectoryOptionsIn,
				     BDSEventAction* eventActionIn,
				     G4int  verboseSteppingEventStartIn,
				     G4int  verboseSteppingEventStopIn,
				     G4bool verboseSteppingPrimaryOnlyIn,
				     G4int  verboseSteppingLevelIn):
  interactive(!batchMode),
  storeTrajectory(storeTrajectoryIn),
  storeTrajectoryOptions(storeTrajectoryOptionsIn),
  eventAction(eventActionIn),
  verboseSteppingEventStart(verboseSteppingEventStartIn),
  verboseSteppingEventStop(verboseSteppingEventStopIn),
  verboseSteppingPrimaryOnly(verboseSteppingPrimaryOnlyIn),
  verboseSteppingLevel(verboseSteppingLevelIn)
{;}

void BDSTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  eventAction->IncrementNTracks();
  G4int  eventIndex = eventAction->CurrentEventIndex();
  G4bool verboseSteppingThisEvent = BDS::VerboseThisEvent(eventIndex, verboseSteppingEventStart, verboseSteppingEventStop);
  G4bool primaryParticle  = track->GetParentID() == 0;
  BDSIntegratorMag::currentTrackIsPrimary = primaryParticle;

  if (primaryParticle && verboseSteppingThisEvent)
    {fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseSteppingLevel);}
  else if (!primaryParticle && verboseSteppingThisEvent && !verboseSteppingPrimaryOnly)
    {fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseSteppingLevel);}
  
  if (!primaryParticle)
    {// ie secondary particle
      // only store if we want to or interactive
      if (storeTrajectory || interactive)
	{
	  auto traj = new BDSTrajectory(track,
					interactive,
					storeTrajectoryOptions);
	  fpTrackingManager->SetStoreTrajectory(1);
	  fpTrackingManager->SetTrajectory(traj);
	}
      else // mark as don't store
	{fpTrackingManager->SetStoreTrajectory(0);}
    }
  else
    {// it's a primary particle
      // if it's a primary track then we always store something
      // but only store the actual trajectory points if we explicitly want
      // trajectory points or we're using the visualiser.
      G4bool storePoints = storeTrajectory || interactive;
      auto traj = new BDSTrajectoryPrimary(track,
					   interactive,
					   storeTrajectoryOptions,
					   storePoints);
      eventAction->RegisterPrimaryTrajectory(traj);
      fpTrackingManager->SetStoreTrajectory(1);
      fpTrackingManager->SetTrajectory(traj);
    }

  //if ion add BDSElectronOccupancy


  BDSUserTrackInformation* trackInfo = new BDSUserTrackInformation(track->GetDynamicParticle());
  track->SetUserInformation(trackInfo);

}

void BDSTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  // turn off verbosity always as we selectively turn it on in the start tracking option
  fpTrackingManager->GetSteppingManager()->SetVerboseLevel(0);
  
#ifdef BDSDEBUG
  G4int trackID = track->GetTrackID();
  if (trackID < 100)
    {// limit range of debug output
      auto status = track->GetTrackStatus();
      G4String name;
      switch (status)
	{
	case G4TrackStatus::fAlive:
	  {name = "fAlive"; break;}
	case G4TrackStatus::fStopButAlive:
	  {name = "fStopButAlive"; break;}
	case G4TrackStatus::fKillTrackAndSecondaries:
	  {name = "fKillTrackAndSecondaries"; break;}
	case G4TrackStatus::fStopAndKill:
	  {name = "fStopAndKill"; break;}
	default:
	  {name = "other"; break;}
	}  
      G4cout << "track ID " << trackID << " status " << name << G4endl;
    }
#endif
  if (track->GetParentID() == 0)
    {
      G4LogicalVolume* lv = track->GetVolume()->GetLogicalVolume();
      std::set<G4LogicalVolume*>* collimators = BDSAcceleratorModel::Instance()->VolumeSet("collimators");
      if (collimators->find(lv) != collimators->end())
	{eventAction->SetPrimaryAbsorbedInCollimator(true);}
    }
}
