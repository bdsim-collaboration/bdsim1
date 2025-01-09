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
#ifndef BDSFIELDINFOEXTRA_H
#define BDSFIELDINFOEXTRA_H

#include "BDSFieldType.hh"
#include "BDSMuonCoolerStructs.hh"

#include <vector>

/**
 * @brief Base class for any extra (complex) information for a particular field.
 * 
 * With BDSFieldInfo we don't want to keep adding even more parameters that all
 * (instances of) field recipes will know about, so we add 1x pointer to extra
 * info of this class type. We can then create specific derived classes for very
 * particular complex field setups.
 * 
 * Any derived classes are preferred to be classes but 'struct-like' 
 * with everything public.
 *
 * @author Laurie Nevay
 */

class BDSFieldInfoExtra
{
public:
  BDSFieldInfoExtra(){;}
  virtual ~BDSFieldInfoExtra(){;}

  /// Derived class must implement for copying.
  virtual BDSFieldInfoExtra* Clone() const = 0;
};

/**
 * @brief necessary extra information for a muon cooler field.
 *
 * @author Laurie Nevay
 */

class BDSFieldInfoExtraMuonCooler: public BDSFieldInfoExtra
{
public:
  BDSFieldInfoExtraMuonCooler() = delete;
  BDSFieldInfoExtraMuonCooler(BDSFieldType magneticFieldTypeIn,
                              BDSFieldType electricFieldTypeIn,
                              const std::vector<BDS::MuonCoolerCoilInfo>& coilInfosIn,
                              const std::vector<BDS::MuonCoolerCavityInfo>& cavityInfosIn):
    magneticFieldType(magneticFieldTypeIn),
    electricFieldType(electricFieldTypeIn),
    coilInfos(coilInfosIn),
    cavityInfos(cavityInfosIn)
  {;}
  virtual ~BDSFieldInfoExtraMuonCooler(){;}

  BDSFieldType magneticFieldType; ///< Type of the magnetic sub-field.
  BDSFieldType electricFieldType; ///< Type of the electric sub-field.
  std::vector<BDS::MuonCoolerCoilInfo> coilInfos;
  std::vector<BDS::MuonCoolerCavityInfo> cavityInfos;

  virtual BDSFieldInfoExtra* Clone() const {return new BDSFieldInfoExtraMuonCooler(*this);}
};

#endif
