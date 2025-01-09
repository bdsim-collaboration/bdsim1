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
#ifndef OPTIONSBASE_H
#define OPTIONSBASE_H

#include <string>
#include <vector>

namespace GMAD
{
  /**
   * @brief Basic options class independent of Geant4.
   * 
   * This is the basic options structure that can be saved to output
   * and should only contain basic built in C++ types. No Geant4 types
   * are to be used. See also the Options class which inherits this one.
   */
  
  class OptionsBase
  {
  public:
    OptionsBase();

    std::string inputFileName;       ///< Input filename.
    std::string visMacroFileName;    ///< Visualisation filename.
    std::string geant4MacroFileName; ///< Geant4 macro to run.
    std::string geant4PhysicsMacroFileName; ///< Geant4 physics macro
    bool        geant4PhysicsMacroFileNameFromExecOptions;
    bool        visDebug;            ///< Flag for visualisation debug.
    int         visVerbosity;        ///< Geant4 vis system verbosity.
  
    ///@{ Parameter for output format
    std::string outputFileName;
    std::string outputFormat;
    bool        outputDoublePrecision;
    int         outputCompressionLevel;
    ///@}
  
    ///@{ Parameter for survey
    bool        survey;
    std::string surveyFileName;
    ///@}
  
    bool batch; ///< Flag for batch / interactive mode

    // verbosity here done in hierarchy of simulation - overall, run, event
    // track, step
    /// General verbosity.
    bool verbose;

    /// Run level verbosity.
    int  verboseRunLevel;

    /// @{ Event level verbosity.
    bool verboseEventBDSIM;      // for bdsim print out
    int  verboseEventLevel;      // for geant4 print out
    int  verboseEventStart;
    int  verboseEventContinueFor;
    /// @}
    
    int  verboseTrackingLevel;  ///< Tracking verbosity.

    /// @{ Stepping level verbosity.
    bool verboseSteppingBDSIM;  // for bdsim print out
    int  verboseSteppingLevel;  // for geant4 print out
    int  verboseSteppingEventStart;
    int  verboseSteppingEventContinueFor;
    bool verboseSteppingPrimaryOnly;
    /// @}
    
    int  verboseImportanceSampling; ////< Verbosity about importance sampling.

    bool verboseSensitivity;
  
    bool circular;                 ///< Flag for circular machine
    int  seed;                     ///< The seed value for the random number generator
    std::string randomEngine;      ///< Name of random engine to use.
    int  nGenerate;                ///< The number of primary events to simulate
    bool recreate;                 ///< Whether to recreate from a file or not.
    std::string recreateFileName;  ///< The file path to recreate a run from.
    int  startFromEvent;           ///< Event to start from when recreating.
    bool writeSeedState;           ///< Write the seed state each event to a text file.
    bool useASCIISeedState;        ///< Whether to use the seed state from an ASCII file.
    std::string seedStateFileName; ///< Seed state file path.

    /// Whether to only generate primary coordinates and quit, or not.
    bool generatePrimariesOnly; 
    
    ///@{ Parameter for controlling geometry export
    bool        exportGeometry;
    std::string exportType;
    std::string exportFileName;
    ///@}

    /// String that points to path where files are searched;
    /// based on environment variable or else input filename
    std::string bdsimPath;
    
    /// list of physics processes
    std::string physicsList;
    bool        physicsVerbose;
    int         physicsVerbosity;
    double      physicsEnergyLimitLow;
    double      physicsEnergyLimitHigh;
    bool        g4PhysicsUseBDSIMRangeCuts;
    bool        g4PhysicsUseBDSIMCutsAndLimits;
    
    int eventOffset;  ///< Event number to start from when recreating from a root file.
    bool recreateSeedState; ///< Load seed state when recreating events.
    
    double elossHistoBinWidth;
    
    /// magnetic field flip (+1 default, -1: flip sign)
    double ffact;

    ///@{ Initial beam line transform w.r.t. the world coordinate frame.
    double beamlineX;
    double beamlineY;
    double beamlineZ;
    double beamlinePhi;
    double beamlineTheta;
    double beamlinePsi;
    double beamlineAxisX;
    double beamlineAxisY;
    double beamlineAxisZ;
    double beamlineAngle;
    bool   beamlineAxisAngle;
    double beamlineS;
    ///@}   

    int    eventNumberOffset;
    
    /// bdsim options
    bool       checkOverlaps;
    /// for element specification
    double xsize, ysize;

