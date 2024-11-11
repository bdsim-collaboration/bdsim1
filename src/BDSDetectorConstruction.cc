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
#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSApertureInfo.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineBLMBuilder.hh"
#include "BDSBeamlineEndPieceBuilder.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlineIntegral.hh"
#include "BDSBeamlinePlacementBuilder.hh"
#include "BDSBeamlineSet.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBLM.hh"
#include "BDSBLMRegistry.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"
#include "BDSComponentFactory.hh"
#include "BDSComponentFactoryUser.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldObjects.hh"
#include "BDSFieldQuery.hh"
#include "BDSFieldQueryInfo.hh"
#include "BDSFieldLoaderQueryPoints.hh"
#include "BDSGap.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHistBinMapper.hh"
#include "BDSIntegratorSet.hh"
#include "BDSLine.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSRegion.hh"
#include "BDSSamplerInfo.hh"
#include "BDSSamplerType.hh"
#include "BDSScorerFactory.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerMeshInfo.hh"
#include "BDSScoringMeshBox.hh"
#include "BDSScoringMeshCylinder.hh"
#include "BDSSDEnergyDeposition.hh"
#include "BDSSDManager.hh"
#include "BDSSDType.hh"
#include "BDSSurvey.hh"
#include "BDSTeleporter.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSTunnelBuilder.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/blmplacement.h"
#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/options.h"
#include "parser/physicsbiasing.h"
#include "parser/placement.h"
#include "parser/samplerplacement.h"
#include "parser/scorermesh.h"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4ProductionCuts.hh"
#include "G4PVPlacement.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4Region.hh"
#include "G4ScoringManager.hh"
#include "G4String.hh"
#include "G4Transform3D.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#if G4VERSION_NUMBER > 1039
#include "G4ChannelingOptrMultiParticleChangeCrossSection.hh"
#endif
#if G4VERSION_NUMBER > 1109
#include "G4HadronicParameters.hh"
#endif

#ifdef BDSCHECKUSERLIMITS
#include "G4UserLimits.hh"
#endif

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


BDSDetectorConstruction::BDSDetectorConstruction(BDSComponentFactoryUser* userComponentFactoryIn):
  placementBL(nullptr),
  designParticle(nullptr),
  userComponentFactory(userComponentFactoryIn),
  nSamplers(0),
  buildPlacementFieldsWorld(false),
  worldLogicalVolume(nullptr)
{
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verbose       = globals->Verbose();
  checkOverlaps = globals->CheckOverlaps();
  circular      = globals->Circular();

  if (globals->RestoreFTPFDiffractionForAGreater10())
#if G4VERSION_NUMBER > 1109
    {
      G4cout << __METHOD_NAME__ << "restoring diffraction for target / projectiles with A > 10 in the FTFP hadronic model (even if not used)" << G4endl;
      G4HadronicParameters::Instance()->SetEnableDiffDissociationForBGreater10(true);
    }
#else
    {
      if (globals->RestoreFTPFDiffractionForAGreater10Set()) // shouldn't warn about default being on
        {BDS::Warning(__METHOD_NAME__, "\"restoreFTPFDiffractionForAGreater10\" is only available for Geant4 v11.1 and later");}
    }
#endif
  
  BDSTrajectoryPoint::dEThresholdForScattering = globals->DEThresholdForScattering();
  
  // instantiate the accelerator model holding class
  acceleratorModel = BDSAcceleratorModel::Instance();
  canSampleAngledFaces = true;
  BDSIntegratorSetType integratorSetType = globals->IntegratorSet();
  if (   (integratorSetType == BDSIntegratorSetType::bdsimtwo)
      || (integratorSetType == BDSIntegratorSetType::geant4)
#if G4VERSION_NUMBER > 1039
      || (integratorSetType == BDSIntegratorSetType::geant4dp)
#endif
      )
    { // set to be value of option, default is false.
      canSampleAngledFaces = globals->SampleElementsWithPoleface();
    }

  UpdateSamplerDiameterAndCountSamplers();
  PrepareExtraSamplerSDs();
  CountPlacementFields();
}

void BDSDetectorConstruction::UpdateSamplerDiameterAndCountSamplers()
{
  nSamplers = 0;
  auto beamline = BDSParser::Instance()->GetBeamline(); // main beam line
  G4double maxBendingRatio = 1e-9;
  for (const auto& blElement : beamline)
    {
      // count number of samplers
      auto st = BDS::DetermineSamplerType(blElement.samplerType);
      if (st != BDSSamplerType::none)
        {nSamplers++;}
      
      G4double length = blElement.l;
      G4double angle  = blElement.angle;
      if (!BDS::IsFinite(length))
        {continue;} // avoid divide by 0
      G4double ratio  = angle / length;
      maxBendingRatio = std::max(maxBendingRatio, ratio);
    }

  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  G4double curvilinearRadius = 0.5*globals->CurvilinearDiameter();
  G4double tolerance = 0.9; // 10% tolerance -> factor of 0.9
  if (maxBendingRatio > 0.4*tolerance) // max ratio for a 2.5m sampler diameter
    {
      G4double curvilinearRadiusBends = (tolerance / maxBendingRatio)*CLHEP::m;
      if (curvilinearRadiusBends < curvilinearRadius)
        {
          G4cout << __METHOD_NAME__ << "Reducing curvilinear diameter from " << 2*curvilinearRadius / CLHEP::m
                 << "m to " << 2*curvilinearRadiusBends / CLHEP::m << "m" << G4endl;
          globals->SetCurvilinearDiameter(2*curvilinearRadiusBends);
          globals->SetCurvilinearDiameterShrunkForBends();
        }
      G4double sd = globals->SamplerDiameter();
      if (curvilinearRadius*2 < sd)
        {
          G4cout << __METHOD_NAME__ << "Reducing sampler diameter from " << sd / CLHEP::m << "m to the same" << G4endl;
          globals->SetSamplerDiameter(2*curvilinearRadius);
        }
    }

  // add number of sampler placements to count of samplers
  nSamplers += (G4int)BDSParser::Instance()->GetSamplerPlacements().size();
}

void BDSDetectorConstruction::PrepareExtraSamplerSDs()
{
  const auto& samplerFilterIDtoPDGSet = BDSParser::Instance()->GetSamplerFilterIDToSet();
  BDSSDManager::Instance()->ConstructSamplerSDsForParticleSets(samplerFilterIDtoPDGSet);
}

void BDSDetectorConstruction::CountPlacementFields()
{
  G4int nFields = 0;
  const auto& placements = BDSParser::Instance()->GetPlacements();
  for (const auto& placement : placements)
    {// here we assume if a bdsim element is used at all that it's active even though it may not be
      if (!placement.fieldAll.empty() || !placement.bdsimElement.empty())
      {nFields++;}
    }
  buildPlacementFieldsWorld = nFields > 0;
}

G4VPhysicalVolume* BDSDetectorConstruction::Construct()
{
  if (verbose || debug)
    {G4cout << __METHOD_NAME__ << "starting accelerator geometry construction\n" << G4endl;}
  
  // construct all parser defined regions
  InitialiseRegions();

  // construct all parser defined aperture objects
  InitialiseApertures();
  
  // construct the main beam line and any other secondary beam lines
  BuildBeamlines();

  // construct placement geometry from parser
  BDSBeamline* mainBeamLine = BDSAcceleratorModel::Instance()->BeamlineSetMain().massWorld;
  auto componentFactory = new BDSComponentFactory(userComponentFactory, false); // false for printing out integrator set again
  placementBL = BDS::BuildPlacementGeometry(BDSParser::Instance()->GetPlacements(),
                                            mainBeamLine,
                                            componentFactory,
                                            designParticle);
  BDSAcceleratorModel::Instance()->RegisterPlacementBeamline(placementBL); // Acc model owns it
  delete componentFactory;

  BDSBeamline* blms = BDS::BuildBLMs(BDSParser::Instance()->GetBLMs(), mainBeamLine);
  if (blms)
    {BDSAcceleratorModel::Instance()->RegisterBLMs(blms);} // Acc model owns it
  
  // build the tunnel and supports
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {BuildTunnel();}

  // build world and calculate coordinates
  auto worldPV = BuildWorld();

  // placement procedure - put everything in the world
  ComponentPlacement(worldPV);
  
  if (verbose || debug)
    {G4cout << __METHOD_NAME__ << "detector Construction done" << G4endl;}
  
  fieldQueries = BDSDetectorConstruction::PrepareFieldQueries(mainBeamLine);
  if (BDSGlobalConstants::Instance()->VerboseSensitivity())
    {VerboseSensitivity();}

#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "printing material table" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
  if (verbose || debug) {G4cout << "Finished listing materials, returning physiWorld" << G4endl;} 
#endif
#ifdef BDSCHECKUSERLIMITS
  PrintUserLimitsSummary(worldPV);
#endif
  return worldPV;
}

