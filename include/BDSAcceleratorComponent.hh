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
#ifndef BDSACCELERATORCOMPONENT_H
#define BDSACCELERATORCOMPONENT_H

#include "globals.hh"          // geant4 globals / types

#include "BDSGeometryComponent.hh"

#include <list>
#include <set>
#include <string>
#include <vector>

class BDSBeamPipeInfo;
class BDSFieldInfo;
class BDSSimpleComponent;
class G4LogicalVolume;
class G4UserLimits;
class G4VisAttributes;

/**
 * @brief Abstract class that represents a component of an accelerator.
 *
 * It must be constructed with a name, length (arc), angle it induces 
 * (x,z plane in the local coordinates of the component) in the 
 * reference trajectory and a string representing its type. The class 
 * has no concept of its position in the beamline or in global coordinates. 
 * This information is contained in an instance of BDSBeamlineElement.
 * 
 * This is an abstract class as the derived class must provide the 
 * implementation of BuildContainerLogicalVolume() that constructs at 
 * least a single volume, which would be assumed to be a container if greater
 * geometry hierarchy is constructed. This is the minimum required so that an 
 * instance of the derived class will operate with the rest of the placement 
 * machinery in BDSIM. The derived class should override Build() to add further 
 * geometry. If Build() is overridden, the base class 
 * BDSAcceleratorComponent::Build() should be called first. This calls
 * BuildContainerVolume() and sets the visual attributes of it.
 * 
 * The class provides deferred construction through the Initialise() function
 * to allow two stage construction if it's required. So, although a derived
 * class is instantiated, the geometry isn't constructed until Initialise() is
 * called.
 * 
 * Note, the geometry of any derived component should be nominally constructed
 * along local z axis (beam direction) and x,y are transverse dimensions in a 
 * right-handed coordinate system.
 *
 * The 'length' of the accelerator component is the arc length, which corresponds
 * to the angle given.  The chord length is internally calculated from the arc
 * length and the angle.  Therefore, the chord length must never be given.
 *
 * The optional input and output face normal (unit) vectors are w.r.t. the 
 * incoming / outgoing reference trajectory and NOT the local geometry of the 
 * component. 
 * 
 * @author Laurie Nevay
 */

class BDSAcceleratorComponent: public BDSGeometryComponent
{
public:
  /// Constructor - this is the minimum information needed to create a
  /// BDSAcceleratorComponent instance. Methods in the class will allow
  /// the derived class to associate the appropriate volumes to the members
  /// of BDSGeometryComponent - the base class. The developer of a derived
  /// class should take care to set all members of BDSGeometryComponent in the
  /// derived class, including extents.
  /// Note, this class has arc length and chord length which are initially set
  /// to be the same, unless angle is != 0 in which case, the chord length is
  /// calculated from arc length. An associated beam pipe info instance can be
  /// attached if the component has a beam pipe. The input and output face normals
  /// should also be specified if non-zero. Additionally, a field info instance
  /// that represents a 'global' field for this component may be specified. Face
  /// normal (unit) vectors are w.r.t. the incoming / outgoing reference trajectory
  /// and NOT the local geometry of the component.
  /// The BDSBeamPipeInfo instance is associated with this class so that the survey
  /// output of BDSIM can query the aperture of any element.
  BDSAcceleratorComponent(const G4String&      name,
			  G4double             arcLength,
			  G4double             angle,
			  const G4String&      type,
			  BDSBeamPipeInfo*     beamPipeInfo       = nullptr,
			  const G4ThreeVector& inputFaceNormalIn  = G4ThreeVector(0,0,-1),
			  const G4ThreeVector& outputFaceNormalIn = G4ThreeVector(0,0, 1),
			  BDSFieldInfo*        fieldInfoIn        = nullptr);
  
  virtual ~BDSAcceleratorComponent();

  /// Two stage construction - first instantiate class, and then second, call this
  /// method to run Build() which constructs geometry.  This allows common construction
  /// tasks to be done in one place in BDSComponentFactory rather than pass as arguments
  /// through the constructors of all derived classes. Also builds read out geometry.
  virtual void Initialise();

  // Communal constructions tasks
  
  /// @{ Copy the bias list to this element
  virtual void SetBiasVacuumList(const std::list<std::string>& biasVacuumListIn)
  {biasVacuumList = biasVacuumListIn;}
  virtual void SetBiasMaterialList(const std::list<std::string>& biasMaterialListIn)
  {biasMaterialList = biasMaterialListIn;}
  virtual void SetBiasMaterialLVList(const std::list<std::string>& biasMaterialLVListIn)
  {biasMaterialLVList = biasMaterialLVListIn;}
  /// @}
  
