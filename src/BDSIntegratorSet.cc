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
#include "BDSDebug.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Version.hh"

BDSIntegratorSet::BDSIntegratorSet(BDSIntegratorType solenoidIn,
                                   BDSIntegratorType dipoleIn,
                                   BDSIntegratorType dipoleQuadrupoleIn,
                                   BDSIntegratorType quadrupoleIn,
                                   BDSIntegratorType sextupoleIn,
                                   BDSIntegratorType octupoleIn,
                                   BDSIntegratorType decapoleIn,
                                   BDSIntegratorType multipoleThickIn,
                                   BDSIntegratorType muonSpoilerIn,
                                   BDSIntegratorType rfpillboxIn,
                                   BDSIntegratorType rfconstantinxIn,
                                   BDSIntegratorType rfconstantinyIn,
                                   BDSIntegratorType rfconstantinzIn,
                                   BDSIntegratorType generalIn,
                                   BDSIntegratorType skewQuadrupoleIn,
                                   BDSIntegratorType skewSextupoleIn,
                                   BDSIntegratorType skewOctupoleIn,
                                   BDSIntegratorType skewDecapoleIn,
                                   BDSIntegratorType dipoleFringeIn,
                                   BDSIntegratorType multipoleThinIn,
                                   BDSIntegratorType multipoleOuterIn,
                                   BDSIntegratorType rmatrixThinIn,
                                   BDSIntegratorType parallelTransporterIn,
                                   BDSIntegratorType undulatorIn,
                                   BDSIntegratorType cavityFringeIn,
                                   BDSIntegratorType gaborLensIn):
  solenoid(solenoidIn),
  dipole(dipoleIn),
  dipoleQuadrupole(dipoleQuadrupoleIn),
  quadrupole(quadrupoleIn),
  sextupole(sextupoleIn),
  octupole(octupoleIn),
  decapole(decapoleIn),
  multipoleThick(multipoleThickIn),
  muonSpoiler(muonSpoilerIn),
  rfpillbox(rfpillboxIn),
  rfconstantinx(rfconstantinxIn),
  rfconstantiny(rfconstantinyIn),
  rfconstantinz(rfconstantinzIn),
  general(generalIn),
  skewQuadrupole(skewQuadrupoleIn),
  skewSextupole(skewSextupoleIn),
  skewOctupole(skewOctupoleIn),
  skewDecapole(skewDecapoleIn),
  dipoleFringe(dipoleFringeIn),
  multipoleThin(multipoleThinIn),
  multipoleOuter(multipoleOuterIn),
  rmatrixThin(rmatrixThinIn),
  parallelTransporter(parallelTransporterIn),
  undulator(undulatorIn),
  cavityFringe(cavityFringeIn),
  gaborLens(gaborLensIn)
{
  isMatrix = false; //default
  // use dipolematrix integrator to check if matrix style as it is the
  // only integrator exclusive to matrix style integrator sets
  if (dipoleIn == BDSIntegratorType::dipolematrix)
    {isMatrix = true;}
}

