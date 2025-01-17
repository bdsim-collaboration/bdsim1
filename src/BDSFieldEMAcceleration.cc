#include "BDSFieldEMAcceleration.hh"

const G4double BDSFieldEMAcceleration::Z0 = CLHEP::mu0 * CLHEP::c_light;

G4double BDSFieldEMAcceleration::Voltage(G4double length, G4int nSteps)
{
  // G4cout << "BDSFieldEMAcceleration::Voltage" << G4endl;

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

  // G4cout << "BDSFieldEMAcceleration::Voltage> " << simpIntegral << G4endl;

  return simpIntegral;
}

G4double BDSFieldEMAcceleration::TransitTimeFactor(G4double omega, G4double length, G4double beta, G4int nSteps)
{
  // G4cout << "BDSFieldEMAcceleration::TransitTimeFactor>" << G4endl;

  auto voltage = Voltage(length, nSteps);


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

  // G4cout << "BDSFieldEMCircularTM::TransitTimeFactor> " << simpIntegral/voltage << G4endl;

  return simpIntegral/voltage;
}


G4double BDSFieldEMAcceleration::MaxE(G4double length, G4int nSteps)
{
  G4double MaxE = -1e99;

  for(int i=0;i<=nSteps;i++) {
    G4double z = (i*length)/nSteps - length/2.0;
    auto field = GetField(G4ThreeVector(0,0,z),0);
    if (field.second.getZ() > MaxE)
    {
      MaxE = field.second.getZ();
    }
  }
  return MaxE;
}

G4double BDSFieldEMAcceleration::MinE(G4double length, G4int nSteps)
{
  G4double MinE = 1e99;

  for(int i=0;i<=nSteps;i++) {
    G4double z = (i*length)/nSteps - length/2.0;
    auto field = GetField(G4ThreeVector(0,0,z),0);
    if (field.second.getZ() < MinE)
    {
      MinE = field.second.getZ();
    }
  }
  return MinE;
}

/*
std::vector<G4double> BDSFieldEMAcceleration::Cavity_zeroes(G4double length, G4int nSteps)
{
  std::vector<G4double> cavity_zeroes;

  for(int i=0;i<=nSteps-1;i++) {
    G4double z = (i*length)/nSteps - length/2.0;
    G4double z_next = ((i+1)*length)/nSteps - length/2.0;
    auto field_z = GetField(G4ThreeVector(0,0,z),0).second.getZ();
    auto field_z_next = GetField(G4ThreeVector(0,0,z_next),0).second.getZ();
    // G4cout << "field_z: " << field_z << "z: " << z << G4endl;
    if ((field_z < 0 && field_z_next > 0) || (field_z > 0 && field_z_next < 0)) {
      cavity_zeroes.push_back((z + ((field_z + (field_z_next - field_z)/2) - field_z ) * (z_next - z) / (field_z_next - field_z)));
    }
  }
  return cavity_zeroes;
}

G4double BDSFieldEMAcceleration::Cell_length(std::vector<G4double> zeroes)
{
  G4double cell_length, first_central_index, second_central_index;

  first_central_index = std::floor(cavity_zeroes.size()/2);
  second_central_index = first_central_index + 1;
  cell_length = zeroes.at(second_central_index) - zeroes.at(first_central_index);
  return cell_length;
}

G4double BDSFieldEMAcceleration::Cell_number(G4double CellLength)
{
  G4double cell_number;

  return cell_number;
}
*/
