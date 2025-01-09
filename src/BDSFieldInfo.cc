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
#include "BDSArrayReflectionType.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldInfoExtra.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolatorType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSModulatorInfo.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4Transform3D.hh"
#include "G4UserLimits.hh"

#include <algorithm>
#include <ostream>

G4UserLimits* BDSFieldInfo::defaultUL = nullptr;

BDSFieldInfo::BDSFieldInfo():
  fieldType(BDSFieldType::none),
  brho(0),
  integratorType(BDSIntegratorType::none),
  magnetStrength(nullptr),
  provideGlobalTransform(false),
  transform(nullptr),
  magneticFieldFilePath(""),
  magneticFieldFormat(BDSFieldFormat::none),
  magneticInterpolatorType(BDSInterpolatorType::nearest3d),
  magneticArrayReflectionTypeSet(BDSArrayReflectionTypeSet()),
  electricFieldFilePath(""),
  electricFieldFormat(BDSFieldFormat::none),
  electricInterpolatorType(BDSInterpolatorType::nearest3d),
  electricArrayReflectionTypeSet(BDSArrayReflectionTypeSet()),
  cacheTransforms(true),
  eScaling(1.0),
  bScaling(1.0),
  timeOffset(0),
  autoScale(false),
  stepLimit(nullptr),
  poleTipRadius(1),
  beamPipeRadius(0),
  chordStepMinimum(-1),
  tilt(0),
  secondFieldOnLeft(false),
  magneticSubFieldName(""),
  electricSubFieldName(""),
  usePlacementWorldTransform(false),
  modulatorInfo(nullptr),
  ignoreUpdateOfMaximumStepSize(false),
  isThin(false),
  transformBeamline(nullptr),
  extraInfo(nullptr),
  nameOfParserDefinition("")
{;}

BDSFieldInfo::BDSFieldInfo(BDSFieldType             fieldTypeIn,
                           G4double                 brhoIn,
                           BDSIntegratorType        integratorTypeIn,
                           BDSMagnetStrength*       magnetStrengthIn,
                           G4bool                   provideGlobalTransformIn,
                           const G4Transform3D&     transformIn,
                           const G4String&          magneticFieldFilePathIn,
                           BDSFieldFormat           magneticFieldFormatIn,
                           BDSInterpolatorType      magneticInterpolatorTypeIn,
                           const G4String&          electricFieldFilePathIn,
                           BDSFieldFormat           electricFieldFormatIn,
                           BDSInterpolatorType      electricInterpolatorTypeIn,
                           G4bool                   cacheTransformsIn,
                           G4double                 eScalingIn,
                           G4double                 bScalingIn,
                           G4double                 timeOffsetIn,
                           G4bool                   autoScaleIn,
                           G4UserLimits*            stepLimitIn,
                           G4double                 poleTipRadiusIn,
                           G4double                 beamPipeRadiusIn,
                           G4bool                   leftIn,
                           const G4String&          magneticSubFieldNameIn,
                           const G4String&          electricSubFieldNameIn):
  fieldType(fieldTypeIn),
  brho(brhoIn),
  integratorType(integratorTypeIn),
  magnetStrength(magnetStrengthIn),
  provideGlobalTransform(provideGlobalTransformIn),
  transform(nullptr),
  magneticFieldFilePath(magneticFieldFilePathIn),
  magneticFieldFormat(magneticFieldFormatIn),
  magneticInterpolatorType(magneticInterpolatorTypeIn),
  magneticArrayReflectionTypeSet(BDSArrayReflectionTypeSet()),
  electricFieldFilePath(electricFieldFilePathIn),
  electricFieldFormat(electricFieldFormatIn),
  electricInterpolatorType(electricInterpolatorTypeIn),
  electricArrayReflectionTypeSet(BDSArrayReflectionTypeSet()),
  cacheTransforms(cacheTransformsIn),
  eScaling(eScalingIn),
  bScaling(bScalingIn),
  timeOffset(timeOffsetIn),
  autoScale(autoScaleIn),
  stepLimit(stepLimitIn),
  poleTipRadius(poleTipRadiusIn),
  beamPipeRadius(beamPipeRadiusIn),
  chordStepMinimum(-1),
  secondFieldOnLeft(leftIn),
  magneticSubFieldName(magneticSubFieldNameIn),
  electricSubFieldName(electricSubFieldNameIn),
  usePlacementWorldTransform(false),
  modulatorInfo(nullptr),
  ignoreUpdateOfMaximumStepSize(false),
  isThin(false),
  transformBeamline(nullptr),
  extraInfo(nullptr),
  nameOfParserDefinition("")
{
  if (transformIn != G4Transform3D::Identity)
    {transform = new G4Transform3D(transformIn);}
  
  // back calculate tilt angle from field transform
  G4ThreeVector unitY(0,1,0);
  G4ThreeVector unitYR = unitY.transform(transformIn.getRotation());
  tilt = -(CLHEP::halfpi - unitYR.getPhi()); // halfpi offset for which axis we choose by convention
}