    /// default magnet geometry parameters
    std::string magnetGeometryType;
    std::string outerMaterialName;
    double      horizontalWidth; // formerly outerDiameter
    double      thinElementLength;
    bool        hStyle; ///< H Style dipoles (if not, C Style).
    double      vhRatio;
    double      coilWidthFraction;
    double      coilHeightFraction;
    bool        ignoreLocalMagnetGeometry;
    bool        buildPoleFaceGeometry;

    /// geometry control
    bool preprocessGDML;
    bool preprocessGDMLSchema;

    /// geometry debug, don't split bends into multiple segments
    bool      dontSplitSBends;

    bool      yokeFields;
    bool      yokeFieldsMatchLHCGeometry;
    bool      useOldMultipoleOuterFields;
    double    scalingFieldOuter;
    bool      integrateKineticEnergyAlongBeamline;
    
    std::string cavityFieldType;

    bool        includeFringeFields;
    bool        includeFringeFieldsCavities;

    ///@{ default beampipe parameters
    double      beampipeThickness;
    std::string apertureType;
    double      aper1;
    double      aper2;
    double      aper3;
    double      aper4;
    std::string beampipeMaterial;
    bool        ignoreLocalAperture;
    ///@}
    
    std::string vacMaterial;   ///< vacuum material
    std::string emptyMaterial; ///< material in container volumes
    std::string worldMaterial;
    std::string worldGeometryFile;
    bool        autoColourWorldGeometryFile;
    std::string importanceWorldGeometryFile;
    std::string importanceVolumeMap;
    // see verboseImportance

    double    worldVolumeMargin; ///< Padding margin for world volume size.

    double    vacuumPressure;
    
    ///@{ tunnel geometry parameters
    bool        buildTunnel;
    bool        buildTunnelStraight;
    std::string tunnelType;
    double      tunnelThickness;
    double      tunnelSoilThickness;
    std::string tunnelMaterial;
    std::string soilMaterial;
    bool        buildTunnelFloor;
    double      tunnelFloorOffset;
    double      tunnelAper1;
    double      tunnelAper2;
    bool        tunnelVisible;
    double      tunnelOffsetX;
    double      tunnelOffsetY;
    double      tunnelMaxSegmentLength;
    ///@}
    
    bool removeTemporaryFiles;
    std::string temporaryDirectory;
    
    // sampler options
    double   samplerDiameter;

    ///@{ Physics processes
    bool     turnOnOpticalAbsorption;
    bool     turnOnMieScattering;
    bool     turnOnRayleighScattering;
    bool     turnOnOpticalSurface;
    ///@}

    ///@{ physics parameters
    double   scintYieldFactor;
    int      maximumPhotonsPerStep;
    int      maximumBetaChangePerStep;
    long     maximumTracksPerEvent;
    double   minimumKineticEnergy;
    double   minimumKineticEnergyTunnel;
    double   minimumRange;
    std::string particlesToExcludeFromCuts;
    double   defaultRangeCut;
    double   prodCutPhotons;
    double   prodCutElectrons;
    double   prodCutPositrons;
    double   prodCutProtons;
    double   neutronTimeLimit;
    double   neutronKineticEnergyLimit;
    bool     useLENDGammaNuclear;
    bool     useElectroNuclear;
    bool     useMuonNuclear;
    bool     useGammaToMuMu;
    bool     usePositronToMuMu;
    bool     usePositronToHadrons;
    bool     restoreFTPFDiffractionForAGreater10;

    bool     beamPipeIsInfiniteAbsorber;
    bool     collimatorsAreInfiniteAbsorbers;
    bool     tunnelIsInfiniteAbsorber;
    int      muonSplittingFactor;
    double   muonSplittingThresholdParentEk;
    int      muonSplittingFactor2;
    double   muonSplittingThresholdParentEk2;
    bool     muonSplittingExcludeWeight1Particles;
    double   muonSplittingExclusionWeight;
    double   xrayAllSurfaceRoughness;
    ///@}

    // biasing options
    double    scaleFactorLaser;
    std::string defaultBiasVacuum;
    std::string defaultBiasMaterial;
    std::string biasForWorldVolume;
    std::string biasForWorldContents;
    std::string biasForWorldVacuum;
    std::string worldVacuumVolumeNames;

