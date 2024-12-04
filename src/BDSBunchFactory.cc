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
#include "BDSBunch.hh"
#include "BDSBunchBox.hh"
#include "BDSBunchCircle.hh"
#include "BDSBunchComposite.hh"
#include "BDSBunchCompositeSDE.hh"
#include "BDSBunchEShell.hh"
#include "BDSBunchEventGenerator.hh"
#include "BDSBunchFactory.hh"
#include "BDSBunchHalo.hh"
#include "BDSBunchHaloFlatSigma.hh"
#include "BDSBunchHDF5.hh"
#include "BDSBunchPtc.hh"
#include "BDSBunchRing.hh"
#include "BDSBunchSphere.hh"
#include "BDSBunchSigmaMatrix.hh"
#include "BDSBunchSixTrack.hh"
#include "BDSBunchSquare.hh"
#include "BDSBunchTwiss.hh"
#include "BDSBunchType.hh"
#include "BDSBunchUserFile.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include <utility>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSBunch* BDSBunchFactory::CreateBunch(const BDSParticleDefinition* beamParticle,
                                       const GMAD::Beam&            beam,
                                       const G4Transform3D&         beamlineTransform,
                                       G4double                     beamlineS,
                                       G4bool                       generatePrimariesOnlyIn)  
{
  G4String distrName = G4String(beam.distrType);
  if (BDS::StrContains(distrName, ":")) // must be eventgeneratorfile:subtype
    {
      std::pair<G4String, G4String> ba = BDS::SplitOnColon(distrName);
      distrName = ba.first; // overwrite with just first bit
      // we can't generate primaries only with event generator file distribution as this
      // only works in BDSPrimaryGeneratorAction at run time - it's not really a bunch
      if (generatePrimariesOnlyIn)
        {throw BDSException(__METHOD_NAME__, "eventgeneratorfile will not work with generator primaries only.");}
    }

  // This will exit if no correct bunch type found.
  BDSBunchType distrType = BDS::DetermineBunchType(distrName);

  return CreateBunch(beamParticle, distrType, beam, beamlineTransform, beamlineS, generatePrimariesOnlyIn);
}

BDSBunch* BDSBunchFactory::CreateBunch(const BDSParticleDefinition* beamParticle,
                                       BDSBunchType                 distrType,
                                       const GMAD::Beam&            beam,
                                       const G4Transform3D&         beamlineTransform,
                                       G4double                     beamlineS,
                                       G4bool                       generatePrimariesOnlyIn)
{ 
  BDSBunch* bdsBunch = nullptr;

  switch (distrType.underlying())
    {
    case BDSBunchType::reference:
      {bdsBunch = new BDSBunch(); break;}
    case BDSBunchType::gaussmatrix:
    case BDSBunchType::gauss:
      {bdsBunch = new BDSBunchSigmaMatrix(); break;}
    case BDSBunchType::gausstwiss:
      {bdsBunch = new BDSBunchTwiss(); break;}
    case BDSBunchType::circle:
      {bdsBunch = new BDSBunchCircle(); break;}
    case BDSBunchType::square:
      {bdsBunch = new BDSBunchSquare(); break;}
    case BDSBunchType::ring:
      {bdsBunch = new BDSBunchRing(); break;}
    case BDSBunchType::eshell:
      {bdsBunch = new BDSBunchEShell(); break;}
    case BDSBunchType::halo:
      {bdsBunch = new BDSBunchHalo(); break;}
    case BDSBunchType::composite:
      {bdsBunch = new BDSBunchComposite(); break;}
    case BDSBunchType::compositesde:
      {bdsBunch = new BDSBunchCompositeSDE(); break;}
    case BDSBunchType::userfile:
      {
        G4String distrFile = G4String(beam.distrFile);
        if(distrFile.rfind("gz") != std::string::npos)    
#ifdef USE_GZSTREAM
          {
            if (distrFile.find("tar") != std::string::npos)
              {throw BDSException(__METHOD_NAME__, "Cannot load tar file -> only gzip compressed");}
            bdsBunch = new BDSBunchUserFile<igzstream>();}
#else
        {
          G4String message = beam.distrFile + " is a compressed file but BDSIM is compiled without GZIP.";
          throw BDSException(__METHOD_NAME__, message);
        }
#endif
        else
          {bdsBunch = new BDSBunchUserFile<std::ifstream>();}
        break;
      }
    case BDSBunchType::ptc:
      {bdsBunch = new BDSBunchPtc(); break;}
    case BDSBunchType::sixtrack:
      {bdsBunch = new BDSBunchSixTrack(); break;}
    case BDSBunchType::sphere:
      {bdsBunch = new BDSBunchSphere(); break;}
    case BDSBunchType::eventgeneratorfile:
    case BDSBunchType::bdsimsampler:
      {bdsBunch = new BDSBunchEventGenerator(); break;}
    case BDSBunchType::box:
      {bdsBunch = new BDSBunchBox(); break;}
    case BDSBunchType::halosigma:
      {bdsBunch = new BDSBunchHaloFlatSigma(); break;}
    case BDSBunchType::hdf5:
      {bdsBunch = new BDSBunchHDF5(); break;}
    default:
      {bdsBunch = new BDSBunch(); break;}
    }

  bdsBunch->SetOptions(beamParticle, beam, distrType, beamlineTransform, beamlineS);
  bdsBunch->SetGeneratePrimariesOnly(generatePrimariesOnlyIn);
  bdsBunch->CheckParameters();
  bdsBunch->Initialise();

  return bdsBunch;
}
