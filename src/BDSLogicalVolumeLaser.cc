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
#include "BDSLaser.hh"
#include "BDSLogicalVolumeLaser.hh"

BDSLogicalVolumeLaser::BDSLogicalVolumeLaser(G4VSolid*             solid,
					     G4Material*           material,
					     const G4String&       name,
					     const BDSLaser*       laserIn,
					     G4FieldManager*       fieldManager,
					     G4VSensitiveDetector* sensitiveDetector,
					     G4UserLimits*         userLimits,
					     G4bool                optimise):
  G4LogicalVolume(solid, material, name, fieldManager, sensitiveDetector,
		  userLimits, optimise),
  laser(new BDSLaser(*laserIn))
{;}

BDSLogicalVolumeLaser::~BDSLogicalVolumeLaser()
{
  delete laser;
}
