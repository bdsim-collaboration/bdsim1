//
// Created by Boogert Stewart on 01/12/2024.
//

#include "BDSFieldEMAxlalFloquetApprox.hh"
#include "BDSMagnetStrength.hh"

BDSFieldEMAxialFloquetApprox::BDSFieldEMAxialFloquetApprox(BDSMagnetStrength const* strength):
                                BDSFieldEMAxialFloquetApprox((*strength)["cavity_cell_length"],
                                                              (*strength)["cavity_cell_voltage"],
                                                              (*strength)["cavity_cell_number"],
                                                              (*strength)["cavity_cell_phase_advance"],
                                                              (*strength)["cavity_phase"],
                                                              (*strength)["synchronousT0"])
{;}

BDSFieldEMAxialFloquetApprox::BDSFieldEMAxialFloquetApprox(G4double cavity_cell_lengthIn,
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
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMAxialFloquetApprox::GetField(const G4ThreeVector& position,
                                                                               const G4double       t) const {
  // Converting from Local Cartesian to Local Cylindrical
  G4double phi = std::atan2(position.y(), position.x());
  G4double r = std::hypot(position.x(), position.y());
  G4double z = position.z();

  // compute Ez
  G4double Ez = 0;
  G4double Ez_tderiv = 0;

  // compute derivatives of Ez for Br and Er
  G4double Er = 0;
  G4double Bt = 0;

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