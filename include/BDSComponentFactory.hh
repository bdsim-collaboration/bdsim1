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
#ifndef BDSCOMPONENTFACTORY_H
#define BDSCOMPONENTFACTORY_H

#include "BDSBeamlineIntegral.hh"
#include "BDSFieldType.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSIntegratorType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSParticleDefinition.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <map>

class G4Colour;
class G4Material;

namespace GMAD
{
  struct Element;
}
class BDSAcceleratorComponent;
class BDSBeamPipeInfo;
class BDSCavityInfo;
class BDSComponentFactoryUser;
class BDSCrystalInfo;
class BDSFieldInfo;
class BDSIntegratorSet;
class BDSLaser;
class BDSMagnet;
class BDSMagnetOuterInfo;
class BDSModulatorInfo;
class BDSTiltOffset;

/**
 * @brief Factory to produce all types of BDSAcceleratorComponents.
 * 
 * Creates from a parser Element the appropriate object (that inherits 
 * BDSAcceleratorComponent) and returns it. Will return nullptr if invalid 
 * type or nothing to be constructed for that particular type. Calculations 
 * on field strength and angle as well as basic parameter validity (minimum 
 * length) are done here.
 *
 * The main interface requires both a previous and next GMAD::Element that the
 * element is constructed with respect to. This is so that drifts can be matched
 * to the faces of bends with pole face rotations etc.  If required in future, a
 * simple interface for a single element could be added that uses this one with 
 * nullptrs.
 * 
 * Functions for common functionality, checks and preparation of recipe classes 
 * are provided publicly (and statically) for use elsewhere.
 */

class BDSComponentFactory
{
public:
  explicit BDSComponentFactory(BDSComponentFactoryUser* userComponentFactoryIn = nullptr,
                               G4bool usualPrintOut = true);
  ~BDSComponentFactory();

  /// Create component from parser Element pointers to next and previous Element
  /// are used to ensure the component will fit and may optionally be made to be
  /// so in the case of a drift.  The optional last argument is the current s
  /// position at the end of the beam line for components that are distance
  /// or phase dependent such as an RF cavity.
  BDSAcceleratorComponent* CreateComponent(GMAD::Element const* elementIn,
					   GMAD::Element const* prevElementIn,
					   GMAD::Element const* nextElementIn,
					   BDSBeamlineIntegral& integral,
                       G4double currentArcLength = 0);
  
  /// Public creation for object that dynamically stops all particles once the primary
  /// has completed a certain number of turns.
  BDSAcceleratorComponent* CreateTerminator(G4double witdth);

  /// Public creation for object that accounts for slight offset between ends of a ring.
  /// The z component of the delta three vector is used for the length of the teleporter.
  BDSAcceleratorComponent* CreateTeleporter(const G4double       teleporterLength,
					    const G4double       teleporterWidth,
					    const G4Transform3D& transformIn);

  /// Create the tilt and offset information object by inspecting the parser element
  static BDSTiltOffset*    CreateTiltOffset(GMAD::Element const* el);

  /// Create a transform from a tilt offset.  If nullptr, returns identity transform.
  static G4Transform3D CreateFieldTransform(const BDSTiltOffset* tiltOffset);

  /// Create a transform for the field for a given element to account for the difference
  /// from the curvilinear coordinates for the tilt and offset of the magnet.
  static G4Transform3D CreateFieldTransform(GMAD::Element const* el);

  /// Prepare the recipe for a piece of beam pipe. Static and public so it can be used by
  /// SBendBuilder.
  static BDSBeamPipeInfo* PrepareBeamPipeInfo(GMAD::Element const* el,
					      const G4ThreeVector& inputFaceNormal  = G4ThreeVector(0,0,-1),
					      const G4ThreeVector& outputFaceNormal = G4ThreeVector(0,0,1));

  /// Interface to other PrepareBeamPipeInfo() for convenience to avoid preparing
  /// face normal vectors repeatedly.
  static BDSBeamPipeInfo* PrepareBeamPipeInfo(GMAD::Element const* el,
					      G4double angleIn,
					      G4double angleOut);

