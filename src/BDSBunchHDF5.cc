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

#include "BDSBunchHDF5.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPlasmaHDF5.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "G4ThreeVector.hh"

BDSBunchHDF5::BDSBunchHDF5():
        BDSBunchFileBased("hdf5"),
        matchDistrFileLength(false),
        iRay(0),
        nRays(0),
        mass(1.0)
{;}

BDSBunchHDF5::~BDSBunchHDF5()
{
  HDF5Data.clear();
}

void BDSBunchHDF5::LoadHDF5Data()
{
  G4cout << "BDSBunchHDF5::LoadHDF5File> opening " << fileName << G4endl;
  auto HDF5 = new BDSPlasmaHDF5;
  HDF5->LoadHDF5(fileName);

  std::vector<double> X = HDF5->GetX();
  std::vector<double> Y = HDF5->GetY();
  std::vector<double> Z = HDF5->GetZ();
  std::vector<double> PX = HDF5->GetPX();
  std::vector<double> PY = HDF5->GetPY();
  std::vector<double> PZ = HDF5->GetPZ();

  for (int i=0; i<X.size(); i++)
  {
    HDF5Data.emplace_back(std::array<double, 6>{X[i], Y[i], Z[i], PX[i], PY[i], PZ[i]});
  }

  // set number of available rays in options
  nRays = (G4int)HDF5Data.size();
}

void BDSBunchHDF5::SetOptions(const BDSParticleDefinition* beamParticle,
                              const GMAD::Beam& beam,
                              const BDSBunchType& distrType,
                              G4Transform3D beamlineTransformIn,
                              const G4double beamlineSIn)
{
  BDSBunchFileBased::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  matchDistrFileLength = G4bool(beam.distrFileMatchLength);
  mass = beamParticle->Mass();
  fileName = BDS::GetFullPath(beam.distrFile);
}

void BDSBunchHDF5::Initialise()
{
  LoadHDF5Data();

  auto g = BDSGlobalConstants::Instance();
  G4bool nGenerateHasBeenSet = g->NGenerateSet();
  G4int nEventsPerLoop = nRays;
  nEventsInFile = nRays;
  G4int nAvailable = nEventsPerLoop * distrFileLoopNTimes;
  G4int nGenerate = g->NGenerate();
  if (matchDistrFileLength)
  {
    if (!nGenerateHasBeenSet)
    {
      g->SetNumberToGenerate(nAvailable);
      G4cout << "BDSBunchHDF5::Initialise> distrFileMatchLength is true -> simulating " << nRays << " events";
      if (distrFileLoopNTimes > 1)
      {G4cout << " " << distrFileLoopNTimes << " times";}
      G4cout << G4endl;
      if (g->Recreate())
      {// have to do this now before the primary generator action is called already in the run
        G4int nEventsRemaining = nAvailable - g->StartFromEvent();
        g->SetNumberToGenerate(nEventsRemaining);
        G4cout << "BDSBunchHDF5::Initialise> distrFileMatchLength + recreation -> simulate the "
               << nEventsRemaining << " lines left given startFromEvent including possible looping" << G4endl;
      }
    }
    else
    {// e.g. if recreating a lower number of events - match is on; but ngenerate is lower - must obey
      G4cout << "BDSBunchHDF5::Initialise> matchDistrFileLength has been requested "
             << "but ngenerate has been specified -> use ngenerate" << G4endl;
      // note we don't need to take care of a recreation offset - this is done later in primary generator action
      if (nGenerate > nAvailable)
      {
        G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") is greater than the number of valid lines (";
        msg += std::to_string(nRays) + ") and distrFileMatchLength is on.\nChange ngenerate to <= # lines";
        msg += ", or don't specifcy ngenerate.\n";
        msg += "This includes nlinesSkip.";
        throw BDSException("BDSBunchHDF5::Initialise>", msg);
      }
    }
  }
  else
  {
    if ( (nGenerate > nRays) && !distrFileLoop )
    {
      G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") is greater than the number of inrays (";
      msg += std::to_string(nRays) + ") but distrFileLoop is false in the beam command";
      throw BDSException(__METHOD_NAME__, msg);
    }
  }
}

BDSParticleCoordsFull BDSBunchHDF5::GetNextParticleLocal()
{
  if ( iRay == nRays) // so that we're safe to still read the last entry
  {
    if (distrFileLoop)
    {
      iRay = 0;
      G4cout << __METHOD_NAME__ << "End of file reached. Returning to beginning of file." << G4endl;
    }
    else
    {throw BDSException(__METHOD_NAME__, "unable to read another event as file finished");}
  }

  G4double x  = HDF5Data[iRay][0] * CLHEP::m + X0;
  G4double y  = HDF5Data[iRay][1] * CLHEP::m + Y0;
  G4double z  = HDF5Data[iRay][2] * CLHEP::m + Z0;
  G4ThreeVector position(x, y, z);

  G4ThreeVector momentumND(HDF5Data[iRay][3], HDF5Data[iRay][4], HDF5Data[iRay][5]);
  G4ThreeVector momentumSI = momentumND*CLHEP::joule*CLHEP::c_light*1e6;

  G4double p = momentumSI.mag();

  G4double xp = momentumSI.x()/momentumSI.z() + Xp0;
  G4double yp = momentumSI.y()/momentumSI.z() + Yp0;
  G4double zp = CalculateZp(xp,yp,Zp0);

  G4double t  = (z-Z0)*CLHEP::m / CLHEP::c_light + T0 * CLHEP::s;

  G4double E  = std::sqrt(std::pow(p,2)+std::pow(mass,2));

  BDSParticleCoordsFull result(x,y,z,xp,yp,zp,t,S0+z,E,/*weight=*/1.0);

  iRay++;

  return result;
}

void BDSBunchHDF5::RecreateAdvanceToEvent(G4int eventOffset)
{
  G4int nAvailable       = nRays * distrFileLoopNTimes;
  G4int nEventsRemaining = nAvailable - eventOffset;
  if (eventOffset >= nRays)
  {
    if (distrFileLoop)
    {
      G4int nToRoll = eventOffset % nRays;
      eventOffset = nToRoll;
    }
    else
    {
      G4String msg = "eventOffset (" + std::to_string(eventOffset);
      msg += ") is greater than the number of inrays in the PTC file";
      throw BDSException(__METHOD_NAME__, msg);
    }
  }

  iRay = eventOffset;
  G4int nGenerate = BDSGlobalConstants::Instance()->NGenerate();
  if (nGenerate > nEventsRemaining && !distrFileLoop)
  {
    G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") requested in recreate mode is greater than number\n";
    msg += "of remaining valid lines in file (" + std::to_string(nEventsRemaining) + ") and distrFileLoop is turned off.";
    throw BDSException("BDSBunchHDF5>", msg);
  }
  // note we cannot update ngenerate here as we're already being called from the primary
  // generator action in the start of the event after BeamOn(nEvents) has been called
  // therefore this adjustment for recreation + match is done earlier in this class
}