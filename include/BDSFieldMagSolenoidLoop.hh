/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSFIELDMAGSOLENOIDLOOP_H
#define BDSFIELDMAGSOLENOIDLOOP_H

#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic field due to a cylinder of current.
 *
 * This follows the parameterisation and uses the algorithm for the generalised complete
 * elliptical integral as described in:
 *
 * Cylindrical Magnets and ideal Solenoids, N. Derby and S. Olbert, American Journal of
 * Physics **78**, 229 (2010); https://doi.org/10.1119/1.3256157 and also at
 * https://arxiv.org/abs/0909.3880.
 *
 * The field is calculated in cylindrical coordinates. A complete description is in the manual.
 *
 * The strength is effectively stored in the member variable mu0OverPiTimesITimesA, but I and B0
 * are accessible for inspection.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSolenoidLoop: public BDSFieldMag
{
public:
  BDSFieldMagSolenoidLoop() = delete;
  /// Explicit constructor. Strength can be either I or B0 as specified by 'strengthIsCurrent'.
  BDSFieldMagSolenoidLoop(G4double strength,
                          G4bool   strengthIsCurrent,
                          G4double radiusIn);
  /// Alternative constructor for field factory that uses "field" (i.e. B) strength
  /// from the magnet strength instance.
  BDSFieldMagSolenoidLoop(BDSMagnetStrength const* strength,
                          G4double radiusIn);

  virtual ~BDSFieldMagSolenoidLoop(){;}

  /// Calculate the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
                                 const G4double       t = 0) const;
  
  /// @{ Accessor.
  inline G4double GetB0() const {return B0;}
  inline G4double GetI()  const {return I;}
  /// @}

private:
  /// Approximation for rho=0 Bz field. Brho=0 by definition. zp and zm are z+halfLength
  /// and z-halfLength. Returns Bz.
  G4double OnAxisBz(G4double zp, G4double zm) const;
  
  G4double a;
  G4double B0;
  G4double I;
  G4double spatialLimit;
  G4double mu0OverPiTimesITimesA;
};

#endif
