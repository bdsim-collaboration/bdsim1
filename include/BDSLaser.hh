/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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
#ifndef BDSLASER_H
#define BDSLASER_H

#include "globals.hh" // geant4 types / globals

/**
 * @brief Describe the function here
 *
 * @author Siobhan Alden
 */

class BDSLaser
{
public:
  BDSLaser(G4double wavelengthIn,
	   G4double m2In,
	   G4double pulseDurationIn,
	   G4double pulseEnergyIn,
	   G4double sigam0In);
  ~BDSLaser();

  /// Copy constructor.
  BDSLaser(const BDSLaser &laser);
  
  //this needs to be called based upon particle coordinates
  G4double Width(G4double particlePosition) const;
  G4double Intensity(G4double radius,G4double distanceFromFocus) const;
  G4double Radius() const ;
  G4double PhotonEnergy(G4double particleGamma,
			G4double overlapAngle,
			G4double particleBeta) const;
  
  /// @{ Accessor.
  inline G4double Wavelength()    const {return wavelength;}
  inline G4double M2()            const {return wavelength;}
  inline G4double PulseDuration() const {return pulseDuration;}
  inline G4double PulseEnergy()   const {return pulseEnergy;}
  inline G4double Sigma0()        const {return sigma0;}
  inline G4double RayleighRange() const {return rayleighRange;}
  inline G4double W0()            const {return 2*sigma0;}
  /// @}

protected:
  BDSLaser() = delete;
  
  G4double wavelength;
  G4double m2;
  G4double pulseDuration;
  G4double pulseEnergy;
  G4double sigma0;

  /// @{ Calculated parameters.
  G4double peakPower;
  G4double rayleighRange;
  /// @}
};

#endif