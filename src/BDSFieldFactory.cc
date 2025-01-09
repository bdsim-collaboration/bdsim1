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
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSFieldClassType.hh"
#include "BDSFieldE.hh"
#include "BDSFieldEGlobal.hh"
#include "BDSFieldEGlobalPlacement.hh"
#include "BDSFieldEInterpolated.hh"
#include "BDSFieldEInterpolated2Layer.hh"
#include "BDSFieldESinusoid.hh"
#include "BDSFieldEZero.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldEMGlobal.hh"
#include "BDSFieldEMGlobalPlacement.hh"
#include "BDSFieldEMInterpolated.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSFieldEMZero.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldGaborLens.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagDecapole.hh"
#include "BDSFieldMagDipole.hh"
#include "BDSFieldMagDipoleOuter.hh"
#include "BDSFieldMagDipoleOuterOld.hh"
#include "BDSFieldMagDipoleQuadrupole.hh"
#include "BDSFieldMagGlobal.hh"
#include "BDSFieldMagGlobalPlacement.hh"
#include "BDSFieldMagInterpolated.hh"
#include "BDSFieldMagInterpolated2Layer.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSFieldMagMultipoleOuter.hh"
#include "BDSFieldMagMultipoleOuterDual.hh"
#include "BDSFieldMagMultipoleOuterDualOld.hh"
#include "BDSFieldMagMultipoleOuterOld.hh"
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSFieldMagOctupole.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagSextupole.hh"
#include "BDSFieldMagSolenoidSheet.hh"
#include "BDSFieldMagSkewOwn.hh"
#include "BDSFieldMagUndulator.hh"
#include "BDSFieldMagZero.hh"
#include "BDSFieldObjects.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorCavityFringe.hh"
#include "BDSIntegratorDecapole.hh"
#include "BDSIntegratorDipoleRodrigues.hh"
#include "BDSIntegratorDipoleRodrigues2.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSIntegratorDipoleFringeScaling.hh"
#include "BDSIntegratorDipoleQuadrupole.hh"
#include "BDSIntegratorEuler.hh"
#include "BDSIntegratorG4RK4MinStep.hh"
#include "BDSIntegratorKickerThin.hh"
#include "BDSIntegratorOctupole.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSIntegratorMultipoleThin.hh"
#include "BDSIntegratorParallelTransport.hh"
#include "BDSIntegratorSextupole.hh"
#include "BDSIntegratorSolenoid.hh"
#include "BDSIntegratorTeleporter.hh"
#include "BDSIntegratorRMatrixThin.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetOuterFactoryLHC.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSModulator.hh"
#include "BDSModulatorInfo.hh"
#include "BDSModulatorSinT.hh"
#include "BDSModulatorTopHatT.hh"
#include "BDSModulatorType.hh"
#include "BDSParser.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/field.h"
#include "parser/modulator.h"

#include "globals.hh" // geant4 types / globals
#include "G4EquationOfMotion.hh"
#include "G4EqMagElectricField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Version.hh"

// geant4 integrators
#include "G4CashKarpRKF45.hh"
#include "G4ClassicalRK4.hh"
#include "G4ConstRK4.hh"
#include "G4ExactHelixStepper.hh"
#include "G4ExplicitEuler.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixHeum.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixMixedStepper.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4ImplicitEuler.hh"
#include "G4NystromRK4.hh"
#include "G4RKG3_Stepper.hh"
#include "G4SimpleHeum.hh"
#include "G4SimpleRunge.hh"
#if G4VERSION_NUMBER > 1029
#include "G4BogackiShampine23.hh"
#include "G4BogackiShampine45.hh"
#include "G4DoLoMcPriRK34.hh"
#include "G4DormandPrince745.hh"
#include "G4DormandPrinceRK56.hh"
#include "G4TsitourasRK45.hh"
#endif
#if G4VERSION_NUMBER > 1039
#include "G4DormandPrinceRK78.hh"
#include "G4RK547FEq1.hh"
#include "G4RK547FEq2.hh"
#include "G4RK547FEq3.hh"
#endif

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <limits>
#include <map>
#include <utility>
#include <vector>

const BDSParticleDefinition* BDSFieldFactory::designParticle = nullptr;
BDSPrimaryGeneratorAction* BDSFieldFactory::primaryGeneratorAction = nullptr;

BDSFieldFactory* BDSFieldFactory::instance = nullptr;

BDSFieldFactory* BDSFieldFactory::Instance()
{
  if (!instance)
    {instance = new BDSFieldFactory();}
  return instance;
}

BDSFieldFactory::BDSFieldFactory():
  useOldMultipoleOuterFields(false)
{
  G4double defaultRigidity = std::numeric_limits<double>::max();
  if (designParticle)
    {defaultRigidity = designParticle->BRho();}
  // we do this so we don't have to have used the parser to use this class
  if (BDSParser::IsInitialised())
    {
      PrepareModulatorDefinitions(BDSParser::Instance()->GetModulators());
      PrepareFieldDefinitions(BDSParser::Instance()->GetFields(), defaultRigidity);
    }
  useOldMultipoleOuterFields = BDSGlobalConstants::Instance()->UseOldMultipoleOuterFields();
}

BDSFieldFactory::~BDSFieldFactory()
{
  for (auto& info : parserDefinitions)
    {delete info.second;}
  for (auto& info : parserModulatorDefinitions)
    {delete info.second;}
}

