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
#include "BDSBeamlineIntegral.hh"
#include "BDSCavityFieldType.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBeamlineIntegral::BDSBeamlineIntegral(const BDSParticleDefinition& incomingParticle,
                                         G4double T0In,
                                         G4double integratedArcLength,
                                         G4bool   integrateKineticEnergyIn):
  integrateKineticEnergy(integrateKineticEnergyIn),
  synchronousTAtEnd(T0In),
  synchronousTAtMiddleOfLastElement(T0In),
  arcLength(integratedArcLength),
  designParticle(incomingParticle),
  changeOfEnergyEncountered(false),
  rigidityCount(0)
{;}

BDSBeamlineIntegral::~BDSBeamlineIntegral()
{;}

void BDSBeamlineIntegral::Integrate(const GMAD::Element& componentAsDefined)
{
  // length
  G4double thisComponentArcLength = componentAsDefined.l*CLHEP::m;
  if (componentAsDefined.type == GMAD::ElementType::_RBEND)
    {// reuse component factor code to work out angle / field and true arc length
      G4double componentChordLength;
      G4double field;
      G4double angle;
      BDSComponentFactory::CalculateAngleAndFieldRBend(&componentAsDefined,
                                                       designParticle.BRho(),
                                                       thisComponentArcLength,
                                                       componentChordLength,
                                                       field,
                                                       angle);
    }
  arcLength += thisComponentArcLength;
  
  G4double v0 = designParticle.Velocity(); // current velocity at the entrance of element

  // calculate change in velocity and kinetic energy
  G4double dEk = 0;
  
  switch (componentAsDefined.type)
    {
    case GMAD::ElementType::_RF:
      {
        G4double particleCharge = designParticle.Charge();
        G4double phase = componentAsDefined.phase * CLHEP::rad;
        G4double frequency = componentAsDefined.frequency * CLHEP::hertz;
        // field model - reproduce behaviour in component factory of default in global constants
        G4String compCFTN = componentAsDefined.cavityFieldType;
        G4String cftName = compCFTN.empty() ? BDSGlobalConstants::Instance()->CavityFieldType() : compCFTN;
        BDSCavityFieldType tp = BDS::DetermineCavityFieldType(cftName);
        BDSFieldType fieldType = BDS::FieldTypeFromCavityFieldType(tp);
        G4double eField = BDSComponentFactory::EFieldFromElement(&componentAsDefined, fieldType, thisComponentArcLength, designParticle);
        switch (tp.underlying())
          {
          case BDSCavityFieldType::constantinz:
            {
              dEk = particleCharge * eField * thisComponentArcLength * std::cos(phase);
              break;
            }
          case BDSCavityFieldType::pillbox:
            {
              if (!BDS::IsFinite(frequency)) // protect against zero division
                {throw BDSException(__METHOD_NAME__, "for a pillbox cavity field, the frequency must be non-zero");}
              G4double transitTimeFactor = BDSFieldEMRFCavity::TransitTimeFactor(frequency, phase, thisComponentArcLength, designParticle.Beta());
              dEk = particleCharge * eField * thisComponentArcLength * transitTimeFactor;
              break;
            }
          case BDSCavityFieldType::axialstandingapprox:
            {
              //dEk = particleCharge *;
            }
          default:
            {break;}
          }
        break;
      }
    default:
      {break;} // no action
    }
  
  G4bool changeOfEnergyEncounteredThisTime = BDS::IsFinite(dEk, 1e-7);
  changeOfEnergyEncountered = changeOfEnergyEncountered || changeOfEnergyEncounteredThisTime;
  // count the number of times rigidity changes so we can uniquely name reused components
  if (changeOfEnergyEncounteredThisTime)
    {rigidityCount++;}
  
  // momentum and therefore BRho
  if (integrateKineticEnergy)
    {designParticle.ApplyChangeInKineticEnergy(dEk);}

  G4double v1 = designParticle.Velocity(); // velocity at the end
  G4double vMean = (v1 + v0) / 2.0;
  G4double dT = thisComponentArcLength / vMean;

  G4double dTMiddle = 0.5*thisComponentArcLength / (0.5*(v1-v0) + v0);
  synchronousTAtMiddleOfLastElement = synchronousTAtEnd + dTMiddle;
  
  // time - now at the start of the next component / end of this component
  synchronousTAtEnd += dT;
}

G4double BDSBeamlineIntegral::ProvideSynchronousTAtCentreOfNextElement(const GMAD::Element* el) const
{
  BDSBeamlineIntegral copy(*this);
  copy.Integrate(*el);
  G4double synchronousTAtMiddle = copy.synchronousTAtMiddleOfLastElement;
  return synchronousTAtMiddle;
}

G4double BDSBeamlineIntegral::ProvideSynchronousTAtEndOfNextElement(const GMAD::Element* el) const
{
  BDSBeamlineIntegral copy(*this);
  copy.Integrate(*el);
  G4double synchronousTAtEndLocal = copy.synchronousTAtEnd;
  return synchronousTAtEndLocal;
}
