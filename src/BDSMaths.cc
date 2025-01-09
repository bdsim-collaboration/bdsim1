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
#include "BDSMaths.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

G4double BDS::CEL(G4double kc,
                  G4double p,
                  G4double c,
                  G4double s,
                  G4int nIterationLimit)
{
  if (!BDS::IsFinite(kc))
    {return NAN;}
  
  G4double errtol = 0.000001;
  G4double k = std::abs(kc);
  G4double pp = p;
  G4double cc = c;
  G4double ss = s;
  G4double em = 1.0;
  if (p > 0)
    {
      pp = std::sqrt(p);
      ss = s/pp;
    }
  else
    {
      G4double f = kc * kc;
      G4double q = 1.0 - f;
      G4double g = 1.0 - pp;
      f = f - pp;
      q = q * (ss - c * pp);
      pp = std::sqrt(f / g);
      cc = (c - ss) / g;
      ss = -q / (g * g * pp) + cc * pp;
    }
  
  G4double f = cc;
  cc = cc + ss/pp;
  G4double g = k/pp;
  ss = 2*(ss + f*g);
  pp = g + pp;
  g = em;
  em = k + em;
  G4double kk = k;
  G4int nLoop = 0;
  while ( (std::abs(g-k) > g*errtol) && nLoop < nIterationLimit)
    {
      k = 2*std::sqrt(kk);
      kk = k*em;
      f = cc;
      cc = cc + ss/pp;
      g = kk / pp;
      ss = 2*(ss + f*g);
      pp = g + pp;
      g = em;
      em = k + em;
      nLoop++;
    }
  G4double result = CLHEP::halfpi*(ss + cc*em)/( em*(em + pp) );
  return result;
}