BDSDetectorConstruction::~BDSDetectorConstruction()
{
#if G4VERSION_NUMBER > 1009
  // delete bias objects
  for (auto i : biasObjects)
    {delete i;}
#endif
  for (auto q : fieldQueries)
    {delete q;}
}

void BDSDetectorConstruction::InitialiseRegions()
{
  BDSRegion* defaultRegion = new BDSRegion("default");
  for (const GMAD::Region& r : BDSParser::Instance()->GetRegions())
    {
      BDSRegion* reg = new BDSRegion(r, defaultRegion);
      G4cout << "New region defined: " << G4endl << *reg << G4endl;
      acceleratorModel->RegisterRegion(reg);
    }
  delete defaultRegion;
}

void BDSDetectorConstruction::InitialiseApertures()
{
  std::map<G4String, BDSApertureInfo*> apertures;
  for (const GMAD::Aperture& a : BDSParser::Instance()->GetApertures())
    {
      BDSApertureInfo* ap = new BDSApertureInfo(a.apertureType,
						a.aper1 * CLHEP::m,
						a.aper2 * CLHEP::m,
						a.aper3 * CLHEP::m,
						a.aper4 * CLHEP::m,
						a.name);
      apertures[a.name] = ap;
    }
  acceleratorModel->RegisterApertures(apertures);
}

void BDSDetectorConstruction::BuildBeamlines()
{
  // build main beam line
  if (verbose || debug)
    {G4cout << "parsing the beamline element list..."<< G4endl;}
  auto g = BDSGlobalConstants::Instance();
  G4Transform3D initialTransform = g->BeamlineTransform();
  
  BDSBeamlineIntegral startingPoint(*designParticle, 0, g->BeamlineS(), g->IntegrateKineticEnergyAlongBeamline());
  BDSBeamlineIntegral* finishingPoint = new BDSBeamlineIntegral(startingPoint);
  
  BDSBeamlineSet mainBeamline = BuildBeamline(BDSParser::Instance()->GetBeamline(),
					      "main beam line",
                startingPoint,
                finishingPoint,
					      initialTransform,
					      circular);
  
  if (finishingPoint->changeOfEnergyEncountered)
    {G4cout << "Design particle properties at end of beam line: " << G4endl << finishingPoint->designParticle;}

  // TODO - don't need this finish integral for now - for multiple beamlines may need ot pass it off
  delete finishingPoint;
#ifdef BDSDEBUG
  G4cout << "Registry size "
         << BDSAcceleratorComponentRegistry::Instance()->size() << G4endl;
  G4cout << "Parser beam line size "
         << BDSParser::Instance()->GetBeamline().size() << G4endl;
  BDSAcceleratorComponentRegistry::Instance()->PrintNumberOfEachType();
#endif

  // print warning if beam line is approximately circular but flag isn't specified
  if (!circular && mainBeamline.massWorld)
    {
      if (mainBeamline.massWorld->ElementAnglesSumToCircle())
        {BDS::Warning("Total sum of all element angles is approximately 2*pi but the circular option was not specified, this simulation may run indefinitely");}
    }
  
  // register the beam line in the holder class for the full model
  acceleratorModel->RegisterBeamlineSetMain(mainBeamline);

  // build secondary beam lines
  // loop over placements and check if any are beam lines (have sequences specified)
  auto placements = BDSParser::Instance()->GetPlacements();
  for (const auto& placement : placements)
    {
      if (placement.sequence.empty())
        {continue;} // no sequence specified -> just a placement
      auto parserLine = BDSParser::Instance()->GetSequence(placement.sequence);

      // determine offset in world for extra beam line
      const BDSBeamline* mbl = mainBeamline.massWorld;
      // TODO - so by default if placement.s is finite, it'll be made w.r.t. the main beam line
      // but this could be any beam line in future if we find the right beam line to pass in.
      G4Transform3D startTransform = CreatePlacementTransform(placement, mbl);
      G4double      startS         = mbl ? mbl->back()->GetSPositionEnd() : 0;
      
      /// TODO - we use the initial design particle... if this splits off the main beam line it
      /// should be the particle at that point
      BDSBeamlineIntegral thisBeamlineStartingPoint(startingPoint.designParticle, 0, startS, g->IntegrateKineticEnergyAlongBeamline());
      BDSBeamlineIntegral* thisBeamlineFinishingPoint = new BDSBeamlineIntegral(thisBeamlineStartingPoint);

      // aux beam line must be non-circular by definition to branch off of beam line (for now)
      // TODO - the naming convention here is repeated in BDSParallelWorldInfo which is registered
      // beforehand separately - fix by making the information originate in one place despite
      // the parallel world instantiated first before Construct() in this class is called.
      G4String beamlineName = placement.name + "_" + placement.sequence;
      BDSBeamlineSet extraBeamline = BuildBeamline(parserLine,
                                                   beamlineName,
                                       thisBeamlineStartingPoint,
                                       thisBeamlineFinishingPoint,
						                           startTransform,
						                           false, // circular
						                           true); // is placement
      // TODO - make use of this finishing transform
      delete thisBeamlineFinishingPoint;
      acceleratorModel->RegisterBeamlineSetExtra(beamlineName, extraBeamline);
    }
}

BDSSamplerInfo* BDSDetectorConstruction::BuildSamplerInfo(const GMAD::Element* element)
{
  BDSSamplerType sType = BDS::DetermineSamplerType(element->samplerType);
  if (sType == BDSSamplerType::none)
    {return nullptr;}
  BDSSamplerInfo* result = new BDSSamplerInfo(element->samplerName,
                                              sType,
                                              (G4int)element->samplerParticleSetID);
  return result;
}

