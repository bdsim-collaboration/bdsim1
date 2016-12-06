#ifndef BDSBENDBUILDER_H
#define BDSBENDBUILDER_H

#include "BDSMagnetType.hh"

#include "globals.hh" // geant4 globals / types

class BDSIntegratorSet;
class BDSLine;
class BDSMagnet;
class BDSMagnetStrength;

namespace GMAD
{
  struct Element;
}

namespace BDS
{  
  /// This calculates and constructs a BDSLine* of BDSMagnet*. The bend is split
  /// into multiple wedges. If a small or zero poleface angle is specified, the
  /// end wedges will have faces angled as appropriate, the remaining wedges will
  /// re-use a single identical 'central' wedge several times. For a stronger angled
  /// poleface, the faces of each wedge fade in/out from the poleface to the cental
  /// wedge in the middle. Thin fringefield elements are placed at the beginning and
  /// end of the beamline if required.
  BDSLine* BuildSBendLine(const GMAD::Element*     element,
			  BDSMagnetStrength* st,
			  const G4double     brho,
			  const BDSIntegratorSet* integratorSet);

  /// Construct beamline for an rbend.  A line is returned with a single
  /// magnet as the main dipole, but can have fringefield magnets placed
  /// either end if specified.
  BDSLine* BuildRBendLine(const GMAD::Element*    element,
			  const GMAD::Element*    prevElement,
			  const GMAD::Element*    nextElement,
			  G4double                angleIn,
			  G4double                angleOut,
			  const G4double          brho,
			  BDSMagnetStrength*      st,
			  const BDSIntegratorSet* integratorSet,
			  const G4double          charge);

  /// Utility function to calculate the number of segments an sbend should be split into.
  /// Based on aperture error tolerance - default is 1mm.
  G4int CalculateNSBendSegments(const G4double length,
				const G4double angle,
				const G4double e1 = 0,
				const G4double e2 = 0,
				const G4double aperturePrecision = 1.0);

  /// Thin magnet for dipole fringe field.
  /// Is beampipe only, no outer magnet.
  BDSMagnet* BuildDipoleFringe(const GMAD::Element* element,
			       G4double           angleIn,
			       G4double           angleOut,
			       G4String           name,
			       BDSMagnetType      magType,
			       BDSMagnetStrength* st,
			       G4double           brho,
			       const BDSIntegratorSet* integratorSet);

  /// Function to return a single secotr bend section.
  /// The faces of each are calculated as appropriate depending
  /// on the poleface angle(s).
  BDSMagnet* BuildSBend(const GMAD::Element* element,
			G4bool             fadeIn,
			G4bool             fadeOut,
			G4int              index,
			G4int              nSBends,
			BDSMagnetStrength* st,
			G4double           brho,
			const BDSIntegratorSet* integratorSet,
			const G4bool            yokeOnLeft);

  /// Function to calculate the value of the fringe field correction term.
  G4double CalculateFringeFieldCorrection(G4double rho,
					  G4double polefaceAngle,
					  G4double fint);
}

#endif
