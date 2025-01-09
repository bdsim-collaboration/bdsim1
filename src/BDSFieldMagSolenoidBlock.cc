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
#include "BDSFieldMagSolenoidBlock.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMaths.hh"
#include "BDSUtilities.hh"
#include "BDSFieldMagSolenoidSheet.hh"
#include "BDSFieldMagVectorSum.hh"


#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <memory>

BDSFieldMagSolenoidBlock::BDSFieldMagSolenoidBlock(BDSMagnetStrength const* strength,
                                                   G4double innerRadiusIn):
  BDSFieldMagSolenoidBlock((*strength)["field"], false, innerRadiusIn, (*strength)["coilRadialThickness"], (*strength)["length"], 0, 1)
{;}


BDSFieldMagSolenoidBlock::BDSFieldMagSolenoidBlock(G4double strength,
                                                   G4bool   strengthIsCurrent,
                                                   G4double innerRadiusIn,
                                                   G4double radialThicknessIn,
                                                   G4double fullLengthZIn,
                                                   G4double toleranceIn,
                                                   G4int    nSheetsIn):
  a(innerRadiusIn),
  radialThickness(radialThicknessIn),
  fullLengthZ(fullLengthZIn),
  B0(0),
  I(0),
  spatialLimit(1e-6*innerRadiusIn),
  mu0OverPiTimesITimesA(1),
  coilTolerance(toleranceIn),
  nSheetsBlock(nSheetsIn)
{
  // 'strength' can be current or B field
  finiteStrength = BDS::IsFinite(std::abs(strength));
  
  // apply relationship B0 = mu_0 I / 2 a for on-axis rho=0,z=0
  if (strengthIsCurrent)
    {
      I = strength;
    }
  else
    {// strength is B0 -> calculate current
      B0 = strength;
      I = B0 * 2 * a / CLHEP::mu0;
    }
  currentDensity = I*radialThickness*fullLengthZ/nSheetsBlock; // Current density in A/m^2 (TODO:Check)

}

G4ThreeVector BDSFieldMagSolenoidBlock::GetField(const G4ThreeVector& position,
                                                 const G4double       /*t*/) const
{
  //G4double z = position.z();
  //G4double rho = position.perp();
  G4double phi = position.phi(); // angle about z axis
  std::unique_ptr<BDSFieldMag> field;
  G4ThreeVector blockField;
  double dr = radialThickness/nSheetsBlock;
  for (int sheet = 0; sheet < nSheetsBlock; sheet++)
          { 
            field = std::make_unique<BDSFieldMagSolenoidSheet>(currentDensity,
                                                               true,
                                                               a+(sheet*dr) + dr/2,
                                                               fullLengthZ,
                                                               coilTolerance);
            blockField += field->GetField(position);

          }
  return blockField;
}