BDSBeamlineSet BDSDetectorConstruction::BuildBeamline(const GMAD::FastList<GMAD::Element>& beamLine,
                                                      const G4String&            name,
                                                      const BDSBeamlineIntegral& startingIntegral,
                                                      BDSBeamlineIntegral*&      integral,
                                                      const G4Transform3D&       initialTransform,
                                                      G4bool                     beamlineIsCircular,
                                                      G4bool                     isPlacementBeamline)
{
  if (beamLine.empty()) // note a line always has a 'line' element first so an empty line will not be 'empty'
    {return BDSBeamlineSet();}
  
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory(userComponentFactory);
  BDSBeamline* massWorld = new BDSBeamline(initialTransform, startingIntegral.arcLength);
    
  if (beamlineIsCircular)
    {
      G4bool unsuitable = UnsuitableFirstElement(beamLine.begin());
      if (unsuitable)
        {
          G4cerr << "The first element in the beam line is unsuitable for a circular "
                 << "model as the first element will " << G4endl << "overlap with the "
                 << "teleporter and terminator - the necessary mechanics for a circular "
                 << "model in Geant4" << G4endl;
          throw BDSException(__METHOD_NAME__, "check construction for circular machine");
        }
    }

  if (beamLine.size() <= 1) // if an empty LINE it still has 1 item in it
    {throw BDSException(__METHOD_NAME__, "BDSIM requires the sequence defined with the use command to have at least one element.");}

  for (auto elementIt = beamLine.begin(); elementIt != beamLine.end(); ++elementIt)
    {
      // find next and previous element, but ignore special elements or thin elements.
      const GMAD::Element* prevElement = nullptr;
      auto prevIt = elementIt;
      while (prevIt != beamLine.begin())
        {
          --prevIt;
          if (prevIt->isSpecial() == false && prevIt->l > BDSGlobalConstants::Instance()->ThinElementLength())
            {
              prevElement = &(*prevIt);
              break;
            }
        }

      const GMAD::Element* nextElement = nullptr;
      auto nextIt = elementIt;
      ++nextIt;
      G4double nextElementInputFace = 0; // get poleface angle for next element whilst testing if next element exists
      while (nextIt != beamLine.end())
        {
          if (nextIt->isSpecial() == false && nextIt->l > BDSGlobalConstants::Instance()->ThinElementLength())
            {
              nextElement = &(*nextIt);
          //rotated entrance face of the next element may modify the exit face of the current element.
          nextElementInputFace = nextElement->e1;
          break;
	    }
	  ++nextIt;
	}
      BDSAcceleratorComponent* temp = theComponentFactory->CreateComponent(&(*elementIt),
									   prevElement,
									   nextElement,
                     *integral);
      if (temp)
        {
          G4bool forceNoSamplerOnThisElement = false;
          if ((!canSampleAngledFaces) && (BDS::IsFinite((*elementIt).e2)))
            {forceNoSamplerOnThisElement = true;}
          if ((!canSampleAngledFaces) && (BDS::IsFinite(nextElementInputFace)))
            {forceNoSamplerOnThisElement = true;}
          if (temp->GetType() == "dump") // don't sample after a dump as there'll be nothing
            {forceNoSamplerOnThisElement = true;}
          BDSSamplerInfo* samplerInfo = forceNoSamplerOnThisElement ? nullptr : BuildSamplerInfo(&(*elementIt));
          BDSTiltOffset* tiltOffset = BDSComponentFactory::CreateTiltOffset(&(*elementIt));
          massWorld->AddComponent(temp, tiltOffset, samplerInfo, integral);
        }
    }

  // Special circular machine bits
  // Add terminator to do ring turn counting logic and kill particles
  // Add teleporter to account for slight ring offset
  if (beamlineIsCircular && !massWorld->empty())
    {
      if (integral->changeOfEnergyEncountered && integral->integrateKineticEnergy)
        {
          G4String msg = "a change in energy was encountered in a circular machine and both\n";
          msg +=         "integrateKineticEnergyAlongBeamline=1 (default is 1) and circular options were used.\n";
          msg +=         "This will be wrong for more than one turn...";
          BDS::Warning(__METHOD_NAME__, msg);
        }
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Circular machine - creating terminator & teleporter" << G4endl;
#endif
      G4double teleporterLength = 0;
      G4Transform3D teleporterTransform = BDS::CalculateTeleporterDelta(massWorld, teleporterLength);

      auto hasBeamPipeInfo = [](BDSBeamlineElement* ble) {return ble->GetBeamPipeInfo() != nullptr;};
      auto firstElementWithBPInfo = std::find_if(massWorld->begin(),  massWorld->end(),  hasBeamPipeInfo);
      auto lastElementWithBPInfo  = std::find_if(massWorld->rbegin(), massWorld->rend(), hasBeamPipeInfo);

      G4double firstbeamPipeMaxExtent = (*firstElementWithBPInfo)->GetBeamPipeInfo()->Extent().MaximumAbsTransverse();
      G4double lastbeamPipeMaxExtent  = (*lastElementWithBPInfo)->GetBeamPipeInfo()->Extent().MaximumAbsTransverse();

      // the extent is a half width, so we double it - also the terminator width.
      G4double teleporterHorizontalWidth = 2 * std::max(firstbeamPipeMaxExtent, lastbeamPipeMaxExtent);
      
      BDSAcceleratorComponent* terminator = theComponentFactory->CreateTerminator(teleporterHorizontalWidth);
      if (terminator)
        {
          terminator->Initialise();
          massWorld->AddComponent(terminator, nullptr, nullptr, integral);
        }
      
      BDSAcceleratorComponent* teleporter = theComponentFactory->CreateTeleporter(teleporterLength,
										  teleporterHorizontalWidth,
										  teleporterTransform);
      if (teleporter)
	{
	  teleporter->Initialise();
	  massWorld->AddComponent(teleporter, nullptr, nullptr, integral);
	}
    }
  
  if (BDSGlobalConstants::Instance()->Survey())
    {
      G4String surveyFileName = BDSGlobalConstants::Instance()->SurveyFileName() + ".dat";
      if (isPlacementBeamline)
        {surveyFileName = BDSGlobalConstants::Instance()->SurveyFileName() + "_" + name + ".dat";}
      BDSSurvey* survey = new BDSSurvey(surveyFileName);
      survey->Write(massWorld);
      delete survey;
    }
  delete theComponentFactory;

  // print summary
  G4cout << __METHOD_NAME__ << "\"" << name << "\" " << G4endl << *massWorld;

  // Build curvilinear geometry w.r.t. beam line.
  BDSCurvilinearBuilder* clBuilder = new BDSCurvilinearBuilder();
  BDSBeamline* clBeamline = clBuilder->BuildCurvilinearBeamLine1To1(massWorld, beamlineIsCircular);
  BDSBeamline* clBridgeBeamline = clBuilder->BuildCurvilinearBridgeBeamLine(clBeamline);
  delete clBuilder;

  // construct beamline of end pieces
  BDSBeamline* endPieces = BDS::BuildEndPieceBeamline(massWorld, circular);
  BDSBeamlineSet beamlineSet;
  beamlineSet.massWorld              = massWorld;
  beamlineSet.curvilinearWorld       = clBeamline;
  beamlineSet.curvilinearBridgeWorld = clBridgeBeamline;
  beamlineSet.endPieces              = endPieces;
  return beamlineSet;
}

void BDSDetectorConstruction::BuildTunnel()
{
  const BDSBeamline* mainBeamLine = acceleratorModel->BeamlineMain();
  BDSBeamline* tunnelBeamline;
  BDSTunnelBuilder* tb = new BDSTunnelBuilder(BDSGlobalConstants::Instance()->TunnelMaxSegmentLength());
  tunnelBeamline = tb->BuildTunnelSections(mainBeamLine);
  delete tb;
  
  acceleratorModel->RegisterTunnelBeamline(tunnelBeamline);
}