BDSFieldInfo::~BDSFieldInfo()
{
  delete magnetStrength;
  delete transform;
  delete stepLimit;
  delete transformBeamline;
  delete extraInfo;
}

BDSFieldInfo::BDSFieldInfo(const BDSFieldInfo& other):
  fieldType(other.fieldType),
  brho(other.brho),
  integratorType(other.integratorType),
  provideGlobalTransform(other.provideGlobalTransform),
  transform(nullptr),
  magneticFieldFilePath(other.magneticFieldFilePath),
  magneticFieldFormat(other.magneticFieldFormat),
  magneticInterpolatorType(other.magneticInterpolatorType),
  magneticArrayReflectionTypeSet(other.magneticArrayReflectionTypeSet),
  electricFieldFilePath(other.electricFieldFilePath),
  electricFieldFormat(other.electricFieldFormat),
  electricInterpolatorType(other.electricInterpolatorType),
  electricArrayReflectionTypeSet(other.electricArrayReflectionTypeSet),
  cacheTransforms(other.cacheTransforms),
  eScaling(other.eScaling),
  bScaling(other.bScaling),
  timeOffset(other.timeOffset),
  autoScale(other.autoScale),
  poleTipRadius(other.poleTipRadius),
  beamPipeRadius(other.beamPipeRadius),
  chordStepMinimum(other.chordStepMinimum),
  tilt(other.tilt),
  secondFieldOnLeft(other.secondFieldOnLeft),
  magneticSubFieldName(other.magneticSubFieldName),
  electricSubFieldName(other.electricSubFieldName),
  usePlacementWorldTransform(other.usePlacementWorldTransform),
  modulatorInfo(other.modulatorInfo),
  ignoreUpdateOfMaximumStepSize(other.ignoreUpdateOfMaximumStepSize),
  isThin(other.isThin),
  transformBeamline(nullptr),
  extraInfo(nullptr),
  nameOfParserDefinition(other.nameOfParserDefinition)
{
  if (other.transform)
    {transform = new G4Transform3D(*other.transform);}
  
  if (other.magnetStrength)
    {magnetStrength = new BDSMagnetStrength(*other.magnetStrength);}
  else
    {magnetStrength = nullptr;} // also nullptr

  if (other.stepLimit)
    {stepLimit = new G4UserLimits(*other.stepLimit);}
  else
    {stepLimit = nullptr;}

  if (other.transformBeamline)
    {transformBeamline = new G4Transform3D(*other.transformBeamline);}

  if (other.extraInfo)
    {extraInfo = other.extraInfo->Clone();}
}

void BDSFieldInfo::SetUserLimits(G4UserLimits* userLimitsIn)
{
  if (stepLimit != defaultUL)
    {delete stepLimit;} // shouldn't delete global default step limit!
  stepLimit = userLimitsIn;
}

void BDSFieldInfo::UpdateUserLimitsLengthMaximumStepSize(G4double maximumStepSize,
                                                         G4bool   warn) const
{
  if (ignoreUpdateOfMaximumStepSize)
    {return;}

  if (stepLimit && (stepLimit != defaultUL))
    {
      G4UserLimits* old = stepLimit;
      stepLimit = BDS::CreateUserLimits(stepLimit, maximumStepSize, 1.0);
      if (stepLimit == old)
        {warn = false;} // no change and warning would print out wrong number
      if ((stepLimit != old) && (old != defaultUL))
        {delete old;}
    }
  else
    {stepLimit = new G4UserLimits(maximumStepSize);}
  
  if (warn)
    {
      G4cout << "Reducing maximum step size of field definition \"" << nameOfParserDefinition
             << "\" to " << maximumStepSize << " mm " << G4endl;
    }
}

