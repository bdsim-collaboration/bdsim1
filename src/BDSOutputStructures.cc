/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "BDSEnergyCounterHit.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputStructures.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSSamplerHit.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <vector>


BDSOutputStructures::BDSOutputStructures(const BDSGlobalConstants* globals):
  localSamplersInitialised(false)
{
  G4bool storeLinks  = globals->StoreELossLinks();
  G4bool storeLocal  = globals->StoreELossLocal();
  G4bool storeGlobal = globals->StoreELossGlobal();

  beamOutput    = new BDSOutputROOTEventBeam();
  optionsOutput = new BDSOutputROOTEventOptions();
  modelOutput   = new BDSOutputROOTEventModel();
  
  eLoss     = new BDSOutputROOTEventLoss(storeLinks, storeLocal, storeGlobal);
  pFirstHit = new BDSOutputROOTEventLoss(true      ,       true,       false);
  pLastHit  = new BDSOutputROOTEventLoss(true      ,       true,       false);
  tHit      = new BDSOutputROOTEventLoss(false     ,      false,       true);
  traj      = new BDSOutputROOTEventTrajectory();
  evtHistos = new BDSOutputROOTEventHistograms();
  evtInfo   = new BDSOutputROOTEventInfo();
  runHistos = new BDSOutputROOTEventHistograms();
  runInfo   = new BDSOutputROOTEventRunInfo();

#ifndef __ROOTDOUBLE__
  primary = new BDSOutputROOTEventSampler<float>("Primary");
#else
  primary = new BDSOutputROOTEventSampler<double>("Primary");
#endif
  samplerTrees.push_back(primary);
  samplerNames.push_back("Primary");
}

BDSOutputStructures::~BDSOutputStructures()
{
  delete beamOutput;
  delete optionsOutput;
  delete modelOutput;
  delete primary;
  delete eLoss;
  delete pFirstHit;
  delete pLastHit;
  delete tHit;
  delete traj;
  delete evtHistos;
  delete evtInfo;
  delete runHistos;
  delete runInfo;
}

void BDSOutputStructures::Create1DHistogram(G4String name, G4String title,
					    G4int nbins, G4double xmin, G4double xmax)
{
  evtHistos->Create1DHistogram(name, title, nbins, xmin, xmax);
  runHistos->Create1DHistogram(name, title, nbins, xmin, xmax);
}

void BDSOutputStructures::Create1DHistogram(G4String name, G4String title,
					    std::vector<double>& edges)
{
  evtHistos->Create1DHistogram(name,title,edges);
  runHistos->Create1DHistogram(name,title,edges);
}

void BDSOutputStructures::InitialiseSamplers()
{
  if (!localSamplersInitialised)
    {
      localSamplersInitialised = true;
      for (auto const samplerName : BDSSamplerRegistry::Instance()->GetUniqueNames())
        {// create sampler structure
#ifndef __ROOTDOUBLE__
	  BDSOutputROOTEventSampler<float>*  res = new BDSOutputROOTEventSampler<float>(samplerName);
#else
	  BDSOutputROOTEventSampler<double>* res = new BDSOutputROOTEventSampler<double>(samplerName);
#endif
	  samplerTrees.push_back(res);
	  samplerNames.push_back(samplerName);
        }
    }
}

void BDSOutputStructures::ClearStructuresModel()
{
  modelOutput->Flush();
}

void BDSOutputStructures::ClearStructuresBeam()
{
  *beamOutput = BDSOutputROOTEventBeam(); // default
}

void BDSOutputStructures::ClearStructuresOptions()
{
  *optionsOutput = BDSOutputROOTEventOptions(); // default
}

void BDSOutputStructures::ClearStructuresEventLevel()
{
  // loop over sampler map and clear vectors
  for(auto i= samplerTrees.begin() ; i != samplerTrees.end() ;++i)
    {(*i)->Flush();}
  eLoss->Flush();
  pFirstHit->Flush();
  pLastHit->Flush();
  tHit->Flush();
  traj->Flush();
  evtHistos->Flush();
}

void BDSOutputStructures::ClearStructuresRunLevel()
{
  runInfo->Flush();
}
