//
// Created by Boogert Stewart on 25/11/2024.
//

#ifndef BDSIM_BDSFIELDEMSTANDINGAXIALAPPROX_HH
#define BDSIM_BDSFIELDEMSTANDINGAXIALAPPROX_HH

#include "BDSFieldEMAcceleration.hh"

class BDSMagnetStrength;

class BDSFieldEMAxialStandingApprox : public BDSFieldEMAcceleration {
public:
  BDSFieldEMAxialStandingApprox() = delete;

  explicit BDSFieldEMAxialStandingApprox(BDSMagnetStrength const *strength);

  BDSFieldEMAxialStandingApprox(G4double voltage,
                                G4double cavityPhase,
                                G4int nCell,
                                G4double cellPhaseAdvance,
                                G4double cellLength,
                                G4double synchronousT);

  G4double GetEz(G4double z, G4double t) const;
  G4double GetEz_tderiv(G4double z, G4double t) const;
  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
                                                           const G4double       t) const;

  virtual G4bool TimeVarying() const {return true;}

private:
  G4double voltage;
  G4double cavityPhase;
  G4int nCell;
  G4double cellPhaseAdvance;
  G4double cellLength;
  G4double synchronousT;

  // calculated from input
  G4double eFieldAmplitude = 0;
  G4double totalFieldLength = 0;
  G4double frequency = 0;
  G4double transitTime = 0;
  std::vector<G4double> zeroes;
  G4double cellL;
};
#endif //BDSIM_BDSFIELDEMSTANDINGAXIALAPPROX_HH
