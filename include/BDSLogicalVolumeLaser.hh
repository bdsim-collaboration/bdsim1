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
#ifndef BDSLOGICALVOLUMELASER_H
#define BDSLOGICALVOLUMELASER_H
#include "G4LogicalVolume.hh"
#include "G4Types.hh"

class BDSLaser;
class G4FieldManager;
class G4Material;
class G4UserLimits;
class G4VSensitiveDetector;
class G4VSolid;

/** 
 * @brief Extended logical volume with laser definition.
 *
 * @author Laurie Nevay
 */

class BDSLogicalVolumeLaser: public G4LogicalVolume
{
public:
  BDSLogicalVolumeLaser(G4VSolid*             solid,
			G4Material*           material,
			const G4String&       name,
			const BDSLaser*       laserIn,
			G4FieldManager*       fieldManager      = nullptr,
			G4VSensitiveDetector* sensitiveDetector = nullptr,
			G4UserLimits*         userLimits        = nullptr,
			G4bool                optimise          = true);
  
  virtual ~BDSLogicalVolumeLaser();

  /// Access the laser.
  const BDSLaser* Laser() const {return laser;}

  /// Override flag function as to whether G4LogicalVolume is extended.
  virtual G4bool IsExtended() const override {return true;}
  
private:
  /// @{ Private copy and assignment constructors.
  BDSLogicalVolumeLaser(const BDSLogicalVolumeLaser& other) = delete;
  BDSLogicalVolumeLaser& operator=(const BDSLogicalVolumeLaser& other) = delete;
  /// @}
  
  /// Laser instance - owned by this class.
  BDSLaser* laser;
};

#endif

