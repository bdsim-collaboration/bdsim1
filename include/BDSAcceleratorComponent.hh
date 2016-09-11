#ifndef BDSACCELERATORCOMPONENT_H
#define BDSACCELERATORCOMPONENT_H

#include "globals.hh"          // geant4 globals / types

#include "BDSGeometryComponent.hh"

#include <list>
#include <string>
#include <vector>

class BDSBeamPipeInfo;
class BDSSimpleComponent;
class G4LogicalVolume;

/**
 * @brief Abstract class that represents a component of an accelerator.
 *
 * It must be constructed with a name, length (arc), angle it
 * induces (x,z plane in the local coordinates of the component) in 
 * the reference trajectory and a string
 * representing its type. The class has no concept of its position
 * in the beamline or in global coordinates. This information is contained
 * in an instance of BDSBeamlineElement.
 * 
 * This is an abstract class as the derived class must provide the 
 * implementation of BuildContainerLogicalVolume() that constructs
 * the basic container. This is the minimum required so that an instance
 * of the derived class will operate with the rest of the placement machinery in
 * BDSIM. Typically, a derived class overrides the Build() function as well.
 * 
 * The class provides deferred construction through the Initialise() function
 * to allow two stage construction if it's required.
 * 
 * Note, the geometry of any derived component should be nominally constructed
 * along local z axis (beam direction) and x,y are transverse dimensions in a 
 * right-handed coordinate system.
 * 
 * This was significantly reworked in version 0.7 from the original. The indicator
 * author is the maintainer of the new version.
 * 
 * @author Laurie Nevay
 */

class BDSAcceleratorComponent: public BDSGeometryComponent
{
public:
  /// Constructor - this is the minimum information needed to create a
  /// BDSAcceleratorComponent instance. Methods in the class will allow
  /// the derived class to associate the appropraite volumes to the members
  /// of BDSGeometryComponent - the base class.  The developer of a derived
  /// class should take care to set all members of BDSGeometryComponent in the
  /// derived class, including extents.
  /// Note, this class has arc length and chord length which are initially set
  /// to be the same, unless angle is != 0 in which case, the chord length is
  /// calculated from arc length.
  BDSAcceleratorComponent(G4String         name,
			  G4double         arcLength,
			  G4double         angle,
			  G4String         type,
			  G4bool           precisionRegion = false,
			  BDSBeamPipeInfo* beamPipeInfo    = nullptr,
			  G4ThreeVector inputFaceNormalIn  = G4ThreeVector(0,0,-1),
			  G4ThreeVector outputFaceNormalIn = G4ThreeVector(0,0, 1));
  
  virtual ~BDSAcceleratorComponent();

  /// Two stage construction - first instantiate class, and then second, call this
  /// method to run Build() which constructs geometry.  This allows common construction
  /// tasks to be done in one place in BDSComponentFactory rather than pass as arguments
  /// through the constructors of all derived classes. Also builds read out geometry.
  virtual void Initialise();

  // Communal constructions tasks
  
  /// @{ Copy the bias list to this element
  void SetBiasVacuumList(std::list<std::string> biasVacuumListIn)
  {biasVacuumList = biasVacuumListIn;}
  void SetBiasMaterialList(std::list<std::string> biasMaterialListIn)
  {biasMaterialList = biasMaterialListIn;}
  /// @}
  
  /// Set whether precision output should be recorded for this component
  void   SetPrecisionRegion(G4bool precisionRegionIn)
  {precisionRegion = precisionRegionIn;}

  // Accessors
  
  /// The name of the component without modification
  inline G4String GetName() const {return name;}

  /// @{ Access the length of the component. Note there is no z length - this is chord length.
  /// Only chord OR arc makes it explicit.
  virtual G4double GetArcLength()   const {return arcLength;} 
  virtual G4double GetChordLength() const {return chordLength;}
  /// @}

  /// Get the angle the component induces in the reference trajectory (rad). 
  /// Note, this is 0 for h and v kickers.
  inline G4double GetAngle() const {return angle;}

  /// Get a string describing the type of the component
  inline G4String GetType() const {return type;}

  /// Whether precision output is to be recorded for this component
  G4bool GetPrecisionRegion() const {return precisionRegion;}

  /// Access beam pipe information
  inline BDSBeamPipeInfo* GetBeamPipeInfo() const {return beamPipeInfo;}

  /// @{ Access face normal unit vector. This is w.r.t. the incoming / outgoing reference
  /// trajectory and NOT the local geometry of the component. Ie for an SBend with no
  /// pole face rotation this is incoming (0,0,-1). Does not account for tilt.
  inline G4ThreeVector InputFaceNormal()  const {return inputFaceNormal;}
  inline G4ThreeVector OutputFaceNormal() const {return outputFaceNormal;}
  /// @}

