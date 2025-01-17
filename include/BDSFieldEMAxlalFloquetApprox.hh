//
// Created by Boogert Stewart on 01/12/2024.
//

#ifndef BDSIM_BDSFIELDEMAXLALFLOQUETAPPROX_HH
#define BDSIM_BDSFIELDEMAXLALFLOQUETAPPROX_HH

#include "BDSFieldEMAcceleration.hh"

class BDSMagnetStrength;

class BDSFieldEMAxialFloquetApprox : public BDSFieldEMAcceleration {
public:
  BDSFieldEMAxialFloquetApprox() = delete;

  explicit BDSFieldEMAxialFloquetApprox(BDSMagnetStrength const *strength);

  BDSFieldEMAxialFloquetApprox(G4double cavity_cell_length,
                               G4double cavity_cell_voltage,
                               G4int cavity_cell_number,
                               G4double cavity_cell_phase_advance,
                               G4double cavity_phase,
                               G4double synchronousT);

  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
                                                           const G4double       t) const;

  virtual G4bool TimeVarying() const {return true;}

private:
  G4double cavity_cell_voltage;
  G4double cavity_phase;
  G4int cavity_cell_number;
  G4double cavity_cell_phase_advance;
  G4double cavity_cell_length;
  G4double synchronousT;
};

#endif //BDSIM_BDSFIELDEMAXLALFLOQUETAPPROX_HH