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
#ifndef BDSFIELDMAGSOLENOIDBLOCK_H
#define BDSFIELDMAGSOLENOIDBLOCK_H

#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic field due to a square annulus of current.
 *
 * Looking in the z,r plane we would see two square blocks with an inner edge at +-r
 * and centered at z = 0. In r, the square extends from innerRadiusIn to innerRadiusIn+radialThicknessIn.
 * There is a +- this in r with the top one being the sign of the strength and the bottom one
 * being the return of the annulus with current in the opposite direction. In phi, it is circular.
 *
 * MUONTBC
 * @author MUONTBC
 */

class BDSFieldMagSolenoidBlock: public BDSFieldMag
{
public:
  BDSFieldMagSolenoidBlock() = delete;
  /// Explicit constructor.
  BDSFieldMagSolenoidBlock(G4double strength,
                           G4bool   strengthIsCurrent,
                           G4double innerRadiusIn,
                           G4double radialThicknessIn,
                           G4double fullLengthZIn,
                           G4double toleranceIn,
                           G4int  nSheetsIn);
  /// Alternative constructor for field factory that uses "field" (i.e. B) strength
  /// from the magnet strength instance, the argument innerRadiusIn, coilRadialThickness,
  /// and length from the strength instance.
  BDSFieldMagSolenoidBlock(const BDSMagnetStrength* st,
                           G4double innerRadiusIn);
  virtual ~BDSFieldMagSolenoidBlock(){;}

  /// Calculate the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
                                 const G4double       t = 0) const;
  
  /// @{ Accessor.
  inline G4double GetB0() const {return B0;}
  inline G4double GetI()  const {return I;}
  /// @}

private:
  
  G4double a;
  G4double radialThickness;
  G4double fullLengthZ;
  G4double B0;
  G4double I;
  G4double spatialLimit;
  G4double mu0OverPiTimesITimesA;
  G4double coilTolerance;
  G4int    nSheetsBlock;
  G4double currentDensity;
};

#endif