void BDSFieldFactory::PrepareFieldDefinitions(const std::vector<GMAD::Field>& definitions,
                                              G4double defaultBRho)
{
  for (const auto& definition : definitions)
    {
      if (definition.type.empty())
        {
          G4String msg = "\"type\" not specified in field definition \"";
          msg += definition.name + "\", but required.";
          throw BDSException(__METHOD_NAME__, msg);
        }
      BDSFieldType    fieldType = BDS::DetermineFieldType(definition.type);
      BDSIntegratorType intType = BDS::DetermineIntegratorType(definition.integrator);

      G4ThreeVector offset = G4ThreeVector(definition.x*CLHEP::m,
                                           definition.y*CLHEP::m,
                                           definition.z*CLHEP::m);

      G4RotationMatrix rm;
      if (definition.axisAngle)
        {
          G4ThreeVector axis = G4ThreeVector(definition.axisX,
                                             definition.axisY,
                                             definition.axisZ);
          rm = G4RotationMatrix(axis, definition.angle*CLHEP::rad);
        }
      else
        {
          if (BDS::IsFinite(definition.phi)   ||
              BDS::IsFinite(definition.theta) ||
              BDS::IsFinite(definition.psi))
            {// only build if finite
              CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(definition.phi*CLHEP::rad,
                                                                definition.theta*CLHEP::rad,
                                                                definition.psi*CLHEP::rad);
              rm = G4RotationMatrix(ang);
            }
          // else rm is default rotation matrix
        }
      
      G4Transform3D transform = G4Transform3D(rm, offset);

      BDSFieldFormat magFormat = BDSFieldFormat::none;
      G4String       magFile   = "";
      G4bool  magFileSpecified = !definition.magneticFile.empty();
      if (magFileSpecified)
        {
          std::pair<G4String, G4String> bf = BDS::SplitOnColon(G4String(definition.magneticFile));
          magFormat = BDS::DetermineFieldFormat(bf.first);
          magFile   = BDS::GetFullPath(bf.second);
        }
      
      BDSFieldFormat eleFormat = BDSFieldFormat::none;
      G4String       eleFile   = "";
      G4bool  eleFileSpecified = !definition.electricFile.empty();
      if (eleFileSpecified)
        {
          std::pair<G4String, G4String> ef = BDS::SplitOnColon(G4String(definition.electricFile));
          eleFormat = BDS::DetermineFieldFormat(ef.first);
          eleFile   = BDS::GetFullPath(ef.second);
        }
      
      BDSInterpolatorType magIntType = BDSInterpolatorType::cubic3d;
      if (magFileSpecified)
        {// determine and check type of integrator
          G4int nDimFF = BDS::NDimensionsOfFieldFormat(magFormat);
          if (!definition.magneticInterpolator.empty())
            {
              magIntType = BDS::DetermineInterpolatorType(G4String(definition.magneticInterpolator));
              // detect if an auto type and match up accordingly, else check it's the right one
              if (BDS::InterpolatorTypeIsAuto(magIntType))
                {magIntType = BDS::InterpolatorTypeSpecificFromAuto(nDimFF, magIntType);}
              else
                {
                  G4int nDimInt = BDS::NDimensionsOfInterpolatorType(magIntType);
                  if (nDimFF != nDimInt)
                    {
                      G4String message = "mismatch in number of dimensions between magnetic interpolator ";
                      message += "and field map format for field definition \"" + definition.name + "\"";
                      throw BDSException(__METHOD_NAME__, message);
                    }
                }
            }
          else
            {magIntType = DefaultInterpolatorType(nDimFF);}
        }
      
      BDSInterpolatorType eleIntType = BDSInterpolatorType::cubic3d;
      if (eleFileSpecified)
        {// determine and check type of integrator
          G4int nDimFF = BDS::NDimensionsOfFieldFormat(eleFormat);
          if (!definition.electricInterpolator.empty())
            {
              eleIntType = BDS::DetermineInterpolatorType(G4String(definition.electricInterpolator));
              // detect if an auto type and match up accordingly, else check it's the right one
              if (BDS::InterpolatorTypeIsAuto(eleIntType))
                {eleIntType = BDS::InterpolatorTypeSpecificFromAuto(nDimFF, eleIntType);}
              else
                {
                  G4int nDimInt = BDS::NDimensionsOfInterpolatorType(eleIntType);
                  if (nDimFF != nDimInt)
                    {
                      G4String message = "mismatch in number of dimensions between electric interpolator ";
                      message += "and field map format for field definition \"" + definition.name + "\"";
                      throw BDSException(__METHOD_NAME__, message);
                    }
                }
            }
          else
            {eleIntType = DefaultInterpolatorType(nDimFF);}
        }
      
      G4UserLimits* fieldLimit = nullptr;
      G4bool ignoreUpdateOfMaximumStepSize = definition.maximumStepLengthOverride > 0;
      if (definition.maximumStepLength > 0 || ignoreUpdateOfMaximumStepSize)
        {// only assign if specified
          auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
          // copy the default and update with the length of the object rather than the default 1m
          G4double limit = G4double(definition.maximumStepLength) * CLHEP::m;
          if (ignoreUpdateOfMaximumStepSize)
            {
              limit = definition.maximumStepLengthOverride * CLHEP::m;
              G4cout << __METHOD_NAME__ << "maximumStepLengthOverride set to " << limit << " mm for field definition \""
                     << definition.name << "\" -> careful!" << G4endl;
            }
          G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, limit, 1.0);
          // only specify a user limit object if the step length was specified
          if (ul != defaultUL)
            {fieldLimit = ul;}
        }
      
      BDSMagnetStrength* st = new BDSMagnetStrength();
      G4double poleTipRadius = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->aper1;
      if (!definition.fieldParameters.empty())
        {PrepareFieldStrengthFromParameters(st, definition.fieldParameters, poleTipRadius);}
      
      BDSFieldInfo* info = new BDSFieldInfo(fieldType,
                                            defaultBRho,
                                            intType,
                                            st,
                                            G4bool(definition.globalTransform),
                                            transform,
                                            magFile,
                                            magFormat,
                                            magIntType,
                                            eleFile,
                                            eleFormat,
                                            eleIntType,
                                            false,   /*don't cache transforms*/
                                            G4double(definition.eScaling),
                                            G4double(definition.bScaling),
                                            G4double(definition.t*CLHEP::s),
                                            G4bool(definition.autoScale),
                                            fieldLimit);
      info->SetScalingRadius(poleTipRadius);
      info->SetModulatorInfo(GetModulatorDefinition(definition.fieldModulator));
      if (ignoreUpdateOfMaximumStepSize)
        {info->SetIgnoreUpdateOfMaximumStepSize(true);}

      if (!definition.magneticSubField.empty())
        {
          if (definition.magneticSubField == definition.name)
            {throw BDSException(__METHOD_NAME__, "error in \"" + definition.name + "\": magneticSubField cannot be the field itself");}
          info->SetMagneticSubField(G4String(definition.magneticSubField));
        }
      if (!definition.electricSubField.empty())
        {
          if (definition.electricSubField == definition.name)
            {throw BDSException(__METHOD_NAME__, "error in \"" + definition.name + "\": electricSubField cannot be the field itself");}
          info->SetElectricSubField(G4String(definition.electricSubField));
        }
      if (!definition.magneticReflection.empty())
        {
          G4String magneticReflection = G4String(definition.magneticReflection);
          BDSArrayReflectionTypeSet mar = BDS::DetermineArrayReflectionTypeSet(magneticReflection);
          info->SetMagneticArrayReflectionType(mar);
        }
      if (!definition.electricReflection.empty())
        {
          G4String electricReflection = G4String(definition.electricReflection);
          BDSArrayReflectionTypeSet ear = BDS::DetermineArrayReflectionTypeSet(electricReflection);
          info->SetElectricArrayReflectionType(ear);
        }
      
      info->SetNameOfParserDefinition(G4String(definition.name));
      if (BDSGlobalConstants::Instance()->Verbose())
        {
          G4cout << "Definition: \"" << definition.name << "\"" << G4endl;
          G4cout << *info << G4endl;
        }
      parserDefinitions[G4String(definition.name)] = info;
    }
}