  /// Access the vacuum volume the main beam goes through in this component if any. Default is
  /// nullptr.
  inline G4LogicalVolume* GetAcceleratorVacuumLogicalVolume() const {return acceleratorVacuumLV;}

  /// Increment (+1) the number of times this component has been copied.
  inline void  IncrementCopyNumber() {copyNumber++;}

  /// Get the number of times this component has been copied.
  inline G4int GetCopyNumber() const {return copyNumber;}

  /// @{ Access the bias list copied from parser
  std::list<std::string> GetBiasVacuumList()   const {return biasVacuumList;}
  std::list<std::string> GetBiasMaterialList() const {return biasMaterialList;}
  /// @}
  
  /// Whether this component has an optional end piece that should be placed
  /// independently or not depending on other items in the beamline.
  BDSSimpleComponent* EndPieceBefore() const {return endPieceBefore;}
  BDSSimpleComponent* EndPieceAfter()  const {return endPieceAfter;}

  void SetInputFaceNormal(const G4ThreeVector& input)   {inputFaceNormal  = input.unit();}
  void SetOutputFaceNormal(const G4ThreeVector& output) {outputFaceNormal = output.unit();}

  // Update the read out geometry volume given new face normals incase of a tilt.
  void UpdateReadOutVolumeWithTilt(G4double tilt);

  // to be deprecated public methods
  
  // in case a mapped field is provided creates a field mesh in global coordinates
  virtual void PrepareField(G4VPhysicalVolume *referenceVolume);

  ///@{ This function should be revisited given recent changes (v0.7)
  void SetGFlashVolumes(G4LogicalVolume* aLogVol)
  {itsGFlashVolumes.push_back(aLogVol);}
  std::vector<G4LogicalVolume*> GetGFlashVolumes() const
  {return itsGFlashVolumes;}
  ///@}
  
protected:
  /// Build the container only. Should be overridden by derived class to add more geometry
  /// apart from the container volume. The overridden Build() function can however, call
  /// make use of this function to call BuildContainerLogicalVolume() by calling
  /// BDSAcceleratorComponent::Build() at the beginning.
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
  {acceleratorVacuumLV = accVacLVIn;}
  
  ///@{ Const protected member variable that may not be changed by derived classes
  const G4String   name;
  const G4double   arcLength;
  const G4String   type;
  ///@}
  
  ///@{ Protected member variable that can be modified by derived classes.
  G4double         chordLength;
  G4double         angle;
  G4bool           precisionRegion;
  BDSBeamPipeInfo* beamPipeInfo;
  ///@}

  /// Useful variables often used in construction
  static G4double    lengthSafety;
  static G4Material* emptyMaterial;
  static G4bool      checkOverlaps;

  /// The logical volume in this component that is the volume the beam passes through that
  /// is typically vacuum. Discretised in this way for cuts / physics process to be assigned
  /// differently from general component material.
  G4LogicalVolume* acceleratorVacuumLV;

  /// A larger length safety that can be used where tracking accuracy isn't required
  /// or more tolerant geometry is required (1um).
  static G4double const lengthSafetyLarge;

  BDSSimpleComponent* endPieceBefore;
  BDSSimpleComponent* endPieceAfter;
  
private:
  /// Private default constructor to force use of provided constructors, which
  /// ensure an object meets the requirements for the rest of the construction
  /// and placement machinery in BDSIM
  BDSAcceleratorComponent();

  /// Assignment and copy constructor not implemented nor used
  BDSAcceleratorComponent& operator=(const BDSAcceleratorComponent&);
  BDSAcceleratorComponent(BDSAcceleratorComponent&);

  /// Build readout geometry volume
  G4LogicalVolume* BuildReadOutVolume(G4String name,
				      G4double chordLength,
				      G4double angle);

  std::vector<G4LogicalVolume*> itsGFlashVolumes;
  //A vector containing the physical volumes in the accelerator component- to be used for geometric importance sampling etc.

  /// Boolean record of whether this component has been already initialised.
  /// This check protects against duplicate initialisation and therefore the potential
  /// memory leaks that would ensue.
  G4bool initialised;
  /// Record of how many times this component has been copied.
  G4int copyNumber;

  /// Copy of bias list from parser for this particlar element
  std::list<std::string> biasVacuumList;
  std::list<std::string> biasMaterialList;

  G4ThreeVector inputFaceNormal;
  G4ThreeVector outputFaceNormal;
  G4double      readOutRadius;    ///< Radius of read out volume solid.
};

#endif
