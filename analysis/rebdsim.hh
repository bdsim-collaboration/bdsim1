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
#ifndef REBDSIM_H
#define REBDSIM_H

#include <string>

inline std::string methodName(const std::string& prettyFunction)
{
  size_t end    = prettyFunction.rfind("(");
  size_t begin  = prettyFunction.rfind(" ",end) + 1;
  // if function has pointer type then begin now is *, if so advance one more
  if (prettyFunction[begin] == '*') {begin++;}
  size_t len = end - begin;
  return prettyFunction.substr(begin,len) +"> ";
}
#define __METHOD_NAME__ methodName(__PRETTY_FUNCTION__)


#endif
