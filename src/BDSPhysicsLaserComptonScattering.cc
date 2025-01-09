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
#include "BDSLaserComptonScattering.hh"
#include "BDSPhysicsLaserComptonScattering.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4Electron.hh"
#include "G4GenericIon.hh"
#include "G4Hydrogen.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Gamma.hh"
#include "G4Version.hh"

BDSPhysicsLaserComptonScattering::BDSPhysicsLaserComptonScattering():
  G4VPhysicsConstructor("BDSPhysicsLaserComptonScattering")
{;}

BDSPhysicsLaserComptonScattering::~BDSPhysicsLaserComptonScattering()
{;}

void BDSPhysicsLaserComptonScattering::ConstructParticle()
{
  G4Electron::ElectronDefinition();
  G4Gamma::Gamma();
}

void BDSPhysicsLaserComptonScattering::ConstructProcess()
{
  if (Activated())
    {return;}
  
  BDSLaserComptonScattering* laserComptonScattering = new BDSLaserComptonScattering();
  G4AutoDelete::Register(laserComptonScattering);
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  G4String name = laserComptonScattering->GetProcessName();
  while ((*aParticleIterator)())
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4int particleID = particle->GetPDGEncoding();
      if (abs(particleID)==11)
      {
	    G4ProcessManager* pmanager = particle->GetProcessManager();
	    pmanager->AddDiscreteProcess(laserComptonScattering);
        }
      if (particleID==18)
      {
        G4ProcessManager* pmanager = particle->GetProcessManager();
        pmanager->AddDiscreteProcess(laserComptonScattering);
        }
    }
  
  SetActivated();
}