    // tracking related parameters
    std::string integratorSet;
    std::string fieldModulator;
    double   lengthSafety;
    double   lengthSafetyLarge;
    double   maximumTrackingTime; ///< Maximum tracking time per track [s].
    double   maximumStepLength;   ///< Maximum permitted step length in any volume.
    double   maximumTrackLength;  ///< Maximum permitted track length [m].
    double   chordStepMinimum;
    double   chordStepMinimumYoke;
    double   deltaIntersection;
    double   minimumEpsilonStep;
    double   maximumEpsilonStep;
    double   minimumEpsilonStepThin;
    double   maximumEpsilonStepThin;
    double   deltaOneStep;
    bool     stopSecondaries;
    bool     killNeutrinos;
    bool     killedParticlesMassAddedToEloss;
    double   minimumRadiusOfCurvature; ///< Minimum allowed radius of curvature.
    bool     sampleElementsWithPoleface;
    double   nominalMatrixRelativeMomCut; ///< Momentum threshold for nominal dipole matrix tracking.
    bool     teleporterFullTransform;     ///< Whether to use the new Transform3D method for the teleporter.
    double   dEThresholdForScattering;
    double   backupStepperMomLimit;    ///< Fractional momentum limit for reverting to backup steppers.

    // hit generation - only two parts that go in the same collection / branch
    bool      sensitiveOuter;
    bool      sensitiveBeamPipe;
    
    // output related options
    int         numberOfEventsPerNtuple;

    bool        storeMinimalData;

    bool        storeApertureImpacts;
    bool        storeApertureImpactsIons;
    bool        storeApertureImpactsAll;
    bool        storeApertureImpactsHistograms;
    double      apertureImpactsMinimumKE;
    bool        storeCavityInfo;
    bool        storeCollimatorInfo;
    bool        storeCollimatorHits;
    bool        storeCollimatorHitsLinks;
    bool        storeCollimatorHitsIons;
    bool        storeCollimatorHitsAll;
    double      collimatorHitsMinimumKE;
    bool        storeEloss;
    bool        storeElossHistograms;
    bool        storeElossVacuum;
    bool        storeElossVacuumHistograms;
    bool        storeElossTunnel;
    bool        storeElossTunnelHistograms;
    bool        storeElossWorld;
    bool        storeElossWorldIntegral;
    bool        storeElossWorldContents;
    bool        storeElossWorldContentsIntegral;
    bool        storeElossTurn;
    bool        storeElossLinks;
    bool        storeElossLocal;
    bool        storeElossGlobal;
    bool        storeElossTime;
    bool        storeElossStepLength;
    bool        storeElossPreStepKineticEnergy;
    bool        storeElossModelID;
    bool        storeElossPhysicsProcesses;
    bool        storeParticleData;
    bool        storePrimaries;
    bool        storePrimaryHistograms;
    
    bool        storeTrajectory;
    // filters
    int         storeTrajectoryDepth;
    int         storeTrajectoryStepPoints;
    bool        storeTrajectoryStepPointLast;
    std::string storeTrajectoryParticle;
    std::string storeTrajectoryParticleID;
    bool        storeTrajectorySecondaryParticles;
    double      storeTrajectoryEnergyThreshold;
    std::string storeTrajectorySamplerID;
    std::string storeTrajectoryELossSRange;

    // for trajectories stored, what data is stored
    bool        storeTrajectoryTransportationSteps;
    bool        trajNoTransportation;  ///< kept only for backwards compatibility.
    bool        storeTrajectoryKineticEnergy;
    bool        storeTrajectoryMomentumVector;
    bool        storeTrajectoryProcesses;
    bool        storeTrajectoryTime;
    bool        storeTrajectoryLocal;
    bool        storeTrajectoryLinks;
    bool        storeTrajectoryIon;
    bool        storeTrajectoryMaterial;
    bool        storeTrajectoryAllVariables;

    // filter logic
    bool        trajectoryFilterLogicAND;

    bool        storeSamplerAll;
    bool        storeSamplerPolarCoords;
    bool        storeSamplerCharge;
    bool        storeSamplerKineticEnergy;
    bool        storeSamplerMass;
    bool        storeSamplerRigidity;
    bool        storeSamplerIon;

    double      trajCutGTZ;
    double      trajCutLTR;
    bool        trajConnect;
    
    bool        storeModel;

    int         samplersSplitLevel;
    int         modelSplitLevel;
    int         uprootCompatible;

    // circular options
    int         nturns;
    std::string ptcOneTurnMapFileName;

    double   printFractionEvents;
    double   printFractionTurns;
    bool     printPhysicsProcesses;

    // visualisation
    int nSegmentsPerCircle; ///< Number of facets per 2pi in visualisation

    // scoring Map
    int    nbinsx;
    int    nbinsy;
    int    nbinsz;
    int    nbinse;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    double emin;
    double emax;
    bool   useScoringMap;

    /// print some properties
    void print() const;
  
    /// A list of all the keys that have been set in this instance.
    std::vector<std::string> setKeys;
  };
}

#endif
