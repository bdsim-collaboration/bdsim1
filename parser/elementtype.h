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
#ifndef ELEMENTTYPE_H
#define ELEMENTTYPE_H

#include <ostream>
#include <string>

namespace GMAD
{
  /// types of elements
  enum class ElementType {
    _NONE        = -1,
    _MARKER      = 1,
    _DRIFT       = 2,
    _RF          = 3,
    _SBEND       = 4, 
    _QUAD        = 5,
    _SEXTUPOLE   = 6,
    _OCTUPOLE    = 7,
    _DECAPOLE    = 8,
    _MULT        = 9,
    _SOLENOID    = 10,
    _LINE        = 11,
    _REV_LINE    = -11, //for line inversion in sublines
    // = 12,
    _ECOL        = 13,
    _RCOL        = 14,
    _LASER       = 15,
    _MATERIAL    = 16,
    _RBEND       = 17,
    // = 18,
    // = 19,
    _ELEMENT     = 20,
    _SCREEN      = 21,
    _AWAKESCREEN = 22,
    _AWAKESPECTROMETER = 23,
    _SHIELD      = 24,
    _VKICKER     = 31,
    _HKICKER     = 32,
    _KICKER      = 33,
    _TKICKER     = 34,
    _TRANSFORM3D = 61,
    _MUONSPOILER = 62,
    _DEGRADER    = 63,
    _THINMULT    = 64,
    _GAP         = 65,
    _THINRMATRIX = 66,
    _PARALLELTRANSPORTER = 67,
    _RMATRIX     = 68,
    _CRYSTALCOL  = 69,
    _UNDULATOR   = 70,
    _JCOL        = 71,
    _WIRESCANNER = 72,
    _USERCOMPONENT = 73,
    _DUMP          = 74,
    _CT            = 75,
    _TARGET        = 76,
    _RFX           = 77,
    _RFY           = 78,
    _GABORLENS     = 79
  };

  /// conversion from enum to string
  std::string typestr(ElementType type);

  /// output operator
  std::ostream& operator << (std::ostream& out, ElementType type);
}

#endif