  /// Set the region name for this component.
  virtual void SetRegion(const G4String& regionIn) {region = regionIn;}

  /// Set the field definition for the whole component.
  void SetField(BDSFieldInfo* fieldInfoIn);

  /// Set a minimum kinetic energy for a component by component level cut. By default
  /// does nothing - only certain components implement a response or user limits to this.
  virtual void SetMinimumKineticEnergy(G4double /*minimumKineticEnergyIn*/){;}

  // Accessors
  
  /// The name of the component without modification
  virtual inline G4String GetName() const {return name;}

  /// @{ Access the length of the component. Note there is no z length - this is chord length.
  /// Only chord OR arc makes it explicit.
  virtual G4double GetArcLength()   const {return arcLength;} 
  virtual G4double GetChordLength() const {return chordLength;}
  /// @}

  /// Get the angle the component induces in the reference trajectory (rad). 
  /// Note, this is 0 for h and v kickers.
  inline G4double GetAngle() const {return angle;}

  /// Calculate the sagitta - ie the distance between the chord and the arc at the centre.
  G4double Sagitta() const;

  /// Get a string describing the type of the component
  inline G4String GetType() const {return type;}

  /// Get the region name for this component.
  G4String GetRegion() const {return region;}

  /// Whether this component has a field or not (ie is active). Implicit cast of pointer to bool.
  virtual G4bool HasAField() const {return fieldInfo;}
  
  /// For when an accelerator component is used in a placement, we need to flag the field transform
  /// should come from a different parallel world and not the regularly curvilinear ones. This should
  /// be used before Initialise() when the field is registered for construction.
  virtual void SetFieldUsePlacementWorldTransform();

  /// Access beam pipe information, which is stored in this class to provide
  /// aperture information when making a survey of the beamline consisting of
  /// accelerator components.
  inline BDSBeamPipeInfo* GetBeamPipeInfo() const {return beamPipeInfo;}

  /// @{ Access face normal unit vector. This is w.r.t. the incoming / outgoing reference
  /// trajectory and NOT the local geometry of the component. Ie for an SBend with no
  /// pole face rotation this is incoming (0,0,-1). Does not account for tilt.
  inline G4ThreeVector InputFaceNormal()  const {return inputFaceNormal;}
  inline G4ThreeVector OutputFaceNormal() const {return outputFaceNormal;}
  /// @}

  /// @{ Whether the face normal is angled at all w.r.t. the incoming / outgoing reference trajectory.
  G4bool AngledInputFace()  const;
  G4bool AngledOutputFace() const;
  /// @}

  /// Return the name of a material associated with the component - ie the primary material.
  virtual G4String Material() const {return "none";}

  /// Access the 'vacuum' volume(s) in this component if any. Default is empty set.
  virtual std::set<G4LogicalVolume*> GetAcceleratorVacuumLogicalVolumes() const {return acceleratorVacuumLV;}

  /// Return a set of logical volumes excluding the ones in the 'vacuum' set.
  virtual std::set<G4LogicalVolume*> GetAcceleratorMaterialLogicalVolumes() const;

  /// Increment (+1) the number of times this component has been copied.
  inline void  IncrementCopyNumber() {copyNumber++;}

  /// Get the number of times this component has been copied.
  inline G4int GetCopyNumber() const {return copyNumber;}

  /// @{ Access the bias list copied from parser
  std::list<std::string> GetBiasVacuumList()   const {return biasVacuumList;}
  std::list<std::string> GetBiasMaterialList() const {return biasMaterialList;}
  std::list<std::string> GetBiasMaterialLVList() const {return biasMaterialLVList;}
  /// @}
  
  /// Whether this component has an optional end piece that should be placed
  /// independently or not depending on other items in the beamline.
  BDSSimpleComponent* EndPieceBefore() const {return endPieceBefore;}
  BDSSimpleComponent* EndPieceAfter()  const {return endPieceAfter;}

  /// @{ Allow updating of face normals. Virtual so derived class may apply it to daughters.
  virtual void SetInputFaceNormal(const G4ThreeVector& input)   {inputFaceNormal  = input.unit();}
  virtual void SetOutputFaceNormal(const G4ThreeVector& output) {outputFaceNormal = output.unit();}
  /// @}
  
