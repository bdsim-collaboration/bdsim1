# /*
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
#include "BDSComptonScatteringEngine.hh"

#include "globals.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>

BDSComptonScatteringEngine::BDSComptonScatteringEngine():
  particleMass(0),
  particleRadius(0),
  partID(0)
{;}

BDSComptonScatteringEngine::~BDSComptonScatteringEngine()
{;}

void BDSComptonScatteringEngine::SetParticle(G4int partIDIn)
{
  partID = partIDIn;
  if (std::abs(partID) == 11)
    {particleMass = G4Electron::ElectronDefinition()->GetPDGMass();}
  else if (partID == 2212)
    {particleMass = G4Proton::ProtonDefinition()->GetPDGMass();}
  // else particle mass just left as whatever it was before?
  particleRadius = (CLHEP::e_squared) / (4 * CLHEP::pi * CLHEP::epsilon0 * particleMass);
}

G4double BDSComptonScatteringEngine::CrossSection(G4double photonEnergyIn, G4int partIn)
{
  SetParticle(partIn);
  G4double e1 = photonEnergyIn / particleMass;

  G4double crossSectionThomson = (8.0/3.0)*CLHEP::pi*particleRadius*particleRadius;

  G4double p1 = ((1 + e1) / std::pow(e1,3)) * ( 2*e1 * (1 + e1) / (1 + 2*e1) - std::log(1 + 2*e1));
  G4double p2 = (1. / (2*e1)) * std::log(1 + 2*e1);
  G4double p3 = (1 + 3*e1) / std::pow((1 + 2*e1), 2);
  G4double ratio = 0.75*(p1 + p2 - p3);
  G4double crossSection = crossSectionThomson*ratio;

  return crossSection;
}

void BDSComptonScatteringEngine::PerformCompton(const G4ThreeVector& boost,G4int partIn)
{
  SetParticle(partIn);
  G4double theta = MCMCTheta();
  G4double scatteredGammaEnergy = ScatteredEnergy(theta);
  G4double phi = MCPhi(theta,scatteredGammaEnergy);
  G4ThreeVector scatteredGammaUnitVector(std::sin(theta)*std::cos(phi), std::sin(theta)*std::sin(phi), std::cos(theta));
  if(incomingGamma.x()<=1e9&&incomingGamma.y()<=1e9)
  {
      scatteredGammaUnitVector.rotate(0,CLHEP::pi,0);
      //rotateUz(incomingGamma.vect().unit());
      //theta -=CLHEP::pi;
      //scatteredGammaUnitVector.setX(std::sin(theta)*std::cos(phi));
      //scatteredGammaUnitVector.setY(std::sin(theta)*std::sin(phi));
      //scatteredGammaUnitVector.setZ(std::cos(theta));
  }
  else{
      G4RotationMatrix* rot = CalculateRotation();
      scatteredGammaUnitVector.transform(*rot);
  }
  scatteredGamma.setVect(scatteredGammaUnitVector * scatteredGammaEnergy);
  scatteredGamma.setE(scatteredGammaEnergy);
  G4ThreeVector scatteredElectronVector(incomingGamma.vect()-scatteredGamma.vect());
  scatteredElectron.setE(incomingGamma.e()+incomingElectron.e()-scatteredGammaEnergy);
  scatteredElectron.setVect(scatteredElectronVector);
  scatteredElectron.boost(boost);
  scatteredGamma.boost(boost);
}

G4double BDSComptonScatteringEngine::MCMCTheta()
{
  G4double theta = std::acos(1-2*G4UniformRand());
  G4double KNTheta = KleinNishinaDifferential(theta);
  G4double KNMax = KleinNishinaDifferential(0);
  G4double KNRandom = G4UniformRand()*KNMax;

  return KNTheta > KNRandom ? theta : MCMCTheta();
}

G4double BDSComptonScatteringEngine::KleinNishinaDifferential(G4double theta)
{
  G4double E0 = incomingGamma.e();
  G4double Ep = E0 / (1.0+(E0/particleMass) * (1.0-std::cos(theta)) );
  return 0.5 * particleRadius * particleRadius * (Ep/E0) * (Ep/E0) * ((Ep/E0)+(E0/Ep)-std::sin(theta)*std::sin(theta));
}


