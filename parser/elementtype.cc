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
#include "elementtype.h"
#include <string>

using namespace GMAD;

std::ostream& GMAD::operator << (std::ostream& out, ElementType type)
{
  return out << typestr(type);
}

std::string GMAD::typestr(ElementType type)
{
  switch(type)
    {
    case ElementType::_MARKER: 
      {return "marker";              break;}
    case ElementType::_DRIFT:
      {return "drift";               break;}
    case ElementType::_RF:
      {return "rf";                  break;}
    case ElementType::_SBEND: 
      {return "sbend";               break;}
    case ElementType::_QUAD:
      {return "quadrupole";          break;}
    case ElementType::_SEXTUPOLE:
      {return "sextupole";           break;}
    case ElementType::_OCTUPOLE:
      {return "octupole";            break;}
    case ElementType::_DECAPOLE:
      {return "decapole";            break;}
    case ElementType::_MULT:
      {return "multipole";           break;}
    case ElementType::_SOLENOID:
      {return "solenoid";            break;}
    case ElementType::_LINE:
      {return "line";                break;}
    case ElementType::_REV_LINE:
      {return "rev_line";            break;}
    case ElementType::_ECOL: 
      {return "ecol";                break;}
    case ElementType::_RCOL: 
      {return "rcol";                break;}
    case ElementType::_LASER:
      {return "laser";               break;}
    case ElementType::_MATERIAL:
      {return "material";            break;}
    case ElementType::_RBEND:
      {return "rbend";               break;}
    case ElementType::_ELEMENT:
      {return "element";             break;}
    case ElementType::_SCREEN:
      {return "screen";              break;}
    case ElementType::_AWAKESCREEN:
      {return "awakescreen";         break;}
    case ElementType::_AWAKESPECTROMETER:
      {return "awakespectrometer";   break;}
    case ElementType::_SHIELD:
      {return "shield";              break;}
    case ElementType::_VKICKER:
      {return "vkicker";             break;}
    case ElementType::_HKICKER:
      {return "hkicker";             break;}
    case ElementType::_KICKER:
      {return "kicker";              break;}
    case ElementType::_TKICKER:
      {return "tkicker";             break;}
    case ElementType::_TRANSFORM3D:
      {return "transform3d";         break;}
    case ElementType::_WIRESCANNER:
      {return "wirescanner";         break;}
    case ElementType::_MUONSPOILER: 
      {return "muonspoiler";         break;}
    case ElementType::_DEGRADER:
      {return "degrader";            break;}
    case ElementType::_THINMULT:
      {return "thinmultipole";       break;}
    case ElementType::_GAP:
      {return "gap";                 break;}
    case ElementType::_THINRMATRIX:
      {return "thinrmatrix";         break;}
    case ElementType::_PARALLELTRANSPORTER:
      {return "paralleltransporter"; break;}
    case ElementType::_RMATRIX:
      {return "rmatrix";             break;}
    case ElementType::_CRYSTALCOL:
      {return "crystalcol";          break;}
    case ElementType::_UNDULATOR:
      {return "undulator";           break;}
    case ElementType::_JCOL:
      {return "jcol";                break;}
    case ElementType::_USERCOMPONENT:
      {return "usercomponent";       break;}
    case ElementType::_DUMP:
      {return "dump";                break;}
    case ElementType::_CT:
      {return "ct";                  break;}
    case ElementType::_TARGET:
      {return "target";              break;}
    case ElementType::_RFX:
      {return "rfx";                 break;}
    case ElementType::_RFY:
      {return "rfy";                 break;}
    case ElementType::_GABORLENS:
      {return "gaborlens";           break;}
    default:
      {return "none"; break;}
    }
}
