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
#include "BDSDebug.hh"
#include "BDSFieldMagSolenoidSheet.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMaths.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>

BDSFieldMagSolenoidSheet::BDSFieldMagSolenoidSheet(BDSMagnetStrength const* strength,
                                                   G4double radiusIn, G4double toleranceIn):
  BDSFieldMagSolenoidSheet((*strength)["field"], false, radiusIn, (*strength)["length"], toleranceIn)
{;}

BDSFieldMagSolenoidSheet::BDSFieldMagSolenoidSheet(G4double strength,
                                                   G4bool   strengthIsCurrent,
                                                   G4double sheetRadius,
                                                   G4double fullLength,
                                                   G4double toleranceIn
                                                   ):
  a(sheetRadius),
  halfLength(0.5*fullLength),
  B0(0.0),
  I(0.0),
  spatialLimit(std::min(1e-5*sheetRadius, 1e-5*fullLength)),
  normalisation(1.0) ,
  coilTolerance(toleranceIn)
{
  finiteStrength = BDS::IsFinite(std::abs(strength));
  // apply relationship B0 = mu_0 I / 2 a for on-axis rho=0,z=0
  if (strengthIsCurrent)
    {
      I = strength;
      B0 = CLHEP::mu0 * strength / (CLHEP::pi*2* halfLength);
    }
  else
    {// strength is B0 -> calculate current
      B0 = strength;
      I = B0 *(CLHEP::pi*2* halfLength) / CLHEP::mu0; 
    }
  
  // The field inside the current cylinder is actually slightly parabolic in rho.
  // The equation for the field takes B0 as the peak magnetic field at the current
  // cylinder sheet. So we evaluate it here then normalise. ~<1% adjustment in magnitude.
  //G4double testBz = OnAxisBz(halfLength, -halfLength);
  //normalisation = B0 / testBz;
}

G4ThreeVector BDSFieldMagSolenoidSheet::GetField(const G4ThreeVector& position,
                                                 const G4double       /*t*/) const
{
  G4double z = position.z();
  G4double rho = position.perp();
  G4double phi = position.phi(); // angle about z axis
   
  // check if close to current source - function not well-behaved at exactly the rho of
  // the current source or at the boundary of +- halfLength
  if (std::abs(rho - a) < spatialLimit && (std::abs(z) < halfLength+2*spatialLimit))
    {return G4ThreeVector();} // close to radius and inside +- z
  
  G4double zp = z + halfLength;
  G4double zm = z - halfLength;
  
  if (std::abs(OnAxisBz(zp, zm)) < coilTolerance)
    { return G4ThreeVector();}
  G4double Brho = 0;
  G4double Bz   = 0;
  
  // approximation for on-axis
  if (std::abs(rho) < spatialLimit)
    {Bz = OnAxisBz(zp, zm);}
  else if (std::abs(OnAxisBz(zp, zm)) < coilTolerance){
      Bz = 0;
    }
    else
    {

      G4double zpSq = zp*zp;
      G4double zmSq = zm*zm;
      
      G4double rhoPlusA = rho + a;
      G4double rhoPlusASq = rhoPlusA * rhoPlusA;
      G4double aMinusRho = a - rho;
      G4double aMinusRhoSq = aMinusRho*aMinusRho;
      
      G4double denominatorP = std::sqrt(zpSq + rhoPlusASq);
      G4double denominatorM = std::sqrt(zmSq + rhoPlusASq);
      
      G4double alphap = a / denominatorP;
      G4double alpham = a / denominatorM;
      
      G4double betap = zp / denominatorP;
      G4double betam = zm / denominatorM;
      
      G4double gamma = (a - rho) / (rhoPlusA);
      G4double gammaSq = gamma * gamma;

      G4double kp = std::sqrt(zpSq + aMinusRhoSq) / denominatorP;
      G4double km = std::sqrt(zmSq + aMinusRhoSq) / denominatorM;
      
      Brho = B0 * (alphap * BDS::CEL(kp, 1, 1, -1) - alpham * BDS::CEL(km, 1, 1, -1));
      Bz = ((B0 * a) / (rhoPlusA)) * (betap * BDS::CEL(kp, gammaSq, 1, gamma) - betam * BDS::CEL(km, gammaSq, 1, gamma));
      // technically possible for integral to return nan, so protect against it and default to B0 along z
      if (std::isnan(Brho))
        {Brho = 0;}
      if (std::isnan(Bz))
        {Bz = B0;}
    }
  // we have to be consistent with the phi we calculated at the beginning,
  // so unit rho is in the x direction.
  G4ThreeVector result = G4ThreeVector(Brho,0,Bz)* normalisation;
  result = result.rotateZ(phi);
  return result;
}

G4double BDSFieldMagSolenoidSheet::OnAxisBz(G4double zp,
                                            G4double zm) const
{
  G4double f1 = zp / std::sqrt( zp*zp + a*a );
  G4double f2 = zm / std::sqrt( zm*zm + a*a );
  G4double Bz = 0.5*B0 *CLHEP::pi* (f1 - f2);
  return Bz;
}
