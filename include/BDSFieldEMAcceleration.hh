//
// Created by Boogert Stewart on 16/08/2024.
//

#ifndef BDSIM_BDSFIELDEMACCELERATION_HH
#define BDSIM_BDSFIELDEMACCELERATION_HH

#include <vector>
#include "BDSFieldEM.hh"

class BDSFieldEMAcceleration : public BDSFieldEM
{
public:
  G4double Voltage(G4double length, G4int nSteps = 200);
  G4double TransitTimeFactor(G4double length, G4double omega, G4double beta = 1 , G4int nSteps = 200);
  G4double MaxE(G4double length, G4int nSteps = 200);
  G4double MinE(G4double length, G4int nSteps = 200);
  //std::vector<G4double> Cavity_zeroes(G4double length, G4int nSteps = 200);
/*
  G4double Cell_length(std::vector<G4double> Cavity_zeroes);
  G4double Cell_length(G4Double Cell_length);
*/

  static const G4double Z0; ///< Impedance of free space.

};

#endif // BDSIM_BDSFIELDEMACCELERATION_HH