namespace BDS
{  
  const BDSIntegratorSet* integratorsBDSIMOne =
    new BDSIntegratorSet(BDSIntegratorType::solenoid,         // solenoid
                         BDSIntegratorType::dipolerodrigues,  // dipole
                         BDSIntegratorType::dipolematrix,     // dipole quadrupole
                         BDSIntegratorType::quadrupole,       // quadrupole
                         BDSIntegratorType::sextupole,        // sextupole
                         BDSIntegratorType::octupole,         // octupole
                         BDSIntegratorType::decapole,         // decapole
                         BDSIntegratorType::g4classicalrk4,   // thick multipole
                         BDSIntegratorType::g4classicalrk4,   // muon spoiler
                         BDSIntegratorType::g4classicalrk4,   // rfpillbox
                         BDSIntegratorType::g4classicalrk4,   // rfconstantinx
                         BDSIntegratorType::g4classicalrk4,   // rfconstantiny
                         BDSIntegratorType::g4classicalrk4,   // rfconstantinz
                         BDSIntegratorType::g4classicalrk4,   // general
                         BDSIntegratorType::g4classicalrk4,   // skew quadrupole
                         BDSIntegratorType::g4classicalrk4,   // skew sextupole
                         BDSIntegratorType::g4classicalrk4,   // skew octupole
                         BDSIntegratorType::g4classicalrk4,   // skew decapole
                         BDSIntegratorType::dipolefringe,     // dipole fringe field
                         BDSIntegratorType::multipolethin,    // thin multipole
                         BDSIntegratorType::g4rk4minimumstep, // multipole outer
                         BDSIntegratorType::rmatrixthin,      // thin rmatrix
                         BDSIntegratorType::paralleltransport,// parallel transport
                         BDSIntegratorType::g4classicalrk4,   // undulator
                         BDSIntegratorType::cavityfringe,     // cavity fringe
                         BDSIntegratorType::g4classicalrk4);  // gabor lens

