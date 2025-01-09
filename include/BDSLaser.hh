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
#ifndef BDSLASER_H
#define BDSLASER_H
#include "G4String.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include "G4StokesVector.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include <vector>

/**
 * @brief Class to provide laser intensity at any point.
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
	   G4double sigma0In,
  	   G4double laserArrivalTimeIn,
       G4double T0In,
       G4ThreeVector polarizationIn,
       G4bool   ignoreRayleighRangeIn = false);
  BDSLaser(G4double wavelengthIn);
  ~BDSLaser();
  
  //this needs to be called based upon particle coordinates
  G4double W(G4double z) const;
  G4double Sigma(G4double z) const {return 0.5*W(z);}
  G4double Intensity(G4double x,double y,double z,double t) const;
  G4double Intensity(const G4ThreeVector& xyz,double t) const;
  G4double Radius() const;
  G4double TemporalProfileGaussian(G4double particleGlobalTime, G4double particleZCoord) const;
  G4double PhotonEnergy(G4double particleGamma,
			G4double overlapAngle,
			G4double particleBeta) const;
  G4double HyperbolicAngle() const;
  G4String GetLaserColour() const;

  /// @{ Accessor.
  inline G4double Wavelength()    const {return wavelength;}
  inline G4double M2()            const {return m2;}
  inline G4double PulseDuration() const {return pulseDuration;}
  inline G4double PulseEnergy()   const {return pulseEnergy;}
  inline G4double Sigma0()        const {return sigma0;}
  inline G4double RayleighRange() const {return rayleighRange;}
  inline G4double W0()            const {return 2*sigma0;}
  inline G4double LaserArrivalTime()   const {return laserArrivalTime;}
  inline G4ThreeVector Polarization() const {return polarization;}
  inline void SetT0(G4double T0In) {T0=T0In;}
  inline G4bool  IgnoreRayleighRange() const {return ignoreRayleighRange;}
  /// @}

protected:
  BDSLaser() = delete;
  
  G4double wavelength;
  G4double m2;
  G4double pulseDuration;
  G4double pulseEnergy;
  G4double sigma0;
  G4double laserArrivalTime;
  G4double T0;
  G4bool   ignoreRayleighRange;
  G4ThreeVector polarization;

  const static std::vector<G4double> wavelengths;
  const static std::vector<G4String> colours;

  /// @{ Calculated parameters.
  G4double peakPower;
  G4double rayleighRange;
  /// @}
};

#endif
