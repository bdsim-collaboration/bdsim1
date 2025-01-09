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
#ifndef BEAMBASE_H
#define BEAMBASE_H

#include <string>
#include <vector>

namespace GMAD
{
  /**
   * @brief Options for a beam distribution.
   *
   * This is the basic structure that can be serialised in an output
   * file and only contains built in C++ types. No Geant4 types are
   * to be used. The Beam class inherits this one.
   */
  class BeamBase
    {
    public:
      BeamBase();

      ///@{ beam parameters
      std::string particle;
      std::string beamParticleName;
      double      beamEnergy;
      double      beamKineticEnergy;
      double      beamMomentum;
      std::string distrType;
      std::string xDistrType;
      std::string yDistrType;
      std::string zDistrType;
      std::string spaceDistrType;
      std::string directionDistrType;
      std::string energyDistrType;
      std::string distrFile;
      std::string distrFileFormat;
      bool        distrFileFromExecOptions; ///< Required to know how to build the absolute path properly
      bool        distrFileMatchLength;
      bool        distrFileLoop;
      int         distrFileLoopNTimes;
      bool        removeUnstableWithoutDecay;
      ///@}
      int         nlinesIgnore; ///< Ignore first lines in the input bunch file.
      int         nlinesSkip;   ///< Number of event lines to skip after the ignore lines.

      /// @{ Bunch offsets in time
      double bunchFrequency;
      double bunchPeriod;
      int    eventsPerBunch;
      /// @}
      
      ///@{ initial beam centroid
      double X0, Y0, Z0, S0;
      double Xp0, Yp0, Zp0;
      double T0;
      double E0;
      double Ek0;
      double P0;
      ///@}

      /// tilt of beam applied as rotation about unit local z
      double tilt;

      /// bunch length
      double sigmaT;

      /// for the gaussian, elliptic shell, ring distributions
      double sigmaE;
      double sigmaEk;
      double sigmaP;
      
      ///@{ initial twiss parameters
      double betx, bety, alfx, alfy, emitx, emity, dispx, dispy, dispxp, dispyp;
      double emitNX, emitNY;
      ///@}
      
      ///@{ for the gaussian beam distribution
      double sigmaX, sigmaXp, sigmaY, sigmaYp;
      ///@}
      
      ///@{ for the circle/square/box beam distribution
      double envelopeX, envelopeXp, envelopeY, envelopeYp, envelopeZ, envelopeZp;
      double envelopeT, envelopeE;
      double envelopeR, envelopeRp;
      bool   zFromT;
      ///@}
      
      ///@{ for the gaussian sigma matrix distribution
      double sigma11, sigma12, sigma13, sigma14, sigma15, sigma16;
      double          sigma22, sigma23, sigma24, sigma25, sigma26;
      double                   sigma33, sigma34, sigma35, sigma36;
      double                            sigma44, sigma45, sigma46;
      double                                     sigma55, sigma56;
      double                                              sigma66;
      ///@}
      
      ///@{ for the elliptic shell distribution
      double shellX, shellXp, shellY, shellYp;
      double shellXWidth, shellXpWidth, shellYWidth, shellYpWidth;
      ///@}
      
      ///@{ for the ring beam distribution
      double Rmin, Rmax;
      ///@}
      
      ///@{ for the halo distribution
      double haloNSigmaXInner;
      double haloNSigmaXOuter;
      double haloNSigmaYInner;
      double haloNSigmaYOuter;
      double haloXCutInner;
      double haloYCutInner;
      double haloXCutOuter;
      double haloYCutOuter;
      double haloXpCutInner;
      double haloYpCutInner;
      double haloXpCutOuter;
      double haloYpCutOuter;
      double haloPSWeightParameter;
      std::string haloPSWeightFunction;
      ///@}

      bool offsetSampleMean;

      /// @{ Event generator file filter.
      int    eventGeneratorNEventsSkip;
      double eventGeneratorMinX;
      double eventGeneratorMaxX;
      double eventGeneratorMinY;
      double eventGeneratorMaxY;
      double eventGeneratorMinZ;
      double eventGeneratorMaxZ;
      double eventGeneratorMinXp;
      double eventGeneratorMaxXp;
      double eventGeneratorMinYp;
      double eventGeneratorMaxYp;
      double eventGeneratorMinZp;
      double eventGeneratorMaxZp;
      double eventGeneratorMinRp;
      double eventGeneratorMaxRp;
      double eventGeneratorMinT;
      double eventGeneratorMaxT;
      double eventGeneratorMinEK;
      double eventGeneratorMaxEK;
      std::string eventGeneratorParticles;
      bool   eventGeneratorWarnSkippedParticles;
      /// @}
  
      /// A list of all the keys that have been set in this instance.
      std::vector<std::string> setKeys;

      double polarization1;
      double polarization2;
      double polarization3;

    };
}

#endif