G4VPhysicalVolume* BDSDetectorConstruction::BuildWorld()
{
  // shortcut
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();

  // calculate extents of everything we need to place in the world first
  std::vector<BDSExtentGlobal> extents;

  // these beam lines should always exist so are safe to access.
  const auto& blMain = acceleratorModel->BeamlineSetMain();
  blMain.GetExtentGlobals(extents);

  // check optional placement beam line (like vector of placements)
  BDSBeamline* plBeamline = acceleratorModel->PlacementBeamline();
  if (plBeamline) // optional - may be nullptr
    {extents.push_back(plBeamline->GetExtentGlobal());}

  // check tunnel beam line
  BDSBeamline* tunnelBeamline = acceleratorModel->TunnelBeamline();
  if (tunnelBeamline)
    {extents.push_back(tunnelBeamline->GetExtentGlobal());}

  // check extra beam lines
  const auto& extras = BDSAcceleratorModel::Instance()->ExtraBeamlines();
  // extras is a map, so iterator has first and second for key and value
  for (const auto& bl : extras)
    {bl.second.GetExtentGlobals(extents);}

  // inspect any sampler placements and calculate their extent without constructing them
  extents.push_back(CalculateExtentOfSamplerPlacements(blMain.massWorld));

  // inspect any scoring meshes and calculate their extent without constructing them
  extents.push_back(CalculateExtentOfScorerMeshes(blMain.massWorld));

  // Expand to maximum extents of each beam line.
  G4ThreeVector worldR;
  // loop over all extents from all beam lines
  for (const auto& ext : extents)
    {
      for (G4int i = 0; i < 3; i++)
        {worldR[i] = std::max(worldR[i], ext.GetMaximumExtentAbsolute()[i]);} // expand with the maximum
    }
  
  G4String         worldName  = "World";
  G4VSolid*        worldSolid = nullptr;
  G4LogicalVolume* worldLV    = nullptr;

  G4String worldGeometryFile = globals->WorldGeometryFile();
  if (!worldGeometryFile.empty())
    {
      if (globals->WorldMaterialSet())
        {BDS::Warning(__METHOD_NAME__, "conflicting options - world material option specified but material will be taken from world GDML file");}
      G4bool ac = globals->AutoColourWorldGeometryFile();
      
      std::vector<G4String> namedWorldVacuumVolumes = BDS::SplitOnWhiteSpace(globals->WorldVacuumVolumeNames());
      
      BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(worldName,
                                                                                worldGeometryFile,
                                                                                nullptr,
                                                                                ac,
                                                                                0, 0,
                                                                                &namedWorldVacuumVolumes,
                                                                                true,
                                                                                BDSSDType::energydepworldcontents,
                                                                                BDSSDType::energydepvacuum);
      
      // get list of 'material' and 'vacuum' volumes for possible biasing of this geometry
      worldVacuumLogicalVolumes = geom->VacuumVolumes();
      auto allWorldVolumes = geom->GetAllLogicalVolumes();
      allWorldVolumes.erase(geom->GetContainerLogicalVolume());
      for (auto volume : worldVacuumLogicalVolumes)
        {allWorldVolumes.erase(volume);}
      worldContentsLogicalVolumes = allWorldVolumes; // cache volumes for biasing
      
      worldExtent = geom->GetExtent();
      BDSExtentGlobal worldExtentGlobal = BDSExtentGlobal(worldExtent, G4Transform3D());
      G4bool worldContainsAllBeamlines = worldExtentGlobal.Encompasses(extents);
      
      G4cout << "External world geometry: \"" << worldGeometryFile << "\"" << G4endl;
      G4cout << "Loaded world extent: \n" << worldExtent << G4endl;

      // cannot construct world if any beamline extent is greater than the world extents
      if (!worldContainsAllBeamlines)
        {
          G4String message = "Beamlines cannot be constructed, beamline extents are larger than \n";
          message += "the extents of the external world";
          throw BDSException(__METHOD_NAME__, message);
        }

      worldSolid = geom->GetContainerSolid();
      worldLV    = geom->GetContainerLogicalVolume();

      // make the world sensitive to energy deposition with its own unique hits collection
      // this will be a nullptr depending on the options.
      // make world sensitive if importance sampling is needed
      if (globals->StoreELossWorld()
          || globals->StoreELossWorldIntegral()
          || globals->UseImportanceSampling()
          || globals->StoreELossWorldContents()
          || globals->StoreELossWorldContentsIntegral())
        {
          geom->AttachSensitiveDetectors();
          // override the logical volume itself with a specific SD
          worldLV->SetSensitiveDetector(BDSSDManager::Instance()->WorldComplete());
        }  
    }
  else
    {
      // add on margin for constructed world volume
      G4double margin = globals->WorldVolumeMargin();
      margin = std::max(margin, 2*CLHEP::m); // minimum margin of 2m.
      worldR += G4ThreeVector(margin,margin,margin); //add 5m extra in every dimension

      G4cout << __METHOD_NAME__ << "World dimensions: " << worldR / CLHEP::m << " m" << G4endl;

      G4String    worldMaterialName = globals->WorldMaterial();
      G4Material* worldMaterial     = BDSMaterials::Instance()->GetMaterial(worldMaterialName);
      worldExtent = BDSExtent(worldR);
      worldSolid = new G4Box(worldName + "_solid", worldR.x(), worldR.y(), worldR.z());


      worldLV = new G4LogicalVolume(worldSolid,              // solid
                                    worldMaterial,           // material
                                    worldName + "_lv");      // name

      // make the world sensitive to energy deposition with its own unique hits collection
      // note with our world, there are no 'contents' to also consider
      if (globals->StoreELossWorld() || globals->StoreELossWorldIntegral())
        {worldLV->SetSensitiveDetector(BDSSDManager::Instance()->WorldComplete());}
    }

  // visual attributes
  // copy the debug vis attributes but change to force wireframe
  G4VisAttributes* debugWorldVis = new G4VisAttributes(*(globals->ContainerVisAttr()));
  debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(debugWorldVis);
  
  // set limits
  worldLV->SetUserLimits(globals->DefaultUserLimits());

  // place the world
  G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr,           // no rotation
                                                 G4ThreeVector(),   // at (0,0,0)
                                                 worldLV,           // its logical volume
                                                 worldName,         // its name
                                                 nullptr,           // its mother  volume
                                                 false,                     // no boolean operation
                                                 0,                 // copy number
                                                 checkOverlaps);    // overlap checking

  // Register the lv & pvs to our holder class for the model
  acceleratorModel->RegisterWorldPV(worldPV);
  acceleratorModel->RegisterWorldLV(worldLV);
  acceleratorModel->RegisterWorldSolid(worldSolid);

  // Register world PV with our auxiliary navigator so steppers and magnetic
  // fields know which geometry to navigate to get local / global transforms.
  // This is the regular world used as a backup to the curvilinear world.
  BDSAuxiliaryNavigator::AttachWorldVolumeToNavigator(worldPV);
  BDSFieldQuery::AttachWorldVolumeToNavigator(worldPV);
  
  /// Give the pv info registry a heads up that these volumes don't have info (optimisation).
  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterExcludedPV(worldPV);
  
  worldLogicalVolume = worldLV; // set for possible biasing
  return worldPV;
}

void BDSDetectorConstruction::ComponentPlacement(G4VPhysicalVolume* worldPV)
{
  // We musn't place parallel world geometry here - their world is produced by
  // Geant4 at the right time, so we have a separate placement call for them
  BDSBeamlineSet mainBL = BDSAcceleratorModel::Instance()->BeamlineSetMain();
  PlaceBeamlineInWorld(mainBL.massWorld,
                       worldPV, checkOverlaps, true, false, false, false, true); // record pv set to element for output
  PlaceBeamlineInWorld(mainBL.endPieces,
                       worldPV, checkOverlaps);
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {
      PlaceBeamlineInWorld(acceleratorModel->TunnelBeamline(),
                           worldPV, checkOverlaps);
    }
  // No energy counter SD added here as individual placements have that attached
  // during construction time
  PlaceBeamlineInWorld(placementBL, worldPV, checkOverlaps, false, false, false, false, true);

  // Place BLMs. Similarly, no sensitivity set here - done at construction time.
  PlaceBeamlineInWorld(BDSAcceleratorModel::Instance()->BLMsBeamline(),
		       worldPV,
		       checkOverlaps,
		       false,
		       false,
		       false,
		       true); // use incremental copy numbers

  const auto& extras = BDSAcceleratorModel::Instance()->ExtraBeamlines();
  for (auto const& bl : extras)
    {// extras is a map so iterator has first and second for key and value
      // note these are currently not sensitive as there's no CL frame for them
      PlaceBeamlineInWorld(bl.second.massWorld, worldPV, checkOverlaps, false, false, false, false, true);
      PlaceBeamlineInWorld(bl.second.endPieces, worldPV, checkOverlaps);
    }
}

