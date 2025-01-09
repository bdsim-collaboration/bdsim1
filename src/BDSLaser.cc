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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSLaser.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <vector>

const std::vector<G4double> BDSLaser::wavelengths = {340.0*CLHEP::nanometer, //magenta
                425.0*CLHEP::nanometer, //purple
                445.0*CLHEP::nanometer, //blue
                520.0*CLHEP::nanometer, //indigo
                565.0*CLHEP::nanometer, //green
                590.0*CLHEP::nanometer, //yellow
                625.0*CLHEP::nanometer, //orange
                740.0*CLHEP::nanometer}; //red

const std::vector<G4String> BDSLaser::colours = {"magenta",
"decapole",
"blue",
"muonspoiler",
"green",
"yellow",
"solenoid",
"red",
"quadrupole"};

BDSLaser::BDSLaser(G4double wavelengthIn,
                   G4double m2In,
                   G4double pulseDurationIn,
                   G4double pulseEnergyIn,
                   G4double sigma0In,
                   G4double laserArrivalTimeIn,
                   G4double T0In,
                   G4ThreeVector polarizationIn,
                   G4bool   ignoreRayleighRangeIn):
  wavelength(wavelengthIn),
  m2(m2In),
  pulseDuration(pulseDurationIn),
  pulseEnergy(pulseEnergyIn),
  sigma0(sigma0In),
  laserArrivalTime(laserArrivalTimeIn),
  T0(T0In),
  polarization(polarizationIn),
  ignoreRayleighRange(ignoreRayleighRangeIn)
{
  if(!BDS::IsFinite(sigma0In))
    {throw BDSException(__METHOD_NAME__, "Laser waist sigma0 is zero.");}
  peakPower = pulseEnergy / ((2.0*std::sqrt(2.0*std::log(2.0)))*pulseDuration);
  rayleighRange = (CLHEP::pi * (2.0*sigma0)*(2.0*sigma0)) / (wavelength * m2);
}

BDSLaser::BDSLaser(G4double wavelengthIn):   wavelength(wavelengthIn),
                                             m2(0),
                                             pulseDuration(0),
                                             pulseEnergy(0),
                                             sigma0(0),
                                             laserArrivalTime(0),
                                             T0(0),
                                             polarization(0),
                                             ignoreRayleighRange(0) {}

BDSLaser::~BDSLaser()
{;}

G4double BDSLaser::W(G4double z) const
{
  if (ignoreRayleighRange)
    {return W0();}
  return W0()*std::sqrt(1.0+std::pow(z/rayleighRange,2.0));
}

G4double BDSLaser::Intensity(G4double x, G4double y, G4double z, G4double t) const
{
  return Intensity(G4ThreeVector(x,y,z), t);
}
G4double BDSLaser::Intensity(const G4ThreeVector& xyz, G4double /*t*/) const
{
  G4double r2 = xyz.perpPart().mag2(); // x^2 + y^2
  G4double wofz = W(xyz.z());
  G4double wofz2 = wofz*wofz;
  return (2.0*peakPower)/(CLHEP::pi*wofz2) * std::exp(-(1.0*(2.0*r2))/wofz2);
}

G4double BDSLaser::Radius() const
{
  return std::sqrt((W0()*std::log(1.0/(CLHEP::e_squared)))/-2.0);
}

G4double BDSLaser::PhotonEnergy(G4double particleGamma,
				G4double overlapAngle,
				G4double particleBeta) const
{
  return particleGamma*((CLHEP::h_Planck*CLHEP::c_light)/wavelength)
         *(1-particleBeta*std::cos(overlapAngle*CLHEP::radian));
}

G4double BDSLaser::HyperbolicAngle() const
{
  return (m2*wavelength)/(2.0*CLHEP::pi*W0());
}

G4double BDSLaser::TemporalProfileGaussian(G4double particleGlobalTime, G4double particleZCoord) const
{
    G4double mu = (particleGlobalTime-(T0+laserArrivalTime))*CLHEP::nanosecond; // can be negative - locates the peak of the pulse in time for a given particleGlobalTime
    G4double sigmaT = pulseDuration/(2.0 * std::sqrt(2.0 * std::log(2.0))) ;
    return std::exp(-((particleZCoord/CLHEP::c_light - mu)*(particleZCoord/CLHEP::c_light-mu)) / (2.0 * sigmaT * sigmaT));
}

G4String BDSLaser::GetLaserColour() const
{
  auto it = std::lower_bound(wavelengths.begin(),wavelengths.end(),wavelength);
  G4int index = std::distance(wavelengths.begin(),it);
  return colours[index];
}
