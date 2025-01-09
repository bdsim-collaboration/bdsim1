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
#ifndef BDSFIELDINFO_H
#define BDSFIELDINFO_H

#include "BDSArrayReflectionType.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolatorType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"

#include <ostream>

class BDSFieldInfoExtra;
class BDSMagnetStrength;
class BDSModulatorInfo;
class G4UserLimits;

/**
 * @brief All info required to build complete field of any type.
 * 
 * With an instance of this class, the field factory should be able
 * to create the desired fully functional field with all required 
 * associated Geant4 objects.
 * 
 * Owns the magnetic strength instance & cavity info instance.
 *
 * Owns G4ThreeVector for unitDirection. It's a pointer to save memory
 * on average.
 *
 * Pole tip radius is used for normalisation purposes for outer yoke fields. 
 * Beam pipe radius is used to decide whether there's a gap between the pole
 * and the beam pipe and therefore to use a normal inner field for that part.
 *
 * Transforms are optional to save memory. The complete required transform is
 * stored as 2 optional bits - the transform from the curvilinear reference frame
 * to that of the specific component (tilt + offset), as well as the transform
 * of the field definition itself (e.g. a field map maybe rotated 90 degrees). The
 * full transform is the compound of these. They're kept separate in case we want
 * to wrap multiple instances (with individual field transforms) in one holder instance
 * (with the beam line transform).
 *
 * @author Laurie Nevay
 */

class BDSFieldInfo
{
public:
  /// Default constructor for zero field effectively
  BDSFieldInfo();
  BDSFieldInfo(BDSFieldType             fieldTypeIn,
	       G4double                 brhoIn,
	       BDSIntegratorType        integratorTypeIn,
	       BDSMagnetStrength*       magnetStrengthIn           = nullptr,
	       G4bool                   provideGlobalTransformIn   = true,
	       const G4Transform3D&     transformIn                = G4Transform3D(),
	       const G4String&          magneticFieldFilePathIn    = "",
	       BDSFieldFormat           magneticFieldFormatIn      = BDSFieldFormat::bdsim1d,
	       BDSInterpolatorType      magneticInterpolatorTypeIn = BDSInterpolatorType::nearest3d,
	       const G4String&          electricFieldFilePathIn    = "",
	       BDSFieldFormat           electricFieldFormatIn      = BDSFieldFormat::bdsim1d,
	       BDSInterpolatorType      electricInterpolatorTypeIn = BDSInterpolatorType::nearest3d,
	       G4bool                   cacheTransformsIn          = false,
	       G4double                 eScalingIn                 = 1.0,
	       G4double                 bScalingIn                 = 1.0,
	       G4double                 timeOffsetIn               = 0,
	       G4bool                   autoScaleIn                = false,
	       G4UserLimits*            stepLimitIn                = nullptr,
	       G4double                 poleTipRadiusIn            = 1,
	       G4double                 beamPipeRadiusIn           = 0,
	       G4bool                   left                       = true,
	       const G4String&          magneticSubFieldNameIn     = "",
	       const G4String&          electricSubFieldNameIn     = "");
  ~BDSFieldInfo();

  /// Copy constructor
  BDSFieldInfo(const BDSFieldInfo& other);
  /// Assignment operator not used
  BDSFieldInfo& operator=(const BDSFieldInfo&) = delete;