  /// Determine which side the yoke of an asymmetric bend should go on based on the angle
  /// of the bend and the overriding setting in the element.
  static G4bool YokeOnLeft(const GMAD::Element*     el,
			   const BDSMagnetStrength* st);

  /// Prepare the element horizontal width in Geant4 units - if not set, use the global default.
  static G4double PrepareHorizontalWidth(GMAD::Element const* el,
					 G4double defaultHorizontalWidth = -1);

  /// Get the scaling factor for a particular outer field depending on the global and individual setting.
  static G4double ScalingFieldOuter(const GMAD::Element* ele);

  /// Prepare the field definition for the yoke of a magnet.
  static BDSFieldInfo* PrepareMagnetOuterFieldInfo(const BDSMagnetStrength*  vacuumSt,
						   const BDSFieldType&       fieldType,
						   const BDSBeamPipeInfo*    bpInfo,
						   const BDSMagnetOuterInfo* outerInfo,
						   const G4Transform3D&      fieldTransform,
						   const BDSIntegratorSet*   integratorSetIn,
						   G4double                  brhoIn,
						   G4double                  outerFieldScaling = 1.0,
                                                   BDSModulatorInfo*         modulatorInfo = nullptr);
  
  /// Prepare the recipe for magnet outer geometry for an element. This uses a
  /// strength instance which (we assume) represents the element. Evenly splits angle
  /// between input and output faces.
  static BDSMagnetOuterInfo* PrepareMagnetOuterInfo(const G4String&          elementNameIn,
						    const GMAD::Element*     el,
						    const BDSMagnetStrength* st,
						    const BDSBeamPipeInfo*   beamPipe,
						    G4double defaultHorizontalWidth    = -1,
						    G4double defaultVHRatio            = 1.0,
						    G4double defaultCoilWidthFraction  = -1,
						    G4double defaultCoilHeightFraction = -1);

  /// Determine the magnet geometry type. If not specified or the global option to ignore
  /// local magnet geometry definitions is on, then the global default is used
  static BDSMagnetGeometryType MagnetGeometryType(const GMAD::Element* el);

  /// Prepare the recipe for magnet outer geometry with full control of the angled faces
  /// and which side the yoke is on. The angle in and out are the face angles relative
  /// to a chord for a straight section of outer magnet geometry.
  static BDSMagnetOuterInfo* PrepareMagnetOuterInfo(const G4String&        elementNameIn,
						    const GMAD::Element*   el,
						    const G4double         angleIn,
						    const G4double         angleOut,
						    const BDSBeamPipeInfo* beamPipe,
						    const G4bool   yokeOnLeft                = false,
						    G4double       defaultHorizontalWidth    = -1,
						    G4double       defaultVHRatio            = -1,
						    G4double       defaultCoilWidthFraction  = -1,
						    G4double       defaultCoilHeightFraction = -1);

  /// Checks if colour is specified for element, else uses the default for that element type.
  /// If the optional material is given and also element->autoColour is true then the material
  /// colouring is used.
  static G4Colour* PrepareColour(GMAD::Element const* element, const G4Material* material = nullptr);

  /// Checks if a material is named in Element::material, else uses the supplied default.
  static G4Material* PrepareMaterial(GMAD::Element const* element,
				     const G4String& defaultMaterialName);

  /// Try and get a material and exit if no such material.
  static G4Material* PrepareMaterial(GMAD::Element const* element);

  /// Utility function to check if the combination of horizontal width, angle and length
  /// will result in overlapping entrance and exit faces and therefore whether to abort.
  static void CheckBendLengthAngleWidthCombo(G4double arcLength,
					     G4double angle,
					     G4double horizontalWidth,
					     const G4String& name = "not given");

  /// Check whether the pole face rotation angles are too big for practical construction.
  static void PoleFaceRotationsNotTooLarge(const GMAD::Element* el,
					   G4double       maxAngle = 0.5*CLHEP::halfpi);

