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
#include "BDSFieldMagSolenoidLoop.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMaths.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSFieldMagSolenoidLoop::BDSFieldMagSolenoidLoop(BDSMagnetStrength const* strength,
                                                 G4double radiusIn):
  BDSFieldMagSolenoidLoop((*strength)["field"], false, radiusIn)
{;}

BDSFieldMagSolenoidLoop::BDSFieldMagSolenoidLoop(G4double strength,
                                                 G4bool   strengthIsCurrent,
                                                 G4double radiusIn):
  a(radiusIn),
  B0(0),
  I(0),
  spatialLimit(1e-6*radiusIn),
  mu0OverPiTimesITimesA(1)
{
  // 'strength' can be current or B field
  finiteStrength = BDS::IsFinite(std::abs(strength));
  
  // apply relationship B0 = mu_0 I / 2 a for on-axis rho=0,z=0
  if (strengthIsCurrent)
    {
      I = strength;
      B0 = CLHEP::mu0 * strength / (2*a);
    }
  else
    {// strength is B0 -> calculate current
      B0 = strength;
      I = B0 * 2 * a / CLHEP::mu0;
    }
  
  mu0OverPiTimesITimesA = CLHEP::mu0 * I * a / CLHEP::pi;
}

G4ThreeVector BDSFieldMagSolenoidLoop::GetField(const G4ThreeVector& position,
                                                 const G4double       /*t*/) const
{
  G4double z = position.z();
  G4double rho = position.perp();
  G4double phi = position.phi(); // angle about z axis
  
  // check if close to current loop - function not well-behaved at exactly
  // the rho of the current loop
  if (std::abs(z) < spatialLimit && (std::abs(rho - a) < spatialLimit))
    {return G4ThreeVector();}
  
  G4double zSq = z*z;
      
  G4double aPlusRho    = a + rho;
  G4double aPlusRhoSq  = aPlusRho * aPlusRho;
  G4double aMinusRho   = a - rho;
  G4double aMinusRhoSq = aMinusRho*aMinusRho;
  
  G4double gamma = aMinusRho / aPlusRho;
      
  G4double zSqPlusAPlusRhoSq  = zSq + aPlusRhoSq;
  G4double zSqPlusAMinusRhoSq = zSq + aMinusRhoSq;
      
  G4double k1Sq = zSqPlusAMinusRhoSq / zSqPlusAPlusRhoSq;
  G4double k1   = std::sqrt(k1Sq);
      
  G4double zSqPlusAPlusRhoSqFactor3o2 = std::pow(zSqPlusAPlusRhoSq, 1.5);
      
  G4double commonFactor = mu0OverPiTimesITimesA / zSqPlusAPlusRhoSqFactor3o2;
  
  G4double Brho = commonFactor * z * BDS::CEL(k1, k1Sq, -1, 1);
  G4double Bz   = commonFactor * aPlusRho * BDS::CEL(k1, k1Sq, 1, gamma);
  
  // technically possible for integral to return nan, so protect against it and default to B0 along z
  if (std::isnan(Brho))
    {Brho = 0;}
  if (std::isnan(Bz))
    {Bz = B0;}
  
  // we have to be consistent with the phi we calculated at the beginning,
  // so unit rho is in the x direction.
  G4ThreeVector result = G4ThreeVector(Brho,0,Bz);
  result = result.rotateZ(phi);
  return result;
}
