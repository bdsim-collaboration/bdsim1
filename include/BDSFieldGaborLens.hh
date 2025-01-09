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
#ifndef BDSFIELDGABORLENS_H
#define BDSFIELDGABORLENS_H

#include "BDSFieldEM.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <utility>

class BDSMagnetStrength;

/**
 * @brief Gabor lens electromagnetic field.
 *
 * @author William Shields
 */

class BDSFieldGaborLens: public BDSFieldEM
{
public:
  BDSFieldGaborLens(BDSMagnetStrength const* strength);

  virtual ~BDSFieldGaborLens(){;}

  /// Accessor to get B and E field.
  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
							   const G4double       t) const;

private:
  /// Private constructor to force use of provided one.
  BDSFieldGaborLens();

  /// speed of light.
  static const G4double c;

  /// proton mass.
  static const G4double pmass;

  /// plasma e-field strength
  G4double plasmaEfield;

  /// solenoid-equivalent field strength
  G4double bfieldMag;

  /// gabor lens kg strength
  G4double kg;

  /// anode radius
  G4double anodeRadius;


};

#endif
