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
#include "BDSLaserPhotoDetachment.hh"
#include "BDSPhysicsLaserPhotoDetachment.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4Electron.hh"
#include "G4GenericIon.hh"
#include "G4Hydrogen.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Version.hh"

BDSPhysicsLaserPhotoDetachment::BDSPhysicsLaserPhotoDetachment():
  G4VPhysicsConstructor("BDSPhysicsLaserPhotoDetachment")
{;}

BDSPhysicsLaserPhotoDetachment::~BDSPhysicsLaserPhotoDetachment()
{;}

void BDSPhysicsLaserPhotoDetachment::ConstructParticle()
{
  G4Electron::ElectronDefinition();
  G4GenericIon::Definition();
  G4Hydrogen::Definition();
}

void BDSPhysicsLaserPhotoDetachment::ConstructProcess()
{
  if (Activated())
    {return;}
  
  BDSLaserPhotoDetachment* laserPhotoDetachment = new BDSLaserPhotoDetachment();
  G4AutoDelete::Register(laserPhotoDetachment);
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  G4String name = laserPhotoDetachment->GetProcessName();
  while ((*aParticleIterator)())
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      G4double atomic = particle->GetAtomicNumber();
      
      if (G4IonTable::IsIon(particle)&&atomic==1)
        {
	  G4ProcessManager* pmanager = particle->GetProcessManager();
	  pmanager->AddDiscreteProcess(laserPhotoDetachment);
        }
    }
  
  SetActivated();
}