  /// A larger length safety that can be used where tracking accuracy isn't required
  /// or more tolerant geometry is required (1um).
  static G4double lengthSafetyLarge;

protected:
  /// This calls BuildContainerLogicalVolume() and then sets the visual attributes
  /// of the container logical volume. This should be overridden by derived class
  /// to add more geometry apart from the container volume. The overridden Build()
  /// function can however, call make use of this function to call
  /// BuildContainerLogicalVolume() by calling BDSAcceleratorComponent::Build()
  /// at the beginning.
  virtual void Build();

  /// Build the container solid and logical volume that all parts of the component will
  /// contained within - must be provided by derived class.
  virtual void BuildContainerLogicalVolume() = 0;

  /// Assign the accelerator tracking volume - only callable by derived classes - ie not public.
  /// This is just setting a reference to the accelerator volume and it is not deleted by
  /// this class (BDSAcceleratorComponent) - therefore, the derived class should also deal with
  /// memory management of this volume - whether this is by using the inherited
  /// (from BDSGeometryComponent) RegisterLogicalVolume() or by manually deleting itself.
  inline void SetAcceleratorVacuumLogicalVolume(G4LogicalVolume* accVacLVIn)
  {acceleratorVacuumLV.insert(accVacLVIn);}

  inline void SetAcceleratorVacuumLogicalVolume(const std::set<G4LogicalVolume*>& accVacLVIn)
  {acceleratorVacuumLV.insert(accVacLVIn.begin(), accVacLVIn.end());}

  /// This tests to see if the length of the BDSAcceleratorComponent is shorter than the
  /// global step length in the global users limits and if so build a unique one for this
  /// component and register it (memory management). It's provided by the member userLimits.
  /// This will be nullptr until this function is called, which is called in this class's
  /// Build(). Putting it here makes the same G4UserLimits object available to all derived
  /// classes potentially saving creation of a duplicate object.
  virtual void BuildUserLimits();

  /// Doesn't change member variables, but may change their contents.
  virtual void AttachUserLimits() const;
  
  ///@{ Const protected member variable that may not be changed by derived classes
  const G4String   name;
  const G4double   arcLength;
  const G4String   type;
  ///@}
  
  ///@{ Protected member variable that can be modified by derived classes.
  G4double         chordLength;
  G4double         angle;
  G4String         region;
  ///@}

  /// Optional beam pipe recipe that is written out to the survey if it exists.
  BDSBeamPipeInfo* beamPipeInfo;

  /// @{ Useful variable often used in construction.
  static G4double    lengthSafety;
  static G4Material* emptyMaterial;
  static G4Material* worldMaterial;
  static G4bool      checkOverlaps;
  static G4bool      sensitiveOuter;
  static G4bool      sensitiveVacuum;
  static G4VisAttributes* containerVisAttr;
  /// @}

  /// A set of logical volumes we classify as 'vacuum' for biasing purposes.
  std::set<G4LogicalVolume*> acceleratorVacuumLV;

  BDSSimpleComponent* endPieceBefore;
  BDSSimpleComponent* endPieceAfter;

  G4UserLimits* userLimits; ///< Cache of user limits.
  
  BDSFieldInfo* fieldInfo; ///< Recipe for field that could overlay this whole component.
  
private:
  /// Private default constructor to force use of provided constructors, which
  /// ensure an object meets the requirements for the rest of the construction
  /// and placement machinery in BDSIM
  BDSAcceleratorComponent() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSAcceleratorComponent& operator=(const BDSAcceleratorComponent&) = delete;
  BDSAcceleratorComponent(BDSAcceleratorComponent&) = delete;
  /// @}

  //A vector containing the physical volumes in the accelerator component- to be used for geometric importance sampling etc.

  /// Boolean record of whether this component has been already initialised.
  /// This check protects against duplicate initialisation and therefore the potential
  /// memory leaks that would ensue.
  G4bool initialised;
  
  /// Record of how many times this component has been copied.
  G4int copyNumber;

  /// @{ Copy of bias list from parser for this particular component.
  std::list<std::string> biasVacuumList;
  std::list<std::string> biasMaterialList;
  std::list<std::string> biasMaterialLVList;
  /// @}
  
  G4ThreeVector inputFaceNormal;  ///< Input face unit normal vector in incoming reference coordinate frame.
  G4ThreeVector outputFaceNormal; ///< Output face unit normal vector in outgoing reference coordinate frame.
};

#endif
