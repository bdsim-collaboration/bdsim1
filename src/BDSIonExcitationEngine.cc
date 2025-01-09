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
#include "BDSIonExcitationEngine.hh"
#include "BDSLaser.hh"

#include "globals.hh"
#include "G4LorentzVector.hh"
#include "Randomize.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIonExcitationEngine::BDSIonExcitationEngine()
{;}

BDSIonExcitationEngine::~BDSIonExcitationEngine()
{;}

G4double BDSIonExcitationEngine::CrossSection(G4double photonEnergy)
{
  G4double degenerancyG1 = 2.0;
  G4double degenerancyG2 = 2.0;
  G4double wavelengthShift = (CLHEP::h_Planck*CLHEP::c_light)/photonEnergy;
  // G4double cs=(wavelengthShift*wavelengthShift*degenerancyG2)/(CLHEP::twopi*degenerancyG1);
  // hard coded as mm for now but need to consider how to get this for each beam
  return (wavelengthShift*wavelengthShift*degenerancyG2)/(CLHEP::twopi*degenerancyG1);
}

void BDSIonExcitationEngine::PhotonAbsorption(G4ThreeVector boost)
{
  // modify ion momentum for gamma absorption in ion rest frame
  scatteredIonAbsorbed.setPx(incomingGamma.px());
  scatteredIonAbsorbed.setPy(incomingGamma.py());
  scatteredIonAbsorbed.setPz(incomingGamma.pz());
  scatteredIonAbsorbed.setE(incomingGamma.e()+incomingIon.e());
  
  //boost back to lab
  scatteredIonAbsorbed.boost(boost); 
}

void BDSIonExcitationEngine::PhotonEmission(G4ThreeVector boost)
{
  //random angles for direction of emitted gamma in rest frame of ion
  G4double theta = G4UniformRand()*CLHEP::twopi;
  //G4double phi = G4UniformRand()*CLHEP::twopi;
  G4double V = G4UniformRand();
  G4double phi = acos(2.0*V-1.0);
  // emitted gamma energy = transition energy (no variation is exact to transition energy)
  G4double emittedGammaEnergy = 230.16*CLHEP::eV;
  // create vector for emitted gamma in ion rest frame
  G4ThreeVector emittedGammaUnitVector;
  //G4double sinTheta = std::sin(theta);
  //G4double cosTheta = std::cos(theta);
  //G4double sinPhi = std::sin(phi);
  //G4double cosPhi = std::cos(phi);
  //emittedGammaUnitVector.set(sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
  G4double z = cos(phi);
  G4double x = std::sqrt(1-z*z)*cos(theta);
  G4double y = std::sqrt(1-z*z)*sin(theta);
  emittedGammaUnitVector.set(x,y,z);
  //set lorentz vector
  emittedGamma.setPx(emittedGammaUnitVector.x()*emittedGammaEnergy);
  emittedGamma.setPy(emittedGammaUnitVector.y()*emittedGammaEnergy);
  emittedGamma.setPz(emittedGammaUnitVector.z()*emittedGammaEnergy);
  emittedGamma.setE(emittedGammaEnergy);
  
  // modify the ions momentum and energy for correct kinematics
  scatteredIonEmission.setPx(incomingIon.px()-emittedGamma.px());
  scatteredIonEmission.setPy(incomingIon.py()-emittedGamma.py());
  scatteredIonEmission.setPz(incomingIon.pz()-emittedGamma.pz());
  scatteredIonEmission.setE(incomingIon.e()-emittedGammaEnergy);
  
  // boost both lorentz vectors into lab frame
  emittedGamma.boost(boost);
  scatteredIonEmission.boost(boost);

}
