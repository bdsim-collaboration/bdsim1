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
#include "BDSException.hh"
#include "BDSFieldGaborLens.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>


BDSFieldGaborLens::BDSFieldGaborLens(BDSMagnetStrength const* strength):
plasmaEfield((*strength)["plasmaEfield"]),
anodeRadius((*strength)["equatorradius"])
{}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldGaborLens::GetField(const G4ThreeVector& position,
                                                                    const G4double       /*t*/) const
{
  G4double Ex = plasmaEfield * position.x();
  G4double Ey = plasmaEfield * position.y();

  G4ThreeVector plasmaE = G4ThreeVector(0, 0, 0);

  G4TwoVector transPos = G4TwoVector(position.getX(), position.getY());
  if (transPos.mag() < anodeRadius)       // no plasma field outside of anode radius
    {plasmaE = G4ThreeVector(Ex, Ey, 0);}

  // TODO: Add local B and E plasma confinement fields:
  G4ThreeVector LocalB = G4ThreeVector(0, 0, 0);
  G4ThreeVector LocalE = G4ThreeVector(0, 0, 0);

  LocalE += plasmaE;

  return std::make_pair(LocalB, LocalE);;
}
