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
#include "BDSPhotoDetachmentEngine.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSPhotoDetachmentEngine::BDSPhotoDetachmentEngine()
{;}

BDSPhotoDetachmentEngine::~BDSPhotoDetachmentEngine()
{;}

G4double BDSPhotoDetachmentEngine::CrossSection(G4double photonEnergyIn)
{
  G4double photonEnergyJoules = photonEnergyIn*(1/CLHEP::joule);
  G4double lambdaShift = ((CLHEP::h_Planck*(1/CLHEP::joule)*(1/CLHEP::second)*CLHEP::c_light*CLHEP::ms)/(photonEnergyJoules));
  G4double lambda_nm = lambdaShift*1.0e9;
  G4double lambda_nm2 = std::pow(lambda_nm, 2);
  G4double lambda_nm3 = std::pow(lambda_nm, 3);
  G4double lambda_nm4 = std::pow(lambda_nm, 4);
  
  G4double crossSectionFit = fitCoefficient1 * lambda_nm4
    - fitCoefficient2 * lambda_nm3
    + fitCoefficient3 * lambda_nm2
    + fitCoefficient4 * lambda_nm
    - fitCoefficient5;
  return crossSectionFit*1.0e-17*CLHEP::centimeter*CLHEP::centimeter;
}

