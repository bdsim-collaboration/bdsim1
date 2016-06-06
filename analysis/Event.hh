#ifndef EVENT_H
#define EVENT_H

#include <vector>

#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"

#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"

/**
 * @brief Event loader.
 * 
 * @author Stewart Boogert
 */

class Event
{
public:
  Event();
  void CommonCtor();
  virtual ~Event();

  /// @{ Accessor
#ifdef __ROOTDOUBLE__  
  BDSOutputROOTEventSampler<double>* GetPrimaries() {return primaries;}
#else
  BDSOutputROOTEventSampler<float>*  GetPrimaries() {return primaries;}
#endif
  BDSOutputROOTEventLoss*            GetLoss()             {return eloss;}
  BDSOutputROOTEventLoss*            GetPrimaryFirstHit()  {return primaryFirstHit;}
  BDSOutputROOTEventLoss*            GetPrimaryLastHit()   {return primaryLastHit;}
  BDSOutputROOTEventLoss*            GetTunnelHit()        {return tunnelHit;}
  BDSOutputROOTEventTrajectory*      GetTrajectory()       {return trajectory;}
  BDSOutputROOTEventHistograms*      GetHistograms()       {return histos;}
  /// @}

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TChain *, std::vector<std::string>&);

  /// @{ Local variable ROOT data is mapped to.
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>              *primaries;
#else
  BDSOutputROOTEventSampler<float>               *primaries;
#endif
  BDSOutputROOTEventLoss                         *eloss;
  BDSOutputROOTEventLoss                         *primaryFirstHit;
  BDSOutputROOTEventLoss                         *primaryLastHit;
  BDSOutputROOTEventLoss                         *tunnelHit;
  BDSOutputROOTEventTrajectory                   *trajectory;
#ifdef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<double>*> samplers;
#else
  std::vector<BDSOutputROOTEventSampler<float>*>  samplers;
#endif
  BDSOutputROOTEventHistograms                   *histos;
  BDSOutputROOTEventInfo                         *info;
  /// @}

private:
  ClassDef(Event,1);
};

#endif