  const BDSIntegratorSet* integratorsBDSIMTwo =
    new BDSIntegratorSet(BDSIntegratorType::solenoid,         // solenoid
                         BDSIntegratorType::dipolerodrigues2, // dipole
                         BDSIntegratorType::dipolematrix,     // dipole quadrupole
                         BDSIntegratorType::quadrupole,       // quadrupole
                         BDSIntegratorType::euler,            // sextupole
                         BDSIntegratorType::euler,            // octupole
                         BDSIntegratorType::euler,            // decapole
                         BDSIntegratorType::g4classicalrk4,   // (thick) multipole
                         BDSIntegratorType::g4classicalrk4,   // muonspoiler
                         BDSIntegratorType::g4classicalrk4,   // rfpillbox
                         BDSIntegratorType::g4classicalrk4,   // rfconstantinx
                         BDSIntegratorType::g4classicalrk4,   // rfconstantiny
                         BDSIntegratorType::g4classicalrk4,   // rfconstantinz
                         BDSIntegratorType::g4classicalrk4,   // general
                         BDSIntegratorType::g4classicalrk4,   // skew quadrupole
                         BDSIntegratorType::g4classicalrk4,   // skew sextupole
                         BDSIntegratorType::g4classicalrk4,   // skew octupole
                         BDSIntegratorType::g4classicalrk4,   // skew decapole
                         BDSIntegratorType::dipolefringe,     // dipole fringe field
                         BDSIntegratorType::multipolethin,    // thin multipole
                         BDSIntegratorType::g4rk4minimumstep, // multipole outer
                         BDSIntegratorType::rmatrixthin,      // thin rmatrix
                         BDSIntegratorType::paralleltransport,// parallel transport
                         BDSIntegratorType::g4classicalrk4,   // undulator
                         BDSIntegratorType::cavityfringe,     // cavity fringe
                         BDSIntegratorType::g4classicalrk4);  // gabor lens
  /// Mad-x style tracking.
  const BDSIntegratorSet*  integratorsBDSIMMatrix =
    new BDSIntegratorSet(BDSIntegratorType::solenoid,            // solenoid
                         BDSIntegratorType::dipolematrix,        // dipole
                         BDSIntegratorType::dipolematrix,        // dipole quadrupole
                         BDSIntegratorType::quadrupole,          // quadrupole
                         BDSIntegratorType::euler,               // sextupole
                         BDSIntegratorType::euler,               // octupole
                         BDSIntegratorType::euler,               // decapole
                         BDSIntegratorType::g4classicalrk4,      // thick multipole
                         BDSIntegratorType::g4classicalrk4,      // muon spoiler
                         BDSIntegratorType::g4classicalrk4,      // rfpillbox
                         BDSIntegratorType::g4classicalrk4,      // rfconstantinx
                         BDSIntegratorType::g4classicalrk4,      // rfconstantiny
                         BDSIntegratorType::g4classicalrk4,      // rfconstantinz
                         BDSIntegratorType::g4classicalrk4,      // general
                         BDSIntegratorType::g4classicalrk4,      // skew quadrupole
                         BDSIntegratorType::g4classicalrk4,      // skew sextupole
                         BDSIntegratorType::g4classicalrk4,      // skew octupole
                         BDSIntegratorType::g4classicalrk4,      // skew decapole
                         BDSIntegratorType::dipolefringe,        // dipole fringe field
                         BDSIntegratorType::multipolethin,       // thin multipole
                         BDSIntegratorType::g4rk4minimumstep,    // multipole outer
                         BDSIntegratorType::rmatrixthin,         // thin rmatrix
                         BDSIntegratorType::paralleltransport,   // parallel transport
                         BDSIntegratorType::g4classicalrk4,      // undulator
                         BDSIntegratorType::cavityfringe,        // cavity fringe
                         BDSIntegratorType::g4classicalrk4);     // gabor lens
  const BDSIntegratorSet*  integratorsBDSIMMatrixFringeScaling =
    new BDSIntegratorSet(BDSIntegratorType::solenoid,            // solenoid
                         BDSIntegratorType::dipolematrix,        // dipole
                         BDSIntegratorType::dipolematrix,        // dipole quadrupole
                         BDSIntegratorType::quadrupole,          // quadrupole
                         BDSIntegratorType::euler,               // sextupole
                         BDSIntegratorType::euler,               // octupole
                         BDSIntegratorType::euler,               // decapole
                         BDSIntegratorType::g4classicalrk4,      // thick multipole
                         BDSIntegratorType::g4classicalrk4,      // muon spoiler
                         BDSIntegratorType::g4classicalrk4,      // rfpillbox
                         BDSIntegratorType::g4classicalrk4,      // rfconstantinz
                         BDSIntegratorType::g4classicalrk4,      // rfconstantiny
                         BDSIntegratorType::g4classicalrk4,      // rfconstantinz
                         BDSIntegratorType::g4classicalrk4,      // general
                         BDSIntegratorType::g4classicalrk4,      // skew quadrupole
                         BDSIntegratorType::g4classicalrk4,      // skew sextupole
                         BDSIntegratorType::g4classicalrk4,      // skew octupole
                         BDSIntegratorType::g4classicalrk4,      // skew decapole
                         BDSIntegratorType::dipolefringescaling, // dipole fringe field
                         BDSIntegratorType::multipolethin,       // thin multipole
                         BDSIntegratorType::g4rk4minimumstep,    // multipole outer - nystrom doesn't work in g4.10.5
                         BDSIntegratorType::rmatrixthin,         // thin rmatrix
                         BDSIntegratorType::paralleltransport,   // parallel transport
                         BDSIntegratorType::g4classicalrk4,      // undulator
                         BDSIntegratorType::cavityfringe,        // cavity fringe
                         BDSIntegratorType::g4classicalrk4);     // gabor lens
  const BDSIntegratorSet* integratorsGeant4 =
    new BDSIntegratorSet(BDSIntegratorType::g4classicalrk4,    // solenoid
                         BDSIntegratorType::g4classicalrk4,    // dipole
                         BDSIntegratorType::g4classicalrk4,    // dipole quadrupole
                         BDSIntegratorType::g4classicalrk4,    // quadrupole
                         BDSIntegratorType::g4classicalrk4,    // sextupole
                         BDSIntegratorType::g4classicalrk4,    // octupole
                         BDSIntegratorType::g4classicalrk4,    // decapole
                         BDSIntegratorType::g4classicalrk4,    // thick multipole
                         BDSIntegratorType::g4classicalrk4,    // muon spoiler
                         BDSIntegratorType::g4classicalrk4,    // rfpillbox
                         BDSIntegratorType::g4classicalrk4,    // rfconstantinx
                         BDSIntegratorType::g4classicalrk4,    // rfconstantiny
                         BDSIntegratorType::g4classicalrk4,    // rfconstantinz
                         BDSIntegratorType::g4classicalrk4,    // general
                         BDSIntegratorType::g4classicalrk4,    // skew quadrupole
                         BDSIntegratorType::g4classicalrk4,    // skew sextupole
                         BDSIntegratorType::g4classicalrk4,    // skew octupole
                         BDSIntegratorType::g4classicalrk4,    // skew decapole
                         BDSIntegratorType::dipolefringe,      // dipole fringe field
                         BDSIntegratorType::multipolethin,     // thin multipole
                         BDSIntegratorType::g4rk4minimumstep,  // multipole outer
                         BDSIntegratorType::rmatrixthin,       // thin rmatrix
                         BDSIntegratorType::paralleltransport, // parallel transport
                         BDSIntegratorType::g4classicalrk4,    // undulator
                         BDSIntegratorType::cavityfringe,      // cavity fringe
                         BDSIntegratorType::g4classicalrk4);   // gabor lens
#if G4VERSION_NUMBER > 1039
  const BDSIntegratorSet* integratorsGeant4DP =
    new BDSIntegratorSet(BDSIntegratorType::g4dormandprince745, // solenoid
                         BDSIntegratorType::g4dormandprince745, // dipole
                         BDSIntegratorType::g4dormandprince745, // dipole quadrupole
                         BDSIntegratorType::g4dormandprince745, // quadrupole
                         BDSIntegratorType::g4dormandprince745, // sextupole
                         BDSIntegratorType::g4dormandprince745, // octupole
                         BDSIntegratorType::g4dormandprince745, // decapole
                         BDSIntegratorType::g4dormandprince745, // thick multipole
                         BDSIntegratorType::g4dormandprince745, // muon spoiler
                         BDSIntegratorType::g4dormandprince745, // rfpillbox
                         BDSIntegratorType::g4dormandprince745, // rfconstantinx
                         BDSIntegratorType::g4dormandprince745, // rfconstantiny
                         BDSIntegratorType::g4dormandprince745, // rfconstantinz
                         BDSIntegratorType::g4dormandprince745, // general
                         BDSIntegratorType::g4dormandprince745, // skew quadrupole
                         BDSIntegratorType::g4dormandprince745, // skew sextupole
                         BDSIntegratorType::g4dormandprince745, // skew octupole
                         BDSIntegratorType::g4dormandprince745, // skew decapole
                         BDSIntegratorType::dipolefringe,       // dipole fringe field
                         BDSIntegratorType::multipolethin,      // thin multipole
                         BDSIntegratorType::g4dormandprince745, // multipole outer
                         BDSIntegratorType::rmatrixthin,        // thin rmatrix
                         BDSIntegratorType::paralleltransport,  // parallel transport
                         BDSIntegratorType::g4dormandprince745, // undulator
                         BDSIntegratorType::cavityfringe,       // cavity fringe
                         BDSIntegratorType::g4classicalrk4);    // gabor lens
#endif
}

