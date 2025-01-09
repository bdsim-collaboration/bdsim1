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
#ifndef BDSFIELDEMVECTORSUM_H
#define BDSFIELDEMVECTORSUM_H

#include "BDSFieldEM.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>
#include <vector>

/**
 * @brief A vector sum of multiple displaced EM fields.
 * 
 * @author Laurie Nevay
 */

class BDSFieldEMVectorSum: public BDSFieldEM
{
public:
  BDSFieldEMVectorSum() ;// = delete; RK - check this
  explicit BDSFieldEMVectorSum(const std::vector<BDSFieldEM*>& fieldsIn,
                               const std::vector<G4ThreeVector>& fieldOffsetsIn,
                               const std::vector<double>& timeOffsetsIn,
                               const std::vector<double>& zLengthsIn);
  virtual ~BDSFieldEMVectorSum();

  virtual void PushBackField(const G4ThreeVector& positionOffset,
                           double timeOffset,
                           double zLength,
                           BDSFieldEM* emfield);
  /// Calculate the field value.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
							  const G4double       t = 0) const;
    
private:
  std::vector<BDSFieldEM*> fields;
  std::vector<G4ThreeVector> fieldOffsets;
  std::vector<G4double> timeOffsets;
  std::vector<double> zLengthsOver2; // store half lengths to save a flop at lookup time

};

#endif