  /// Get either the "gradient" member or the voltage and divide by the cavityLength
  /// argument (provided in case of reduced length) to get the E field in Geant4 units.
  /// BRho is required to ensure the field is accelerating for the given particle. This
  /// is a static function so we can't use the member variable integral.
  static G4double EFieldFromElement(GMAD::Element const* el,
                                    BDSFieldType fieldType,
                                    G4double cavityLength,
                                    const BDSParticleDefinition& incomingParticle);

  /// Calculate the field and angle of an rbend from information in the element noting the
  /// 'l' in an element is the chord length of an rbend. Variables passed by reference and
  /// are updated as output. Note, this uses the MADX convention of +ve angle -> deflection
  /// in -ve x.
  static void CalculateAngleAndFieldRBend(const GMAD::Element* el,
                                          G4double brhoIn,
                                          G4double& arcLength,
                                          G4double& chordLength,
                                          G4double& field,
                                          G4double& angle);

  /// Utility function to prepare crystal recipe for an element. Produces a unique object
  /// this class doesn't own.
  BDSCrystalInfo* PrepareCrystalInfo(const G4String& crystalName) const;

private:
  /// No default constructor
  BDSComponentFactory() = delete;

  BDSComponentFactoryUser* userComponentFactory; ///< User component factory if any.
  G4double lengthSafety;      ///< Length safety from global constants.
  G4double thinElementLength; ///< Length of a thin element.
  G4bool includeFringeFields; ///< Cache of whether to include fringe fields.
  G4bool yokeFields;          ///< Cache of whether to include yoke magnetic fields.
  BDSModulatorInfo* defaultModulator; ///< Default modulator for all components.
  BDSBeamlineIntegral* integralUpToThisComponent; ///< To save passing it through many functions arguments.
  G4double synchronousTAtMiddleOfThisComponent;

  /// Simple setter used to add Beta0 to a strength instance.
  inline void SetBeta0(BDSMagnetStrength* stIn) const {(*stIn)["beta0"] = integralUpToThisComponent->designParticle.Beta();}
  /// Simple accessor to simplify repetitive code.
  inline G4double BRho() const {return integralUpToThisComponent->designParticle.BRho();}

  /// element for storing instead of passing around
  GMAD::Element const* element = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element const* prevElement = nullptr;
  /// element access to previous element (can be nullptr)
  GMAD::Element const* nextElement = nullptr;
  
  /// Private enum for kicker types.
  enum class KickerType {horizontal, vertical, general};
  
  /// Private enum for RF cavity principle accelerating direction
  enum class RFFieldDirection {x, y, z};

  BDSAcceleratorComponent* CreateDrift(G4double angleIn, G4double angleOut);
  BDSAcceleratorComponent* CreateRF(RFFieldDirection direction);
  BDSAcceleratorComponent* CreateSBend();
  BDSAcceleratorComponent* CreateRBend();
  BDSAcceleratorComponent* CreateKicker(KickerType type);
  BDSAcceleratorComponent* CreateQuad();
  BDSAcceleratorComponent* CreateSextupole();
  BDSAcceleratorComponent* CreateOctupole();
  BDSAcceleratorComponent* CreateDecapole();
  BDSAcceleratorComponent* CreateMultipole();
  BDSAcceleratorComponent* CreateThinMultipole(G4double angleIn);
  BDSAcceleratorComponent* CreateElement();
  BDSAcceleratorComponent* CreateSolenoid();
  BDSAcceleratorComponent* CreateParallelTransporter();
  BDSAcceleratorComponent* CreateRectangularCollimator();
  BDSAcceleratorComponent* CreateTarget();
  BDSAcceleratorComponent* CreateEllipticalCollimator();
  BDSAcceleratorComponent* CreateJawCollimator();
  BDSAcceleratorComponent* CreateMuonSpoiler();
  BDSAcceleratorComponent* CreateShield();
  BDSAcceleratorComponent* CreateDegrader();
  BDSAcceleratorComponent* CreateGap();
  BDSAcceleratorComponent* CreateCrystalCollimator();
  BDSAcceleratorComponent* CreateLaser();
  BDSAcceleratorComponent* CreateScreen();
  BDSAcceleratorComponent* CreateTransform3D();
  BDSAcceleratorComponent* CreateWireScanner();
  BDSAcceleratorComponent* CreateRMatrix();
  BDSAcceleratorComponent* CreateThinRMatrix(G4double        angleIn,
					     const G4String& name);
  BDSAcceleratorComponent* CreateThinRMatrix(G4double angleIn,
					     BDSMagnetStrength*       stIn,
					     const G4String&          name,
					     BDSIntegratorType        intType = BDSIntegratorType::rmatrixthin,
					     BDSFieldType             fieldType = BDSFieldType::rmatrix,
					     G4double                 beamPipeRadius = 0,
					     BDSModulatorInfo*        fieldModulator = nullptr);
  BDSAcceleratorComponent* CreateUndulator();
  BDSAcceleratorComponent* CreateDump();
  BDSAcceleratorComponent* CreateLaserwire(G4double currentArcLength);

#ifdef USE_DICOM
  BDSAcceleratorComponent* CreateCT();
#endif
  BDSAcceleratorComponent* CreateCavityFringe(G4double                 angleIn,
					      BDSMagnetStrength*       stIn,
					      const G4String&          name,
					      G4double                 irisRadius,
					      BDSModulatorInfo*        fieldModulator = nullptr);

#ifdef USE_AWAKE
  BDSAcceleratorComponent* CreateAwakeScreen();
  BDSAcceleratorComponent* CreateAwakeSpectrometer();
#endif

