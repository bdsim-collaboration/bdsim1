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
#include "beamBase.h"

#include <iostream>

using namespace GMAD;

BeamBase::BeamBase()
{
  particle             = "";
  beamParticleName     = "";
  beamEnergy           = 0;
  beamKineticEnergy    = 0;
  beamMomentum         = 0;
  distrType            = "reference";
  xDistrType           = "reference";
  yDistrType           = "reference";
  zDistrType           = "reference";
  spaceDistrType       = "reference";
  directionDistrType   = "reference";
  energyDistrType      = "reference";
  distrFile            = "";
  distrFileFormat      = "";
  distrFileFromExecOptions = false;
  distrFileMatchLength = true;
  distrFileLoop        = false;
  distrFileLoopNTimes  = 1;
  removeUnstableWithoutDecay = true;
  nlinesIgnore         = 0;
  nlinesSkip           = 0;
  
  bunchFrequency = 0;
  bunchPeriod = 0;
  eventsPerBunch = 0;
  
  X0  = 0.0;
  Y0  = 0.0;
  Z0  = 0.0;
  S0  = 0.0;
  Xp0 = 0.0;
  Yp0 = 0.0;
  Zp0 = 0.0;
  T0  = 0.0;
  E0  = 0.0;
  Ek0 = 0.0;
  P0  = 0.0;
  tilt    = 0.0;
  sigmaT  = 0.0;
  sigmaE  = 0.0;
  sigmaEk = 0.0;
  sigmaP  = 0.0;

  betx    = 0.0;
  bety    = 0.0;
  alfx    = 0.0;
  alfy    = 0.0;
  emitx   = 0.0;
  emity   = 0.0;
  emitNX  = 0.0;
  emitNY  = 0.0;
  dispx   = 0.0;
  dispy   = 0.0;
  dispxp  = 0.0;
  dispyp  = 0.0;
  sigmaX  = 0.0;
  sigmaXp = 0.0;
  sigmaY  = 0.0;
  sigmaYp = 0.0;
  
  envelopeX  = 0.0;
  envelopeXp = 0.0;
  envelopeY  = 0.0;
  envelopeYp = 0.0;
  envelopeZ  = 0.0;
  envelopeZp = 0.0;
  envelopeT  = 0.0;
  envelopeE  = 0.0;
  envelopeR  = 0.0;
  envelopeRp = 0.0;
  zFromT = false;
  
  sigma11 = 0.0,sigma12 = 0.0,sigma13 = 0.0,sigma14 = 0.0,sigma15 = 0.0,sigma16 = 0.0;
  sigma22 = 0.0,sigma23 = 0.0,sigma24 = 0.0,sigma25 = 0.0,sigma26 = 0.0;
  sigma33 = 0.0,sigma34 = 0.0,sigma35 = 0.0,sigma36 = 0.0;
  sigma44 = 0.0,sigma45 = 0.0,sigma46 = 0.0;
  sigma55 = 0.0,sigma56 = 0.0;
  sigma66 = 0.0;
  
  shellX  = 0.0;
  shellXp = 0.0;
  shellY  = 0.0;
  shellYp = 0.0;
  shellXWidth  = 0.0;
  shellXpWidth = 0.0;
  shellYWidth  = 0.0;
  shellYpWidth = 0.0;

  Rmin = 0.0;
  Rmax = 0.0;

  haloNSigmaXInner      = 0.0;
  haloNSigmaXOuter      = 1e9;
  haloNSigmaYInner      = 0.0;
  haloNSigmaYOuter      = 1e9;
  haloXCutInner         = 0.0;
  haloYCutInner         = 0.0;
  haloXCutOuter         = 1e9;
  haloYCutOuter         = 1e9;
  haloXpCutInner        = 0.0;
  haloYpCutInner        = 0.0;
  haloXpCutOuter        = 1e9;
  haloYpCutOuter        = 1e9;
  haloPSWeightParameter = 1.0;
  haloPSWeightFunction  = "";

  offsetSampleMean = false;
  
  eventGeneratorNEventsSkip = 0;
  eventGeneratorMinX  = -1e6;
  eventGeneratorMaxX  =  1e6;
  eventGeneratorMinY  = -1e6;
  eventGeneratorMaxY  =  1e6;
  eventGeneratorMinZ  = -1e6;
  eventGeneratorMaxZ  =  1e6;
  eventGeneratorMinXp = -1;
  eventGeneratorMaxXp = 1.1; // > 1 for no ambiguity (max value 1)
  eventGeneratorMinYp = -1;
  eventGeneratorMaxYp = 1.1;
  eventGeneratorMinZp = -1;
  eventGeneratorMaxZp = 1.1;
  eventGeneratorMinRp = -1;
  eventGeneratorMaxRp = 1.1;
  eventGeneratorMinT  = -1;
  eventGeneratorMaxT  = 1e50;
  eventGeneratorMinEK = -1;
  eventGeneratorMaxEK = 1e50;
  eventGeneratorParticles = "";
  eventGeneratorWarnSkippedParticles = true;

  polarization1 = 0;
  polarization2 = 0;
  polarization3 = 0;

}
