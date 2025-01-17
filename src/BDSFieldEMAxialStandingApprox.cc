//
// Created by Boogert Stewart on 25/11/2024.
//

#include "CLHEP/Units/PhysicalConstants.h"
#include "BDSFieldEMAxialStandingApprox.hh"
#include "BDSMagnetStrength.hh"

BDSFieldEMAxialStandingApprox::BDSFieldEMAxialStandingApprox(BDSMagnetStrength const* strength):
    BDSFieldEMAxialStandingApprox((*strength)["cavity_cell_length"],
                                  (*strength)["cavity_cell_voltage"],
                                  (*strength)["cavity_cell_number"],
                                  (*strength)["cavity_cell_phase_advance"],
                                  (*strength)["cavity_phase"],
                                  (*strength)["synchronousT0"])
{;}

BDSFieldEMAxialStandingApprox::BDSFieldEMAxialStandingApprox(G4double cavity_cell_lengthIn,
                                                             G4double cavity_cell_voltageIn,
                                                             G4int cavity_cell_numberIn,
                                                             G4double cavity_cell_phase_advanceIn,
                                                             G4double cavity_phaseIn,
                                                             G4double synchronousTIn) :
                                                             cavity_cell_length(cavity_cell_lengthIn),
                                                             cavity_cell_voltage(cavity_cell_voltageIn),
                                                             cavity_cell_number(cavity_cell_numberIn),
                                                             cavity_cell_phase_advance(cavity_cell_phase_advanceIn),
                                                             cavity_phase(cavity_phaseIn),
                                                             synchronousT(synchronousTIn) {
  G4int nStep = 200;
  G4double beta = 1;

  cavity_cell_length = cavity_cell_length*CLHEP::m;
  cavity_length = cavity_cell_number*cavity_cell_length;
  frequency = cavity_cell_phase_advance/(cavity_cell_length/CLHEP::c_light)/2./M_PI;

  eFieldAmplitude = 1.0;
  G4double synchronousT_old = synchronousT;
  synchronousT = 0.0;
  transitTime = this->TransitTimeFactor(2*M_PI*frequency,cavity_cell_length,beta,nStep);
  eFieldAmplitude = (cavity_cell_voltage/transitTime)/this->Voltage(cavity_cell_length,nStep);
  synchronousT = synchronousT_old;

  //zeroes = Zeroes(totalFieldLength,nStep);
  //cellL = CellLength(zeroes);

  /*
  std::cout << "frequency       =" << frequency << std::endl;
  std::cout << "cellLength      =" << cavity_cell_length << std::endl;
  std::cout << "transitTime     =" << transitTime << std::endl;
  std::cout << "eFieldAmplitude =" << eFieldAmplitude << std::endl;

  std::cout << "Zeroes = ";
  for (const auto& val : zeroes) {
    std::cout << val << " ";
  }
  std::cout << std::endl;
  std::cout << "cellL=" << cellL << std::endl;
  */
}
G4double BDSFieldEMAxialStandingApprox::GetEz(G4double z, G4double t) const {

  // compute spatial phase
  G4double zphase = z/cavity_cell_length*cavity_cell_phase_advance;

  // compute time phase
  G4double tphase = 2*M_PI*frequency*(t-synchronousT-cavity_phase);

  // compute Ez
  G4double Ez = eFieldAmplitude*cos(zphase)*cos(tphase);

  return Ez;
}

G4double BDSFieldEMAxialStandingApprox::GetEz_tderiv(G4double z, G4double t) const {

  // compute spatial phase
  G4double zphase = z/cavity_cell_length*cavity_cell_phase_advance;

  // compute time phase
  G4double tphase = 2*M_PI*frequency*(t-synchronousT-cavity_phase);

  // compute Ez
  G4double Ez_tderiv = -eFieldAmplitude * (2*M_PI) * frequency * cos(zphase) * sin(tphase);
  return Ez_tderiv;
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMAxialStandingApprox::GetField(const G4ThreeVector& position,
                                                                                const G4double       t) const {
  // Converting from Local Cartesian to Local Cylindrical
  G4double phi = std::atan2(position.y(), position.x());
  G4double r = std::hypot(position.x(), position.y());
  G4double z = position.z();

  // compute Ez
  G4double Ez = GetEz(z,t);
  G4double Ez_tderiv = GetEz_tderiv(z,t);

  // compute derivatives of Ez for Br and Er
  G4double Er = -1/2. * r * (GetEz(z+0.1,t) - GetEz(z-0.1,t))/0.2;
  G4double Bt = 1/pow(CLHEP::c_light,2) * r/2.0 * Ez_tderiv;

  G4double Bz = 0;

  // E transform to cartestian
  G4double Ex = Er * cos(phi);
  G4double Ey = Er * sin(phi);

  // B transform to cartestian
  G4double Bx = - Bt * sin(phi);
  G4double By = + Bt * cos(phi);

  // Local B and E field vectors
  G4ThreeVector LocalB = G4ThreeVector(Bx, By, Bz);
  G4ThreeVector LocalE = G4ThreeVector(Ex, Ey, Ez);

  auto result = std::make_pair(LocalB, LocalE);
  return result;
}