void BDSFieldFactory::PrepareModulatorDefinitions(const std::vector<GMAD::Modulator>& definitions)
{
  for (const auto& definition : definitions)
    {
      if (definition.type.empty())
        {
          G4String msg = "\"type\" not specified in modulator definition \"";
          msg += definition.name + "\", but required.";
          throw BDSException(__METHOD_NAME__, msg);
        }
      BDSModulatorType modulatorType = BDS::DetermineModulatorType(definition.type);

      G4double frequency = definition.frequency * CLHEP::hertz;
      G4double phase = definition.phase * CLHEP::rad;
      if (BDS::IsFinite(definition.tOffset))
        {
          if (BDS::IsFinite(definition.tOffset))
            {throw BDSException(__METHOD_NAME__, "definition: " + definition.name + " has both tOffset and phase specified - only one can be used.");}
          phase = definition.tOffset*CLHEP::s * frequency * CLHEP::twopi;
        }
      
      // We can't calculate any global phase here because this one modulator info may
      // be used by multiple beam line elements at different locations
      BDSModulatorInfo* info = new BDSModulatorInfo(modulatorType,
                                                    frequency,
                                                    phase,
                                                    definition.amplitudeScale,
                                                    definition.amplitudeOffset,
                                                    definition.T0,
                                                    definition.T1);
      info->nameOfParserDefinition = definition.name;
      parserModulatorDefinitions[G4String(definition.name)] = info;
    }
}

G4double BDSFieldFactory::ConvertToDoubleWithException(const G4String& value,
                                                       const G4String& parameterNameForError) const
{
  G4double result = 0;
  try
    {result = std::stod(value);}
  catch (std::exception& e)
    {
      G4String msg(e.what());
      G4String baseMsg = "Unable to interpret value (\"" + value + "\" of parameter \"";
      baseMsg += parameterNameForError + "\" as a number: ";
      throw BDSException(__METHOD_NAME__, baseMsg + msg);
    }
  return result;
}

void BDSFieldFactory::PrepareFieldStrengthFromParameters(BDSMagnetStrength* st,
                                                         const G4String& fieldParameters,
                                                         G4double& poleTipRadius) const
{
  // use function from BDSUtilities to process user params string into
  // map of strings and values.
  std::map<G4String, G4String> map = BDS::GetUserParametersMap(fieldParameters, '=');
  
  for (const auto& keyValue : map)
    {
      if (BDSMagnetStrength::ValidKey(keyValue.first))
        {
          G4double value = ConvertToDoubleWithException(keyValue.second, keyValue.first);
          (*st)[keyValue.first] = value * BDSMagnetStrength::Unit(keyValue.first);
        }
      else if (keyValue.first == "poletipradius")
        {poleTipRadius = ConvertToDoubleWithException(keyValue.second, keyValue.first) * CLHEP::m;}
      else
        {
          G4String msg = "Invalid key \"" + keyValue.first + "\" for field parameters. ";
          msg += "Acceptable parameters are: \n";
          const std::vector<G4String>& allKeys = BDSMagnetStrength::AllKeys();
          for (G4int i = 0; i < (G4int)allKeys.size(); i++)
            {
              msg += allKeys[i] + ", ";
              if ((i % 10 == 0) && (i > 0))
                {msg += "\n";}
            }
          throw BDSException(__METHOD_NAME__, msg);
        }
    }
}

BDSFieldInfo* BDSFieldFactory::GetDefinition(const G4String& name) const
{
  // Here we test if the string is empty and return nullptr. We do this so
  // this method can be used without exiting when no key is specified at all.
  // If a key is given and not found, then that requires the users attention.
  if (name.empty())
    {return nullptr;}
  auto result = parserDefinitions.find(name);
  if (result == parserDefinitions.end())
    {// not a valid key
      G4cerr << __METHOD_NAME__ << "\"" << name << "\" is not a valid field specifier" << G4endl;
      G4cout << "Defined field specifiers are:" << G4endl;
      for (const auto& it : parserDefinitions)
        {G4cout << "\"" << it.first << "\"" << G4endl;}
      throw BDSException(__METHOD_NAME__, "invalid field name");
    }
  return result->second;
}

BDSModulatorInfo* BDSFieldFactory::GetModulatorDefinition(const G4String& modulatorName) const
{
  if (modulatorName.empty())
    {return nullptr;}
  
  auto search = parserModulatorDefinitions.find(modulatorName);
  if (search == parserModulatorDefinitions.end())
    {
      G4cerr << __METHOD_NAME__ << "\"" << modulatorName << "\" is not a valid modulator definition name" << G4endl;
      G4cout << "Defined modulator definitions are:" << G4endl;
      for (const auto& it : parserModulatorDefinitions)
        {G4cout << "\"" << it.first << "\"" << G4endl;}
      throw BDSException(__METHOD_NAME__, "invalid modulator name");
    }
  else
    {return search->second;}
}

BDSFieldObjects* BDSFieldFactory::CreateField(const BDSFieldInfo&      info,
                                              const BDSMagnetStrength* scalingStrength,
                                              const G4String&          scalingKey)
{
  // Forward on to delegate functions for the main types of field
  // such as E, EM and Magnetic
  BDSFieldObjects* field = nullptr;
  
  if (info.FieldType() == BDSFieldType::none)
    {return field;} // as nullptr

  BDSFieldClassType clas = BDS::DetermineFieldClassType(info.FieldType());
  try
    {
      switch (clas.underlying())
	{
	case BDSFieldClassType::magnetic:
	  {field = CreateFieldMag(info, scalingStrength, scalingKey); break;}
	case BDSFieldClassType::electromagnetic:
	  {field = CreateFieldEM(info); break;}
	case BDSFieldClassType::electric:
	  {field = CreateFieldE(info); break;}
	case BDSFieldClassType::irregular:
	  {field = CreateFieldIrregular(info); break;}
	default:
	  {break;} // this will return nullptr
	}
    }
  catch (BDSException& e)
    {
      e.AppendToMessage("\nProblem with field possibly named \"" + info.NameOfParserDefinition() + "\"");
      throw e;
    }
  return field;
}

BDSInterpolatorType BDSFieldFactory::DefaultInterpolatorType(G4int numberOfDimensions)
{
  BDSInterpolatorType result;
  switch (numberOfDimensions)
    {
      case 1:
        {result = BDSInterpolatorType::cubic1d; break;}
      case 2:
        {result = BDSInterpolatorType::cubic2d; break;}
      case 3:
        {result = BDSInterpolatorType::cubic3d; break;}
      case 4:
        {result = BDSInterpolatorType::cubic4d; break;}
      default:
        {throw BDSException(__METHOD_NAME__, "unsupported number of dimensions " + std::to_string(numberOfDimensions));}
    }
  return result;
}
      
