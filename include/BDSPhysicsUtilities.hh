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
#ifndef BDSPHYSICSUTILITIES_H
#define BDSPHYSICSUTILITIES_H

#include "globals.hh"
#include "G4Version.hh"

#include "parser/fastlist.h"
#include "parser/physicsbiasing.h"

#include <set>
#include <string>
#include <utility>

class BDSParticleDefinition;
class G4DynamicParticle;
class G4GenericBiasingPhysics;
class G4ParticleDefinition;
class G4VModularPhysicsList;

#if G4VERSION_NUMBER > 1049
// lots of extra code to disable bad particle killing in Geant4.10.5 series
#include <utility>
class G4CoupledTransportation;
class G4Transportation;
#endif

namespace GMAD
{
  class Beam;
}

namespace BDS
{
  /// Whether a particle is an ion. A proton is counted NOT as an ion.
  G4bool IsIon(const G4ParticleDefinition* particle);

  /// Calls IsIon above but also a proton with any bound electrons is considered an ion.
  G4bool IsIon(const G4DynamicParticle* particle);
  
  /// Detect whether we're using a Geant4 provided physics list or whether we'll use the
  /// BDSIM modular physics and construct it.
  G4VModularPhysicsList* BuildPhysics(const G4String& physicsList, G4int verbosity = 1);

  /// Count how many out of the set of keys in a beam instance are set.
  G4int NBeamParametersSet(const GMAD::Beam&            beamDefinition,
                           const std::set<std::string>& keys);

  /// Throw an exception if too few or too many parameters are set for the supplied keys.
  void ConflictingParametersSet(const GMAD::Beam&            beamDefinition,
                                const std::set<std::string>& keys,
                                G4int                        nSet,
                                G4bool                       warnZeroParamsSet = true,
                                const G4String&              unitString = "");

  /// Construct the design and beam particle definitions. Even if these are the same, unique
  /// objects are created for and must be deleted elsewhere. Two pointers are passed by
  /// reference that will be updated with the allocated objects. The Boolean by reference
  /// argument is to tell whether they definitions (although unique objects) define the same
  /// particle.
  void ConstructDesignAndBeamParticle(const GMAD::Beam& beamDefinition,
				      G4double ffact,
				      BDSParticleDefinition*& designParticle,
				      BDSParticleDefinition*& beamParticle,
				      G4bool& beamDifferentFromDesignParticle);
  
  /// Construct particle definition. Ensure that particle is instantiated
  /// from a Geant4 point of view.  'ffact' is typically 1 or -1 used to flip
  /// the sign of the rigidity for difference between convention and what's required.
  /// Only one of totalEnergy, kineticEnergy and momentum should be non-zero.
  BDSParticleDefinition* ConstructParticleDefinition(const G4String& particleNameIn,
                                                     G4double totalEnergyIn,
                                                     G4double kineticEnergyIn,
                                                     G4double momentumIn,
						     G4double ffact = 1);

  /// Ensure required beam particle has been constructed for Geant4 purposes. Expects
  /// lowercase letters, not the Geant4 exact string.
  void ConstructBeamParticleG4(const G4String& name);
  
  /// Construct the minimum particle set required (gamma, electron, positron,
  /// proton and anti-proton.
  void ConstructMinimumParticleSet();
  
  /// Construct all leptons, pi+- K+- K. Utility function for
  /// when we expect other possible beam particles.
  void ConstructExtendedParticleSet();

  /// Print all the processes by name as registered to the primary particle type.
  /// Note, this should only be done after the physics lists are fully constructed.
  void PrintPrimaryParticleProcesses(const G4String& primaryParticleName);

  /// Print all constructed particle names. Note, this should only be done after the
  /// physics lists are fully constructed.
  void PrintDefinedParticles();
  
  G4GenericBiasingPhysics* BuildAndAttachBiasWrapper(const GMAD::FastList<GMAD::PhysicsBiasing>& biases);

  /// Build muon splitting biasing and wrap the various processes in the physics list.
  void BuildMuonBiasing(G4VModularPhysicsList* physicsList);

#if G4VERSION_NUMBER > 1039
  /// Build the physics required for channelling to work correctly.
  G4VModularPhysicsList* ChannellingPhysicsComplete(G4bool useEMD  = false,
						    G4bool regular = false,
						    G4bool em4     = false,
						    G4bool emss    = false);
#endif

  /// Set the range cuts on a physics list. This is split into a separate function to allow it
  /// to be applied to physics lists both from BDSIM's modular physics list and other sources
  /// with the same mechanism.
  void SetRangeCuts(G4VModularPhysicsList* physicsList, G4int verbosity = 1);

  /// Check if the user has requested a changed energy validity range and set the appropriate
  /// variables in the G4ProductionCutsTable.
  void CheckAndSetEnergyValidityRange();

#if G4VERSION_NUMBER > 1049
  /// Apply FixGeant105ThreshholdsForParticle to the beam particle definition.
  void FixGeant105ThreshholdsForBeamParticle(const BDSParticleDefinition* particleDefinition);

  /// Set 1keV, 10keV and 1500 for warning energy, important energy and number of trials in
  /// Geant4's looping thresholds.
  void FixGeant105ThreshholdsForParticle(const G4ParticleDefinition* particleDefinition);

  /// Taken from Geant4 field01 example. Set low values.
  void ChangeLooperParameters(const G4ParticleDefinition* particleDef);

  /// Taken from Geant4 field01 example. Get the two possible transportation processes.
  std::pair<G4Transportation*, G4CoupledTransportation*> FindTransportation(const G4ParticleDefinition* particleDef);
#endif
}

#endif
