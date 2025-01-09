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
#ifndef COOLINGCHANNEL_H
#define COOLINGCHANNEL_H

#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Cooling channel parameters
   * 
   * @author Laurie Nevay
   */
  
  class CoolingChannel: public Published<CoolingChannel>
  {
  public:
    std::string name; ///< Object name

    std::string surroundingMaterial;
    
    int                    nCells;
    double                 cellLengthZ;

    int                    nCoils;
    std::list<double>      coilInnerRadius;
    std::list<double>      coilRadialThickness;
    std::list<double>      coilLengthZ;
    std::list<double>      coilCurrent;
    std::list<double>      coilOffsetZ;
    std::list<std::string> coilMaterial;
    bool                   mirrorCoils;
    double                 onAxisTolerance;
    int                    nSheets;

    int                    nAbsorbers;
    std::list<std::string> absorberType;
    std::list<std::string> absorberMaterial;
    std::list<double>      absorberOffsetZ;
    std::list<double>      absorberCylinderLength;
    std::list<double>      absorberCylinderRadius;
    std::list<double>      absorberWedgeOpeningAngle;
    std::list<double>      absorberWedgeHeight;
    std::list<double>      absorberWedgeRotationAngle;
    std::list<double>      absorberWedgeOffsetX;
    std::list<double>      absorberWedgeOffsetY;
    std::list<double>      absorberWedgeApexToBase;

    int                    nRFCavities;
    std::list<double>      rfOffsetZ;
    std::list<double>      rfLength;
    std::list<double>      rfVoltage;
    std::list<double>      rfPhase;
    std::list<double>      rfFrequency;
    std::list<double>      rfWindowThickness;
    std::list<std::string> rfWindowMaterial;
    std::list<double>      rfWindowRadius;
    std::list<std::string> rfCavityMaterial;
    std::list<std::string> rfCavityVacuumMaterial;
    std::list<double>      rfCavityRadius;
    std::list<double>      rfCavityThickness;
    std::list<double>      rfTimeOffset;
  
    std::string integrator;
    std::string magneticFieldModel;
    std::string electricFieldModel;
    
    /// Constructor
    CoolingChannel();
    /// Reset
    void clear();
    /// Print some properties
    void print()const;
    /// Set methods by property name and value
    template <typename T>
    void set_value(const std::string& property, T value);
    // Template overloading for Array pointers
    /// Set method for lists
    void set_value(const std::string& property, Array* value);

  private:
    /// publish members
    void PublishMembers();
    
    std::map<std::string, std::list<double>*> attribute_map_list_double;
    std::map<std::string, std::list<std::string>*> attribute_map_list_string;
  };
  
  template <typename T>
  void CoolingChannel::set_value(const std::string& property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "coolingchannel> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try
        {set(this,property,value);}
      catch(const std::runtime_error&)
        {
          std::cerr << "Error: coolingchannel> unknown option \"" << property << "\" with value " << value  << std::endl;
          exit(1);
        }
    }
}

#endif
