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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagVectorSum.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <vector>

BDSFieldMagVectorSum::BDSFieldMagVectorSum(const std::vector<BDSFieldMag*>& fieldsIn,
                                           const std::vector<G4ThreeVector>& fieldOffsetsIn):
  fields(fieldsIn),
  fieldOffsets(fieldOffsetsIn)
{
  if (fields.size() != fieldOffsets.size())
    {throw BDSException(__METHOD_NAME__, "number of fields and number of offsets do not match");}
}

BDSFieldMagVectorSum::~BDSFieldMagVectorSum()
{
  for (auto f : fields)
    {delete f;}
}

G4ThreeVector BDSFieldMagVectorSum::GetField(const G4ThreeVector& position,
                                             const G4double       t) const
{
  G4ThreeVector result;
  for (G4int i = 0; i < (G4int)fields.size(); i++)
    {
      G4ThreeVector p = position - fieldOffsets[i];
      G4ThreeVector v = fields[i]->GetField(p,t);
      result += v;
    }
  return result;
}