std::ostream& operator<< (std::ostream& out, BDSFieldInfo const& info)
{
  out << "Parser definition name: \"" << info.nameOfParserDefinition << "\"" << G4endl;
  out << "Field type:          " << info.fieldType                << G4endl;
  out << "Rigidity:            " << info.brho                     << G4endl;
  out << "Integrator:          " << info.integratorType           << G4endl;
  out << "Global transform?    " << info.provideGlobalTransform   << G4endl;
  out << "B map file:          " << info.magneticFieldFilePath    << G4endl;
  out << "B map file format:   " << info.magneticFieldFormat      << G4endl;
  out << "B interpolator       " << info.magneticInterpolatorType << G4endl;
  out << "B array reflection:  " << info.magneticArrayReflectionTypeSet << G4endl;
  out << "E map file:          " << info.electricFieldFilePath    << G4endl;
  out << "E map file format:   " << info.electricFieldFormat      << G4endl;
  out << "E interpolator       " << info.electricInterpolatorType << G4endl;
  out << "E array reflection:  " << info.electricArrayReflectionTypeSet << G4endl;
  out << "Transform caching:   " << info.cacheTransforms          << G4endl;
  out << "E Scaling:           " << info.eScaling                 << G4endl;
  out << "B Scaling:           " << info.bScaling                 << G4endl;
  out << "t offset             " << info.timeOffset               << G4endl;
  out << "Auto scale           " << info.autoScale                << G4endl;
  out << "Pole tip radius:     " << info.poleTipRadius            << G4endl;
  out << "Beam pipe radius:    " << info.beamPipeRadius           << G4endl;
  out << "Chord Step Min:      " << info.chordStepMinimum         << G4endl;
  out << "Tilt:                " << info.tilt                     << G4endl;
  out << "Second field on left " << info.secondFieldOnLeft        << G4endl;
  out << "Magnetic sub field   " << info.magneticSubFieldName     << G4endl;
  out << "Electric sub field   " << info.electricSubFieldName     << G4endl;
  if (info.modulatorInfo)
    {out << "Modulator            " << *(info.modulatorInfo) << G4endl;}
  out << "Use placement world transform " << info.usePlacementWorldTransform << G4endl;
  out << "Ignore update of maximum step size " << info.ignoreUpdateOfMaximumStepSize << G4endl;
  if (info.magnetStrength)
    {out << "Magnet strength:     " << *(info.magnetStrength)      << G4endl;}
  if (info.stepLimit)
    {
      G4Track t = G4Track(); // dummy track
      G4double maxStep = info.stepLimit->GetMaxAllowedStep(t);
      out << "Step limit:        " << maxStep << G4endl;
    }
  out << "extra info " << (info.extraInfo ? "present" : "not present") << G4endl;
  return out;
}

void BDSFieldInfo::Translate(const G4ThreeVector& translationIn)
{
  if (!transform)
    {transform = new G4Transform3D();}
  G4RotationMatrix       rm = transform->getRotation();
  G4ThreeVector translation = transform->getTranslation();
  translation += translationIn;
  G4Transform3D* newTransform = new G4Transform3D(rm, translation);
  delete transform;
  transform = newTransform;
}

G4double BDSFieldInfo::SynchronousT() const
{
  if (!magnetStrength)
    {return 0;}
  else
    {return (*magnetStrength)["synchronousT0"];}
}

G4Transform3D BDSFieldInfo::Transform() const
{
  return transform ? *transform : G4Transform3D::Identity;
}

G4Transform3D BDSFieldInfo::TransformBeamline() const
{
  return transformBeamline ? *transformBeamline : G4Transform3D::Identity;
}

G4Transform3D BDSFieldInfo::TransformComplete() const
{
  return Transform() * TransformBeamline();
}

void BDSFieldInfo::SetTransform(const G4Transform3D& transformIn)
{
  delete transform;
  transform = new G4Transform3D(transformIn);
}

void BDSFieldInfo::SetTransformBeamline(const G4Transform3D& transformIn)
{
  delete transformBeamline;
  transformBeamline = new G4Transform3D(transformIn);
}

void BDSFieldInfo::SetFieldAsThin()
{
  isThin = true;
}
