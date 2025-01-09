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
#ifndef ELEMENT_H
#define ELEMENT_H

#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <string>

#include "published.h"

namespace GMAD
{
  enum class ElementType;

  struct Parameters;
 
  /**
   * @brief Element class
   * 
   * @author I. Agapov
   */

  struct Element: public Published<Element>
  {
    ElementType type; ///< element enum
    std::string name;
    std::string userTypeName; ///< User component element type name.
    std::string userParameters; ///< String for passing user parameters through.

    double l; ///< length in metres
    double scaling; ///< Overall scaling of field strength.
    double scalingFieldOuter; ///< Extra arbitrary scaling for outer field - compounded with 'scaling'.
    double ks; ///< solenoid
    // double k0; // for dipole field B or angle is used
    double k1; ///< quadrupole
    double k2; ///< sextupole
    double k3; ///< octupole
    double k4; ///< decapole
    double angle; ///< bending angle
    double B; ///< magnetic field
    double e1; ///< input pole face rotation for bends
    double e2; ///< output pole face rotation for bends
    double fint; ///< fringe field integral at the dipole entrance
    double fintx;  ///< fringe field integral at the dipole exit
    double fintK2; ///< second fringe field integral at the dipole entrance - for TRANSPORT matching
    double fintxK2;///< second fringe field integral at the dipole exit - for TRANSPORT matching
    double hgap;   ///< half distance of pole separation for purposes of fringe fields - 'half gap'
    double h1; ///< input pole face curvature for bends
    double h2; ///< output pole face curvature for bends
    double kick;   ///< fractional delta p for either h or v kicker
    double hkick;  ///< fractional delta px for hkicker
    double vkick;  ///< fractional delta py for vkicker
    std::list<double> knl; ///< multipole expansion coefficients
    std::list<double> ksl; ///< skew multipole expansion
    double gradient;  ///< for rf cavities in V / m
    double E;         ///< voltage for rf cavities in V that will be assumed over length l
    double frequency; ///< frequency for rf cavity in Hz
    double phase;     ///< phase of rf cavity (rad)
    double tOffset;   ///< time offset used for phase calculation (ns)
    std::string fieldModulator;

    ///@{ rmatrix elements, only 4x4
    double kick1;
    double kick2;
    double kick3;
    double kick4;
    double rmat11;
    double rmat12;
    double rmat13;
    double rmat14;
    double rmat21;
    double rmat22;
    double rmat23;
    double rmat24;
    double rmat31;
    double rmat32;
    double rmat33;
    double rmat34;
    double rmat41;
    double rmat42;
    double rmat43;
    double rmat44;
    ///@}


    ///@{ beampipe information, new aperture model
    double beampipeThickness;
    double aper1;
    double aper2;
    double aper3;
    double aper4;
    std::string apertureType;
    std::string beampipeMaterial;
    std::string vacuumMaterial;
    ///@}
  
    // magnet geometry
    std::string magnetGeometryType;
    double horizontalWidth; // formerly outerDiameter
    bool   yokeOnInside;
    int    hStyle;  ///< -1 = unset; 0 = false (ie c style); 1 = true, use hstyle
    double vhRatio; ///< ratio of vertial to horizontal for some magnets
    double coilWidthFraction; ///< Fraction of available h space the coil will take up.
    double coilHeightFraction; ///< Fraction of availalbe v space the coil will take up.
    double tilt;    ///< tilt
    double xsize, ysize; ///< collimator aperture or laser spotsize for laser
    double xsizeOut, ysizeOut; ///< collimator aperture or laser spotsize for laser
    double xsizeLeft, xsizeRight; ///< individual collimator jaw half widths
    double jawTiltLeft, jawTiltRight; ///< jaw collimator jaw tilts (angle in x-z plane)
    double offsetX; ///< offset X
    double offsetY; ///< offset Y

    // screen parameters
    double tscint; ///<thickness of scintillating part of screen
    double twindow; ///<thickness of window
    double tmount; ///<thickness of the screen mount
    double windowScreenGap; ///< air gap between window and screen
    double screenXSize, screenYSize; ///< for screen
    ///@{ for screen
    std::list<double>      layerThicknesses;
    std::list<std::string> layerMaterials;
    std::list<int>         layerIsSampler;
    ///@}
    
    ///@{ for AWAKE spectrometer
    double screenPSize; ///<Phosphor particle size in screen
    double screenEndZ;
    double poleStartZ;
    double screenWidth;
    double awakeMagnetOffsetX;
    std::string windowmaterial;
    std::string scintmaterial;
    std::string mountmaterial;
    ///@}

