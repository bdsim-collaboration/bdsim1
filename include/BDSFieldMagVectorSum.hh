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
#ifndef BDSFIELDMAGVECTORSUM_H
#define BDSFIELDMAGVECTORSUM_H

#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <vector>

/**
 * @brief A vector sum of multiple displaced magnetic fields.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagVectorSum: public BDSFieldMag
{
public:
  BDSFieldMagVectorSum() = delete;
  explicit BDSFieldMagVectorSum(const std::vector<BDSFieldMag*>& fieldsIn,
                                const std::vector<G4ThreeVector>& fieldOffsetsIn);
  virtual ~BDSFieldMagVectorSum();

  /// Calculate the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
                                 const G4double       t = 0) const;

private:
  std::vector<BDSFieldMag*> fields;
  std::vector<G4ThreeVector> fieldOffsets;
};

#endif
