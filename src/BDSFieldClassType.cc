/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSFieldClassType.hh"

#include <map>
#include <string>

// dictionary for BDSFieldClassType for reflexivity
template<>
std::map<BDSFieldClassType, std::string>* BDSFieldClassType::dictionary =
  new std::map<BDSFieldClassType, std::string> ({
      {BDSFieldClassType::magnetic,        "magnetic"},
      {BDSFieldClassType::electric,        "electric"},
      {BDSFieldClassType::electromagnetic, "electromagnetic"},
      {BDSFieldClassType::irregular,       "irregular"}
    });

BDSFieldClassType BDS::DetermineFieldClassType(BDSFieldType fType)
{
  switch (fType.underlying())
    {
    case BDSFieldType::bfieldzero:
    case BDSFieldType::bmap1d:
    case BDSFieldType::bmap2d:
    case BDSFieldType::bmap3d:
    case BDSFieldType::bmap4d:
    case BDSFieldType::mokka:
    case BDSFieldType::solenoid:
    case BDSFieldType::solenoidsheet:
    case BDSFieldType::solenoidloop:
    case BDSFieldType::solenoidblock:
    case BDSFieldType::dipole:
    case BDSFieldType::dipole3d:
    case BDSFieldType::quadrupole:
    case BDSFieldType::dipolequadrupole:
    case BDSFieldType::sextupole:
    case BDSFieldType::octupole:
    case BDSFieldType::decapole:
    case BDSFieldType::multipole:
    case BDSFieldType::muonspoiler:
    case BDSFieldType::skewquadrupole:
    case BDSFieldType::skewsextupole:
    case BDSFieldType::skewoctupole:
    case BDSFieldType::skewdecapole:
    case BDSFieldType::undulator:
    case BDSFieldType::multipoleouterdipole:
    case BDSFieldType::multipoleouterquadrupole:
    case BDSFieldType::multipoleoutersextupole:
    case BDSFieldType::multipoleouteroctupole:
    case BDSFieldType::multipoleouterdecapole:
    case BDSFieldType::skewmultipoleouterquadrupole:
    case BDSFieldType::skewmultipoleoutersextupole:
    case BDSFieldType::skewmultipoleouteroctupole:
    case BDSFieldType::skewmultipoleouterdecapole:
    case BDSFieldType::multipoleouterdipole3d:
    case BDSFieldType::multipoleouterdipolelhc:
    case BDSFieldType::multipoleouterquadrupolelhc:
    case BDSFieldType::multipoleoutersextupolelhc:
      {return BDSFieldClassType::magnetic;}
    case BDSFieldType::efieldzero:
    case BDSFieldType::emap1d:
    case BDSFieldType::emap2d:
    case BDSFieldType::emap3d:
    case BDSFieldType::emap4d:
    case BDSFieldType::rfconstantinx:
    case BDSFieldType::rfconstantiny:
    case BDSFieldType::rfconstantinz:
      {return BDSFieldClassType::electric;}
    case BDSFieldType::none:
    case BDSFieldType::ebfieldzero:
    case BDSFieldType::ebmap1d:
    case BDSFieldType::ebmap2d:
    case BDSFieldType::ebmap3d:
    case BDSFieldType::ebmap4d:
    case BDSFieldType::rfpillbox:
    case BDSFieldType::muoncooler:
      {return BDSFieldClassType::electromagnetic;}
    case BDSFieldType::teleporter:
    case BDSFieldType::rmatrix:
    case BDSFieldType::cavityfringe:
    case BDSFieldType::paralleltransporter:
      {return BDSFieldClassType::irregular;}
    default:
      {return BDSFieldClassType::electromagnetic;}
    }
}