  /// Helper method for common magnet construction
  BDSMagnet* CreateMagnet(const GMAD::Element* el,
			  BDSMagnetStrength*   st,
			  BDSFieldType         fieldType,
			  BDSMagnetType        magnetType,
			  G4double             angle = 0.0,
			  const G4String&      nameSuffix = "") const;

  /// Test the component length is sufficient for practical construction.
  G4bool HasSufficientMinimumLength(GMAD::Element const* el,
				    G4bool               printWarning = true);

  /// Prepare the vacuum material from the element or resort to default in options.
  G4Material* PrepareVacuumMaterial(GMAD::Element const* el) const;
  
  /// Prepare all RF cavity models in the component factory. Kept here and copies delivered.
  /// This class deletes them upon destruction.
  void PrepareCavityModels();

  /// Prepare all colours defined in the parser.
  void PrepareColours();

  /// Prepare all crystals in defined the parser.
  void PrepareCrystals();

  /// Prepare all lasers defined in the parser.
  void PrepareLasers();

  /// Utility function to prepare laser. Prepares a unique object this class
  /// doesn't own.
  BDSLaser* PrepareLaser(GMAD::Element const* el) const;

  /// Utility function to prepare model info. Retrieve from cache of ones translated
  /// parser objects or create a default based on the element's aperture if none specified.
  /// Will always return a unique object that's not owned by this class. We need the
  /// frequency in case there are no good defaults and we'll make the cavity size
  /// according to the wavelength of the rf field.
  BDSCavityInfo* PrepareCavityModelInfo(GMAD::Element const* el,
					G4double             frequency) const;

  /// Create a default cavity model based on an element's aperture and material.
  /// Will always return a unique object that's not owned by this class.
  BDSCavityInfo* PrepareCavityModelInfoForElement(GMAD::Element const* el,
						  G4double             frequency) const;

  /// Utility function to prepare field strength object for rf cavity. This takes a pointer
  /// for both incoming and outgoing strengths that this function will allocate by reference.
  BDSMagnetStrength* PrepareCavityStrength(GMAD::Element const* el,
                                           BDSFieldType         fieldType,
					   G4double             cavityLength,
					   BDSMagnetStrength*&  fringeIn,
					   BDSMagnetStrength*&  fringeOut) const;

  /// Set the field definition on a BDSAcceleratorComponent from the string definition
  /// name in a parser element. In the case of a BDSMagnet, (exclusively) set the vacuum
  /// and outer field in place of the one general field.
  void SetFieldDefinitions(GMAD::Element const* el,
                           BDSAcceleratorComponent* component) const;