    ///@{ for 3d transform and laser
    double xdir;
    double ydir;
    double zdir;
    ///@}
    double waveLength; ///< for laser wire and 3d transforms
    double phi, theta, psi; ///< for 3d transforms
    double axisX, axisY, axisZ;
    bool   axisAngle;

    ///@{ for degrader
    int    numberWedges;
    double wedgeLength;
    double degraderHeight;
    double materialThickness;
    double degraderOffset;
    ///@}

    ///@{ for laserwire
    std::string laserBeam;
    double laserOffsetTheta;
    double laserOffsetPhi;
    double laserOffsetX;
    double laserOffsetY;
    double laserOffsetZ;
    ///@}

    ///@[ for wirescanner
    double wireDiameter;
    double wireLength;
    double wireOffsetX;
    double wireOffsetY;
    double wireOffsetZ;
    double wireAngle;
    ///@}
    
    ///@{ for undulator
    double undulatorPeriod;
    double undulatorGap;
    double undulatorMagnetHeight;
    ///@}
  
    ///@{ temporary string for bias setting
    std::string bias;
    std::string biasMaterial;
    std::string biasVacuum;
    ///@}
    /// physics biasing list for the material
    std::list<std::string> biasMaterialList;
    /// physics biasing list for the vacuum
    std::list<std::string> biasVacuumList;

    /// minimum kinetic energy for user limits - respected on element by element basis
    double minimumKineticEnergy;

    std::string samplerName;      ///< name of sampler (default empty)
    std::string samplerType;      ///< element has a sampler of this type (default "none")
    double samplerRadius;         ///< radius for cylindrical sampler
    /// ID to a map for a set of which partIDs to store for a sampler. We use an integer
    /// to a map we keep in the parser to save memory, so we don't copy a set to every
    /// beam line element.
    int    samplerParticleSetID;
    
    std::string region;      ///< region with range cuts
    std::string fieldOuter;  ///< Outer field.
    std::string fieldVacuum; ///< Vacuum field.
    std::string fieldAll;    ///< Field for everything.
    
    std::string geometryFile;     ///< For Element. File for external geometry.
    bool        stripOuterVolume; ///< For Element. Make it an assembly.
    bool        autoColour;       ///< Automagically colour the external geometry.

    bool        elementLengthIsArcLength; ///< For Element. Treat the length as arc length, if not chord.

    std::string material;
    std::string namedVacuumVolumes; ///< For imported geometry - identify vacuum volumes.
    bool        markAsCollimator;
    std::string spec;            ///< Arbitrary specification to pass to beamline builder.
    std::string cavityModel;     ///< Name of geometry model object for rfconstantinz cavities.
    std::string cavityFieldType; ///< Name for type of field to use in a cavity.

    std::string dicomDataPath; ///< for CT, file for DICOM construction data
    std::string dicomDataFile; ///< for CT, file for DICOM construction data

    /// Override colour for certain items
    std::string colour;

    std::string crystalLeft;
    std::string crystalRight;
    std::string crystalBoth;
    double      crystalAngleYAxisLeft;
    double      crystalAngleYAxisRight;
  
    /// Whether the angle was set. Unique as we may technically have 0 angle but a finite
    /// field. This allows us to distinguish later on.
    /// NOTE: this is not used in Params.
    bool   angleSet;

    bool   scalingFieldOuterSet;

    /// in case the element is a list itself (line)
    std::list<Element>* lst;

    /// print method
    void print(int ident=0) const;

    /// flush method
    void flush();

    /// check if element is of a special type
    bool isSpecial() const;
    /// property lookup by name (slow method)
    /// only for properties with type int/double!
    double property_lookup(std::string property_name) const;

    /// set sampler info
    void setSamplerInfo(std::string samplerType,
                        std::string samplerName,
                        double samplerRadius,
                        int samplerParticleSetIDIn = -1);

    ///@{ set method from Parameters structure
    void set(const Parameters& params);
    void set(const Parameters& params,std::string nameIn, ElementType typeIn);
    ///@}
    /// Set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value);
 
    /// constructor
    Element();

  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
    /// map that translates between alternative parser names for members, could be made static
    std::map<std::string,std::string> alternativeNames;

  protected:
    /// returns 'official' member name for property
    std::string getPublishedName(const std::string& name) const;
  };

  template <typename T>
  void Element::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "element> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try
	{Published<Element>::set(this,property,value);}
      catch(const std::runtime_error&)
	{
	  std::cerr << "Error: element> unknown property \"" << property << "\" with value \"" << value << "\"" << std::endl;
	  exit(1);
	}
    }
}
 
#endif