void BDSDetectorConstruction::PlaceBeamlineInWorld(BDSBeamline*          beamline,
						   G4VPhysicalVolume*    containerPV,
						   G4bool                checkOverlaps,
						   G4bool                setRegions,
						   G4bool                registerInfo,
						   G4bool                useCLPlacementTransform,
						   G4bool                useIncrementalCopyNumbers,
                                                   G4bool                registerPlacementNamesForOutput)
{
  if (!beamline)
    {return;}

  G4int i = 0;
  for (auto element : *beamline)
    {
      // Do nothing for gap element
      if (dynamic_cast<BDSGap*>(element->GetAcceleratorComponent()))
       {continue;}

      if (setRegions)
	{
	  auto accComp = element->GetAcceleratorComponent();
	  const G4String regionName = accComp->GetRegion();
	  if (!regionName.empty()) // ie string is defined so we should attach region
	    {
	      G4Region* region = BDSAcceleratorModel::Instance()->Region(regionName);
	      auto contLV = accComp->GetContainerLogicalVolume();
	      contLV->SetRegion(region);
	      region->AddRootLogicalVolume(contLV);
	    }
	}

      // setup the sensitivity
      element->GetAcceleratorComponent()->AttachSensitiveDetectors();
      
      // make the placement
      G4int copyNumber = useIncrementalCopyNumbers ? i : element->GetCopyNo();
      G4String placementName = element->GetPlacementName() + "_pv";
      std::set<G4VPhysicalVolume*> pvs = element->PlaceElement(placementName, containerPV, useCLPlacementTransform,
                                                               copyNumber, checkOverlaps);
      
      if (registerInfo)
        {
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(element->GetName(),
								     placementName,
								     element->GetSPositionMiddle(),
								     element->GetIndex(),
								     beamline);
	  
	  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(pvs, theinfo, true);
        }
      if (registerPlacementNamesForOutput)
	{BDSPhysicalVolumeInfoRegistry::Instance()->RegisterPVsForOutput(element, pvs);}
      i++; // for incremental copy numbers
    }
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::Placement& placement,
								const BDSBeamline*     beamLine,
								G4double*              S,
								BDSExtent*             placementExtent,
								const G4String&        objectTypeForErrorMsg)
{
  G4Transform3D result;

  // 3 scenarios
  // 1) global placement X,Y,Z + rotation
  // 2) w.r.t. beam line placement x,y,S + rotation
  // 3) w.r.t. element in beam line placement elementName + x,y,s + rotation
  
  // in all cases, need the rotation
  G4RotationMatrix rm = G4RotationMatrix();
  if (placement.axisAngle)
    {
      G4ThreeVector axis = G4ThreeVector(placement.axisX,
					 placement.axisY,
					 placement.axisZ);
      rm = G4RotationMatrix(axis, placement.angle*CLHEP::rad);
    }
  else
    {
      if (BDS::IsFinite(placement.phi)   ||
	  BDS::IsFinite(placement.theta) ||
	  BDS::IsFinite(placement.psi))
	{// only build if finite
	  CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(placement.phi*CLHEP::rad,
							    placement.theta*CLHEP::rad,
							    placement.psi*CLHEP::rad);
	  rm = G4RotationMatrix(ang);
	}
    } 

  // create a transform w.r.t. the beam line if s is finite and it's not w.r.t a
  // particular element. If it's w.r.t. a particular element, treat s as local curvilinear
  // s and use as local 'z' in the transform.
  if (!placement.referenceElement.empty())
    {// scenario 3
      if (!beamLine)
        {throw BDSException(__METHOD_NAME__, "no valid beam line yet for " + objectTypeForErrorMsg + " w.r.t. a beam line.");}
      const BDSBeamlineElement* element = beamLine->GetElement(placement.referenceElement,
							 placement.referenceElementNumber);
      if (!element)
	{
	  G4cerr << __METHOD_NAME__ << "No element named \"" << placement.referenceElement << "\" (instance #"
		 << placement.referenceElementNumber << ") in " << objectTypeForErrorMsg << " \""
		 << placement.name << "\"" << G4endl;
	  G4cout << "Note, this may be because the element is a bend and split into " << G4endl;
	  G4cout << "multiple sections with unique names. Run the visualiser to get " << G4endl;
	  G4cout << "the name of the segment, or place w.r.t. the element before / after." << G4endl;
	  throw BDSException("Error in "+objectTypeForErrorMsg+".");
	}
      // in this case we should use s for longitudinal offset - warn user if mistakenly using z
      if (BDS::IsFinite(placement.z))
	{
	  G4String message = objectTypeForErrorMsg + " \"" + placement.name + "\" is placed using";
	  message += " a referenceElement but the z offset is\n non zero. Note, s should be used to";
	  message += " offset the placement in this case and z will\n have no effect.";
	  BDS::Warning(message);
	}
      G4double sCoordinate = element->GetSPositionMiddle(); // start from middle of element
      sCoordinate += placement.s * CLHEP::m; // add on (what's considered) 'local' s from the placement
      if (S)
	{*S = sCoordinate;}

      G4ThreeVector offset = G4ThreeVector();
      if (placementExtent)
      	{offset = SideToLocalOffset(placement, beamLine, *placementExtent);}

      G4Transform3D beamlinePart = beamLine->GetGlobalEuclideanTransform(sCoordinate,
									 placement.x * CLHEP::m + offset.x(),
									 placement.y * CLHEP::m + offset.y());
      G4Transform3D localRotation(rm, G4ThreeVector());
      result = beamlinePart * localRotation;      
    }
  else if (BDS::IsFinite(placement.s))
    {// scenario 2
      if (!beamLine)
        {throw BDSException(__METHOD_NAME__, "no valid beam line yet placement w.r.t. a beam line.");}
      G4ThreeVector offset = G4ThreeVector();
      if (placementExtent)
	{offset = SideToLocalOffset(placement, beamLine, *placementExtent);}

      G4Transform3D beamlinePart = beamLine->GetGlobalEuclideanTransform(placement.s * CLHEP::m,
									 placement.x * CLHEP::m + offset.x(),
									 placement.y * CLHEP::m + offset.y());

      G4Transform3D localRotation(rm, G4ThreeVector());
      result = beamlinePart * localRotation;
      if (S)
	{*S = placement.s*CLHEP::m;}
    }
  else
    {// scenario 1
      G4ThreeVector translation = G4ThreeVector(placement.x*CLHEP::m,
						placement.y*CLHEP::m,
						placement.z*CLHEP::m);
      
      
      result = G4Transform3D(rm, translation);
      if (S)
	{*S = -1000;} // default
    }
  
  return result;
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::ScorerMesh& scorerMesh,
								const BDSBeamline*      beamLine,
								G4double*               S)
{
  // convert a scorermesh to a general placement for generation of the transform only.
  GMAD::Placement convertedPlacement(scorerMesh);
  return CreatePlacementTransform(convertedPlacement, beamLine, S, nullptr, "scorermesh");
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::SamplerPlacement& samplerPlacement,
								const BDSBeamline*            beamLine,
								G4double*                     S)
{
  // convert a sampler placement to a general placement for generation of the transform only.
  GMAD::Placement convertedPlacement(samplerPlacement); 
  return CreatePlacementTransform(convertedPlacement, beamLine, S, nullptr, "samplerplacement");
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::BLMPlacement& blmPlacement,
								const BDSBeamline*        beamLine,
								G4double*                 S,
								BDSExtent*                blmExtent)
{
  // convert a sampler placement to a general placement for generation of the transform.
  GMAD::Placement convertedPlacement(blmPlacement);
  return CreatePlacementTransform(convertedPlacement, beamLine, S, blmExtent, "blm");
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::Query& queryPlacement,
                                                                const BDSBeamline* beamLine,
                                                                G4double* S)
{
  GMAD::Placement convertedPlacement(queryPlacement);
  return CreatePlacementTransform(convertedPlacement, beamLine, S, nullptr, "query");
}

G4ThreeVector BDSDetectorConstruction::SideToLocalOffset(const GMAD::Placement& placement,
							 const BDSBeamline*     beamLine,
							 const BDSExtent&       placementExtent)
{
  G4ThreeVector result = G4ThreeVector();
  G4String side = G4String(placement.side);
  side = BDS::LowerCase(side);
  
  // Get the iterators pointing to the first and last elements
  // that the placement lines up with.
  G4double pathLength = placement.s*CLHEP::m;
  std::pair<G4double, G4double> extentZ = placementExtent.ExtentZ();
  G4double sLow  = pathLength + extentZ.first;
  G4double sHigh = pathLength + extentZ.second;
  // iterator pointing to lower bound
  auto start = beamLine->FindFromS(sLow);
  auto end   = beamLine->FindFromS(sHigh);
  if (end != beamLine->end())
    {end++;}
  
  // Fold across the extents returning the greatest extent. The transverse extents
  // will give be the transverse extents of the beamline section.
  BDSExtent sectionMaxExtent = BDSExtent();
  for (auto iter = start; iter != end; ++iter)
    {sectionMaxExtent = BDS::MaximumCombinedExtent((*iter)->GetExtent(), sectionMaxExtent);}

  // Multiplied by 5 because it works...
  G4double ls = 5 * BDSGlobalConstants::Instance()->LengthSafetyLarge();

  if (BDS::IsFinite(placement.sideOffset))
    {ls = placement.sideOffset * CLHEP::m;}
  
  if (side == "top")
    {
      result.setY(sectionMaxExtent.YPos() + placementExtent.YPos() + ls);
      G4double xOffset = sectionMaxExtent.XPos() - 0.5*sectionMaxExtent.DX();
      result.setX(xOffset);
    }
  else if (side == "bottom")
    {
      result.setY(sectionMaxExtent.YNeg() + placementExtent.YNeg() - ls);
      G4double xOffset = sectionMaxExtent.XPos() - 0.5*sectionMaxExtent.DX();
      result.setX(xOffset);
    }
  else if (side == "left")
    {result.setX(sectionMaxExtent.XPos() + placementExtent.XPos() + ls);}
  else if (side == "right")
    {result.setX(sectionMaxExtent.XNeg() + placementExtent.XNeg() - ls);}
  else if (side != "")
    {throw BDSException(std::string("Unknown side in placement: " + side));}
  return result;
}

BDSExtent BDSDetectorConstruction::CalculateExtentOfSamplerPlacement(const GMAD::SamplerPlacement& sp) const
{
  BDSExtent apertureExtent;
  if (sp.apertureModel.empty())
    {
      if (sp.samplerType == "plane")
      {
        BDSApertureInfo aperture = BDSApertureInfo(sp.shape,
                                                   sp.aper1 * CLHEP::m,
                                                   sp.aper2 * CLHEP::m,
                                                   sp.aper3 * CLHEP::m,
                                                   sp.aper4 * CLHEP::m,
                                                   sp.name);
        apertureExtent = aperture.Extent();
      }
      else if (sp.samplerType == "cylinder" || sp.samplerType == "cylinderforward") // we ignore the possibility of only a part-cylinder
        {apertureExtent = BDSExtent(sp.aper1*CLHEP::m, sp.aper1*CLHEP::m, sp.aper2*CLHEP::m);}
      else if (sp.samplerType == "sphere" || sp.samplerType == "sphereforward") //  we ignore the possibility of only a part-sphere
        {apertureExtent = BDSExtent(sp.aper1*CLHEP::m, sp.aper1*CLHEP::m, sp.aper1*CLHEP::m);}
      else
        {throw BDSException(__METHOD_NAME__, "unknown samplerType \"" + sp.samplerType + "\" in samplerplacement \"" + sp.name + "\"");}
    }
  else
    {
      BDSApertureInfo* aperture = BDSAcceleratorModel::Instance()->Aperture(sp.apertureModel);
      apertureExtent = aperture->Extent();
    }
  
  // aperture is only transverse - fiddle z
  BDSExtent result = BDSExtent(apertureExtent.XNeg(), apertureExtent.XPos(),
                               apertureExtent.YNeg(), apertureExtent.YPos(),
                               1*CLHEP::um, 1*CLHEP::um);
  return result;
}