  /// Check the info doesn't already have a modulator definition (exception if it does).
  /// and update it.
  void SetModulatorDefinition(GMAD::Element const* el,
                              BDSFieldInfo* info) const;
  
  /// Prepare magnet strength for multipoles
  BDSMagnetStrength* PrepareMagnetStrengthForMultipoles(GMAD::Element const* el) const;

  /// Prepare magnet strength for rmatrix
  BDSMagnetStrength* PrepareMagnetStrengthForRMatrix(GMAD::Element const* el) const;

  /// Map of cavity model info instances by name
  std::map<G4String, BDSCavityInfo*> cavityInfos;

  /// Maps of crystal info instances by name.
  std::map<G4String, BDSCrystalInfo*> crystalInfos;

  /// Map of laser instances by name. Owned by this class.
  std::map<G4String, BDSLaser*> lasers;

  /// Local copy of reference to integrator set to use.
  const BDSIntegratorSet* integratorSet;

  /// Local copy of enum of the integrator set, i.e. which one it is specifically.
  const BDSIntegratorSetType integratorSetType;

  /// Calculate the field from a given angle through a length of field - uses member
  /// rigidity and charge. Length & angle in g4 m / rad units.
  G4double FieldFromAngle(const G4double angle,
			  const G4double arcLength) const;

  /// Calculate the angle through a length of field - uses member
  /// rigidity and charge. Length & field in g4 m / tesla units
  G4double AngleFromField(const G4double field,
			  const G4double arcLength) const;

  /// Calculate field and angle of a sector bend. Note, this uses the MADX convention
  /// of +ve angle -> deflection in -ve x.
  void CalculateAngleAndFieldSBend(GMAD::Element const* el,
				   G4double&            angle,
				   G4double&            field) const;

  /// Calculate the angle of a bend whether it's an rbend or an sbend.
  G4double BendAngle(const GMAD::Element* el) const;

  /// Calculate the outgoing angle of the face (in the horizontal plane) for
  /// a given element. Calculates the bend angle and applies e1 / e2 pole face
  /// rotations the correct way depending on sign of angle. The angle is w.r.t.
  /// outgoing curvilinear coordinates, so for an rbend with e2=0, the returned
  /// angle will be half the bend angle. For an sbend, with e2=0, it'll be 0.
  G4double OutgoingFaceAngle(const GMAD::Element* el) const;

  /// Calculate the incoming angel of the face (in the horizontal plane) for
  /// a given element. Calculates the bend angle and applies e1 / e2 pole face
  /// rotations the correct way depending on sign of angle. The angle is w.r.t.
  /// incoming curvilinear coordinates, so for an rbend with e1=0, the returned
  /// angle will be half the bend angle. For an sbend, with e1=0, it'll be 0.
  G4double IncomingFaceAngle(const GMAD::Element* el) const;

  /// Return the modulator definition for a given element if one is specified
  /// in fieldModulator, else return the global default which could also be nullptr.
  BDSModulatorInfo* ModulatorDefinition(const GMAD::Element* el, G4bool inDevelopment=false) const; // TBC

  /// TBC - remove when modulators are implemented fully.
  void INDEVELOPMENTERROR() const;

  /// Pull out the right value - either 'kick' or 'h/vkick' for the appropriate
  /// type of kicker from the current member element.
  void GetKickValue(G4double& hkick,
		    G4double& vkick,
		    const KickerType type) const;

  /// Registry of modified elements stored by original name and number of times
  /// modified - 0 counting. This is so when we modify elements beyond their definition
  /// they get a unique name as they're literally not the same object as the unmodified
  /// one so they don't share the placement counter for unique naming. This allows
  /// accurate geometry trees in the visualiser.
  std::map<G4String, G4int> modifiedElements;

  /// Variable used to pass around the possibly modified name of an element.
  G4String elementName;

  /// Only allow colours to be constructed from parser definitions once. Static so we can use
  /// a component factory many times without calling multiple times.
  static G4bool coloursInitialised;
};
#endif
