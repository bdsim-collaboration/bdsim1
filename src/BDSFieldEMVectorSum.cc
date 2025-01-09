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
#include "BDSFieldEM.hh"
#include "BDSFieldEMVectorSum.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>
#include <vector>

BDSFieldEMVectorSum::BDSFieldEMVectorSum()
{}

BDSFieldEMVectorSum::BDSFieldEMVectorSum(const std::vector<BDSFieldEM*>& fieldsIn,
                                         const std::vector<G4ThreeVector>& fieldOffsetsIn,
                                         const std::vector<G4double>& timeOffsetsIn, //type change from earlier version
                                         const std::vector<double>& zLengthsIn):
  fields(fieldsIn),
  fieldOffsets(fieldOffsetsIn),
  timeOffsets(timeOffsetsIn),
  zLengthsOver2(zLengthsIn)
{
  if (fields.size() != fieldOffsets.size())
    {throw BDSException(__METHOD_NAME__, "number of fields and number of position offsets do not match");}
  if (fields.size() != timeOffsets.size())
    {throw BDSException(__METHOD_NAME__, "number of fields and number of time offsets do not match");}
  if (fields.size() != zLengthsOver2.size())
    {throw BDSException(__METHOD_NAME__, "number of fields and number of z lengths do not match");}

  for (auto it = zLengthsOver2.begin(); it < zLengthsOver2.end(); ++it)
    {
      *it /= 2.;
    }
}

BDSFieldEMVectorSum::~BDSFieldEMVectorSum()
{
  for (auto f : fields)
    {delete f;}
}

void BDSFieldEMVectorSum::PushBackField(const G4ThreeVector& positionOffset,
                             double timeOffset,
                             double zLength,
                             BDSFieldEM* emfield) {
  fields.push_back(emfield); // memory is now owned by this
  fieldOffsets.push_back(positionOffset);
  timeOffsets.push_back(timeOffset);
  zLengthsOver2.push_back(zLength/2);
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMVectorSum::GetField(const G4ThreeVector& position,
                                                                     const G4double       t) const
{
  std::pair<G4ThreeVector, G4ThreeVector> result;
  for (G4int i = 0; i < (G4int)fields.size(); i++)
    {
      G4ThreeVector dr = position - fieldOffsets[i];
      if (fabs(dr.z()) > zLengthsOver2[i])
        {
          continue; // out of bounding box
        }
      G4double dt = t - timeOffsets[i];
      auto deltaField = fields[i]->GetField(dr,dt);
      result.first += deltaField.first;
      result.second += deltaField.second;
      
    }
  return result;
}