BDSExtentGlobal BDSDetectorConstruction::CalculateExtentOfSamplerPlacements(const BDSBeamline* beamLine) const
{
  BDSExtentGlobal result;
  std::vector<GMAD::SamplerPlacement> samplerPlacements = BDSParser::Instance()->GetSamplerPlacements();
  for (const auto& samplerPlacement : samplerPlacements)
    {
      BDSExtent samplerExtent = CalculateExtentOfSamplerPlacement(samplerPlacement);
      G4Transform3D placementTransform = CreatePlacementTransform(samplerPlacement, beamLine);
      BDSExtentGlobal samplerExtentG = BDSExtentGlobal(samplerExtent, placementTransform);
      result = result.ExpandToEncompass(samplerExtentG);
    }
  return result;
}

BDSExtent BDSDetectorConstruction::CalculateExtentOfScorerMesh(const GMAD::ScorerMesh& sm) const
{
  BDSScorerMeshInfo recipe(sm);
  return recipe.Extent();
}

BDSExtentGlobal BDSDetectorConstruction::CalculateExtentOfScorerMeshes(const BDSBeamline* beamLine) const
{
  BDSExtentGlobal result;
  std::vector<GMAD::ScorerMesh> scorerMeshes = BDSParser::Instance()->GetScorerMesh();
  for (const auto& mesh : scorerMeshes)
    {
      BDSExtent meshExtent = CalculateExtentOfScorerMesh(mesh);
      G4Transform3D placementTransform = CreatePlacementTransform(mesh, beamLine);
      BDSExtentGlobal meshExtentG = BDSExtentGlobal(meshExtent, placementTransform);
      result = result.ExpandToEncompass(meshExtentG);
    }
  return result;
}

#if G4VERSION_NUMBER > 1009
BDSBOptrMultiParticleChangeCrossSection*
BDSDetectorConstruction::BuildCrossSectionBias(const std::list<std::string>& biasList,
                                               const std::list<std::string>& defaultBias,
                                               const G4String& elementName)
{
  // no accelerator components to bias
  if (biasList.empty() && defaultBias.empty())
    {return nullptr;}
  
  std::list<std::string> biasesAll = biasList.empty() ? defaultBias : biasList;
  
  // build a unique 'key' as the sorted set of bias names
  std::set<std::string> biasNamesSorted = {biasesAll.begin(), biasesAll.end()};
  G4String biasSetKey;
  G4String biasSetPrintOut;
  for (const auto& n : biasNamesSorted)
    {
      biasSetKey += n + "_";
      biasSetPrintOut += n + " ";
    }
  biasSetKey = BDS::StrStrip(biasSetKey, '_', BDS::StringStripType::trailing);
  
  auto exists = biasSetObjects.find(biasSetKey);
  if (exists != biasSetObjects.end())
    {return exists->second;}

  // loop over all physics biasing
  G4cout << "Bias> Creating unique set of bias objects ( " << biasSetPrintOut << ")" << G4endl;
  BDSBOptrMultiParticleChangeCrossSection* eg = new BDSBOptrMultiParticleChangeCrossSection();

  const auto& biasObjectList = BDSParser::Instance()->GetBiasing();
  for (std::string const & bs : biasesAll)
    {
      auto result = biasObjectList.find(bs);
      if (result == biasObjectList.end())
	{throw BDSException("Error: bias named \"" + bs + "\" not found for element named \"" + elementName + "\"");}
      const GMAD::PhysicsBiasing& pb = *result;
      
      if(debug)
	{G4cout << __METHOD_NAME__ << "bias loop : " << bs << " " << pb.particle << " " << pb.process << G4endl;}
      
      eg->AddParticle(pb.particle);
      
      // loop through all processes
      if (pb.flag.size() != pb.processList.size())
        {throw BDSException(__METHOD_NAME__, "number of flag entries in \"" + pb.name + "\" doesn't match number of processes");}
      if (pb.factor.size() != pb.processList.size())
        {throw BDSException(__METHOD_NAME__, "number of factor entries in \"" + pb.name + "\" doesn't match number of processes");}
      for (unsigned int p = 0; p < pb.processList.size(); ++p)
	{eg->SetBias(bs, pb.particle,pb.processList[p],pb.factor[p],(G4int)pb.flag[p]);}
    }

  biasObjects.push_back(eg);
  biasSetObjects[biasSetKey] = eg; // cache it
  return eg;
}
#endif

