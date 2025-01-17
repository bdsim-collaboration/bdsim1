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
#include "BDSCavityInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldEMCircularTM.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>

const G4double BDSFieldEMCircularTM::Z0 = CLHEP::mu0 * CLHEP::c_light;

const G4double BDSFieldEMCircularTM::JNsZeros[10][10] = {{2.40482556,  5.52007811,  8.65372791, 11.79153444, 14.93091771, 18.07106397, 21.21163663, 24.35247153, 27.49347913, 30.63460647},
                                                         {3.83170597,  7.01558667, 10.17346814, 13.32369194, 16.47063005, 19.61585851, 22.76008438, 25.90367209, 29.04682853, 32.18967991},
                                                         {5.1356223 ,  8.41724414, 11.61984117, 14.79595178, 17.95981949, 21.11699705, 24.27011231, 27.42057355, 30.5692045 , 33.71651951},
                                                         {6.3801619 ,  9.76102313, 13.01520072, 16.22346616, 19.40941523, 22.58272959, 25.7481667 , 28.90835078, 32.06485241, 35.21867074},
                                                         {7.58834243, 11.06470949, 14.37253667, 17.61596605, 20.82693296, 24.01901952, 27.19908777, 30.37100767, 33.53713771, 36.69900113},
                                                         {8.77148382, 12.3386042 , 15.70017408, 18.98013388, 22.2177999,  25.43034115, 28.62661831, 31.81171672, 34.98878129, 38.15986856},
                                                         {9.93610952, 13.58929017, 17.00381967, 20.32078921, 23.58608444, 26.82015198, 30.03372239, 33.23304176, 36.42201967, 39.60323942},
                                                         {11.08637002, 14.82126873, 18.28758283, 21.64154102, 24.93492789,28.19118846, 31.42279419, 34.63708935, 37.83871738, 41.03077369},
                                                         {12.22509226, 16.03777419, 19.55453643, 22.94517313, 26.26681464,29.54565967, 32.79580004, 36.02561506, 39.240448  , 42.44388774},
                                                         {13.35430048, 17.24122038, 20.80704779, 24.23388526, 27.58374896,30.88537897, 34.15437792, 37.40009998, 40.62855372, 43.84380142}};

G4double BesselJ(G4int n, G4double x) {
  if(n == 0) {
    return TMath::BesselJ0(x);
  }
  else if(n==1) {
    return TMath::BesselJ1(x);
  }
  else if (n>1) {
    return 2*(n-1)/x*BesselJ(n-1,x) - BesselJ(n-2,x);
  }
  else {
    return 2*(n+1)/x*BesselJ(n+1,x) - BesselJ(n+2,x);
  }
}

G4double BesselJDeriv(G4int n, G4double x) {
  return -(BesselJ(n+1,x) - BesselJ(n-1,x))/2;
}

BDSFieldEMCircularTM::BDSFieldEMCircularTM(BDSMagnetStrength const* strength):
  BDSFieldEMCircularTM((*strength)["cavity_efield"],
                       (*strength)["cavity_radius"],
                       (*strength)["cavity_length"],
                       (*strength)["cavity_m"],
                       (*strength)["cavity_n"],
                       (*strength)["cavity_p"],
                       (*strength)["cavity_tphase"],
                       (*strength)["cavity_zphase"],
                       (*strength)["cavity_travelling"],
                       (*strength)["frequency"],
                       (*strength)["synchronousT0"])
{;}

