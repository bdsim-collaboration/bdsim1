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

#ifndef LASER_H
#define LASER_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD {
/**
 * @brief Placement class for laser
 *
 * @author Siobhan Alden
 */
  class Laser : public Published<Laser>
  {
  public:
    std::string name;         ///< Name of this laser.
    double wavelength{};
    double m2{};
    double pulseEnergy{};
    double pulseDuration{};
    double w0{};
    double sigma0{};
    double laserArrivalTime{};
    double laserPolarization1{};
    double laserPolarization2{};
    double laserPolarization3{};
    bool   ignoreRayleighRange{};

    
    /// constructor
    Laser();
    /// reset
    void clear();
    /// print some properties
    void print() const;
    /// set methods by property name and value
    template<typename T>
    void set_value(std::string property, T value);
    
  private:
    /// publish members
    void PublishMembers();
  };
  
  template<typename T>
  void Laser::set_value(std::string property, T value) {
#ifdef BDSDEBUG
    std::cout << "Laser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try {
      set(this, property, value);
    }
    catch (std::runtime_error) {
      std::cerr << "Error: placement> unknown option \"" << property << "\" with value " << value << std::endl;
      exit(1);
    }
  }
}

#endif