void BDSDetectorConstruction::BuildPhysicsBias() 
{
#if G4VERSION_NUMBER > 1009
  BDSAcceleratorComponentRegistry* registry = BDSAcceleratorComponentRegistry::Instance();
  if (debug)
    {G4cout << __METHOD_NAME__ << "registry=" << registry << G4endl;}

#if G4VERSION_NUMBER > 1039
  // only available in 4.10.4 onwards
  // crystal biasing necessary for implementation of variable density
  std::set<G4LogicalVolume*>* crystals = BDSAcceleratorModel::Instance()->VolumeSet("crystals");
  if (!crystals->empty())
    {
      G4cout << __METHOD_NAME__ << "Using crystal biasing: true" << G4endl; // to match print out style further down
      auto crystalBiasing = new G4ChannelingOptrMultiParticleChangeCrossSection();
      for (auto crystal : *crystals)
	{crystalBiasing->AttachTo(crystal);}
    }
#endif

  auto blmSet = BDSBLMRegistry::Instance()->BLMs();
  for (auto blm : blmSet)
    {
      G4String biasNamesS = blm->Bias();
      if (biasNamesS.empty())
        {continue;}
      auto biasNamesV = BDS::SplitOnWhiteSpace(biasNamesS);
      std::list<std::string> biasNames = {biasNamesV.begin(), biasNamesV.end()};
      std::list<std::string> emptyDefaultBias;
      auto biasForBLM = BuildCrossSectionBias(biasNames, emptyDefaultBias, blm->GetName());
      for (auto lv : blm->GetAllLogicalVolumes())
        {biasForBLM->AttachTo(lv);}
      biasForBLM->AttachTo(blm->GetContainerLogicalVolume()); // in some cases it's just a single volume
    }

  auto g = BDSGlobalConstants::Instance();
  G4String defaultBiasVacuum      = BDSParser::Instance()->GetOptions().defaultBiasVacuum;
  auto defaultBiasVacuumVector    = BDS::SplitOnWhiteSpace(defaultBiasVacuum);
  auto defaultBiasVacuumList      = std::list<std::string>(defaultBiasVacuumVector.begin(), defaultBiasVacuumVector.end());
  G4String defaultBiasMaterial    = BDSParser::Instance()->GetOptions().defaultBiasMaterial;
  auto defaultBiasMaterialVector  = BDS::SplitOnWhiteSpace(defaultBiasMaterial);
  auto defaultBiasMaterialList    = std::list<std::string>(defaultBiasMaterialVector.begin(), defaultBiasMaterialVector.end());
  G4String biasForWorldVolume     = g->BiasForWorldVolume();
  auto biasForWorldVolumeVector   = BDS::SplitOnWhiteSpace(biasForWorldVolume);
  auto biasForWorldVolumeList     = std::list<std::string>(biasForWorldVolumeVector.begin(), biasForWorldVolumeVector.end());
  G4String biasForWorldContents   = g->BiasForWorldContents();
  auto biasForWorldContentsVector = BDS::SplitOnWhiteSpace(biasForWorldContents);
  auto biasForWorldContentsList   = std::list<std::string>(biasForWorldContentsVector.begin(), biasForWorldContentsVector.end());
  G4String biasForWorldVacuum     = g->BiasForWorldVacuum();
  auto biasForWorldVacuumVector   = BDS::SplitOnWhiteSpace(biasForWorldVacuum);
  auto biasForWorldVacuumList     = std::list<std::string>(biasForWorldVacuumVector.begin(), biasForWorldVacuumVector.end());
  
  G4bool useDefaultBiasVacuum    = !defaultBiasVacuum.empty();
  G4bool useDefaultBiasMaterial  = !defaultBiasMaterial.empty();
  const auto& biasObjectList     = BDSParser::Instance()->GetBiasing();
  G4bool useBiasForWorldVolume   = !biasForWorldVolume.empty();
  G4bool useBiasForWorldContents = !biasForWorldContents.empty();
  G4bool useBiasForWorldVacuum   = !biasForWorldVacuum.empty();
  G4bool biasesDefined           = !biasObjectList.empty();
  
  G4bool overallUseBiasing = useDefaultBiasVacuum || useDefaultBiasMaterial || biasesDefined || useBiasForWorldVolume || useBiasForWorldContents;
  G4cout << __METHOD_NAME__ << "Using generic biasing: " << BDS::BoolToString(overallUseBiasing) << G4endl;
  if (!overallUseBiasing)
    {return;} // no biasing used -> dont attach as just overhead for no reason
  
  // apply per element biases
  std::unordered_map<ACRegistryKey, BDSAcceleratorComponent*> allAcceleratorComponents = registry->AllComponentsIncludingUnique();
  for (auto const & item : allAcceleratorComponents)
    {
      if (debug)
        {G4cout << __METHOD_NAME__ << "checking component named: " << item.first.componentName << G4endl;}
      BDSAcceleratorComponent* accCom = item.second;
      BDSLine* l = dynamic_cast<BDSLine*>(accCom);
      if (l)
        {continue;} // do nothing for lines because each sub-component already has all definitions
      G4String accName = accCom->GetName();
      std::set<G4LogicalVolume*> vacuumLVs = accCom->GetAcceleratorVacuumLogicalVolumes();
      
      // Build vacuum bias object based on vacuum bias list in the component
      auto vacuumBiasList = accCom->GetBiasVacuumList();
      if (!vacuumBiasList.empty() && vacuumLVs.empty())
	{BDS::Warning("biasVacuum set for component \"" + accName + "\" but there's no 'vacuum' volume for it and it can't be biased.\nRemove biasVacuum or name it with the namedVacuumVolumes parameter.");}
      if ((!vacuumBiasList.empty() || useDefaultBiasVacuum) && !vacuumLVs.empty())
        {
          auto egVacuum = BuildCrossSectionBias(vacuumBiasList, defaultBiasVacuumList, accName);
	  for (auto lv : vacuumLVs)
            {
              if (debug)
                {G4cout << __METHOD_NAME__ << "vacuum volume name: " << lv << " " << lv->GetName() << G4endl;}
              egVacuum->AttachTo(lv);
            }
	}
      
      // Build material bias object based on material bias list in the component
      auto materialBiasList = accCom->GetBiasMaterialList();
      if (!materialBiasList.empty() || useDefaultBiasMaterial)
	{
	  auto egMaterial = BuildCrossSectionBias(materialBiasList, defaultBiasMaterialList, accName);
	  auto allLVs     = accCom->GetAcceleratorMaterialLogicalVolumes();
	  if (debug)
	    {G4cout << __METHOD_NAME__ << "# of logical volumes for biasing under 'material': " << allLVs.size() << G4endl;}
	  for (auto lv : allLVs)
	    {// BDSAcceleratorComponent automatically removes 'vacuum' volumes from all so we don't need to check
		  if (debug)
		    {G4cout << __METHOD_NAME__ << "Biasing 'material' logical volume: " << lv << " " << lv->GetName() << G4endl;}
		  egMaterial->AttachTo(lv);
	    }
	}

      // Build material bias object for a specific LV based on material bias list in the component
      auto nameAndMaterialLVBiasList = accCom->GetBiasMaterialLVList();
      if (!nameAndMaterialLVBiasList.empty() || useDefaultBiasMaterial)
      {
        std::map<std::string, std::string> namesAndBiasesMap;
        for (G4String lvbias : nameAndMaterialLVBiasList)
        {
          auto splitpos = lvbias.find(':');
          auto lvname = lvbias.substr(0,splitpos);
          auto biasname = lvbias.substr(splitpos+1);
          G4cout << lvname << " " << biasname << G4endl;
          namesAndBiasesMap[lvname] = biasname;
        }
        auto allLVs       = accCom->GetAcceleratorMaterialLogicalVolumes();
        if (debug)
        {G4cout << __METHOD_NAME__ << "# of logical volumes for biasing under 'materialLV': " << allLVs.size() << G4endl;}
        for (auto lv : allLVs)
        {// BDSAcceleratorComponent automatically removes 'vacuum' volumes from all so we don't need to check
          if (debug)
          {G4cout << __METHOD_NAME__ << "Biasing 'materialLV' logical volume: " << lv << " " << lv->GetName() << G4endl;}
          for (const auto& nameAndBias : namesAndBiasesMap)
          {
            if (lv->GetName().find(nameAndBias.first) != std::string::npos)
            {
              auto egMaterialLV = BuildCrossSectionBias({nameAndBias.second}, defaultBiasMaterialList, accName);
              egMaterialLV->AttachTo(lv);
            }
          }
        }
      }
    }
  
  if (useBiasForWorldContents)
    {
      std::list<std::string> emptyList;
      auto egWC = BuildCrossSectionBias(emptyList, biasForWorldContentsList, "world_contents_bias");
      for (auto lv : worldContentsLogicalVolumes)
	      {egWC->AttachTo(lv);}
    }
  if (useBiasForWorldVolume)
    {
      std::list<std::string> emptyList;
      auto egWV = BuildCrossSectionBias(emptyList, biasForWorldVolumeList, "world_volume_bias");
      egWV->AttachTo(worldLogicalVolume);
    }
  if (useBiasForWorldVacuum)
    {
      std::list<std::string> emptyList;
      auto egWVac = BuildCrossSectionBias(emptyList, biasForWorldVacuumList, "world_vacuum_bias");
      for (auto lv : worldVacuumLogicalVolumes)
	{egWVac->AttachTo(lv);}
    }
#endif
}

void BDSDetectorConstruction::ConstructSDandField()
{
  auto flds = BDSFieldBuilder::Instance()->CreateAndAttachAll(); // avoid shadowing 'fields'
  acceleratorModel->RegisterFields(flds);

  ConstructScoringMeshes();
}

G4bool BDSDetectorConstruction::UnsuitableFirstElement(GMAD::FastList<GMAD::Element>::FastListConstIterator element)
{
  // skip past any line elements in parser to find first non-line element
  while ((*element).type == GMAD::ElementType::_LINE)
    {element++;}
  
  if ((*element).type == GMAD::ElementType::_RBEND)
    {return true;}  // unsuitable
  else if (BDS::IsFinite((*element).e1))
    {return true;}  // unsuitable
  else
    {return false;} // suitable
}