  /// @{ Accessor
  inline BDSFieldType        FieldType()                const {return fieldType;}
  inline G4double            BRho()                     const {return brho;}
  inline BDSIntegratorType   IntegratorType()           const {return integratorType;}
  inline BDSMagnetStrength*  MagnetStrength()           const {return magnetStrength;}
  inline G4bool              ProvideGlobal()            const {return provideGlobalTransform;}
  inline G4String            MagneticFile()             const {return magneticFieldFilePath;}
  inline BDSFieldFormat      MagneticFormat()           const {return magneticFieldFormat;}
  inline BDSInterpolatorType MagneticInterpolatorType() const {return magneticInterpolatorType;}
  inline G4String            ElectricFile()             const {return electricFieldFilePath;}
  inline BDSFieldFormat      ElectricFormat()           const {return electricFieldFormat;}
  inline BDSInterpolatorType ElectricInterpolatorType() const {return electricInterpolatorType;}
  inline G4bool              CacheTransforms()          const {return cacheTransforms;}
  inline G4double            EScaling()                 const {return eScaling;}
  inline G4double            BScaling()                 const {return bScaling;}
  inline G4double            TimeOffset()               const {return timeOffset;}
  inline G4bool              AutoScale()                const {return autoScale;}
  inline G4UserLimits*       UserLimits()               const {return stepLimit;}
  inline G4double            PoleTipRadius()            const {return poleTipRadius;}
  inline G4double            BeamPipeRadius()           const {return beamPipeRadius;}
  inline G4double            ChordStepMinimum()         const {return chordStepMinimum;}
  inline G4double            Tilt()                     const {return tilt;}
  inline G4bool              SecondFieldOnLeft()        const {return secondFieldOnLeft;}
  inline G4String            MagneticSubFieldName()     const {return magneticSubFieldName;}
  inline G4String            ElectricSubFieldName()     const {return electricSubFieldName;}
  inline G4String            NameOfParserDefinition()   const {return nameOfParserDefinition;}
  inline G4bool              UsePlacementWorldTransform() const {return usePlacementWorldTransform;}
  inline const BDSArrayReflectionTypeSet& MagneticArrayReflectionType() const {return magneticArrayReflectionTypeSet;}
  inline const BDSArrayReflectionTypeSet& ElectricArrayReflectionType() const {return electricArrayReflectionTypeSet;}
  inline BDSModulatorInfo*   ModulatorInfo()            const {return modulatorInfo;}
  inline G4bool IgnoreUpdateOfMaximumStepSize() const {return ignoreUpdateOfMaximumStepSize;}
  inline G4bool              IsThin()                   const {return isThin;}
  inline BDSFieldInfoExtra*  ExtraInfo()                const {return extraInfo;}
  /// @}

  G4double SynchronousT() const;
  
  G4Transform3D Transform() const;         ///< Transform for the field definition only.
  G4Transform3D TransformBeamline() const; ///< Transform from the curvilinear coordinates to the beam line component.
  G4Transform3D TransformComplete() const; ///< Compound transform of field + beam line transform.

  /// Set Transform - could be done afterwards once instance of this class is passed around.
  inline void SetFieldType(BDSFieldType fieldTypeIn) {fieldType = fieldTypeIn;}
  inline void SetIntegratorType(BDSIntegratorType typeIn) {integratorType = typeIn;}
  inline void SetProvideGlobalTransform(G4bool provideGlobalTransformIn) {provideGlobalTransform = provideGlobalTransformIn;}
  inline void SetMagneticInterpolatorType(BDSInterpolatorType typeIn) {magneticInterpolatorType = typeIn;}
  inline void SetMagneticArrayReflectionType(const BDSArrayReflectionTypeSet& typeIn) {magneticArrayReflectionTypeSet = typeIn;}
  inline void SetElectricArrayReflectionType(const BDSArrayReflectionTypeSet& typeIn) {electricArrayReflectionTypeSet = typeIn;}
  inline void SetBScaling(G4double bScalingIn) {bScaling  = bScalingIn;}
  inline void SetAutoScale(G4bool autoScaleIn) {autoScale = autoScaleIn;}
  inline void SetScalingRadius(G4double poleTipRadiusIn) {poleTipRadius = poleTipRadiusIn;}
  inline void SetBeamPipeRadius(G4double beamPipeRadiusIn) {beamPipeRadius = beamPipeRadiusIn;}
  inline void SetChordStepMinimum(G4double chordStepMinimumIn) {chordStepMinimum = chordStepMinimumIn;}
  inline void SetSecondFieldOnLeft(G4bool leftIn) {secondFieldOnLeft = leftIn;}
  inline void SetMagneticSubField(const G4String& mfnIn) {magneticSubFieldName = mfnIn;}
  inline void SetElectricSubField(const G4String& efnIn) {electricSubFieldName = efnIn;}
  inline void SetUsePlacementWorldTransform(G4bool use) {usePlacementWorldTransform = use;}
  inline void SetModulatorInfo(BDSModulatorInfo* modulatorInfoIn) {modulatorInfo = modulatorInfoIn;}
  inline void SetIgnoreUpdateOfMaximumStepSize(G4bool ignoreUpdateOfMaximumStepSizeIn) {ignoreUpdateOfMaximumStepSize = ignoreUpdateOfMaximumStepSizeIn;}