BDSFieldObjects* BDSFieldFactory::CreateFieldMag(const BDSFieldInfo&      info,
                                                 const BDSMagnetStrength* scalingStrength,
                                                 const G4String&          scalingKey)
{
  const BDSMagnetStrength* strength = info.MagnetStrength();
  BDSFieldMag* field = CreateFieldMagRaw(info, scalingStrength, scalingKey);
  if (!field)
    {return nullptr;} // return nullptr of right type

  BDSFieldMag* resultantField = field;
  // Optionally provide local to global transform using curvilinear coordinate system.
  if (info.UsePlacementWorldTransform())
    {resultantField = new BDSFieldMagGlobalPlacement(field);}
  else if (info.ProvideGlobal())
    {resultantField = new BDSFieldMagGlobal(field);}

  // Always this equation of motion for magnetic (only) fields
  BDSMagUsualEqRhs* eqOfM = new BDSMagUsualEqRhs(resultantField);

  // Create appropriate integrator
  G4MagIntegratorStepper* integrator = CreateIntegratorMag(info, eqOfM, strength);

  BDSFieldObjects* completeField = new BDSFieldObjects(&info, resultantField, eqOfM, integrator);
  return completeField;
}

BDSFieldMag* BDSFieldFactory::CreateFieldMagRaw(const BDSFieldInfo&      info,
                                                const BDSMagnetStrength* scalingStrength,
                                                const G4String&          scalingKey)
{
  BDSFieldMag* field = nullptr;
  const BDSMagnetStrength* strength = info.MagnetStrength();
  G4double brho           = info.BRho();
  G4double poleTipRadius  = info.PoleTipRadius();
  G4double beamPipeRadius = info.BeamPipeRadius();
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::bmap1d:
    case BDSFieldType::bmap2d:
    case BDSFieldType::bmap3d:
    case BDSFieldType::bmap4d:
    case BDSFieldType::mokka:
      {
        BDSFieldMagInterpolated* ff = BDSFieldLoader::Instance()->LoadMagField(info,
                                                                               scalingStrength,
                                                                               scalingKey);
        if (ff)
          {info.UpdateUserLimitsLengthMaximumStepSize(ff->SmallestSpatialStep(), true);}
        field = ff;
        break;
      }
    case BDSFieldType::bfieldzero:
      {field = new BDSFieldMagZero(); break;}
    case BDSFieldType::solenoid:
    case BDSFieldType::dipole:
    case BDSFieldType::dipole3d:
      {field = new BDSFieldMagDipole(strength); break;}
    case BDSFieldType::solenoidsheet:
      {field = new BDSFieldMagSolenoidSheet(strength, poleTipRadius); break;}
    case BDSFieldType::quadrupole:
      {field = new BDSFieldMagQuadrupole(strength, brho); break;}
    case BDSFieldType::undulator:
      {field = new BDSFieldMagUndulator(strength, beamPipeRadius); break;}
    case BDSFieldType::dipolequadrupole:
      {field = new BDSFieldMagDipoleQuadrupole(strength, brho); break;}
    case BDSFieldType::sextupole:
      {field = new BDSFieldMagSextupole(strength, brho); break;}
    case BDSFieldType::octupole:
      {field = new BDSFieldMagOctupole(strength, brho); break;}
    case BDSFieldType::decapole:
      {field = new BDSFieldMagDecapole(strength, brho); break;}
    case BDSFieldType::multipole:
      {field = new BDSFieldMagMultipole(strength, brho); break;}
    case BDSFieldType::muonspoiler:
      {field = new BDSFieldMagMuonSpoiler(strength, brho); break;}
    case BDSFieldType::skewquadrupole:
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagQuadrupole(strength, brho), CLHEP::pi/4.); break;}
    case BDSFieldType::skewsextupole:
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagSextupole(strength, brho), CLHEP::pi/6.); break;}
    case BDSFieldType::skewoctupole:
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagOctupole(strength, brho), CLHEP::pi/8.); break;}
    case BDSFieldType::skewdecapole:
      {field = new BDSFieldMagSkewOwn(new BDSFieldMagDecapole(strength, brho), CLHEP::pi/10.); break;}
    case BDSFieldType::multipoleouterdipole:
      {// suitable only for querying transversely in x,y - no 3d nature
        BDSFieldMag* innerField = new BDSFieldMagDipole(strength);
        G4bool positiveField = (*strength)["field"] < 0; // convention for dipoles - "positive"
        if (useOldMultipoleOuterFields)
          {field = new BDSFieldMagMultipoleOuterOld(1, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else // positive field for a dipole has no meaning for this class - fix at false
          {field = new BDSFieldMagMultipoleOuter(1, poleTipRadius, innerField, false, brho, GetOuterScaling(strength));}
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::multipoleouterquadrupole:
      {
        BDSFieldMag* innerField = new BDSFieldMagQuadrupole(strength, brho);
        G4bool positiveField = (*strength)["k1"] > 0;
        if (useOldMultipoleOuterFields)
          {field = new BDSFieldMagMultipoleOuterOld(2, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {field = new BDSFieldMagMultipoleOuter(2, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::multipoleoutersextupole:
      {
        BDSFieldMag* innerField = new BDSFieldMagSextupole(strength, brho);
        G4bool positiveField = (*strength)["k2"] > 0;
        if (useOldMultipoleOuterFields)
          {field = new BDSFieldMagMultipoleOuterOld(3, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {field = new BDSFieldMagMultipoleOuter(3, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::multipoleouteroctupole:
      {
        BDSFieldMag* innerField = new BDSFieldMagOctupole(strength, brho);
        G4bool positiveField = (*strength)["k3"] > 0;
        if (useOldMultipoleOuterFields)
          {field = new BDSFieldMagMultipoleOuterOld(4, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {field = new BDSFieldMagMultipoleOuter(4, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::multipoleouterdecapole:
      {
        BDSFieldMag* innerField = new BDSFieldMagDecapole(strength, brho);
        G4bool positiveField = (*strength)["k4"] > 0;
        if (useOldMultipoleOuterFields)
          {field = new BDSFieldMagMultipoleOuterOld(5, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {field = new BDSFieldMagMultipoleOuter(5, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::skewmultipoleouterquadrupole:
      {
        BDSFieldMag* innerField = new BDSFieldMagQuadrupole(strength, brho);
        G4bool positiveField = (*strength)["k1"] > 0;
        BDSFieldMag* normalField;
        if (useOldMultipoleOuterFields)
          {normalField = new BDSFieldMagMultipoleOuterOld(2, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {normalField = new BDSFieldMagMultipoleOuter(2, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/4.);
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::skewmultipoleoutersextupole:
      {
        BDSFieldMag* innerField = new BDSFieldMagSextupole(strength, brho);
        G4bool positiveField = (*strength)["k2"] > 0;
        BDSFieldMag* normalField;
        if (useOldMultipoleOuterFields)
          {normalField = new BDSFieldMagMultipoleOuterOld(3, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {normalField = new BDSFieldMagMultipoleOuter(3, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/6.);
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::skewmultipoleouteroctupole:
      {
        BDSFieldMag* innerField = new BDSFieldMagOctupole(strength, brho);
        G4bool positiveField = (*strength)["k3"] > 0;
        BDSFieldMag* normalField;
        if (useOldMultipoleOuterFields)
          {normalField = new BDSFieldMagMultipoleOuterOld(4, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {normalField = new BDSFieldMagMultipoleOuter(4, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/8.);
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::skewmultipoleouterdecapole:
      {
        BDSFieldMag* innerField = new BDSFieldMagDecapole(strength, brho);
        G4bool positiveField = (*strength)["k4"] > 0;
        BDSFieldMag* normalField;
        if (useOldMultipoleOuterFields)
          {normalField = new BDSFieldMagMultipoleOuterOld(5, poleTipRadius, innerField, positiveField, GetOuterScaling(strength));}
        else
          {normalField = new BDSFieldMagMultipoleOuter(5, poleTipRadius, innerField, positiveField, brho, GetOuterScaling(strength));}
        field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/10.);
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::multipoleouterdipole3d:
      {
        if (useOldMultipoleOuterFields)
          {field = new BDSFieldMagDipoleOuterOld(strength, poleTipRadius, GetOuterScaling(strength));}
        else
          {field = new BDSFieldMagDipoleOuter(strength, poleTipRadius, GetOuterScaling(strength));}
        break;
      }
    case BDSFieldType::multipoleouterdipolelhc:
      {
        BDSFieldMag* innerField = new BDSFieldMagDipole(strength);
        G4bool positiveField = (*strength)["field"] < 0; // convention for dipoles - "positive"
        G4bool positiveField2 = (*strength)["angle"] > 0;
        G4double dx = BDSMagnetOuterFactoryLHC::beamSeparation;
        if (useOldMultipoleOuterFields)
          {
            field = new BDSFieldMagMultipoleOuterDualOld(1, poleTipRadius, innerField, positiveField, dx,
                                                         info.SecondFieldOnLeft(), GetOuterScaling(strength));
          }
        else
          {
            field = new BDSFieldMagMultipoleOuterDual(1, poleTipRadius, innerField, positiveField2, brho, dx,
                                                      info.SecondFieldOnLeft(), GetOuterScaling(strength));
          }
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::multipoleouterquadrupolelhc:
      {
        BDSFieldMag* innerField = new BDSFieldMagQuadrupole(strength, brho);
        G4bool positiveField = (*strength)["k1"] > 0;
        G4double dx = BDSMagnetOuterFactoryLHC::beamSeparation;
        if (useOldMultipoleOuterFields)
          {
            field = new BDSFieldMagMultipoleOuterDualOld(2, poleTipRadius, innerField, positiveField, dx,
                                                         info.SecondFieldOnLeft(), GetOuterScaling(strength));
          }
        else
          {
            field = new BDSFieldMagMultipoleOuterDual(2, poleTipRadius, innerField, positiveField, brho, dx,
                                                      info.SecondFieldOnLeft(), GetOuterScaling(strength));
          }
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::multipoleoutersextupolelhc:
      {
        BDSFieldMag* innerField = new BDSFieldMagSextupole(strength, brho);
        G4bool positiveField = (*strength)["k2"] > 0;
        G4double dx = BDSMagnetOuterFactoryLHC::beamSeparation;
        if (useOldMultipoleOuterFields)
          {
            field = new BDSFieldMagMultipoleOuterDualOld(3, poleTipRadius, innerField, positiveField, dx,
                                                         info.SecondFieldOnLeft(), GetOuterScaling(strength));
          }
        else
          {
            field = new BDSFieldMagMultipoleOuterDual(3, poleTipRadius, innerField, positiveField, dx, brho,
                                                      info.SecondFieldOnLeft(), GetOuterScaling(strength));
          }
        delete innerField; // no longer required
        break;
      }
    case BDSFieldType::paralleltransporter:
    default:
      {// there is no need for case BDSFieldType::none as this won't be used in this function.
        break;
      }
    }

  // Set transform for local geometry offset
  // Do this before wrapping in global converter BDSFieldMagGlobal so that the sub-field
  // has it and not the global wrapper.
  if (field)
    {
      field->SetTransform(info.TransformComplete());
  
      if (info.ModulatorInfo())
        {
          BDSModulator* modulator = CreateModulator(info.ModulatorInfo(), info);
          if (modulator->VariesWithTime() && field->TimeVarying())
            {BDS::Warning(__METHOD_NAME__, "using a time varying modulation on a time varying field for field \"" + info.NameOfParserDefinition() + "\"");}
          field->SetModulator(modulator);
          if (info.IgnoreUpdateOfMaximumStepSize())
            {
              G4cout << "maximumStepLengthOverride used for field definition \"" << info.NameOfParserDefinition()
                     << "\" so will not reduce maximum step size for modulator\n";
              G4cout << "Would be " << modulator->RecommendedMaxStepLength() << " mm" << G4endl;
            }
          info.UpdateUserLimitsLengthMaximumStepSize(modulator->RecommendedMaxStepLength(), true);
        }
    }
  
  if (!info.MagneticSubFieldName().empty() && field)
    {
      // set the transform of the 'main' field to only the transform defined in that field definition
      field->SetTransform(info.Transform());
      
      auto mainField = dynamic_cast<BDSFieldMagInterpolated*>(field);
      if (!mainField)
              {throw BDSException(__METHOD_NAME__, "subfield specified for non-field map type field - not supported");}
  
      BDSFieldInfo* subFieldRecipe = new BDSFieldInfo(*(GetDefinition(info.MagneticSubFieldName())));
      BDSFieldMag* subFieldRaw = CreateFieldMagRaw(*subFieldRecipe, scalingStrength, scalingKey);
      auto subField = dynamic_cast<BDSFieldMagInterpolated*>(subFieldRaw);
      if (!subField)
        {throw BDSException(__METHOD_NAME__, "subfield type is not a field map type field - not supported");}
      field = new BDSFieldMagInterpolated2Layer(mainField, subField);
      // the transform goes beamline transform to the 2Layer class, then inside that the individual field transforms
      field->SetTransform(info.TransformBeamline());
      delete subFieldRecipe;
    }
  
  return field;
}

BDSFieldObjects* BDSFieldFactory::CreateFieldEM(const BDSFieldInfo& info)
{
  BDSFieldEM* field = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::rfpillbox:
      {field = new BDSFieldEMRFCavity(info.MagnetStrength()); break;}
    case BDSFieldType::ebmap1d:
    case BDSFieldType::ebmap2d:
    case BDSFieldType::ebmap3d:
    case BDSFieldType::ebmap4d:
      {
        BDSFieldEMInterpolated* ff = BDSFieldLoader::Instance()->LoadEMField(info);
        if (ff)
          {info.UpdateUserLimitsLengthMaximumStepSize(ff->SmallestSpatialStep(), true);}
        field = ff;
        break;
      }
    case BDSFieldType::ebfieldzero:
      {field = new BDSFieldEMZero(); break;}
    case BDSFieldType::gaborlens:
      {field = new BDSFieldGaborLens(info.MagnetStrength()); break;}
    default:
      return nullptr;
      break;
    }
  
  // Set transform for local geometry offset
  if (field)
    {
      field->SetTransform(info.TransformComplete());
  
      if (info.ModulatorInfo())
        {
          BDSModulator* modulator = CreateModulator(info.ModulatorInfo(), info);
          if (modulator->VariesWithTime() && field->TimeVarying())
            {BDS::Warning(__METHOD_NAME__, "using a time varying modulation on a time varying field for field \"" + info.NameOfParserDefinition() + "\"");}
          field->SetModulator(modulator);
          info.UpdateUserLimitsLengthMaximumStepSize(modulator->RecommendedMaxStepLength(), true);
        }
    }
  
  if (!field)
    {return nullptr;}
  
  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldEM* resultantField = field;
  if (info.UsePlacementWorldTransform())
    {resultantField = new BDSFieldEMGlobalPlacement(field);}
  else if (info.ProvideGlobal())
    {resultantField = new BDSFieldEMGlobal(field);}

  // Equation of motion for em fields
  G4EqMagElectricField* eqOfM = new G4EqMagElectricField(resultantField);

  // Create appropriate integrator
  G4MagIntegratorStepper* integrator = CreateIntegratorEM(info, eqOfM);

  BDSFieldObjects* completeField = new BDSFieldObjects(&info, resultantField, eqOfM, integrator);
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CreateFieldE(const BDSFieldInfo& info)
{
  BDSFieldE* field = CreateFieldERaw(info);
  if (!field)
    {return nullptr;}
  
  // Optionally provide local to global transform using curvilinear coordinate system.
  BDSFieldE* resultantField = field;
  if (info.UsePlacementWorldTransform())
    {resultantField = new BDSFieldEGlobalPlacement(field);}
  else if (info.ProvideGlobal())
    {resultantField = new BDSFieldEGlobal(field);}

  // Equation of motion for em fields
  G4EqMagElectricField* eqOfM = new G4EqMagElectricField(resultantField);

  // Create appropriate integrator
  G4MagIntegratorStepper* integrator = CreateIntegratorE(info, eqOfM);

  BDSFieldObjects* completeField = new BDSFieldObjects(&info, resultantField, eqOfM, integrator);
  return completeField;
}

BDSFieldE* BDSFieldFactory::CreateFieldERaw(const BDSFieldInfo& info)
{
  BDSFieldE* field = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::rfconstantinx:
    case BDSFieldType::rfconstantiny:
    case BDSFieldType::rfconstantinz:
      {field = new BDSFieldESinusoid(info.MagnetStrength()); break;}
    case BDSFieldType::emap1d:
    case BDSFieldType::emap2d:
    case BDSFieldType::emap3d:
    case BDSFieldType::emap4d:
      {
        BDSFieldEInterpolated* ff = BDSFieldLoader::Instance()->LoadEField(info);
        if (ff)
          {info.UpdateUserLimitsLengthMaximumStepSize(ff->SmallestSpatialStep(), true);}
        field = ff;
        break;
      }
    case BDSFieldType::efieldzero:
      {field = new BDSFieldEZero(); break;}
    default:
      return nullptr;
      break;
    }
  
  // Set transform for local geometry offset
  if (field)
    {
      field->SetTransform(info.TransformComplete());
  
      if (info.ModulatorInfo())
        {
          BDSModulator* modulator = CreateModulator(info.ModulatorInfo(), info);
          if (modulator->VariesWithTime() && field->TimeVarying())
            {BDS::Warning(__METHOD_NAME__, "using a time varying modulation on a time varying field for field \"" + info.NameOfParserDefinition() + "\"");}
          field->SetModulator(modulator);
          info.UpdateUserLimitsLengthMaximumStepSize(modulator->RecommendedMaxStepLength(), true);
        }
    }
  
  if (!info.ElectricSubFieldName().empty() && field)
    {
      // set the transform of the 'main' field to only the transform defined in that field definition
      field->SetTransform(info.Transform());
      
      auto mainField = dynamic_cast<BDSFieldEInterpolated*>(field);
      if (!mainField)
        {throw BDSException(__METHOD_NAME__, "subfield specified for non-field map type field - not supported");}
      
      BDSFieldInfo* subFieldRecipe = new BDSFieldInfo(*(GetDefinition(info.ElectricSubFieldName())));
      BDSFieldE* subFieldRaw = CreateFieldERaw(*subFieldRecipe);
      auto subField = dynamic_cast<BDSFieldEInterpolated*>(subFieldRaw);
      if (!subField)
        {throw BDSException(__METHOD_NAME__, "subfield type is not a field map type field - not supported");}
      field = new BDSFieldEInterpolated2Layer(mainField, subField);
      // the transform goes beamline transform to the 2Layer class, then inside that the individual field transforms
      field->SetTransform(info.TransformBeamline());
      delete subFieldRecipe;
    }
  
  return field;
}

BDSFieldObjects* BDSFieldFactory::CreateFieldIrregular(const BDSFieldInfo& info)
{
  // special routine for each special / irregular field
  BDSFieldObjects* result = nullptr;
  switch (info.FieldType().underlying())
    {
    case BDSFieldType::teleporter:
      {result = CreateTeleporter(info); break;}
    case BDSFieldType::rmatrix:
      {result = CreateRMatrix(info); break;}
    case BDSFieldType::cavityfringe:
      {result = CreateCavityFringe(info); break;}
    case BDSFieldType::paralleltransporter:
      {result = CreateParallelTransport(info); break;}
    default:
      {break;}
    }
  return result;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorMag(const BDSFieldInfo&      info,
                                                             G4Mag_EqRhs*             eqOfM,
                                                             const BDSMagnetStrength* strength)
{
  const G4double minimumRadiusOfCurvature = 10*CLHEP::cm;
  G4double                      brho = info.BRho();
  G4MagIntegratorStepper* integrator = nullptr;
  // these ones can only be used for magnetic field
  switch (info.IntegratorType().underlying())
    {
    case BDSIntegratorType::solenoid:
      integrator = new BDSIntegratorSolenoid(strength, brho, eqOfM); break;
    case BDSIntegratorType::dipolerodrigues:
      integrator = new BDSIntegratorDipoleRodrigues(strength, brho, eqOfM); break;
    case BDSIntegratorType::dipolerodrigues2:
      integrator = new BDSIntegratorDipoleRodrigues2(eqOfM, minimumRadiusOfCurvature); break;
    case BDSIntegratorType::dipolematrix:
      integrator = new BDSIntegratorDipoleQuadrupole(strength, brho, eqOfM, minimumRadiusOfCurvature, designParticle, info.Tilt()); break;
    case BDSIntegratorType::quadrupole:
      integrator = new BDSIntegratorQuadrupole(strength, brho, eqOfM, minimumRadiusOfCurvature); break;
    case BDSIntegratorType::sextupole:
      integrator = new BDSIntegratorSextupole(strength, brho, eqOfM); break;
    case BDSIntegratorType::octupole:
      integrator = new BDSIntegratorOctupole(strength, brho, eqOfM); break;
    case BDSIntegratorType::decapole:
      integrator = new BDSIntegratorDecapole(strength, brho, eqOfM); break;
    case BDSIntegratorType::multipolethin:
      integrator = new BDSIntegratorMultipoleThin(strength, brho, eqOfM); break;
    case BDSIntegratorType::dipolefringe:
      integrator = new BDSIntegratorDipoleFringe(strength, brho, eqOfM, minimumRadiusOfCurvature, info.Tilt()); break;
    case BDSIntegratorType::dipolefringescaling:
      integrator = new BDSIntegratorDipoleFringeScaling(strength, brho, eqOfM, minimumRadiusOfCurvature, info.Tilt()); break;
    case BDSIntegratorType::euler:
      integrator = new BDSIntegratorEuler(eqOfM); break;
    case BDSIntegratorType::kickerthin:
      integrator = new BDSIntegratorKickerThin(strength, brho, eqOfM, minimumRadiusOfCurvature); break;
    case BDSIntegratorType::g4rk4minimumstep:
      integrator = new BDSIntegratorG4RK4MinStep(eqOfM, BDSGlobalConstants::Instance()->ChordStepMinimumYoke()); break;
    case BDSIntegratorType::rmatrixthin:
      integrator = new BDSIntegratorRMatrixThin(strength, eqOfM, info.BeamPipeRadius()); break;
    case BDSIntegratorType::cavityfringe:
      integrator = new BDSIntegratorCavityFringe(strength, eqOfM, brho, info.BeamPipeRadius()); break;
    case BDSIntegratorType::g4constrk4:
      integrator = new G4ConstRK4(eqOfM); break;
    case BDSIntegratorType::g4exacthelixstepper:
      integrator = new G4ExactHelixStepper(eqOfM); break;
    case BDSIntegratorType::g4helixexpliciteuler:
      integrator = new G4HelixExplicitEuler(eqOfM); break;
    case BDSIntegratorType::g4helixheum:
      integrator = new G4HelixHeum(eqOfM); break;
    case BDSIntegratorType::g4heliximpliciteuler:
      integrator = new G4HelixImplicitEuler(eqOfM); break;
    case BDSIntegratorType::g4helixmixedstepper:
      integrator = new G4HelixMixedStepper(eqOfM); break;
    case BDSIntegratorType::g4helixsimplerunge:
      integrator = new G4HelixSimpleRunge(eqOfM); break;
    case BDSIntegratorType::g4nystromrk4:
      integrator = new G4NystromRK4(eqOfM); break;
    case BDSIntegratorType::g4rkg3stepper:
      integrator = new G4RKG3_Stepper(eqOfM); break;
    case BDSIntegratorType::g4cashkarprkf45:
    case BDSIntegratorType::g4classicalrk4:
    case BDSIntegratorType::g4expliciteuler:
    case BDSIntegratorType::g4impliciteuler:
    case BDSIntegratorType::g4simpleheum:
    case BDSIntegratorType::g4simplerunge:
#if G4VERSION_NUMBER > 1029
    case BDSIntegratorType::g4bogackishampine23:
    case BDSIntegratorType::g4bogackishampine45:
    case BDSIntegratorType::g4dolomcprirk34:
    case BDSIntegratorType::g4dormandprince745:
    case BDSIntegratorType::g4dormandprincerk56:
    case BDSIntegratorType::g4tsitourasrk45:
#endif
#if G4VERSION_NUMBER > 1039
    case BDSIntegratorType::g4dormandprincerk78:
    case BDSIntegratorType::g4rk547feq1:
    case BDSIntegratorType::g4rk547feq2:
    case BDSIntegratorType::g4rk547feq3:
#endif
      integrator = CreateIntegratorEM(info, (G4EquationOfMotion*)eqOfM); break;
    default:
      break; // returns nullptr;
    }
  
  return integrator;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorEM(const BDSFieldInfo& info,
                                                            G4EquationOfMotion* eqOfM)
{
  G4MagIntegratorStepper* integrator = nullptr;
  switch (info.IntegratorType().underlying())
    {
      // do the EM ones first, then complain
    case BDSIntegratorType::g4cashkarprkf45:
      integrator = new G4CashKarpRKF45(eqOfM, 8); break;
    case BDSIntegratorType::g4classicalrk4:
      integrator = new G4ClassicalRK4(eqOfM, 8); break;
    case BDSIntegratorType::g4expliciteuler:
      integrator = new G4ExplicitEuler(eqOfM, 8); break;
    case BDSIntegratorType::g4impliciteuler:
      integrator = new G4ImplicitEuler(eqOfM, 8); break;
    case BDSIntegratorType::g4simpleheum:
      integrator = new G4SimpleHeum(eqOfM, 8); break;
    case BDSIntegratorType::g4simplerunge:
      integrator = new G4SimpleRunge(eqOfM, 8); break;
#if G4VERSION_NUMBER > 1029
    case BDSIntegratorType::g4bogackishampine23:
      {integrator = new G4BogackiShampine45(eqOfM, 8); break;}
    case BDSIntegratorType::g4bogackishampine45:
      {integrator = new G4BogackiShampine45(eqOfM, 8); break;}
    case BDSIntegratorType::g4dolomcprirk34:
      {integrator = new G4DoLoMcPriRK34(eqOfM, 8); break;}
    case BDSIntegratorType::g4dormandprince745:
      {integrator = new G4DormandPrince745(eqOfM, 8); break;}
    case BDSIntegratorType::g4dormandprincerk56:
      {integrator = new G4DormandPrinceRK56(eqOfM, 8); break;}
    case BDSIntegratorType::g4tsitourasrk45:
      {integrator = new G4TsitourasRK45(eqOfM, 8); break;}
#endif
#if G4VERSION_NUMBER > 1039
    case BDSIntegratorType::g4dormandprincerk78:
      {integrator = new G4DormandPrinceRK78(eqOfM, 8); break;}
    case BDSIntegratorType::g4rk547feq1:
      {integrator = new G4RK547FEq1(eqOfM, 8); break;}
    case BDSIntegratorType::g4rk547feq2:
      {integrator = new G4RK547FEq2(eqOfM, 8); break;}
    case BDSIntegratorType::g4rk547feq3:
      {integrator = new G4RK547FEq3(eqOfM, 8); break;}
#endif
    case BDSIntegratorType::solenoid:
    case BDSIntegratorType::dipolerodrigues:
    case BDSIntegratorType::quadrupole:
    case BDSIntegratorType::sextupole:
    case BDSIntegratorType::octupole:
    case BDSIntegratorType::decapole:
    case BDSIntegratorType::dipolefringe:
    case BDSIntegratorType::g4constrk4:
    case BDSIntegratorType::g4exacthelixstepper:
    case BDSIntegratorType::g4helixexpliciteuler:
    case BDSIntegratorType::g4helixheum:
    case BDSIntegratorType::g4heliximpliciteuler:
    case BDSIntegratorType::g4helixmixedstepper:
    case BDSIntegratorType::g4helixsimplerunge:
    case BDSIntegratorType::g4nystromrk4:
    case BDSIntegratorType::g4rkg3stepper:
      {
        G4cerr << "Error: integrator \"" << info.IntegratorType() << "\" is not suitable for an EM field." << G4endl;
        G4cout << "Suitable integrators are:" << G4endl;
        std::vector<BDSIntegratorType> types = {
          BDSIntegratorType::g4cashkarprkf45,
          BDSIntegratorType::g4classicalrk4,
          BDSIntegratorType::g4expliciteuler,
          BDSIntegratorType::g4impliciteuler,
          BDSIntegratorType::g4simpleheum,
          BDSIntegratorType::g4simplerunge
#if G4VERSION_NUMBER > 1029
          ,
          BDSIntegratorType::g4bogackishampine23,
          BDSIntegratorType::g4bogackishampine45,
          BDSIntegratorType::g4dolomcprirk34,
          BDSIntegratorType::g4dormandprince745,
          BDSIntegratorType::g4dormandprincerk56,
          BDSIntegratorType::g4tsitourasrk45
#endif
#if G4VERSION_NUMBER > 1039
          ,
          BDSIntegratorType::g4dormandprincerk78,
          BDSIntegratorType::g4rk547feq1,
          BDSIntegratorType::g4rk547feq2,
          BDSIntegratorType::g4rk547feq3
#endif
        };
        for (auto type : types)
          {G4cout << type << G4endl;}
        throw BDSException(__METHOD_NAME__, "invalid integrator type");
      }
    default:
      break; // returns nullptr;
    }
  return integrator;
}

G4MagIntegratorStepper* BDSFieldFactory::CreateIntegratorE(const BDSFieldInfo& info,
                                                           G4EquationOfMotion* eqOfM)
{
  return CreateIntegratorEM(info,eqOfM);
}

BDSFieldObjects* BDSFieldFactory::CreateTeleporter(const BDSFieldInfo& info)
{
  G4MagneticField* bGlobalField = new BDSFieldMagZero();
  G4Mag_EqRhs*     bEqOfMotion = new G4Mag_UsualEqRhs(bGlobalField);

  G4MagIntegratorStepper* integrator;
  auto mapfile = BDSGlobalConstants::Instance()->PTCOneTurnMapFileName(); // TBC - this shouldn't come from global constants
  BDSPTCOneTurnMap* otm = nullptr;

  if (!mapfile.empty())
    {
      otm = new BDSPTCOneTurnMap(mapfile, designParticle);
      primaryGeneratorAction->RegisterPTCOneTurnMap(otm);
    }

  integrator = new BDSIntegratorTeleporter(bEqOfMotion, info.TransformComplete(),
                                           (*info.MagnetStrength())["length"],
                                           otm);
  
  BDSFieldObjects* completeField = new BDSFieldObjects(&info, bGlobalField,
                                                       bEqOfMotion, integrator);
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CreateRMatrix(const BDSFieldInfo& info)
{
  G4MagneticField* bGlobalField       = new BDSFieldMagZero();
  G4Mag_EqRhs*     bEqOfMotion        = new G4Mag_UsualEqRhs(bGlobalField);
  G4MagIntegratorStepper* integrator  = new BDSIntegratorRMatrixThin(info.MagnetStrength(),bEqOfMotion,0.95*info.BeamPipeRadius());
  BDSFieldObjects* completeField      = new BDSFieldObjects(&info, bGlobalField,
                                                            bEqOfMotion, integrator);
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CreateCavityFringe(const BDSFieldInfo& info)
{
  BDSFieldMag* bGlobalField           = new BDSFieldMagZero();
  BDSMagUsualEqRhs* bEqOfMotion       = new BDSMagUsualEqRhs(bGlobalField);
  G4MagIntegratorStepper* integrator  = new BDSIntegratorCavityFringe(info.MagnetStrength(),bEqOfMotion,info.BRho(),0.95*info.BeamPipeRadius());
  BDSFieldObjects* completeField      = new BDSFieldObjects(&info, bGlobalField,
                                                                  bEqOfMotion, integrator);
  return completeField;
}

BDSFieldObjects* BDSFieldFactory::CreateParallelTransport(const BDSFieldInfo& info)
{
  G4MagneticField* bGlobalField       = new BDSFieldMagZero();
  G4Mag_EqRhs*     bEqOfMotion        = new G4Mag_UsualEqRhs(bGlobalField);
  G4MagIntegratorStepper* integrator  = new BDSIntegratorParallelTransport(bEqOfMotion);
  BDSFieldObjects* completeField      = new BDSFieldObjects(&info, bGlobalField,
                                                            bEqOfMotion, integrator);
  return completeField;
}

G4double BDSFieldFactory::GetOuterScaling(const BDSMagnetStrength* st) const
{
  G4double result = 1.0;
  if (st->KeyHasBeenSet("scalingOuter"))
    {result = (*st)["scalingOuter"];}
  return result;
}

BDSModulator* BDSFieldFactory::CreateModulator(const BDSModulatorInfo* modulatorRecipe,
                                               const BDSFieldInfo& info) const
{
  if (!modulatorRecipe)
    {return nullptr;}
  BDSModulator* result = nullptr;
  try
    {
      switch (modulatorRecipe->modulatorType.underlying())
        {
        case BDSModulatorType::sint:
          {
            result = new BDSModulatorSinT(modulatorRecipe->frequency,
                                          modulatorRecipe->phase,
                                          info.SynchronousT(),
                                          modulatorRecipe->amplitudeOffset,
                                          modulatorRecipe->scale);
            break;
          }
        case BDSModulatorType::singlobalt:
          {
            result = new BDSModulatorSinT(modulatorRecipe->frequency,
                                          modulatorRecipe->phase,
                                          0.0,
                                          modulatorRecipe->amplitudeOffset,
                                          modulatorRecipe->scale);
            break;
          }
        case BDSModulatorType::tophatt:
          {
            result = new BDSModulatorTopHatT(modulatorRecipe->T0,
                                             modulatorRecipe->T1,
                                             modulatorRecipe->scale);
            break;
          }
        case BDSModulatorType::none:
        default:
          {break;}
        }
    }
  catch (BDSException& e)
    {
      G4String extraMsg = "\nProblem in field definition for component \"" + info.NameOfParserDefinition() + "\"";
      e.AppendToMessage(extraMsg);
      throw e;
    }
  return result;
}