BDSFieldEMCircularTM::BDSFieldEMCircularTM(G4double eFieldMaxIn,
                                           G4double radiusIn,
                                           G4double lengthIn,
                                           G4int    mIn,
                                           G4int    nIn,
                                           G4int    pIn,
                                           G4double tphaseIn,
                                           G4double zphaseIn,
                                           G4bool   travellingIn,
                                           G4double frequencyIn,
                                           G4double synchronousTIn):
                                                                      eFieldMax(eFieldMaxIn),
                                                                      radius(radiusIn),
                                                                      length(lengthIn),
                                                                      m(mIn),
                                                                      n(nIn),
                                                                      p(pIn),
                                                                      tphase(tphaseIn),
                                                                      zphase(zphaseIn),
                                                                      travelling(travellingIn),
                                                                      frequency(frequencyIn),
                                                                      synchronousT(synchronousTIn)
{
  if (! travelling) {
    G4cout << "BDSFieldEMCircularTM::BDSFieldEMCircularTM> set frequency=" << frequency << G4endl;
    kmn = JNsZeros[m][n - 1]/ radius;
    kz = p * CLHEP::pi / length;
    omega = sqrt(pow(CLHEP::c_light,2) * (pow(kmn,2) + pow(kz,2)));
    if(frequency ==0) { // if there is a set frequency use that
      frequency = omega/CLHEP::twopi;
      G4cout << "BDSFieldEMCircularTM::BDSFieldEMCircularTM> frequency is zero, calculating frequency=" << frequency << G4endl;
    }
  }
  else {
    omega = CLHEP::twopi * frequency;
  }

  voltage = Voltage();
  ttFactor = TransitTimeFactor();
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMCircularTM::GetField(const G4ThreeVector& position,
                                                                       const G4double       t) const
{
  // Converting from Local Cartesian to Local Cylindrical
  G4double phi = std::atan2(position.y(),position.x());
  G4double r   = std::hypot(position.x(),position.y());
  G4double z   = position.z();

  // divergences @ r=0, so avoid a little
  if(r == 0) {
    r = 1e-15;
  }

  // t phase
  G4double tmodE = cos(omega*(t - synchronousT) + tphase);
  G4double tmodB = sin(omega*(t - synchronousT) + tphase);

  //G4cout << "position,time> " << position << " " << t << " " << tphase << " " << tmodE << " " << tmodB << G4endl;
  //G4cout << t << " " << position << G4endl;
  //G4cout << "time> " << t << " " << synchronousT << " " << tphase << " " << tmodE << " " << tmodB << G4endl;

  //G4double tmod = 1;

  // electric field
  G4double Ez = tmodE * eFieldMax * BesselJ(m, kmn*r) * cos(m*phi) * cos(p*CLHEP::pi*z/length + zphase);
  G4double Er = -tmodE * p*CLHEP::pi/length * radius/JNsZeros[m][n-1] * eFieldMax * BesselJDeriv(m, kmn*r) * cos(m*phi) * sin(p*CLHEP::pi*z/length + zphase);
  G4double Et = -tmodE * p*CLHEP::pi/length*m*pow(radius,2)/pow(JNsZeros[m][n-1],2)/r * eFieldMax * BesselJ(m, kmn*r) * sin(m*phi) * sin(p*CLHEP::pi*z/length + zphase)  ;

  // magnetic field
  G4double Bz = CLHEP::tesla * tmodB * 0;
  G4double Br = CLHEP::tesla * tmodB * omega*m*pow(radius,2)/pow(JNsZeros[m][n-1],2)/r/pow(CLHEP::c_light,2) * eFieldMax * BesselJ(m,kmn*r) * sin(m*phi) * cos(p*CLHEP::pi*z/length + zphase);
  G4double Bt = CLHEP::tesla * tmodB * omega*radius/JNsZeros[m][n-1]/pow(CLHEP::c_light,2) * eFieldMax * BesselJDeriv(m, kmn*r) * cos(m*phi) * cos(p*CLHEP::pi*z/length + zphase);

  // E transform to cartestian
  G4double Ex = Er * cos(phi) - Et * sin(phi);
  G4double Ey = Er * sin(phi) + Et * cos(phi);

  // B transform to cartestian
  G4double Bx = Br * cos(phi) - Bt * sin(phi);
  G4double By = Br * sin(phi) + Bt * cos(phi);

  // Local B and E field vectors
  G4ThreeVector LocalB = G4ThreeVector(Bx, By, Bz);
  G4ThreeVector LocalE = G4ThreeVector(Ex, Ey, Ez);

  // G4cout << "field> " << LocalB << " " << LocalE << G4endl;
  auto result = std::make_pair(LocalB, LocalE);
  return result;
}

G4double BDSFieldEMCircularTM::GetFrequency() {
  return frequency;
}

void BDSFieldEMCircularTM::SetMaxEField(G4double eFieldMaxIn) {
  eFieldMax = eFieldMaxIn;
}


G4double BDSFieldEMCircularTM::Voltage(G4int nSteps)
{
  G4cout << "BDSFieldEMCircularTM::Gain" << G4endl;

  // save variables for reinstating later
  G4double tphase_old = tphase;
  G4double synchronousT_old = synchronousT;

  // set to approriate values
  tphase = 0.0;
  synchronousT = 0.0;

  std::vector<G4double> zV;
  std::vector<G4double> EzV;

  // compute fields
  for(int i=0;i<=nSteps;i++) {
    G4double z = (i*length)/nSteps - length/2.0;
    auto field = GetField(G4ThreeVector(0,0,z),0);
    zV.push_back(z);
    EzV.push_back(field.second.getZ());
  }

  // simpson's rule
  G4double simpIntegral = 0;
  for(int i=0;i<=nSteps-2;i+=2) {
    simpIntegral += (zV[i+2]-zV[i])/6*(EzV[i] + 4*EzV[i+1] + EzV[i+2]);
  }

  G4cout << "Gain> " << simpIntegral << G4endl;

  // reset variables
  tphase = tphase_old;
  synchronousT = synchronousT_old;

  return simpIntegral;
}

G4double BDSFieldEMCircularTM::TransitTimeFactor(G4double beta, G4int nSteps)
{
  G4cout << "BDSFieldEMCircularTM::TransitTimeFactor" << G4endl;

  voltage = Voltage(nSteps);
  // save variables for reinstating later
  G4double tphase_old = tphase;
  G4double synchronousT_old = synchronousT;

  // set to approriate values
  tphase = 0.0;
  synchronousT = 0.0;

  std::vector<G4double> zV;
  std::vector<G4double> EzV;

  // compute fields
  for(int i=0;i<=nSteps;i++) {
    G4double z = (i*length)/nSteps - length/2.0;
    auto field = GetField(G4ThreeVector(0,0,z),0);
    zV.push_back(z);
    EzV.push_back(field.second.getZ()*cos(omega*z/beta/CLHEP::c_light));
  }

  // simpson's rule
  G4double simpIntegral = 0;
  for(int i=0;i<=nSteps-2;i+=2) {
    simpIntegral += (zV[i+2]-zV[i])/6*(EzV[i] + 4*EzV[i+1] + EzV[i+2]);
  }

  G4cout << "TransitTimeFactor> " << simpIntegral/voltage << G4endl;

  // reset variables
  tphase = tphase_old;
  synchronousT = synchronousT_old;

  return simpIntegral/voltage;
}