  /// *= for BScaling.
  inline void CompoundBScaling(G4double extraBScalingIn) {bScaling *= extraBScalingIn;}
  /// *= for EScaling.
  inline void CompoundEScaling(G4double extraEScalingIn) {eScaling *= extraEScalingIn;}

  void SetTransform(const G4Transform3D& transformIn); ///< Set the field definition transform.
  void SetTransformBeamline(const G4Transform3D& transformIn); ///< Set the beam line transform.

  /// Delete and replace the user limits which this class owns (only if not default ul).
  void SetUserLimits(G4UserLimits* userLimitsIn);
  
  void SetNameOfParserDefinition(const G4String& nameIn) {nameOfParserDefinition = nameIn;}

  /// Set the additional 'extra' info member.
  void SetExtraInfo(BDSFieldInfoExtra* extraInfoIn) {extraInfo = extraInfoIn;}
  
  /// Update the user limits object (stepLimit) to the minimum of the current and supplied maximum
  /// step size. Mutable, so can be called on const object.
  void UpdateUserLimitsLengthMaximumStepSize(G4double maximumStepSize,
                                             G4bool   warn = false) const;

  /// Translate - adds an additional translation to the transform member variable. May only
  /// be known at assembly time given parameterised geometry. Used by AWAKE Spectrometer only.
  void Translate(const G4ThreeVector& translationIn);

  /// Turn on or off transform caching.
  inline void CacheTransforms(G4bool cacheTransformsIn) {cacheTransforms = cacheTransformsIn;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSFieldInfo const &info);

  static G4UserLimits* defaultUL; ///< Cache of default user limits

  /// Set thin to allow geant tracking error controls to be set seperately for thin elements
  void SetFieldAsThin();

private: 
  BDSFieldType             fieldType;
  G4double                 brho;
  BDSIntegratorType        integratorType;
  BDSMagnetStrength*       magnetStrength;
  G4bool                   provideGlobalTransform;
  G4Transform3D*           transform;  ///< Transform w.r.t. solid field will be attached to
  G4String                 magneticFieldFilePath;
  BDSFieldFormat           magneticFieldFormat;
  BDSInterpolatorType      magneticInterpolatorType;
  BDSArrayReflectionTypeSet magneticArrayReflectionTypeSet;
  G4String                 electricFieldFilePath;
  BDSFieldFormat           electricFieldFormat;
  BDSInterpolatorType      electricInterpolatorType;
  BDSArrayReflectionTypeSet electricArrayReflectionTypeSet;
  G4bool                   cacheTransforms;
  G4double                 eScaling;
  G4double                 bScaling;
  G4double                 timeOffset;
  G4bool                   autoScale;
  mutable G4UserLimits*    stepLimit;
  G4double                 poleTipRadius;  ///< Radius at which point the field will be scaled to.
  G4double                 beamPipeRadius; ///< Optional radius of beam pipe.
  G4double                 chordStepMinimum;
  G4double                 tilt;           ///< Cache of tilt of field.
  G4bool                   secondFieldOnLeft; ///< Flag for case of two-beam field - if not left, it's right.
  G4String                 magneticSubFieldName;
  G4String                 electricSubFieldName;
  G4bool                   usePlacementWorldTransform;
  BDSModulatorInfo*        modulatorInfo;
  G4bool                   ignoreUpdateOfMaximumStepSize; ///< To be used when enforcing a larger maximum step size value.
  G4bool                   isThin;

  /// Transform from curvilinear frame to this field - ie beam line bit only.
  G4Transform3D*           transformBeamline;
  BDSFieldInfoExtra*       extraInfo;

  G4String nameOfParserDefinition;
  
  // We need a default to pass back if none is specified.
  const static G4ThreeVector defaultUnitDirection;
};

#endif