void BDSDetectorConstruction::ConstructScoringMeshes()
{
  // needed for filtering
  G4LogicalVolume* worldLV = acceleratorModel->WorldLV();

  std::vector<GMAD::ScorerMesh> scoringMeshes = BDSParser::Instance()->GetScorerMesh();
  std::vector<GMAD::Scorer> scorers = BDSParser::Instance()->GetScorers();

  if (scoringMeshes.empty())
    {return;}

  G4ScoringManager* scManager = G4ScoringManager::GetScoringManager();
  scManager->SetVerboseLevel(1);

  // convert all the parser scorer definitions into recipes (including parameter checking)
  std::map<G4String, BDSScorerInfo> scorerRecipes;
  for (const auto& scorer : scorers)
    {
      BDSScorerInfo si = BDSScorerInfo(scorer, true); // true = upgrade to 3d as required for 3d mesh here
      scorerRecipes.insert(std::make_pair(si.name, si));
    }

  // construct meshes
  BDSScorerFactory scorerFactory;
  for (const auto& mesh : scoringMeshes)
    {
      // convert to recipe class as this checks parameters
      BDSScorerMeshInfo meshRecipe = BDSScorerMeshInfo(mesh);
      
      // name we'll use for the mesh
      G4String meshName = meshRecipe.name;
      
      // TBC - could be any beam line in future - just w.r.t. main beam line just now
      const BDSBeamline* mbl = BDSAcceleratorModel::Instance()->BeamlineMain();
      G4Transform3D placement = CreatePlacementTransform(mesh, mbl);

      BDSScoringMeshBox* scorerBox = nullptr;
      BDSScoringMeshCylinder* scorerCylindrical = nullptr;
      const BDSHistBinMapper* mapper = nullptr;

      G4String geometryType = BDS::LowerCase(G4String(mesh.geometryType));

      if (geometryType == "box")
        {// create a scoring box
          scorerBox = new BDSScoringMeshBox(meshName, meshRecipe, placement);
          mapper = scorerBox->Mapper();
        }
      else if (geometryType == "cylindrical")
        {// create a scoring cylinder
          scorerCylindrical = new BDSScoringMeshCylinder(meshName, meshRecipe, placement);
          mapper = scorerCylindrical->Mapper();
        }
      else
        {
          G4String msg = "mesh geometry type \"" + geometryType + "\" is not correct. The possible options are \"box\" and \"cylindrical\"";
          throw BDSException(__METHOD_NAME__, msg);
        }

      // add the scorer(s) to the scoring mesh
      std::vector<G4String> meshPrimitiveScorerNames; // final vector of unique mesh + ps names
      std::vector<G4double> meshPrimitiveScorerUnits;
      std::vector<G4String> scorerNames;

      std::vector<G4String> words = BDS::SplitOnWhiteSpace(mesh.scoreQuantity);
      for (const auto& word : words)
        {
          auto search = scorerRecipes.find(word);
          if (search == scorerRecipes.end())
            {throw BDSException(__METHOD_NAME__, "scorerQuantity \"" + word + "\" for mesh \"" + meshName + "\" not found.");}

          G4double psUnit = 1.0;
          G4VPrimitiveScorer* ps = scorerFactory.CreateScorer(&(search->second), mapper, &psUnit, worldLV);
          // The mesh internally creates a multifunctional detector which is an SD and has
          // the name of the mesh. Any primitive scorer attached is added to the mfd. To get
          // the hits map we need the full name of the unique primitive scorer so we build that
          // name here and store it.
          G4String uniqueName = meshName + "/" + ps->GetName();
          meshPrimitiveScorerNames.push_back(uniqueName);
          meshPrimitiveScorerUnits.push_back(psUnit);

          // sets the current ps but appends to list of multiple
          if (geometryType == "box")
            {scorerBox->SetPrimitiveScorer(ps);} 
          else if (geometryType == "cylindrical")
            {scorerCylindrical->SetPrimitiveScorer(ps);}
          
          BDSScorerHistogramDef outputHistogram(meshRecipe, uniqueName, ps->GetName(), psUnit, *mapper);
          BDSAcceleratorModel::Instance()->RegisterScorerHistogramDefinition(outputHistogram);
          BDSAcceleratorModel::Instance()->RegisterScorerPlacement(meshName, placement);
        }
      
      if (geometryType == "box")
        {scManager->RegisterScoringMesh(scorerBox);} // sets the current ps but appends to list of multiple
      else if (geometryType == "cylindrical")
        {scManager->RegisterScoringMesh(scorerCylindrical);}// sets the current ps but appends to list of multiple

      // register it with the sd manager as this is where we get all collection IDs from
      // in the end of event action. This must come from the mesh as it creates the
      // multifunctionaldetector and therefore has the complete name of the scorer collection
      BDSSDManager::Instance()->RegisterPrimitiveScorerNames(meshPrimitiveScorerNames, &meshPrimitiveScorerUnits);
    }
}

std::vector<BDSFieldQueryInfo*> BDSDetectorConstruction::PrepareFieldQueries(const BDSBeamline* mainBeamline)
{
  std::vector<BDSFieldQueryInfo*> result;
  const std::vector<GMAD::Query>& parserQueries = BDSParser::Instance()->GetQuery();
  for (const auto& def : parserQueries)
    {
      G4bool assumeQueryMagnetic = false;
      if (!def.queryMagneticField && !def.queryElectricField)
        {
          assumeQueryMagnetic = true;
          G4cout << __METHOD_NAME__ << "neither \"queryMagneticField\" nor \"queryElectricField\" are turned on for definition \""
          << def.name << "\"" << G4endl;
          G4cout << "-> querying magnetic field by default" << G4endl;
        }

      if (!def.pointsFile.empty())
        {
          std::vector<G4String> columnNames;
          auto points = BDS::LoadFieldQueryPoints(G4String(def.pointsFile), &columnNames);
          result.emplace_back(new BDSFieldQueryInfo(G4String(def.name),
                                                    G4String(def.outfileMagnetic),
                                                    G4String(def.outfileElectric),
                                                    G4bool(def.queryMagneticField) || assumeQueryMagnetic,
                                                    G4bool(def.queryElectricField),
                                                    points,
                                                    columnNames,
                                                    G4bool(def.overwriteExistingFiles),
                                                    G4String(def.fieldObject),
                                                    def.checkParameters,
                                                    def.drawArrows,
                                                    def.drawZeroValuePoints,
                                                    def.drawBoxes,
                                                    def.boxAlpha));
        }
      else
        {
          G4Transform3D globalTransform3D = CreatePlacementTransform(def, mainBeamline);
          auto rot = globalTransform3D.getRotation();
          rot = rot.inverse();
          G4AffineTransform globalTransform(rot, globalTransform3D.getTranslation());
          
          result.emplace_back(new BDSFieldQueryInfo(G4String(def.name),
                                                    G4String(def.outfileMagnetic),
                                                    G4String(def.outfileElectric),
                                                    G4bool(def.queryMagneticField) || assumeQueryMagnetic,
                                                    G4bool(def.queryElectricField),
                                                    {def.nx, def.xmin*CLHEP::m, def.xmax*CLHEP::m},
                                                    {def.ny, def.ymin*CLHEP::m, def.ymax*CLHEP::m},
                                                    {def.nz, def.zmin*CLHEP::m, def.zmax*CLHEP::m},
                                                    {def.nt, def.tmin*CLHEP::ns, def.tmax*CLHEP::ns},
                                                    globalTransform,
                                                    G4bool(def.overwriteExistingFiles),
                                                    G4String(def.fieldObject),
                                                    G4bool(def.printTransform),
                                                    def.checkParameters,
                                                    def.drawArrows,
                                                    def.drawZeroValuePoints,
                                                    def.drawBoxes,
                                                    def.boxAlpha));
        }
    }
  return result;
}

#ifdef BDSCHECKUSERLIMITS
void BDSDetectorConstruction::PrintUserLimitsSummary(const G4VPhysicalVolume* world) const
{
  G4cout << "USERLIMITS START" << G4endl;
  G4double globalMinEK = BDSGlobalConstants::Instance()->MinimumKineticEnergy();
  PrintUserLimitsPV(world, globalMinEK);
  G4cout << "USERLIMITS END" << G4endl;
}

void BDSDetectorConstruction::PrintUserLimitsPV(const G4VPhysicalVolume* aPV, G4double globalMinEK) const
{
  const auto lv = aPV->GetLogicalVolume();
  const auto ul = lv->GetUserLimits();
  if (ul)
    {
      G4Track dummyTrack;
      G4double ekUL = ul->GetUserMinEkine(dummyTrack);
      //G4cout << lv->GetName() << " Ek Min: " << ekUL << G4endl;
      if (ekUL < globalMinEK)
        {G4cout << lv->GetName() << " Ek Min: " << ekUL << " MeV < global: " << globalMinEK << " MeV" << G4endl;}
    }
  else
    {G4cout << lv->GetName() << " no G4UserLimits" << G4endl;}
  for (G4int i = 0; i < (G4int)lv->GetNoDaughters(); i++)
    {PrintUserLimitsPV(lv->GetDaughter(i), globalMinEK);}
}
#endif

void BDSDetectorConstruction::VerboseSensitivity() const
{
  if (!worldLogicalVolume)
    {return;}
  G4cout << "\nSensitivity Summary:\n" << G4endl;
  PrintSensitiveDetectorsOfLV(worldLogicalVolume, 0);
  G4cout << "\n\n" << G4endl;
}

void BDSDetectorConstruction::PrintSensitiveDetectorsOfLV(const G4LogicalVolume* lv, G4int currentDepth) const
{
  for (G4int i = 0; i < currentDepth; i++)
    {G4cout << "-> ";}
  G4cout << lv->GetName() << " ";
  auto sensitiveDetector = lv->GetSensitiveDetector();
  if (sensitiveDetector)
    {G4cout << sensitiveDetector->GetName();}
  else
    {G4cout << "none";}
  G4cout << G4endl;

  for (std::size_t i = 0; i < lv->GetNoDaughters(); i++)
    {PrintSensitiveDetectorsOfLV(lv->GetDaughter(i)->GetLogicalVolume(), currentDepth+1);}
}