const BDSIntegratorSet* BDS::IntegratorSet(G4String sets)
{
  return BDS::IntegratorSet(BDS::DetermineIntegratorSetType(sets));
}

const BDSIntegratorSet* BDS::IntegratorSet(BDSIntegratorSetType set)
{
  switch (set.underlying())
    {
    case BDSIntegratorSetType::geant4:
      {return BDS::integratorsGeant4; break;}
#if G4VERSION_NUMBER > 1039
    case BDSIntegratorSetType::geant4dp:
      {return BDS::integratorsGeant4DP; break;}
#endif
    case BDSIntegratorSetType::bdsimone:
      {return BDS::integratorsBDSIMOne;  break;}
    case BDSIntegratorSetType::bdsimtwo:
      {return BDS::integratorsBDSIMTwo;}
    case BDSIntegratorSetType::bdsimmatrix:
      {return BDS::integratorsBDSIMMatrix;}
    case BDSIntegratorSetType::bdsimmatrixfringescaling:
      {return BDS::integratorsBDSIMMatrixFringeScaling;}
    default:
      {return BDS::integratorsBDSIMOne;  break;}
    }
}

BDSIntegratorType BDSIntegratorSet::Integrator(const BDSFieldType field) const
{
  switch (field.underlying())
    {
    case BDSFieldType::none:
      {return general; break;}// shouldn't really happen, but for completeness.
    case BDSFieldType::bmap1d:
    case BDSFieldType::bmap2d:
    case BDSFieldType::bmap3d:
    case BDSFieldType::bmap4d:
    case BDSFieldType::ebmap1d:
    case BDSFieldType::ebmap2d:
    case BDSFieldType::ebmap3d:
    case BDSFieldType::ebmap4d:
    case BDSFieldType::emap1d:
    case BDSFieldType::emap2d:
    case BDSFieldType::emap3d:
    case BDSFieldType::emap4d:
    case BDSFieldType::mokka:
      {
        G4cout << __METHOD_NAME__ << "WARNING - this is overriding the specified field maps integrator" << G4endl;
        return general;
        break;
      }
    case BDSFieldType::solenoid:
      {return solenoid;         break;}
    case BDSFieldType::dipole:
      {return dipole;           break;}
    case BDSFieldType::dipolequadrupole:
      {return dipoleQuadrupole; break;}
    case BDSFieldType::quadrupole:
      {return quadrupole;       break;}
    case BDSFieldType::sextupole:
      {return sextupole;        break;}
    case BDSFieldType::octupole:
      {return octupole;         break;}
    case BDSFieldType::decapole:
      {return decapole;         break;}
    case BDSFieldType::multipole:
      {return multipoleThick;   break;}
    case BDSFieldType::muonspoiler:
      {return muonSpoiler;      break;}
    case BDSFieldType::skewquadrupole:
      {return skewQuadrupole;   break;}
    case BDSFieldType::skewsextupole:
      {return skewSextupole;    break;}
    case BDSFieldType::skewoctupole:
      {return skewOctupole;     break;}
    case BDSFieldType::skewdecapole:
      {return skewDecapole;     break;}
    case BDSFieldType::rfpillbox:
      {return rfpillbox;        break;}
    case BDSFieldType::rfconstantinx:
      {return rfconstantinx;    break;}
    case BDSFieldType::rfconstantiny:
      {return rfconstantiny;    break;}
    case BDSFieldType::rfconstantinz:
      {return rfconstantinz;    break;}
    case BDSFieldType::rmatrix:
      {return rmatrixThin;      break;}
    case BDSFieldType::paralleltransporter:
      {return parallelTransporter; break;}
    case BDSFieldType::cavityfringe:
      {return cavityFringe;     break;}
    case BDSFieldType::gaborlens:
      {return gaborLens;        break;}
    case BDSFieldType::undulator:
      {return undulator;        break;}
    case BDSFieldType::dipole3d:
      {return general;          break;}
    case BDSFieldType::multipoleouterdipole:
    case BDSFieldType::multipoleouterquadrupole:
    case BDSFieldType::multipoleoutersextupole:
    case BDSFieldType::multipoleouteroctupole:
    case BDSFieldType::multipoleouterdecapole:
    case BDSFieldType::skewmultipoleouterquadrupole:
    case BDSFieldType::skewmultipoleoutersextupole:
    case BDSFieldType::skewmultipoleouteroctupole:
    case BDSFieldType::skewmultipoleouterdecapole:
    case BDSFieldType::multipoleouterdipole3d:
    case BDSFieldType::multipoleouterdipolelhc:
    case BDSFieldType::multipoleouterquadrupolelhc:
    case BDSFieldType::multipoleoutersextupolelhc:
      {return multipoleOuter;   break;}
    default:
      {return general;          break;}
    }
}
