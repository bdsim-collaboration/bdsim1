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
#include "optionsBase.h"

#include <iostream>

using namespace GMAD;

OptionsBase::OptionsBase()
{
  // Default Values for Options

  // executable options
  inputFileName         = "optics.mad";
  visMacroFileName      = "";
  geant4MacroFileName   = "";
  geant4PhysicsMacroFileName = "";
  geant4PhysicsMacroFileNameFromExecOptions = false;
  visDebug              = false;
  visVerbosity          = 0;
  
  outputFileName        = "output";
  outputFormat          = "rootevent";
#ifdef __ROOTDOUBLE__
  outputDoublePrecision = true;
#else
  outputDoublePrecision = false;
#endif
  outputCompressionLevel= 5;
  survey                = false;
  surveyFileName        = "survey.dat";
  batch                 = false;
  
  verbose               = false;
  
  verboseRunLevel       = 0;

  verboseEventBDSIM       = false;
  verboseEventLevel       = 0;
  verboseEventStart       = -1;
  verboseEventContinueFor = -1;
  
  verboseTrackingLevel  = 0;
  
  verboseSteppingBDSIM            = false;
  verboseSteppingLevel            = 0;
  verboseSteppingEventStart       = -1;
  verboseSteppingEventContinueFor = -1;
  verboseSteppingPrimaryOnly      = false;
  
  verboseImportanceSampling = 0;

  verboseSensitivity = false;
  
  circular              = false;
  seed                  = -1;
  randomEngine          = "hepjames";
  nGenerate             = 1;
  recreate              = false;
  recreateFileName      = "";
  startFromEvent        = 0;
  writeSeedState        = false;
  useASCIISeedState     = false;
  seedStateFileName     = "";
  generatePrimariesOnly = false;
  exportGeometry        = false;
  exportType            = "gdml";
  exportFileName        = "geometry";
  bdsimPath             = "";
  
  // very important options
  physicsList                    = ""; //default - only transportation
  physicsVerbose                 = false;
  physicsVerbosity               = 1;
  physicsEnergyLimitLow          = 0;
  physicsEnergyLimitHigh         = 0;
  g4PhysicsUseBDSIMRangeCuts     = true;
  g4PhysicsUseBDSIMCutsAndLimits = true;
  
  eventOffset           = 0;
  recreateSeedState     = true;

  elossHistoBinWidth    = 1.0; // m
  
  ffact                 = 1.0;

  // beam line offset and rotation
  beamlineX         = 0;
  beamlineY         = 0;
  beamlineZ         = 0;
  beamlinePhi       = 0;
  beamlineTheta     = 0;
  beamlinePsi       = 0;
  beamlineAxisX     = 0;
  beamlineAxisY     = 0;
  beamlineAxisZ     = 0;
  beamlineAngle     = 0;
  beamlineAxisAngle = false;
  beamlineS         = 0;

  eventNumberOffset       = 0;

  // general geometrical parameters
  checkOverlaps           = false;
  xsize=0.0, ysize=0.0;

  // magnet geometry
  magnetGeometryType         = "polessquare";
  outerMaterialName          = "iron";
  horizontalWidth            = 0.6;
  thinElementLength          = 1e-7;
  hStyle                     = false; // vhRatio < 0 as signal to use geometry factory default
  vhRatio                    = -1;
  coilWidthFraction          = -1;
  coilHeightFraction         = -1;
  ignoreLocalMagnetGeometry  = false;
  buildPoleFaceGeometry      = true;

  preprocessGDML       = true;
  preprocessGDMLSchema = true;

  // geometry debugging
  // always split sbends into smaller chunks by default
  dontSplitSBends      = false;
  includeFringeFields  = true;
  includeFringeFieldsCavities = true;

  yokeFields           = true;
  yokeFieldsMatchLHCGeometry = true;
  useOldMultipoleOuterFields = false;
  scalingFieldOuter    = 1.0;
  integrateKineticEnergyAlongBeamline = true;
  
  cavityFieldType = "constantinz";
  
  // beam pipe / aperture
  beampipeThickness    = 0.0025;
  apertureType         = "circular";
  aper1                = 0.025; // also beampipeRadius
  aper2                = 0;
  aper3                = 0;
  aper4                = 0;
  beampipeMaterial     = "StainlessSteel";
  ignoreLocalAperture  = false;
  
  vacMaterial          = "Vacuum";
  emptyMaterial        = "G4_Galactic";
  worldMaterial        = "G4_AIR";
  worldGeometryFile    = "";
  autoColourWorldGeometryFile = true;
  importanceWorldGeometryFile = "";
  importanceVolumeMap  = "";
  worldVolumeMargin = 5; //m

  vacuumPressure       = 1e-12;

  // tunnel options
  buildTunnel         = false;
  buildTunnelStraight = false;
  tunnelType          = "circular";
  tunnelThickness     = 0.1;
  tunnelSoilThickness = 1.0;
  tunnelMaterial      = "concrete";
  soilMaterial        = "soil";
  buildTunnelFloor    = true;
  tunnelFloorOffset   = 1.0; // m
  tunnelAper1         = 2.0; // m
  tunnelAper2         = 2.0; // m
  tunnelVisible       = true;
  tunnelOffsetX       = 0;
  tunnelOffsetY       = 0;
  tunnelMaxSegmentLength = 50.0; // m

  removeTemporaryFiles = true;
  temporaryDirectory = "";
  
  // samplers
  samplerDiameter     = 5; // m

  // physics processes
  turnOnOpticalAbsorption  = true;
  turnOnMieScattering      = true;
  turnOnRayleighScattering = true;
  turnOnOpticalSurface     = true;
  scintYieldFactor         = 1.0;
  maximumPhotonsPerStep    = -1;  ///< -1 -> no action taken (could want 0)
  maximumBetaChangePerStep = 10;
  maximumTracksPerEvent    = 0;   ///< 0 -> no action taken
  minimumKineticEnergy     = 0;
  minimumKineticEnergyTunnel = 0;
  minimumRange             = 0;
  particlesToExcludeFromCuts = "";
  defaultRangeCut          = 1e-3;
  prodCutPhotons           = 1e-3;
  prodCutElectrons         = 1e-3;
  prodCutPositrons         = 1e-3;
  prodCutProtons           = 1e-3;
  neutronTimeLimit         = 1e-6;
  neutronKineticEnergyLimit = 0;
  useLENDGammaNuclear      = false;
  useElectroNuclear        = true; // defaults to match g4 em extra physics defaults
  useMuonNuclear           = true;
  useGammaToMuMu           = false;
  usePositronToMuMu        = false;
  usePositronToHadrons     = false;
  restoreFTPFDiffractionForAGreater10 = true;

  beamPipeIsInfiniteAbsorber      = false;
  collimatorsAreInfiniteAbsorbers = false;
  tunnelIsInfiniteAbsorber        = false;
  muonSplittingFactor = 1;
  muonSplittingThresholdParentEk = 0;
  muonSplittingFactor2 = 1;
  muonSplittingThresholdParentEk2 = 0;
  muonSplittingExcludeWeight1Particles = false;
  muonSplittingExclusionWeight = 1e99;
  xrayAllSurfaceRoughness = 0;
  
  // biasing options
  scaleFactorLaser         = 1;
  defaultBiasVacuum        = "";
  defaultBiasMaterial      = "";
  biasForWorldVolume       = "";
  biasForWorldContents     = "";
  biasForWorldVacuum       = "";
  worldVacuumVolumeNames   = "";

  // tracking options
  integratorSet            = "bdsimmatrix";
  fieldModulator           = "";
  lengthSafety             = 1e-9;   // be very careful adjusting this as it affects all the geometry
  lengthSafetyLarge        = 1e-6;   // be very careful adjusting this as it affects all the geometry
  maximumTrackingTime      = -1;      // s, nonsensical - used for testing
  maximumStepLength        = 20;      // m, quite big
  maximumTrackLength       = 1e90;    // m, no limit but smaller than DBL_MAX for safe *CLHEP::m
  chordStepMinimum         = 1e-9;    // m // minimum step in a field for an integrator
  chordStepMinimumYoke     = 1e-6;
  deltaIntersection        = 1e-8;    // m - should be greater than lengthSafety!
  sampleElementsWithPoleface  = false;   // affects dipole tracking in certain integrator sets when true
  nominalMatrixRelativeMomCut = 0.05;  // be careful adjusting this as it affects dipolequadrupole tracking
  teleporterFullTransform  = true;
  dEThresholdForScattering = 1e-11; // GeV
  backupStepperMomLimit    = 0.1;   // fraction of unit momentum

  // default value in Geant4, old value 0 - error must be greater than this
  minimumEpsilonStep       = 1e-12;   // used to be 1e-25 but since v11.1 this has to be greater than double precision
  maximumEpsilonStep       = 1e-7;    // default value in Geant4, old value 1e-7
  minimumEpsilonStepThin   = 1e-12;   // only for thin elements
  maximumEpsilonStepThin   = 1e-3;    // only for thin elements
  deltaOneStep             = 1e-6;    // maximum allowed spatial error in position (1um)
  stopSecondaries          = false;
  killNeutrinos            = false;
  killedParticlesMassAddedToEloss = false;
  minimumRadiusOfCurvature = 0.05; // 5cm - typical aperture

  // hit generation
  sensitiveOuter       = true;
  sensitiveBeamPipe    = true;
  
  // output / analysis options
  numberOfEventsPerNtuple  = 0;

  storeMinimalData = false;
  
  storeApertureImpacts       = true;
  storeApertureImpactsHistograms = true;
  storeApertureImpactsIons   = false;
  storeApertureImpactsAll    = false;
  apertureImpactsMinimumKE   = 0;
  storeCavityInfo            = true;
  storeCollimatorInfo        = false;
  storeCollimatorHits        = false;
  storeCollimatorHitsLinks   = false;
  storeCollimatorHitsIons    = false;
  storeCollimatorHitsAll     = false;
  collimatorHitsMinimumKE    = 0;
  storeEloss                 = true;
  storeElossHistograms       = true;
  storeElossVacuum           = false;
  storeElossVacuumHistograms = false;
  storeElossTunnel           = false;
  storeElossTunnelHistograms = false;
  storeElossWorld            = false;
  storeElossWorldIntegral    = false;
  storeElossWorldContents    = false;
  storeElossWorldContentsIntegral = false;
  storeElossTurn             = false;
  storeElossLinks            = false;
  storeElossLocal            = false;
  storeElossGlobal           = false;
  storeElossTime             = false;
  storeElossStepLength       = false;
  storeElossPreStepKineticEnergy = false;
  storeElossModelID          = false;
  storeElossPhysicsProcesses = false;
  storeParticleData          = true;
  storePrimaries             = true;
  storePrimaryHistograms     = true;
  
  storeTrajectory                = false;
  
  storeTrajectoryDepth           = 0;
  storeTrajectoryStepPoints      = 0;
  storeTrajectoryStepPointLast   = false;
  storeTrajectoryParticle        = "";
  storeTrajectoryParticleID      = "";
  storeTrajectorySecondaryParticles = false;
  storeTrajectoryEnergyThreshold = -1.0;
  storeTrajectorySamplerID       = "";
  storeTrajectoryELossSRange     = "";
  
  storeTrajectoryTransportationSteps = true;
  trajNoTransportation               = false; ///< kept only for backwards compatibility.
  storeTrajectoryKineticEnergy       = true;
  storeTrajectoryMomentumVector      = false;
  storeTrajectoryProcesses           = false;
  storeTrajectoryTime                = false;
  storeTrajectoryLocal               = false;
  storeTrajectoryLinks               = false;
  storeTrajectoryIon                 = false;
  storeTrajectoryMaterial            = false;
  storeTrajectoryAllVariables        = false;

  trajectoryFilterLogicAND = false;
  
  storeSamplerAll          = false;
  storeSamplerPolarCoords  = false;
  storeSamplerCharge       = false;
  storeSamplerKineticEnergy = true;
  storeSamplerMass         = false;
  storeSamplerRigidity     = false;
  storeSamplerIon          = false;

  trajCutGTZ               = 1e99;  // minimum z position, so large default value
  trajCutLTR               = 0.0;   // maximum radius in mm, so small default value
  trajConnect              = false; // connect disconnected trajectory trees
  
  storeModel               = true;

  samplersSplitLevel       = 0;
  modelSplitLevel          = 1;
  uprootCompatible         = 0;

  // circular options
  nturns                   = 1;
  ptcOneTurnMapFileName    = "";

  printFractionEvents   = 0.1;
  printFractionTurns    = 0.2;
  printPhysicsProcesses = false;
  
  // visualisation
  nSegmentsPerCircle       = 50;

  // scoring map
  nbinsx = 1;
  nbinsy = 1;
  nbinsz = 1;
  nbinse = 1;
  xmin   = -0.5;
  xmax   = 0.5;
  ymin   = -0.5;
  ymax   = 0.5;
  zmin   = 0;
  zmax   = 1;
  emin   = 1e-12;
  emax   = 1e4;
  useScoringMap = false;
}


void OptionsBase::print() const
{
  std::cout<<"Options                  " << std::endl;
  std::cout<<"n particles            : " << nGenerate                << std::endl;
  std::cout<<"BV sign                : " << ffact                    << std::endl;
  std::cout<<"Optical absorption on  : " << turnOnOpticalAbsorption  << std::endl;
  std::cout<<"Mie scattering on      : " << turnOnMieScattering      << std::endl;
  std::cout<<"Rayleigh scattering on : " << turnOnRayleighScattering << std::endl;
  std::cout<<"Optical surface on     : " << turnOnOpticalSurface     << std::endl;
}