G4RotationMatrix* BDSComptonScatteringEngine::CalculateRotation()
{
    G4ThreeVector theoryIncomingPhoton (0,0,1);
    G4ThreeVector crossProduct = theoryIncomingPhoton.cross(incomingGamma.vect().unit());
    G4double dotProduct = theoryIncomingPhoton.dot(incomingGamma.vect().unit());
    G4double multiplier = 1.0/(1.0+dotProduct);
    G4RotationMatrix* identity = new G4RotationMatrix;
    G4ThreeVector vrow1 (1, -1*crossProduct.z(),crossProduct.y());
    G4ThreeVector vrow2 (crossProduct.z(),1,-1.0*crossProduct.x());
    G4ThreeVector vrow3 (-1.0*crossProduct.y(), crossProduct.x(),1);
    G4ThreeVector v2row1 (multiplier*(-crossProduct.y()*crossProduct.y()-crossProduct.z()*crossProduct.z()), multiplier*crossProduct.x()*crossProduct.y(),multiplier*crossProduct.x()*crossProduct.z());
    G4ThreeVector v2row2 (multiplier*crossProduct.x()*crossProduct.y(),multiplier*(-crossProduct.x()*crossProduct.x()-crossProduct.z()*crossProduct.z()),multiplier*crossProduct.y()*crossProduct.z());
    G4ThreeVector v2row3 (multiplier*crossProduct.x()*crossProduct.z(), multiplier*crossProduct.y()*crossProduct.z(),multiplier*(-crossProduct.x()*crossProduct.x()-crossProduct.y()*crossProduct.y()));
    identity->setRows(vrow1+v2row1,vrow2+v2row2,vrow3+v2row3);
    return identity;
}
G4double BDSComptonScatteringEngine::ScatteredEnergy(G4double theta)
{
    return incomingGamma.e()/(1+(incomingGamma.e()/particleMass)*(1-std::cos(theta)));
}

G4double BDSComptonScatteringEngine::MCPhi(G4double theta, G4double scatteredEnergy)
{
    G4double maxPhiVal = PolarizationCrossSectionMaxPhi(theta,scatteredEnergy);
    G4double maxCrossSec1 = PolarizationCrossSectionPhi(theta, maxPhiVal,scatteredEnergy);
    G4double maxCrossSec2 = PolarizationCrossSectionPhi(theta, maxPhiVal+CLHEP::pi,scatteredEnergy);
    G4double maxCrossSec;
    if(maxCrossSec1>=maxCrossSec2)
    {maxCrossSec=maxCrossSec1;}
    else
    {maxCrossSec=maxCrossSec2;}
    G4double phi = CLHEP::twopi*G4UniformRand();
    G4double randCrossSecPhi = PolarizationCrossSectionPhi(theta, phi, scatteredEnergy);
    G4double randCrossSec = maxCrossSec*G4UniformRand();
    return randCrossSecPhi > randCrossSec ? phi : MCPhi(theta, scatteredEnergy);
}

G4double BDSComptonScatteringEngine::PolarizationCrossSectionPhi(G4double theta, G4double phi, G4double Ep)
{
    G4double E0=incomingGamma.e();
    G4double constants = ((particleRadius*particleRadius)/2.0)*(Ep/E0)*(Ep/E0);
    G4double first = 1.0+std::cos(theta)*std::cos(theta)+(E0-Ep)*(1.0-std::cos(theta));
    G4double second = std::sin(theta)*std::sin(theta)*(incomingGammaPolarization.p1()*std::cos(phi)+incomingGammaPolarization.p2()*std::sin(phi));
    G4double third = -1.0*incomingGammaPolarization.p3()*(1.0-std::cos(theta))*
        (Ep*std::sin(theta)*(incomingElectronPolarization.p1()*std::cos(phi)+incomingElectronPolarization.p2()*std::sin(phi))
        +incomingElectronPolarization.p3()*std::cos(theta)*(E0+Ep));
    return constants*(first+second+third);

}

G4double BDSComptonScatteringEngine::PolarizationCrossSectionMaxPhi(G4double theta ,G4double Ep)
{
    //G4double numer = incomingGammaPolarization.p2()*std::sin(theta)*std::sin(theta)+incomingGammaPolarization.p3()*incomingElectronPolarization.p1()*Ep*std::sin(theta)*(1.0-std::cos(theta));

    //G4double denom = -1*incomingGammaPolarization.p3()*incomingElectronPolarization.p2()*Ep*std::sin(theta)*(1.0-std::cos(theta))+incomingGammaPolarization.p1()*std::sin(theta)*std::sin(theta);

    G4double numer = -incomingGammaPolarization.p3()*(1.0-std::cos(theta))*Ep*std::sin(theta)*incomingElectronPolarization.p2() -
                        std::sin(theta)*std::sin(theta)*incomingGammaPolarization.p2();
    G4double denom = std::sin(theta)*std::sin(theta)*incomingGammaPolarization.p1()+
                    incomingGammaPolarization.p3()*(1.0-std::cos(theta))*Ep*std::sin(theta)*incomingElectronPolarization.p1();
    G4double phiVal = std::atan(numer/denom);
    if(phiVal<0)
    {phiVal+=CLHEP::pi;}
    if(numer ==0)
    {return 0;}
    if (denom ==0)
    {return 0;}
    else
    {return phiVal;}
}



