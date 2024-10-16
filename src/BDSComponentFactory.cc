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
#include "BDSComponentFactory.hh"

// elements
#ifdef USE_AWAKE
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSAwakeSpectrometer.hh"
#endif
#include "BDSCavityElement.hh"
#include "BDSCollimatorCrystal.hh"
#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorJaw.hh"
#include "BDSCollimatorRectangular.hh"
#include "BDSCollimatorBeamMask.hh"
#include "BDSColours.hh"
#include "BDSColourFromMaterial.hh"
#include "BDSComponentFactoryUser.hh"
#ifdef USE_DICOM
#include "BDSCT.hh"
#include "BDSDicomIntersectVolume.hh"
#endif
#include "BDSDegrader.hh"
#include "BDSDrift.hh"
#include "BDSDump.hh"
#include "BDSElement.hh"
#include "BDSLaserWire.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSSamplerPlane.hh"
#include "BDSScreen.hh"
#include "BDSShield.hh"
#include "BDSTarget.hh"
#include "BDSTeleporter.hh"
#include "BDSTerminator.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"
#include "BDSWireScanner.hh"
#include "BDSUndulator.hh"
#include "BDSWarning.hh"

// general
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamlineIntegral.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBendBuilder.hh"
#include "BDSLine.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityFieldType.hh"
#include "BDSCavityType.hh"
#include "BDSCrystalInfo.hh"
#include "BDSCrystalType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSGap.hh"
#include "BDSGasCapillary.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "parser/element.h"
#include "parser/elementtype.h"
#include "parser/cavitymodel.h"
#include "parser/newcolour.h"
#include "parser/crystal.h"

#include <cmath>
#include <limits>
#include <string>
#include <utility>

using namespace GMAD;

G4bool BDSComponentFactory::coloursInitialised = false;

BDSComponentFactory::BDSComponentFactory(BDSComponentFactoryUser* userComponentFactoryIn,
                                         G4bool                   usualPrintOut):
  userComponentFactory(userComponentFactoryIn),
  lengthSafety(BDSGlobalConstants::Instance()->LengthSafety()),
  thinElementLength(BDSGlobalConstants::Instance()->ThinElementLength()),
  includeFringeFields(BDSGlobalConstants::Instance()->IncludeFringeFields()),
  yokeFields(BDSGlobalConstants::Instance()->YokeFields()),
  defaultModulator(nullptr),
  integralUpToThisComponent(nullptr),
  synchronousTAtMiddleOfThisComponent(0),
  integratorSetType(BDSGlobalConstants::Instance()->IntegratorSet())
{
  integratorSet = BDS::IntegratorSet(integratorSetType);
  if (usualPrintOut)
    {G4cout << __METHOD_NAME__ << "using \"" << integratorSetType << "\" set of integrators" << G4endl;}

  PrepareColours();      // prepare colour definitions from parser
  PrepareCavityModels(); // prepare rf cavity model info from parser
  PrepareCrystals();     // prepare crystal model info from parser

  // TBC - leave as nullptr
  //defaultModulator = BDSFieldFactory::Instance()->GetModulatorDefinition(BDSGlobalConstants::Instance()->FieldModulator());
}

BDSComponentFactory::~BDSComponentFactory()
{
  for (const auto& info : cavityInfos)
    {delete info.second;}
  for (const auto&  info : crystalInfos)
    {delete info.second;}

  // Deleted here although not used directly here as new geometry can only be
  // created through this class.
  try
    {//no exceptions in destructor
      delete BDSBeamPipeFactory::Instance();
      delete BDSMagnetOuterFactory::Instance();
    }
  catch (...)
    {;}
}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element const* elementIn,
							      Element const* prevElementIn,
							      Element const* nextElementIn,
							      BDSBeamlineIntegral& integral)
{
  switch (elementIn->type)
  {
    case ElementType::_MARKER:
    case ElementType::_LINE:
    case ElementType::_REV_LINE:
      {return nullptr;}
    default:
      {break;}
  }
#ifdef BDSDEBUG
  G4cout << elementIn->name << "\t " << integral.arcLength/CLHEP::m << "\t " << integral.synchronousTAtEnd << G4endl;
#endif
  
  element = elementIn;
  prevElement = prevElementIn;
  nextElement = nextElementIn;
  integralUpToThisComponent = &integral; // <- this is used for brho and beta throughout this factory for the current call of it
  synchronousTAtMiddleOfThisComponent = integralUpToThisComponent->ProvideSynchronousTAtCentreOfNextElement(elementIn);
  G4double angleIn  = 0.0;
  G4double angleOut = 0.0;
  G4bool registered = false;
  // Used for multiple instances of the same element but different poleface rotations.
  // Ie only a drift is modified to match the pole face rotation of a magnet.
  G4bool differentFromDefinition = false;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << element->name << "\"" << G4endl;  
#endif
  G4String searchName = element->name;
  if (integral.rigidityCount > 0)
    {searchName += "_" + std::to_string(integral.rigidityCount);}
  if (BDSAcceleratorComponentRegistry::Instance()->IsRegistered(searchName, integral.designParticle.BRho()))
    {registered = true;}

  if (element->type == ElementType::_DRIFT)
    {
      if (prevElement)
        {angleIn  = OutgoingFaceAngle(prevElement);}
      if (nextElement)
        {angleOut = IncomingFaceAngle(nextElement);}
      
      //if drift has been modified at all
      if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
	{differentFromDefinition = true;}
    }
  else if (element->type == ElementType::_RBEND)
    {// bend builder will construct it to match - but here we just know it's different
      // match a previous rbend with half the angle
      if (prevElement)
	{
	  if (prevElement->type == ElementType::_RBEND) // also if includeFringeFields
	    {differentFromDefinition = true;}
	}
      // match the upcoming rbend with half the angle
      if (nextElement)
	{
	  if (nextElement->type == ElementType::_RBEND) // also if includeFringeFields
	    {differentFromDefinition = true;}
	}
    }
  else if (element->type == ElementType::_SBEND)
    {
      if (prevElement)
	{
	  if (prevElement->type == ElementType::_SBEND && includeFringeFields)
	    {differentFromDefinition = true;}
	}
      if (nextElement)
	{
	  if (nextElement->type == ElementType::_SBEND && includeFringeFields)
	    {differentFromDefinition = true;}
	}
    }
  else if (element->type == ElementType::_THINMULT || (element->type == ElementType::_MULT && !HasSufficientMinimumLength(element, false)) || (element->type == ElementType::_THINRMATRIX))
    {
    // thinmultipole only uses one angle - so `angleIn`
       if (prevElement && nextElement)
	{// both exist
	  ElementType prevType = prevElement->type;
	  ElementType nextType = nextElement->type;
	  if (prevType == ElementType::_DRIFT && nextType != ElementType::_DRIFT)
	    {angleIn = -IncomingFaceAngle(nextElement);} // previous is drift which will match next
	  else if (prevType != ElementType::_DRIFT && nextType == ElementType::_DRIFT)
	    {angleIn = OutgoingFaceAngle(prevElement);}  // next is drift which will match prev
	}
       else if (prevElement)
	 {angleIn = OutgoingFaceAngle(prevElement);} // only previous element - match it
       else
	 {angleIn = IncomingFaceAngle(nextElement);} // only next element - match it
       
       // flag as unique only if the angleIn is changed and the geometry is built at an angle
       if (BDS::IsFinite(angleIn))
	 {differentFromDefinition = true;}
    }
  else if (element->type == ElementType::_SOLENOID)
    {// we build incoming / outgoing fringe fields for solenoids
      if (prevElement)
	{
	  if (prevElement->type == ElementType::_SOLENOID)
	    {differentFromDefinition = true;}
	}
      if (nextElement)
	{
	  if (nextElement->type == ElementType::_SOLENOID)
	    {differentFromDefinition = true;}
	}
    }
  
  // Check if the component already exists and return that.
  // Don't use the registry for output elements since reliant on unique name.
  if (registered && !differentFromDefinition)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using already manufactured component" << G4endl;
#endif
      integral.Integrate(*elementIn); // update beamline integral for this component
      return BDSAcceleratorComponentRegistry::Instance()->GetComponent(searchName, integral.designParticle.BRho());
    }

  // Update name for this component
  elementName = searchName;

  if (differentFromDefinition)
    {
      G4int val;
      if (modifiedElements.find(elementName) == modifiedElements.end())
	{
	  modifiedElements[elementName] = 0;
	  val = 0;
	}
      else
	{
	  val = modifiedElements[elementName] + 1;
	  modifiedElements[elementName] = val;
	}
      elementName += "_mod_" + std::to_string(val);
    }

  BDSAcceleratorComponent* component = nullptr;
  try
  {
  switch(element->type)
    {
    case ElementType::_DRIFT:
      {component = CreateDrift(angleIn, angleOut); break;}
    case ElementType::_RF:
      {
	component = CreateRF(RFFieldDirection::z);
	differentFromDefinition = true; // unique phase for every placement in beam line
	break;
      }
    case ElementType::_RFX:
      {
        component = CreateRF(RFFieldDirection::x);
        differentFromDefinition = true; // unique phase for every placement in beam line
        break;
      }
    case ElementType::_RFY:
      {
        component = CreateRF(RFFieldDirection::y);
        differentFromDefinition = true; // unique phase for every placement in beam line
        break;
      }
    case ElementType::_SBEND:
      {component = CreateSBend(); break;}
    case ElementType::_RBEND:
      {component = CreateRBend(); break;}
    case ElementType::_HKICKER:
      {component = CreateKicker(KickerType::horizontal); break;}
    case ElementType::_VKICKER:
      {component = CreateKicker(KickerType::vertical); break;}
    case ElementType::_KICKER:
    case ElementType::_TKICKER:
      {component = CreateKicker(KickerType::general); break;}
    case ElementType::_QUAD:
      {component = CreateQuad(); break;}
    case ElementType::_SEXTUPOLE:
      {component = CreateSextupole(); break;}
    case ElementType::_OCTUPOLE:
      {component = CreateOctupole(); break;}
    case ElementType::_DECAPOLE:
      {component = CreateDecapole(); break;}
    case ElementType::_MULT:
      {
        if(!BDS::IsFinite(element->l))
	  {
	    component = CreateThinMultipole(angleIn);
	    break;
	  }
	component = CreateMultipole();
        break;
      }
    case ElementType::_THINMULT:
      {component = CreateThinMultipole(angleIn); break;}
    case ElementType::_ELEMENT:
      {component = CreateElement(); break;}
    case ElementType::_SOLENOID:
      {component = CreateSolenoid(); break;}
    case ElementType::_ECOL:
      {component = CreateEllipticalCollimator(); break;}
    case ElementType::_RCOL:
      {component = CreateRectangularCollimator(); break;}
    case ElementType::_BMCOL:
      {component = CreateBeamMaskCollimator(); break;}
    case ElementType::_GASCAP:
      {component = CreateGasCapillary(); break;}
    case ElementType::_TARGET:
      {component = CreateTarget(); break;}
    case ElementType::_JCOL:
      {component = CreateJawCollimator(); break;}
    case ElementType::_MUONSPOILER:
      {component = CreateMuonSpoiler(); break;}
    case ElementType::_SHIELD:
      {component = CreateShield(); break;}
    case ElementType::_DEGRADER:
      {component = CreateDegrader(); break;}
    case ElementType::_WIRESCANNER:
      {component = CreateWireScanner(); break;}
    case ElementType::_GAP:
      {component = CreateGap(); break;}
    case ElementType::_CRYSTALCOL:
      {component = CreateCrystalCollimator(); break;}
    case ElementType::_LASER:
      {component = CreateLaser(); break;}
    case ElementType::_SCREEN:
      {component = CreateScreen(); break;}
    case ElementType::_TRANSFORM3D:
      {component = CreateTransform3D(); break;}
    case ElementType::_THINRMATRIX:
      {component = CreateThinRMatrix(angleIn, elementName); break;}
    case ElementType::_PARALLELTRANSPORTER:
      {component = CreateParallelTransporter(); break;}
    case ElementType::_RMATRIX:
      {component = CreateRMatrix(); break;}
    case ElementType::_UNDULATOR:
      {component = CreateUndulator(); break;}
    case ElementType::_USERCOMPONENT:
      {
	if (!userComponentFactory)
	  {throw BDSException(__METHOD_NAME__, "no user component factory registered");}
	G4String typeName = G4String(element->userTypeName);
	if (!userComponentFactory->CanConstructComponentByName(typeName))
	  {throw BDSException(__METHOD_NAME__, "no such component \"" + element->userTypeName + "\" registered.");}
	else
	  {
	    component = userComponentFactory->ConstructComponent(typeName,
								 element,
								 prevElement,
								 nextElement,
								 integral);
	  }
	break;
      }
    case ElementType::_DUMP:
      {component = CreateDump(); break;}
    case ElementType::_CT:
#ifdef USE_DICOM
      {component = CreateCT(); break;}
#else
      {throw BDSException(__METHOD_NAME__, "ct element can't be used - not compiled with dicom module!");}
#endif
    case ElementType::_AWAKESCREEN:
#ifdef USE_AWAKE
      {component = CreateAwakeScreen(); break;}
#else
      throw BDSException(__METHOD_NAME__, "Awake Screen can't be used - not compiled with AWAKE module!");
#endif
    case ElementType::_AWAKESPECTROMETER:
#ifdef USE_AWAKE
      {component = CreateAwakeSpectrometer(); break;}
#else
      {throw BDSException(__METHOD_NAME__, "Awake Spectrometer can't be used - not compiled with AWAKE module!");}
#endif
      // common types, but nothing to do here
    case ElementType::_MARKER:
    case ElementType::_LINE:
    case ElementType::_REV_LINE:
      {component = nullptr; break;}
    default:
      {
	G4cerr << __METHOD_NAME__ << "unknown type " << element->type << G4endl;
	throw BDSException(__METHOD_NAME__, "");
	break;
      }
    }
  }
  catch (BDSException& e)
    {
      e.AppendToMessage("\nError in creating component \"" + elementName + "\"");
      throw e;
    }
  
  // note this test will only be reached (and therefore the component registered)
  // if both the component didn't exist and it has been constructed
  if (component)
    {
      component->SetBiasVacuumList(element->biasVacuumList);
      component->SetBiasMaterialList(element->biasMaterialList);
      component->SetBiasMaterialLVList(element->biasMaterialLVList);
      component->SetRegion(element->region);
      // the minimum kinetic energy is only implemented in certain components
      component->SetMinimumKineticEnergy(element->minimumKineticEnergy*CLHEP::GeV);

      // infinite absorbers for collimators - must be done after SetMinimumKineticEnergy and
      // specific to these elements. must be done before initialise too.
      switch (element->type)
	{
	case ElementType::_ECOL:
	case ElementType::_RCOL:
	case ElementType::_JCOL:
    case ElementType::_BMCOL:
	  {
	    if (BDSGlobalConstants::Instance()->CollimatorsAreInfiniteAbsorbers())
	      {component->SetMinimumKineticEnergy(std::numeric_limits<double>::max());}
	    break;
	  }
	default:
	  {break;}	  
	}
      
      SetFieldDefinitions(element, component);
      component->Initialise();
      // register component and memory
      BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(component, integral.designParticle.BRho(), differentFromDefinition);
      
      integral.Integrate(*elementIn); // update beamline integral for this component
    }
  
  return component;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTeleporter(const G4double       teleporterLength,
							       const G4double       teleporterHorizontalWidth,
							       const G4Transform3D& transformIn)
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["length"] = teleporterLength; // convey length scale to integrator
  BDSFieldInfo* vacuumFieldInfo = new BDSFieldInfo(BDSFieldType::teleporter,
                                                   BRho(),
						   BDSIntegratorType::teleporter,
						   st,
						   true,
						   transformIn);
  
  G4cout << "---->creating Teleporter, " << "l = " << teleporterLength/CLHEP::m << "m" << G4endl;

  return( new BDSTeleporter(teleporterLength, teleporterHorizontalWidth, vacuumFieldInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDrift(G4double angleIn, G4double angleOut)
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  // Create normal vectors
  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn, angleOut);

  // current element tilt
  G4double currentTilt  = element->tilt * CLHEP::rad;
  G4double prevTilt = 0;
  G4double nextTilt = 0;
  if (prevElement)
    {prevTilt = prevElement->tilt * CLHEP::rad;}
  if (nextElement)
    {nextTilt = nextElement->tilt * CLHEP::rad;}
  G4ThreeVector inputFaceNormal  = faces.first.rotateZ(prevTilt - currentTilt);
  G4ThreeVector outputFaceNormal = faces.second.rotateZ(nextTilt - currentTilt);

  const G4double length = element->l*CLHEP::m;

  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, inputFaceNormal, outputFaceNormal);

  const BDSExtent extent = beamPipeInfo->Extent();
  G4bool facesWillIntersect = BDS::WillIntersect(inputFaceNormal, outputFaceNormal,
						 length, extent, extent);

  if (facesWillIntersect)
    {
      G4cerr << __METHOD_NAME__ << "Drift \"" << elementName
	     << "\" between \"";
      if (prevElement)
	{G4cerr << prevElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\" and \"";
      if (nextElement)
	{G4cerr << nextElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\" is too short given its width and the angle of its faces." << G4endl;
      throw BDSException(__METHOD_NAME__, "");
    }

  return (new BDSDrift(elementName,
		       length,
		       beamPipeInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRF(RFFieldDirection direction)
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  BDSFieldType fieldType;
  switch (direction)
    {// simple sinusoidal E field only
    case RFFieldDirection::x:
      {fieldType = BDSFieldType::rfconstantinx; break;}
    case RFFieldDirection::y:
      {fieldType = BDSFieldType::rfconstantiny; break;}
    case RFFieldDirection::z:
      {
        G4String cftName = element->cavityFieldType.empty() ? BDSGlobalConstants::Instance()->CavityFieldType() : element->cavityFieldType;
        BDSCavityFieldType cft = BDS::DetermineCavityFieldType(cftName);
        fieldType = BDS::FieldTypeFromCavityFieldType(cft);
  
        // optional more complex cavity field along z - done here only for the body and purposively
        // excluded from the general setting of fieldVacuum later on which would overwrite it
        if (!(element->fieldVacuum.empty()))
          {
            BDSFieldInfo* field = BDSFieldFactory::Instance()->GetDefinition(element->fieldVacuum);
            fieldType = field->FieldType();
          }
        break;
      }
    }
  
  BDSIntegratorType intType = integratorSet->Integrator(fieldType);
  
  // note cavity length is not the same as currentArcLength
  G4double cavityLength = element->l * CLHEP::m;

  // use cavity fringe option, includeFringeFields does not affect cavity fringes
  G4bool buildCavityFringes = BDSGlobalConstants::Instance()->IncludeFringeFieldsCavities();
  // don't build fringes for transverse rf cavities - none provided
  if (fieldType == BDSFieldType::rfconstantinx || fieldType == BDSFieldType::rfconstantiny)
    {buildCavityFringes = false;}

  G4bool buildIncomingFringe = buildCavityFringes;
  // only check if trying to build fringes to begin with as this check should only ever turn off fringe building
  if (prevElement && buildIncomingFringe) // could be nullptr
    {// only build fringe if previous element isn't another cavity
      buildIncomingFringe = prevElement->type != ElementType::_RF;
    }
  
  G4bool buildOutgoingFringe = buildCavityFringes;
  // only check if trying to build fringes to begin with as this check should only ever turn off fringe building
  if (nextElement && buildOutgoingFringe) // could be nullptr
    {// only build fringe if next element isn't another cavity
      buildOutgoingFringe = nextElement->type != ElementType::_RF;
    }

  if (buildIncomingFringe)
    {cavityLength -= thinElementLength;}
  if (buildOutgoingFringe)
    {cavityLength -= thinElementLength;}
  
  // supply currentArcLength (not element length) to strength as it's needed
  // for time offset from s=0 position
  BDSMagnetStrength* stIn  = nullptr; // deleted later if not needed
  BDSMagnetStrength* stOut = nullptr;
  BDSMagnetStrength* st = PrepareCavityStrength(element, fieldType, cavityLength, stIn, stOut);
  // st already has the synchronous time information in it
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
                                               BRho(),
					       intType,
					       st,
					       true,
					       fieldTrans);
  auto modulator = ModulatorDefinition(element);
  vacuumField->SetModulatorInfo(modulator); // works even if none

  // limit step length in field - crucial to this component
  // to get the motion correct this has to be less than one oscillation.
  // Don't set if frequency is zero as the field will have no oscillation, so we can integrate
  // safely over longer steps without the field changing.
  if (BDS::IsFinite(element->frequency))
    {
      auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
      G4double stepFraction  = 0.025;
      G4double period = 1. / (element->frequency*CLHEP::hertz);
      // choose the smallest length scale based on the length of the component of the distance
      // travelled in one period - so improved for high frequency fields
      G4double limit = std::min((*st)["length"], integralUpToThisComponent->designParticle.Velocity()*period) * stepFraction;
      auto ul = BDS::CreateUserLimits(defaultUL, limit, 1.0);
      if (ul != defaultUL)
        {vacuumField->SetUserLimits(ul);}
    }
  
  BDSCavityInfo* cavityInfo = PrepareCavityModelInfo(element, (*st)["frequency"]);

  // update 0 point of field with geometry
  // this relies on BDSFieldInfo not copying the strength so when we update the contents of *st
  // it is applied inside vaccumFieldInfo
  (*st)["equatorradius"] = cavityInfo->equatorRadius;
  G4Material* vacuumMaterial = PrepareVacuumMaterial(element);

  // aperture radius. Default is beam pipe radius / aper1 if a cavity model isn't specified.
  G4double cavityApertureRadius = cavityInfo->irisRadius;

  if (!BDS::IsFinite((*st)["efield"]) || !buildCavityFringes)
    {// ie no rf field - don't bother with fringe effects
      delete stIn;
      delete stOut;
      return new BDSCavityElement(elementName,
                                  cavityLength,
                                  vacuumMaterial,
                                  vacuumField,
                                  cavityInfo);
    }
  
  BDSLine* cavityLine = new BDSLine(elementName);

  if (buildIncomingFringe)
    {
      //BDSMagnetStrength* stIn = PrepareCavityFringeStrength(element, cavityLength, currentArcLength, true);
      // update with info for fringe matrix elements
      (*stIn)["rmat11"] = 1;
      (*stIn)["rmat21"] = 0;
      (*stIn)["rmat22"] = 1;
      (*stIn)["rmat33"] = 1;
      (*stIn)["rmat43"] = 0;
      (*stIn)["rmat44"] = 1;
      (*stIn)["length"] = BDSGlobalConstants::Instance()->ThinElementLength();
      (*stIn)["isentrance"] = true;
      auto cavityFringeIn = CreateCavityFringe(0, stIn, elementName + "_fringe_in", cavityApertureRadius, modulator);
      cavityLine->AddComponent(cavityFringeIn);
    }
  else
    {delete stIn;}

  auto cavity = new BDSCavityElement(elementName,
	                             cavityLength,
	                             vacuumMaterial,
	                             vacuumField,
	                             cavityInfo);

  cavityLine->AddComponent(cavity);

  if (buildOutgoingFringe)
    {
      //BDSMagnetStrength* stOut = PrepareCavityFringeStrength(element, cavityLength, currentArcLength, false);
      // update with info for fringe matrix elements
      (*stOut)["rmat11"] = 1;
      (*stOut)["rmat21"] = 0;
      (*stOut)["rmat22"] = 1;
      (*stOut)["rmat33"] = 1;
      (*stOut)["rmat43"] = 0;
      (*stOut)["rmat44"] = 1;
      (*stOut)["length"] = BDSGlobalConstants::Instance()->ThinElementLength();
      (*stOut)["isentrance"] = false;
      auto cavityFringeIn = CreateCavityFringe(0, stOut, elementName + "_fringe_out", cavityApertureRadius, modulator);
      cavityLine->AddComponent(cavityFringeIn);
    }
  else
    {delete stOut;}

  return cavityLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);

  // don't check here on whether the possibly next / previous sbend will clash with
  // pole face angles - let that be checked after element construction in the beamline
  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double angle = 0;
  G4double field = 0;
  CalculateAngleAndFieldSBend(element, angle, field);
  (*st)["angle"]  = angle;
  (*st)["field"]  = field*element->scaling;
  (*st)["by"]     = 1;// bx,by,bz is unit field direction, so (0,1,0) here
  (*st)["length"] = element->l * CLHEP::m; // arc length
  (*st)["scaling"]= element->scaling;
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;
  auto modulator = ModulatorDefinition(element, true);

  // quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->scaling*element->k1;}

#ifdef BDSDEBUG
  G4cout << "Angle (rad) " << (*st)["angle"] / CLHEP::rad   << G4endl;
  G4cout << "Field (T)   " << (*st)["field"] / CLHEP::tesla << G4endl;
#endif
  // geometric face angles (can be different from specification depending on integrator set used)
  G4double incomingFaceAngle = IncomingFaceAngle(element);
  G4double outgoingFaceAngle = OutgoingFaceAngle(element);

  auto sBendLine = BDS::BuildSBendLine(elementName, element, st, BRho(), integratorSet,
                                       incomingFaceAngle, outgoingFaceAngle,
				       includeFringeFields, prevElement, nextElement, modulator);
  
  return sBendLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRBend()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);

  // don't check here on whether the possibly next / previous sbend will clash with
  // pole face angles - let that be checked after element construction in the beamline

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double arcLength = 0, chordLength = 0, field = 0, angle = 0;
  CalculateAngleAndFieldRBend(element, BRho(), arcLength, chordLength, field, angle);
  
  (*st)["angle"]  = angle;
  (*st)["field"]  = field * element->scaling;
  (*st)["by"]     = 1;// bx,by,bz is unit field direction, so (0,1,0) here
  (*st)["length"] = arcLength;
  (*st)["scaling"]= element->scaling;
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;

  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->scaling * element->k1;}

  // geometric face angles (can be different from specification depending on integrator set used)
  G4double incomingFaceAngle = IncomingFaceAngle(element);
  G4double outgoingFaceAngle = OutgoingFaceAngle(element);

  // Check the faces won't overlap due to too strong an angle with too short a magnet
  auto bp = PrepareBeamPipeInfo(element);
  BDSMagnetOuterInfo* oiCheck = PrepareMagnetOuterInfo("checking", element,
                                                       -incomingFaceAngle, -outgoingFaceAngle,
                                                       bp, element->yokeOnInside);
  CheckBendLengthAngleWidthCombo(arcLength, (*st)["angle"], oiCheck->MinimumIntersectionRadiusRequired(), elementName);
  delete oiCheck;
  delete bp;

  // the above in / out face angles are not w.r.t. the local coords - subtract angle/2 to convert
  // this may seem like undoing the += in the functions, but they're used for the beam pipes
  // and matching.
  incomingFaceAngle -= 0.5*angle;
  outgoingFaceAngle -= 0.5*angle;

  BDSLine* rbendline = BDS::BuildRBendLine(elementName, element, prevElement, nextElement,
                                           BRho(), st, integratorSet,
					   incomingFaceAngle, outgoingFaceAngle,
					   includeFringeFields,
					   ModulatorDefinition(element, true));
  return rbendline;
}

void BDSComponentFactory::GetKickValue(G4double& hkick,
				       G4double& vkick,
				       const KickerType type) const
{
  G4bool kickFinite = BDS::IsFinite(element->kick);
  switch (type)
    {
    case KickerType::horizontal:
      {
	hkick = kickFinite ? element->kick : element->hkick;
	// backwards compatibility - if both are zero but angle if finite
	// for this element - use that.
	if (!BDS::IsFinite(hkick) && BDS::IsFinite(element->angle))
          {hkick = element->angle;} //+ve to match hkick definition
	vkick = 0;
	break;
      }
    case KickerType::vertical:
      {
	vkick = kickFinite ? element->kick : element->vkick;
	// backwards compatibility - if both are zero but angle if finite
	// for this element - use that.
	if (!BDS::IsFinite(vkick) && BDS::IsFinite(element->angle))
          {vkick = element->angle;} //+ve to match vkick definition
	hkick = 0;
	break;
      }
    case KickerType::general:
      {
	hkick = element->hkick;
	vkick = element->vkick;
	// element->kick will be ignored
	if (BDS::IsFinite(element->kick))
	  {BDS::Warning(__METHOD_NAME__, "'kick' parameter defined in element \"" + elementName + "\" but will be ignored as general kicker.");}
      }
    default:
      {break;}
    }
}

BDSAcceleratorComponent* BDSComponentFactory::CreateKicker(KickerType type)
{
  const G4String baseName       = elementName;
  BDSMagnetStrength* st         = new BDSMagnetStrength();
  SetBeta0(st);
  BDSFieldType       fieldType  = BDSFieldType::dipole;
  BDSIntegratorType  intType    = BDSIntegratorType::g4classicalrk4; // default
  G4double           chordLength;
  G4double           scaling    = element->scaling;
  G4double           hkick      = 0;
  G4double           vkick      = 0;
  GetKickValue(hkick, vkick, type);
  (*st)["scaling"] = scaling; // needed in kicker fringes otherwise default is zero
  (*st)["hkick"] = scaling * hkick;
  (*st)["vkick"] = scaling * vkick;
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;

  // create fringe magnet strengths. Copies supplied strength object so it should contain all
  // the kicker strength information as well as the added fringe information
  BDSMagnetStrength* fringeStIn = BDS::GetFringeMagnetStrength(element,
                                                               st,
                                                               0,
                                                               element->e1,
                                                               element->e2,
                                                               element->fintx,
                                                               true);
  BDSMagnetStrength* fringeStOut = new BDSMagnetStrength(*fringeStIn);
  (*fringeStOut)["isentrance"] = false;

  // check if the fringe effect is finite
  G4bool finiteEntrFringe = false;
  G4bool finiteExitFringe = false;
  if (BDS::IsFinite(BDS::FringeFieldCorrection(fringeStIn, true)) ||
      BDS::IsFinite(BDS::SecondFringeFieldCorrection(fringeStIn, true)))
    {finiteEntrFringe = true;}
  if (BDS::IsFinite(BDS::FringeFieldCorrection(fringeStOut, true)) ||
      BDS::IsFinite(BDS::SecondFringeFieldCorrection(fringeStOut, true)))
    {finiteExitFringe = true;}

  // only build the fringe elements if the poleface rotation or fringe field correction terms are finite
  G4bool buildEntranceFringe = false;
  G4bool buildExitFringe     = false;
  if (BDS::IsFinite(element->e1) || finiteEntrFringe)
    {buildEntranceFringe = true;}
  if (BDS::IsFinite(element->e2) || finiteExitFringe)
    {buildExitFringe = true;}
  if (!includeFringeFields)
    {
      buildEntranceFringe = false;
      buildExitFringe = false;
    }

  G4bool isThin = false;  // required for setting field as 'thin' later (which controls geant tracking error parameters)
  if (!HasSufficientMinimumLength(element, false)) // false for don't print warning
    {// thin kicker
      fieldType   = BDSFieldType::bfieldzero;
      intType     = BDSIntegratorType::kickerthin;
      chordLength = thinElementLength;
      isThin      = true;

      // Fringe and poleface effects for a thin kicker require an effective bending radius, rho.
      // Lack of length and angle knowledge means the field is the only way rho can be calculated.
      // A zero field would lead to div by zero errors in the integrator constructor, therefore
      // do not replace the kicker magnet strength with the fringe magnet strength which should prevent
      // any fringe/poleface effects from ever being applied.
      if (!BDS::IsFinite(element->B))
        {
          // only print warning if a poleface or fringe field effect was specified
          if (buildEntranceFringe || buildExitFringe)
            {
              G4cout << __METHOD_NAME__ << "WARNING - finite B field required for kicker pole face and fringe fields,"
                        " effects are unavailable for element ""\"" << elementName << "\"." << G4endl;
            }
          buildEntranceFringe = false;
          buildExitFringe = false;
        }
      else if (type == KickerType::general)
        {
          // A thin kicker or tkicker element has possible hkick and vkick combination, meaning the
          // field direction cannot be assumed. Therefore, we are unsure of poleface angle and fringe
          // effects so don't replace the kicker magnet strength with the fringe magnet strength
        
          // only print warning if a poleface or fringe field effect was specified
          if (buildEntranceFringe || buildExitFringe)
            {
              G4cerr << __METHOD_NAME__ << " Poleface and fringe field effects are unavailable "
                     << "for the thin (t)kicker element ""\"" << elementName << "\"." << G4endl;
            }
          buildEntranceFringe = false;
          buildExitFringe = false;
        }
      
      else
        {// Good to apply fringe effects.
          // overwrite magnet strength with copy of fringe strength. Should be safe as it has the
          // kicker information from copying previously.
	        delete st;
          st = new BDSMagnetStrength(*fringeStIn);
          // supply the scaled field for a thin kicker field as it is required to calculate
          // the effective bending radius needed for fringe field and poleface effects
          (*st)["field"] = element->B * CLHEP::tesla  * scaling;
          (*fringeStIn) ["field"] = (*st)["field"];
          (*fringeStOut) ["field"] = (*st)["field"];

          // set field for vertical kickers - element needs rotating as poleface rotation is assumed
          // to be about the vertical axis unless explicitly tilted.
          if (type == KickerType::vertical)
            {(*st)["bx"] = 1;}
          else
            {(*st)["by"] = 1;}
        }
    }
  else
    {// thick kicker
      chordLength = element->l*CLHEP::m;
      // sin(angle) = dP -> angle = sin^-1(dP)
      G4double angleX = std::asin(hkick * scaling);
      G4double angleY = std::asin(vkick * scaling);

      if (std::isnan(angleX))
        {throw BDSException(__METHOD_NAME__, "hkick too strong for element \"" + element->name + "\" ");}
      if (std::isnan(angleY))
        {throw BDSException(__METHOD_NAME__, "vkick too strong for element \"" + element->name + "\" ");}

      // Setup result variables - 'x' and 'y' refer to the components along the direction
      // the particle will change. These will therefore not be Bx and By.
      G4double fieldX = 0;
      G4double fieldY = 0;

      // if B is specified and hkick and vkick (including backwards compatible check on
      // 'angle') are not, then use the field for the appropriate component
      // can only be 1d in this case -> doesn't work for tkicker
      if (BDS::IsFinite(element->B) && (!BDS::IsFinite(hkick) && !BDS::IsFinite(vkick)))
	{
	  switch (type)
	    {// 'X' and 'Y' are the angle of bending here, not the B field direction.
	    case KickerType::horizontal:
	    case KickerType::general:
	      {fieldX = element->B * CLHEP::tesla * scaling; break;}
	    case KickerType::vertical:
	      {fieldY = element->B * CLHEP::tesla * scaling; break;}
	    default:
	      {break;} // do nothing - no field - just for compiler warnings
	    }
	}
      else
	{
	  if (BDS::IsFinite(angleX))
	    {// with comments
	      // calculate the chord length of the arc through the field from the straight
	      // ahead length for this element which here is 'chordLength'.
	      G4double fieldChordLengthX = chordLength / std::cos(0.5*angleX);
	      
	      // now calculate the bending radius
	      G4double bendingRadiusX = fieldChordLengthX * 0.5 / sin(std::abs(angleX) * 0.5);
	      
	      // no calculate the arc length of the trajectory based on each bending radius
	      G4double arcLengthX = std::abs(bendingRadiusX * angleX);
	      
	      // -ve here in horizontal only for convention matching
	      fieldX = FieldFromAngle(-angleX, arcLengthX);
	    } // else fieldX default is 0
	  
	  if (BDS::IsFinite(angleY))
	    {// same as x, no need for comments
	      G4double fieldChordLengthY = chordLength / std::cos(0.5*angleY);
	      G4double bendingRadiusY    = fieldChordLengthY * 0.5 / sin(std::abs(angleY) * 0.5);
	      G4double arcLengthY        = std::abs(bendingRadiusY * angleY);
	      fieldY = FieldFromAngle(angleY,  arcLengthY);
	    }
	}
      
      // note field for kick in x is unit Y, hence B = (y,x,0) here
      // field calculated from scaled angle so no need to scale field here
      G4ThreeVector     field = G4ThreeVector(fieldY, fieldX, 0);
      G4double       fieldMag = field.mag();
      G4ThreeVector unitField = field.unit();
      
      (*st)["field"] = fieldMag;
      (*st)["bx"]    = unitField.x();
      (*st)["by"]    = unitField.y();

      // preserve sign of field strength for fringe integrators
      // needed to calculate correct value of rho
      (*fringeStIn)["field"] = (*st)["field"];
      (*fringeStOut)["field"] = (*st)["field"];
      if (fieldX < 0 || fieldY < 0)
        {
          (*fringeStIn)["field"] *= -1;
          (*fringeStOut)["field"] *= -1;
        }
    }

  (*fringeStIn) ["bx"] = (*st)["bx"];
  (*fringeStIn) ["by"] = (*st)["by"];
  (*fringeStOut)["bx"] = (*st)["bx"];
  (*fringeStOut)["by"] = (*st)["by"];

  BDSMagnetType t;
  G4double defaultVHRatio = 1.5;
  switch (type)
    {
    case KickerType::horizontal:
    case KickerType::general:
      {t = BDSMagnetType::hkicker; break;}
    case KickerType::vertical:
      {
	t = BDSMagnetType::vkicker;
	defaultVHRatio = 1./defaultVHRatio; // inverted for vertical magnet
	break;
      }
    default:
      {t = BDSMagnetType::hkicker; break;}
    }

  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
                                               BRho(),
					       intType,
					       st,
					       true,
					       fieldTrans);
  vacuumField->SetModulatorInfo(ModulatorDefinition(element, true)); // works even if none
  if (isThin)
    {vacuumField->SetFieldAsThin();}

  G4bool yokeOnLeft = YokeOnLeft(element, st);
  auto bpInf = PrepareBeamPipeInfo(element);
  
  // Decide on a default horizontalWidth for the kicker - try 0.3x ie smaller kicker
  // than typical magnet, but if that would not fit around the beam pipe - go back to
  // the default horizontalWidth. Code further along will warn if it still doesn't fit.
  const G4double globalDefaultHW = BDSGlobalConstants::Instance()->HorizontalWidth();
  G4double defaultHorizontalWidth = 0.3 * globalDefaultHW;
  BDSExtent bpExt = bpInf->Extent();
  G4double bpDX = bpExt.DX();
  G4double bpDY = bpExt.DY();
  if (bpDX > defaultHorizontalWidth && bpDX < globalDefaultHW)
    {defaultHorizontalWidth = globalDefaultHW;}
  else if (bpDY > defaultHorizontalWidth && bpDY > globalDefaultHW)
    {defaultHorizontalWidth = globalDefaultHW;}
  
  auto magOutInf = PrepareMagnetOuterInfo(elementName, element, 0, 0, bpInf, yokeOnLeft,
					  defaultHorizontalWidth, defaultVHRatio, 0.9);
  
  BDSFieldInfo* outerField = nullptr;
  G4bool externalOuterField = !(element->fieldOuter.empty());
  if (yokeFields && !externalOuterField)
    {
      outerField = PrepareMagnetOuterFieldInfo(st,
					       fieldType,
					       bpInf,
					       magOutInf,
					       fieldTrans,
					       integratorSet,
                                               BRho(),
					       ScalingFieldOuter(element),
					       ModulatorDefinition(element, true));
    }
  
  if (!HasSufficientMinimumLength(element, false))
    {
      delete fringeStIn;
      delete fringeStOut;
      // fringe effect applied in integrator so nothing more to do.
      // no outer field as thin component here
      return new BDSMagnet(t,
			   baseName,
			   chordLength,
			   bpInf,
			   magOutInf,
			   vacuumField,
			   0, nullptr,  // default values for optional args (angle, outerFieldInfo)
			   true);       // isThin
    }
  else
    {
      BDSLine* kickerLine = new BDSLine(baseName);
      // subtract fringe length from kicker to preserve element length
      G4double kickerChordLength = chordLength;
      if (buildEntranceFringe)
        {kickerChordLength -= thinElementLength;}
      if (buildExitFringe)
        {kickerChordLength -= thinElementLength;}

      if (buildEntranceFringe)
        {
          G4String entrFringeName = baseName + "_e1_fringe";
          BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, 0, 0,
                                                          entrFringeName,
                                                          fringeStIn,
                                                          BRho(),
                                                          integratorSet,
							  fieldType);
          kickerLine->AddComponent(startfringe);
        }
      
      G4String kickerName = baseName;
      BDSMagnet* kicker = new BDSMagnet(t,
					kickerName,
					kickerChordLength,
					bpInf,
					magOutInf,
					vacuumField,
                                        0,
                                        outerField);
      kickerLine->AddComponent(kicker);
      
      if (buildEntranceFringe)
	{
          G4String exitFringeName = baseName + "_e2_fringe";
          BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, 0, 0,
                                                        exitFringeName,
                                                        fringeStOut, BRho(),
                                                        integratorSet, fieldType);
          kickerLine->AddComponent(endfringe);
        }
      return kickerLine;
    }
}

BDSAcceleratorComponent* BDSComponentFactory::CreateQuad()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k1"] = element->k1 * element->scaling;

  return CreateMagnet(element, st, BDSFieldType::quadrupole, BDSMagnetType::quadrupole);
}  
  
BDSAcceleratorComponent* BDSComponentFactory::CreateSextupole()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k2"] = element->k2 * element->scaling;

  return CreateMagnet(element, st, BDSFieldType::sextupole, BDSMagnetType::sextupole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateOctupole()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k3"] = element->k3 * element->scaling;

  return CreateMagnet(element, st, BDSFieldType::octupole, BDSMagnetType::octupole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDecapole()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k4"] = element->k4 * element->scaling;
  
  return CreateMagnet(element, st, BDSFieldType::decapole, BDSMagnetType::decapole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMultipole()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  BDSMagnetStrength* st = PrepareMagnetStrengthForMultipoles(element);
  
  return CreateMagnet(element, st, BDSFieldType::multipole, BDSMagnetType::multipole,
		      (*st)["angle"]); // multipole could bend beamline
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinMultipole(G4double angleIn)
{
  BDSMagnetStrength* st = PrepareMagnetStrengthForMultipoles(element);
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, angleIn, -angleIn);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(elementName, element,
							       -angleIn, angleIn, beamPipeInfo);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  BDSIntegratorType intType = integratorSet->multipoleThin;
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::multipole,
                                               BRho(),
					       intType,
					       st,
					       true,
					       fieldTrans);
  vacuumField->SetModulatorInfo(ModulatorDefinition(element, true));
  vacuumField->SetFieldAsThin();
  
  BDSMagnet* thinMultipole =  new BDSMagnet(BDSMagnetType::thinmultipole,
					    elementName,
					    thinElementLength,
					    beamPipeInfo,
					    magnetOuterInfo,
					    vacuumField,
					    0, nullptr,  // default values for optional args (angle, outerFieldInfo)
					    true);       // isThin

  thinMultipole->SetExtent(BDSExtent(beamPipeInfo->aper1,
				     beamPipeInfo->aper1,
				     thinElementLength*0.5));

  return thinMultipole;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateElement()
{
  if (!HasSufficientMinimumLength(element)) 
    {throw BDSException(__METHOD_NAME__, "insufficient length for element \"" + element->name + "\" - must specify a suitable length");}

  // we don't specify the field explicitly here - this is done generically
  // in the main CreateComponent method with SetFieldDefinitions.
  std::vector<G4String> vacuumBiasVolumeNames = BDS::SplitOnWhiteSpace(G4String(element->namedVacuumVolumes));

  G4double angle = -element->angle * CLHEP::rad; // this is to match the convention of MADX for bends
  G4double l = element->l * CLHEP::m;
  // calculate the arc length as that's what we need for BDSAcceleratorComponent,
  // which in turn calculates the chord length internally for placement.
  G4double arcLength = BDS::ArcLengthFromChordLength(l, angle);
  if (element->elementLengthIsArcLength)
    {arcLength = l;} // reset back to just l
  return (new BDSElement(elementName,
			 arcLength,
			 PrepareHorizontalWidth(element),
			 element->geometryFile,
			 angle,
			 &vacuumBiasVolumeNames,
			 element->autoColour,
			 element->markAsCollimator,
			 element->stripOuterVolume));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSolenoid()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  (*st)["bz"]    = 1;
  G4double chordLength = element->l * CLHEP::m;
  // arbitrary fraction of 0.8 for current length of full length - used for the yoke field that varies in z
  (*st)["length"] = chordLength * 0.8;
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;
  const G4double scaling = element->scaling;
  if (BDS::IsFinite(element->B))
    {
      (*st)["field"] = scaling * element->B * CLHEP::tesla;
      (*st)["ks"]    = (*st)["field"] / BRho();
    }
  else
    {
      (*st)["field"] = (scaling * element->ks / CLHEP::m) * BRho();
      (*st)["ks"]    = element->ks;
    }

  if (!BDS::IsFinite((*st)["field"]) || !includeFringeFields)
    {// ie no strength solenoid - don't bother with fringe effects
      return CreateMagnet(element, st, BDSFieldType::solenoid, BDSMagnetType::solenoid);
    }
  
  // lambda to help - sign convention - this is the 'entry' version
  auto strength = [](G4double phi){
		    BDSMagnetStrength* s = new BDSMagnetStrength();
		    (*s)["rmat11"] = 1;
		    (*s)["rmat22"] = 1;
		    (*s)["rmat33"] = 1;
		    (*s)["rmat44"] = 1;
		    (*s)["rmat41"] = -phi;
		    (*s)["rmat23"] = phi;
		    return s;
		  };

  G4bool buildIncomingFringe = true;
  if (prevElement) // could be nullptr
    {// only build fringe if previous element isn't another solenoid
      buildIncomingFringe = prevElement->type != ElementType::_SOLENOID;
    }
  G4bool buildOutgoingFringe = true;
  if (nextElement) // could be nullptr
    {// only build fringe if next element isn't another solenoid
      buildOutgoingFringe = nextElement->type != ElementType::_SOLENOID;
    }

  G4double solenoidBodyLength = element->l * CLHEP::m;

  if (buildIncomingFringe)
    {solenoidBodyLength -= thinElementLength;}
  if (buildOutgoingFringe)
    {solenoidBodyLength -= thinElementLength;}
  
  // scale factor to account for reduced body length due to fringe placement.
  G4double lengthScaling = solenoidBodyLength / (element->l * CLHEP::m);
  G4double s = 0.5*(*st)["ks"] * lengthScaling; // already includes scaling
  BDSLine* bLine = new BDSLine(elementName);
  
  auto modulator = ModulatorDefinition(element, true);
  
  if (buildIncomingFringe)
    {
      auto stIn        = strength(s);
      auto solenoidIn  = CreateThinRMatrix(0, stIn, elementName + "_fringe_in",
                                           BDSIntegratorType::rmatrixthin, BDSFieldType::rmatrix, 0, modulator);
      bLine->AddComponent(solenoidIn);
    }

  // Do not use CreateMagnet method as solenoid body length needs to be reduced to conserve total
  // element length. The solenoid strength is scaled accordingly.

  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);
  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::solenoid);
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::solenoid,
                                               BRho(),
                                               intType,
                                               st,
                                               true,
                                               fieldTrans);
  vacuumField->SetModulatorInfo(modulator);

  BDSMagnetOuterInfo* outerInfo = PrepareMagnetOuterInfo(elementName + "_centre", element, st, bpInfo);
  vacuumField->SetScalingRadius(outerInfo->innerRadius); // purely for completeness of information - not required
  BDSFieldInfo* outerField = nullptr;

  // only make a default multipolar field if the yokeFields flag is on and
  // there isn't an 'outerField' specified for the element
  G4bool externalOuterField = !(element->fieldOuter.empty());
  if (yokeFields && !externalOuterField)
    {
      outerField = PrepareMagnetOuterFieldInfo(st,
					       BDSFieldType::solenoid,
					       bpInfo,
					       outerInfo,
					       fieldTrans,
					       integratorSet,
                                               BRho(),
                                               ScalingFieldOuter(element),
                                               modulator);
      
      // determine a suitable radius for the current carrying coil of the solenoid
      // this defines the field geometry
      // there is no coil in our geometry so it is a bit fictional
      G4double beamPipeRadius = bpInfo->IndicativeRadius();
      G4double outerRadius = outerInfo->horizontalWidth * 0.5;
      G4double coilRadius = beamPipeRadius + 0.25*(outerRadius - beamPipeRadius);
      outerField->SetScalingRadius(coilRadius);
    }

  auto solenoid = new BDSMagnet(BDSMagnetType::solenoid,
                         elementName,
                         solenoidBodyLength,
                         bpInfo,
                         outerInfo,
                         vacuumField,
                         0,
                         outerField);

  bLine->AddComponent(solenoid);

  if (buildOutgoingFringe)
    {
      auto stOut = strength(-s);
      auto solenoidOut = CreateThinRMatrix(0, stOut, elementName + "_fringe_out",
                                           BDSIntegratorType::rmatrixthin, BDSFieldType::rmatrix, 0, modulator);
      bLine->AddComponent(solenoidOut);
    }
  
  return bLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateParallelTransporter()
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  return CreateMagnet(element, st, BDSFieldType::paralleltransporter, BDSMagnetType::paralleltransporter);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRectangularCollimator()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
  G4bool circularOuter = false;
  G4String apertureType = G4String(element->apertureType);
  if (apertureType == "circular")
    {circularOuter = true;}
  auto material = PrepareMaterial(element);
  return new BDSCollimatorRectangular(elementName,
				      element->l*CLHEP::m,
				      PrepareHorizontalWidth(element),
				      material,
				      PrepareVacuumMaterial(element),
				      element->xsize*CLHEP::m,
				      element->ysize*CLHEP::m,
				      element->xsizeOut*CLHEP::m,
				      element->ysizeOut*CLHEP::m,
				      PrepareColour(element, material),
				      circularOuter);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateBeamMaskCollimator()
{
  if (!HasSufficientMinimumLength(element))
  {return nullptr;}
  G4bool circularOuter = false;
  G4String cavityModel = G4String(element->cavityModel);
  if (cavityModel == "circular")
  {circularOuter = true;}
  return new BDSCollimatorBeamMask(elementName,
                                   element->l*CLHEP::m,
                                   PrepareBeamPipeInfo(element),
                                   PrepareHorizontalWidth(element),
                                   PrepareMaterial(element),
                                   PrepareVacuumMaterial(element),
                                   element->xsize*CLHEP::m,
                                   element->ysize*CLHEP::m,
                                   element->xsize2*CLHEP::m,
                                   element->ysize2*CLHEP::m,
                                   element->offsetX*CLHEP::m,
                                   element->offsetY*CLHEP::m,
                                   element->offsetX2*CLHEP::m,
                                   element->offsetY2*CLHEP::m,
                                   element->tilt2*CLHEP::rad,
                                   PrepareColour(element),
                                   circularOuter);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateGasCapillary()
{
  if (!HasSufficientMinimumLength(element))
  {return nullptr;}

  G4bool circularOuter = false;
  G4String cavityModel = G4String(element->cavityModel);
  if (cavityModel == "circular")
  {circularOuter = true;}

  std::vector<std::string> materials{ element->layerMaterials.begin(), element->layerMaterials.end() };

  return new BDSGasCapillary(elementName,
                             element->l*CLHEP::m,
                             PrepareBeamPipeInfo(element),
                             PrepareHorizontalWidth(element),
                             BDSMaterials::Instance()->GetMaterial(materials[0]),
                             BDSMaterials::Instance()->GetMaterial(materials[1]),
                             BDSMaterials::Instance()->GetMaterial(materials[2]),
                             element->xsize*CLHEP::m,
                             element->materialThickness*CLHEP::m,
                             circularOuter);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTarget()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
  G4bool circularOuter = false;
  G4String apertureType = G4String(element->apertureType);
  if (apertureType == "circular")
    {circularOuter = true;}
  auto material = PrepareMaterial(element);
  return new BDSTarget(elementName,
		       element->l*CLHEP::m,
		       PrepareHorizontalWidth(element),
		       material,
		       PrepareColour(element, material),
		       circularOuter);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateEllipticalCollimator()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  G4bool circularOuter = false;
  G4String apertureType = G4String(element->apertureType);
  if (apertureType == "circular")
    {circularOuter = true;}
  auto material = PrepareMaterial(element);
  return new BDSCollimatorElliptical(elementName,
				     element->l*CLHEP::m,
				     PrepareHorizontalWidth(element),
				     material,
				     PrepareVacuumMaterial(element),
				     element->xsize*CLHEP::m,
				     element->ysize*CLHEP::m,
				     element->xsizeOut*CLHEP::m,
				     element->ysizeOut*CLHEP::m,
				     PrepareColour(element, material),
				     circularOuter);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateJawCollimator()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
  auto material = PrepareMaterial(element);
  return new BDSCollimatorJaw(elementName,
			      element->l*CLHEP::m,
			      PrepareHorizontalWidth(element),
                              element->xsize*CLHEP::m,
                              element->ysize*CLHEP::m,
                              element->xsizeLeft*CLHEP::m,
                              element->xsizeRight*CLHEP::m,
                              element->jawTiltLeft*CLHEP::rad,
                              element->jawTiltRight*CLHEP::rad,
			      true,
			      true,
			      material,
			      PrepareVacuumMaterial(element),
			      PrepareColour(element, material));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMuonSpoiler()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  G4double elLength = element->l*CLHEP::m;
  BDSFieldInfo* outerField  = nullptr;
  if (BDS::IsFinite(element->B))
    {
      BDSMagnetStrength* st = new BDSMagnetStrength();
      (*st)["field"] = element->scaling * element->B * CLHEP::tesla;
      BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::muonspoiler);
      G4Transform3D fieldTrans = CreateFieldTransform(element);
      outerField = new BDSFieldInfo(BDSFieldType::muonspoiler,
                                    BRho(),
				    intType,
				    st,
				    true,
				    fieldTrans);
      auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
      G4double limit = elLength / 20.0;
      auto ul = BDS::CreateUserLimits(defaultUL, limit, 1.0);
      if (ul != defaultUL)
        {outerField->SetUserLimits(ul);}
    }
  auto bpInfo = PrepareBeamPipeInfo(element);
  
  return new BDSMagnet(BDSMagnetType::muonspoiler,
		       elementName,
                       elLength,
		       bpInfo,
		       PrepareMagnetOuterInfo(elementName, element, 0, 0, bpInfo), // 0 angled face in and out
		       nullptr,
		       0,
		       outerField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateShield()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);

  G4Colour* colour = PrepareColour(element);
  G4Material* material = PrepareMaterial(element, "concrete");

  BDSShield* shield = new BDSShield(elementName,
				    element->l*CLHEP::m,
				    PrepareHorizontalWidth(element),
				    element->xsize*CLHEP::m,
				    element->ysize*CLHEP::m,
				    material,
				    colour,
				    bpInfo);
  return shield;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDegrader()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  G4double degraderOffset;
  if ((element->materialThickness < 0) && (element->degraderOffset < 0))
    {throw BDSException(__METHOD_NAME__, "both \"materialThickness\" and \"degraderOffset\" are either undefined or < 0");}
  if (element->degraderOffset < 0)
    {throw BDSException(__METHOD_NAME__, "\"degraderOffset\" cannot be < 0");}
  if (element->materialThickness > element->l)
    {throw BDSException(__METHOD_NAME__, "\"materialThickness\" cannot be greater than the element length");}

  if ((element->materialThickness <= 0) && (element->degraderOffset >= 0))
    {degraderOffset = element->degraderOffset*CLHEP::m;}
  else
    {
      //Width of wedge base
      G4double wedgeBasewidth = (element->l*CLHEP::m /element->numberWedges) - lengthSafety;

      //Angle between hypotenuse and height (in the triangular wedge face)
      G4double theta = std::atan(wedgeBasewidth / (2.0*element->wedgeLength*CLHEP::m));

      //Overlap distance of wedges
      G4double thicknessPerWedge = (wedgeBasewidth - element->materialThickness*CLHEP::m/element->numberWedges);
      degraderOffset = (0.5*thicknessPerWedge) * (std::sin(CLHEP::halfpi - theta) / std::sin(theta));
    }

  // include base thickness in each wedge so it covers the whole beam aperture when set to the thickest
  // possible amount of material, otherwise a fraction of the beam wouldn't pass through the wedges.
  auto bpi = PrepareBeamPipeInfo(element);
  G4double baseWidth = bpi->aper1;
  delete bpi;
  auto material = PrepareMaterial(element);
  return (new BDSDegrader(elementName,
			  element->l*CLHEP::m,
			  PrepareHorizontalWidth(element),
			  element->numberWedges,
			  element->wedgeLength*CLHEP::m,
			  element->degraderHeight*CLHEP::m,
			  degraderOffset,
			  baseWidth,
			  material,
			  PrepareVacuumMaterial(element),
			  PrepareColour(element, material)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateWireScanner()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  if (BDS::IsFinite(element->angle))
    {throw BDSException(__METHOD_NAME__, "\"angle\" parameter set for wirescanner \"" + elementName + "\" but this should not be set. Please unset and use \"wireAngle\".");}

  G4ThreeVector wireOffset = G4ThreeVector(element->wireOffsetX * CLHEP::m,
					   element->wireOffsetY * CLHEP::m,
					   element->wireOffsetZ * CLHEP::m);
  
  return (new BDSWireScanner(elementName,
			     element->l*CLHEP::m,
			     PrepareBeamPipeInfo(element),
			     PrepareMaterial(element),
			     element->wireDiameter*CLHEP::m,
			     element->wireLength*CLHEP::m,
			     element->wireAngle*CLHEP::rad,
			     wireOffset));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateUndulator()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  const BDSFieldType undField = BDSFieldType::undulator;

  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);
  BDSIntegratorType intType = integratorSet->Integrator(undField);
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;
  SetBeta0(st);
  (*st)["length"] = element->undulatorPeriod * CLHEP::m;
  (*st)["field"] = element->scaling * element->B * CLHEP::tesla;

  BDSFieldInfo* vacuumFieldInfo = new BDSFieldInfo(undField,
                                                   BRho(),
                                                   intType,
                                                   st,
                                                   true,
                                                   fieldTrans);
  vacuumFieldInfo->SetModulatorInfo(ModulatorDefinition(element, true));
  //BDSFieldInfo* outerFieldInfo = PrepareMagnetOuterFieldInfo(st, undField, bpInfo, 0, fieldTrans);
  BDSFieldInfo* outerFieldInfo = nullptr;
  // limit step length in field - crucial to this component
  // to get the motion correct this has to be less than one oscillation
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  G4double limit = (*st)["length"] * 0.075;
  auto ul = BDS::CreateUserLimits(defaultUL, limit, 1.0);
  if (ul != defaultUL)
    {vacuumFieldInfo->SetUserLimits(ul);}

  return (new BDSUndulator(elementName,
			   element->l * CLHEP::m,
			   element->undulatorPeriod * CLHEP::m,
			   element->undulatorMagnetHeight * CLHEP::m,
			   PrepareHorizontalWidth(element),
			   element->undulatorGap * CLHEP::m,
			   bpInfo,
			   vacuumFieldInfo,
			   outerFieldInfo,
			   element->material));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDump()
{
  G4double chordLength = element->l*CLHEP::m;
  if (!HasSufficientMinimumLength(element, false))
    {
      G4cout << __METHOD_NAME__ << "using default length of 1 mm for dump" << G4endl;
      chordLength = 1*CLHEP::mm;
    }

  G4bool circular = false;
  G4String apertureType = G4String(element->apertureType);
  if (apertureType == "circular")
    {circular = true;}
  else if (apertureType != "rectangular" && !apertureType.empty())
    {throw BDSException(__METHOD_NAME__, "unknown shape for dump: \"" + apertureType + "\"");}

  BDSDump* result = new BDSDump(elementName,
				chordLength,
				PrepareHorizontalWidth(element),
				circular);
  return result;
}

#ifdef USE_DICOM
BDSAcceleratorComponent* BDSComponentFactory::CreateCT()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  BDSCT* result = new BDSCT(elementName,
			    element->dicomDataPath,
			    element->dicomDataFile);
  new BDSDicomIntersectVolume(); // TBC
   
  return result;
}
#endif

BDSAcceleratorComponent* BDSComponentFactory::CreateGap()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  return (new BDSGap(elementName,
                     element->l*CLHEP::m,
                     element->angle*CLHEP::rad));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateCrystalCollimator()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSCrystalInfo* left = nullptr;
  BDSCrystalInfo* right = nullptr;
  if (!element->crystalBoth.empty())
    {
      left  = PrepareCrystalInfo(G4String(element->crystalBoth));
      right = PrepareCrystalInfo(G4String(element->crystalBoth));
    }
  else if (element->crystalBoth.empty() && !element->crystalRight.empty() && !element->crystalLeft.empty())
    {
      left  = PrepareCrystalInfo(G4String(element->crystalLeft));
      right = PrepareCrystalInfo(G4String(element->crystalRight));
    }
  else if (element->crystalRight.empty())
    {left  = PrepareCrystalInfo(G4String(element->crystalLeft));}
  else
    {right = PrepareCrystalInfo(G4String(element->crystalRight));}

  if (left && !right &&  BDS::IsFinite(element->crystalAngleYAxisRight))
    {
      G4cout << G4endl << G4endl << __METHOD_NAME__
             << "Left crystal being used but right angle set - perhaps check input for element "
             << elementName << G4endl << G4endl;
    }
  if (!left && right &&  BDS::IsFinite(element->crystalAngleYAxisLeft))
    {
      G4cout << G4endl << G4endl << __METHOD_NAME__
	     << "Right crystal being used but left angle set - perhaps check input for element "
	     << elementName << G4endl << G4endl;
    }
  
  return (new BDSCollimatorCrystal(elementName,
				   element->l*CLHEP::m,
				   PrepareBeamPipeInfo(element),
				   left,
				   right,
				   element->xsize*CLHEP::m, // symmetric for now
				   element->xsize*CLHEP::m,
				   element->crystalAngleYAxisLeft*CLHEP::rad,
				   element->crystalAngleYAxisRight*CLHEP::rad));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateLaser()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
	
  G4double length = element->l*CLHEP::m;
  G4double lambda = element->waveLength*CLHEP::m;
	
  G4ThreeVector direction = G4ThreeVector(element->xdir,element->ydir,element->zdir);
  G4ThreeVector position  = G4ThreeVector(0,0,0);
	
  return (new BDSLaserWire(elementName, length, lambda, direction) );       
}

BDSAcceleratorComponent* BDSComponentFactory::CreateScreen()
{
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}
	
  G4TwoVector size;
  size.setX(element->screenXSize*CLHEP::m);
  size.setY(element->screenYSize*CLHEP::m);
  G4cout << __METHOD_NAME__ << " - size = " << size << G4endl;
  
  BDSScreen* theScreen = new BDSScreen( elementName,
					element->l*CLHEP::m,
					PrepareBeamPipeInfo(element),
					size,
					element->angle); 
  if (element->layerThicknesses.size() != element->layerMaterials.size())
    {
      throw BDSException(__METHOD_NAME__, "Element \"" + elementName + "\" must have the " +
			 "same number of materials as layers - check 'layerMaterials'");
    }

  if (element->layerThicknesses.empty())
    {throw BDSException(__METHOD_NAME__, "Element: \"" + elementName + "\" has 0 screen layers");}
  
  std::list<std::string>::const_iterator itm;
  std::list<double>::const_iterator itt;
  std::list<int>::const_iterator itIsSampler;
  for (itt = element->layerThicknesses.begin(),
	 itm = element->layerMaterials.begin(),
	 itIsSampler = element->layerIsSampler.begin();
       itt != element->layerThicknesses.end();
       ++itt, ++itm)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - screen layer: thickness: " 
	     << *(itt) << ", material "  << (*itm)
	     <<	", isSampler: "  << (*itIsSampler) << G4endl;
#endif
      if (!(element->layerIsSampler.empty()))
	{
	  theScreen->AddScreenLayer((*itt)*CLHEP::m, *itm, *itIsSampler);
	  ++itIsSampler;
	}
      else
	{theScreen->AddScreenLayer((*itt)*CLHEP::m, *itm);}
    }
  return theScreen;
}

#ifdef USE_AWAKE
BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeScreen()
{
  return (new BDSAwakeScintillatorScreen(elementName,
					 element->scintmaterial,
					 element->tscint*CLHEP::m,
					 element->windowScreenGap*CLHEP::m,
					 element->angle,
					 element->twindow*CLHEP::m,
					 element->windowmaterial));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeSpectrometer()
{
  BDSFieldInfo* awakeField = nullptr;
  if (element->fieldAll.empty())
    {
      BDSMagnetStrength* awakeStrength = new BDSMagnetStrength();
      SetBeta0(awakeStrength);
      (*awakeStrength)["field"] = element->scaling * element->B * CLHEP::tesla;
      (*awakeStrength)["by"]    = 1; // bx,by,bz is unit field direction, so (0,1,0) here

      G4Transform3D fieldTrans = CreateFieldTransform(element);
      awakeField = new BDSFieldInfo(BDSFieldType::dipole,
                                    BRho(),
				    BDSIntegratorType::g4classicalrk4,
				    awakeStrength,
				    true,
				    fieldTrans);
      awakeField->SetModulatorInfo(ModulatorDefinition(element, true));
    }
  else
    {awakeField = BDSFieldFactory::Instance()->GetDefinition(element->fieldAll);}
  return (new BDSAwakeSpectrometer(elementName,
                                   element->awakeMagnetOffsetX*CLHEP::m,
				   element->l*CLHEP::m,
				   awakeField,
				   element->poleStartZ*CLHEP::m,
				   element->scintmaterial,
				   element->tscint*CLHEP::m,
				   element->screenPSize*CLHEP::m,
				   element->windowScreenGap*CLHEP::m,
				   element->angle,
				   element->twindow*CLHEP::m,
				   element->windowmaterial,
				   element->tmount*CLHEP::m,
				   element->mountmaterial,
				   element->screenEndZ*CLHEP::m,
				   element->spec,
				   element->screenWidth*CLHEP::m));
}
#endif // end of USE_AWAKE

BDSAcceleratorComponent* BDSComponentFactory::CreateTransform3D()
{
  if (element->axisAngle)
    {
      if (BDS::IsFinite(element->phi) || BDS::IsFinite(element->theta) || BDS::IsFinite(element->psi))
        {BDS::Warning(__METHOD_NAME__, "component \""+element->name+"\" has axisAngle=1 but one or more of (phi,theta,psi) are non-zero - check");}
      return new BDSTransform3D(elementName,
                                element->xdir * CLHEP::m,
                                element->ydir * CLHEP::m,
                                element->zdir * CLHEP::m,
                                element->axisX,
                                element->axisY,
                                element->axisZ,
                                element->angle * CLHEP::rad);
    }
  else
    {
      if (BDS::IsFinite(element->angle) || BDS::IsFinite(element->axisX) || BDS::IsFinite(element->axisY) || BDS::IsFinite(element->axisZ))
        {BDS::Warning(__METHOD_NAME__, "component \""+element->name+"\" does not have axisAngle=1 but one or more axisAngle variables are non-zero - check");}
      return new BDSTransform3D(elementName,
                                element->xdir * CLHEP::m,
                                element->ydir * CLHEP::m,
                                element->zdir * CLHEP::m,
                                element->phi   * CLHEP::rad,
                                element->theta * CLHEP::rad,
                                element->psi   * CLHEP::rad);
    }
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTerminator(const G4double width)
{
#ifdef BDSDEBUG
  G4cout << "---->creating Terminator" << G4endl;
#endif
  return new BDSTerminator(width);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRMatrix()
{
  BDSMagnetStrength* st = PrepareMagnetStrengthForRMatrix(element);

  GMAD::Element* elementNew = new GMAD::Element(*element);
  elementNew->l = (element->l-thinElementLength)/2.0;

  BDSAcceleratorComponent* parallelTransport1 = CreateMagnet(elementNew,
							     st,
							     BDSFieldType::paralleltransporter,
							     BDSMagnetType::paralleltransporter);
  BDSAcceleratorComponent* rmatrix = CreateThinRMatrix(0,
						       elementName + "_centre");
  BDSAcceleratorComponent* parallelTransport2 = CreateMagnet(elementNew,
							     st,
							     BDSFieldType::paralleltransporter,
							     BDSMagnetType::paralleltransporter);

  const G4String baseName = elementName;
  BDSLine* bLine = new BDSLine(baseName);
  bLine->AddComponent(parallelTransport1);
  bLine->AddComponent(rmatrix);
  bLine->AddComponent(parallelTransport2);

  delete elementNew;

  return bLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinRMatrix(G4double        angleIn,
								const G4String& name)
{
  BDSMagnetStrength* st = PrepareMagnetStrengthForRMatrix(element);
  auto modulator = ModulatorDefinition(element, true);
  return CreateThinRMatrix(angleIn, st, name, BDSIntegratorType::rmatrixthin, BDSFieldType::rmatrix, 0, modulator);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinRMatrix(G4double                 angleIn,
								BDSMagnetStrength*       st,
								const G4String&          name,
								BDSIntegratorType        intType,
								BDSFieldType             fieldType,
								G4double                 beamPipeRadius,
								BDSModulatorInfo*        fieldModulator)
{
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, angleIn, -angleIn);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;

  // override beampipe radius if supplied - must be set to be iris size for cavity model fringes.
  if (BDS::IsFinite(beamPipeRadius))
	  {beamPipeInfo->aper1 = beamPipeRadius;}

  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(name, element, -angleIn, angleIn, beamPipeInfo);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
                                               BRho(),
                                               intType,
                                               st,
                                               true,
                                               fieldTrans);
  vacuumField->SetBeamPipeRadius(beamPipeInfo->aper1);
  vacuumField->SetModulatorInfo(fieldModulator);
  vacuumField->SetFieldAsThin();

  BDSMagnet* thinRMatrix =  new BDSMagnet(BDSMagnetType::rmatrix,
                                          name,
                                          thinElementLength,
                                          beamPipeInfo,
                                          magnetOuterInfo,
                                          vacuumField,
                                          0, nullptr,  // default values for optional args (angle, outerFieldInfo)
                                          true);       // isThin

  thinRMatrix->SetExtent(BDSExtent(beamPipeInfo->aper1,
                                   beamPipeInfo->aper1,
                                   thinElementLength*0.5));

  return thinRMatrix;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateCavityFringe(G4double                 angleIn,
								 BDSMagnetStrength*       st,
								 const G4String&          name,
								 G4double                 irisRadius,
								 BDSModulatorInfo*        fieldModulator)
{
  BDSIntegratorType intType = integratorSet->cavityFringe;
  BDSFieldType fieldType = BDSFieldType::cavityfringe;
  BDSAcceleratorComponent* cavityFringe = CreateThinRMatrix(angleIn, st, name, intType, fieldType, irisRadius, fieldModulator);
  return cavityFringe;
}

BDSMagnet* BDSComponentFactory::CreateMagnet(const GMAD::Element* el,
					     BDSMagnetStrength*   st,
					     BDSFieldType         fieldType,
					     BDSMagnetType        magnetType,
					     G4double             angle,
					     const G4String&      nameSuffix) const
{
  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);
  BDSIntegratorType intType = integratorSet->Integrator(fieldType);
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;
  SetBeta0(st);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
                                               BRho(),
					       intType,
					       st,
					       true,
					       fieldTrans);
  vacuumField->SetModulatorInfo(ModulatorDefinition(el, true));

  BDSMagnetOuterInfo* outerInfo = PrepareMagnetOuterInfo(elementName + nameSuffix, element, st, bpInfo);
  vacuumField->SetScalingRadius(outerInfo->innerRadius); // purely for completeness of information - not required
  BDSFieldInfo* outerField = nullptr;

  // only make a default multipolar field if the yokeFields flag is on and
  // there isn't an 'outerField' specified for the element
  G4bool externalOuterField = !(el->fieldOuter.empty());
  if (yokeFields && !externalOuterField)
    {
      outerField = PrepareMagnetOuterFieldInfo(st,
					       fieldType,
					       bpInfo,
					       outerInfo,
					       fieldTrans,
					       integratorSet,
                                               BRho(),
                                               ScalingFieldOuter(element),
					       ModulatorDefinition(element, true));
    }

  return new BDSMagnet(magnetType,
		       elementName + nameSuffix,
		       el->l * CLHEP::m,
		       bpInfo,
		       outerInfo,
		       vacuumField,
		       angle,
		       outerField);
}

G4bool BDSComponentFactory::HasSufficientMinimumLength(Element const* el,
						       G4bool         printWarning)
{
  if (el->l < 1e-7) // 'l' already in metres from parser
    {
      if (printWarning)
	{BDS::Warning("---> NOT creating element \"" + el->name + "\" -> l < 1e-7 m: l = " + std::to_string(el->l) + " m");}
      return false;
    }
  else
    {return true;}
}

void BDSComponentFactory::PoleFaceRotationsNotTooLarge(Element const* element,
						       G4double maxAngle)
{
  if (std::abs(element->e1) > maxAngle)
    {throw BDSException(__METHOD_NAME__, "Pole face angle e1: " + std::to_string(element->e1) +
			" is greater than " + std::to_string(maxAngle));}
  if (std::abs(element->e2) > maxAngle)
    {throw BDSException(__METHOD_NAME__, "Pole face angle e2: " + std::to_string(element->e2) +
			" is greater than " + std::to_string(maxAngle));}
}

G4bool BDSComponentFactory::YokeOnLeft(const Element*           element,
				       const BDSMagnetStrength* st)
{
  // for lhcleft and lhcright - purposively override this behaviour
  auto mgt = MagnetGeometryType(element);
  if (mgt == BDSMagnetGeometryType::lhcleft)
    {return true;} // active beam pipe is right and yoke is on the left
  if (mgt == BDSMagnetGeometryType::lhcright)
    {return false;}
  
  G4double angle    = (*st)["angle"];
  G4double hkickAng = -(*st)["hkick"]; // not really angle but proportional in the right direction
  G4double vkickAng = -(*st)["vkick"];
  if (!BDS::IsFinite(angle) && BDS::IsFinite(hkickAng))
    {angle = hkickAng;}
  if (!BDS::IsFinite(angle) && BDS::IsFinite(vkickAng))
    {angle = vkickAng;}
  G4bool yokeOnLeft;
  if ((angle < 0) && (element->yokeOnInside))
    {yokeOnLeft = true;}
  else if ((angle > 0) && (!(element->yokeOnInside)))
    {yokeOnLeft = true;}
  else
    {yokeOnLeft = false;}
  return yokeOnLeft;
}

G4double BDSComponentFactory::ScalingFieldOuter(const GMAD::Element* ele)
{
  return ele->scalingFieldOuterSet ? ele->scalingFieldOuter : BDSGlobalConstants::Instance()->ScalingFieldOuter();
}

BDSFieldInfo* BDSComponentFactory::PrepareMagnetOuterFieldInfo(const BDSMagnetStrength*  vacuumSt,
							       const BDSFieldType&       fieldType,
							       const BDSBeamPipeInfo*    bpInfo,
							       const BDSMagnetOuterInfo* outerInfo,
							       const G4Transform3D&      fieldTransform,
							       const BDSIntegratorSet*   integratorSetIn,
							       G4double                  brhoIn,
                                                               G4double                  outerFieldScaling,
                                                               BDSModulatorInfo*         modulatorInfo)
{  
  BDSFieldType outerType;
  switch (fieldType.underlying())
    {
    case BDSFieldType::dipole:
      {outerType = BDSFieldType::multipoleouterdipole;    break;}
    case BDSFieldType::quadrupole:
      {outerType = BDSFieldType::multipoleouterquadrupole; break;}
    case BDSFieldType::sextupole:
      {outerType = BDSFieldType::multipoleoutersextupole;  break;}
    case BDSFieldType::octupole:
      {outerType = BDSFieldType::multipoleouteroctupole;   break;}
    case BDSFieldType::decapole:
      {outerType = BDSFieldType::multipoleouterdecapole;   break;}
    case BDSFieldType::skewquadrupole:
      {outerType = BDSFieldType::skewmultipoleouterquadrupole; break;}
    case BDSFieldType::skewsextupole:
      {outerType = BDSFieldType::skewmultipoleoutersextupole;  break;}
    case BDSFieldType::skewoctupole:
      {outerType = BDSFieldType::skewmultipoleouteroctupole;   break;}
    case BDSFieldType::skewdecapole:
      {outerType = BDSFieldType::skewmultipoleouterdecapole;   break;}
    case BDSFieldType::dipole3d:
    case BDSFieldType::solenoid:
      {outerType = BDSFieldType::solenoidsheet; break;}
    default:
      {return nullptr; break;} // no possible outer field for any other magnet types
    }

  BDSMagnetStrength* stCopy = new BDSMagnetStrength(*vacuumSt);
  (*stCopy)["scalingOuter"] = outerFieldScaling;
  BDSIntegratorType intType = integratorSetIn->Integrator(outerType);
  BDSFieldInfo* outerField  = new BDSFieldInfo(outerType,
					       brhoIn,
					       intType,
					       stCopy,
					       true,
					       fieldTransform);
  outerField->SetModulatorInfo(modulatorInfo);
  outerField->SetChordStepMinimum(BDSGlobalConstants::Instance()->ChordStepMinimumYoke());
  if (outerInfo)
    {
      outerField->SetScalingRadius(outerInfo->innerRadius);
      outerField->SetSecondFieldOnLeft(outerInfo->yokeOnLeft);
      auto gt = outerInfo->geometryType;
      G4bool yfmLHC = BDSGlobalConstants::Instance()->YokeFieldsMatchLHCGeometry();
      if ((gt == BDSMagnetGeometryType::lhcleft || gt == BDSMagnetGeometryType::lhcright) && yfmLHC)
        {
          if (fieldType == BDSFieldType::dipole)
            {outerField->SetFieldType(BDSFieldType::multipoleouterdipolelhc);}
          else if (fieldType == BDSFieldType::quadrupole)
            {outerField->SetFieldType(BDSFieldType::multipoleouterquadrupolelhc);}
          else if (fieldType == BDSFieldType::sextupole)
            {outerField->SetFieldType(BDSFieldType::multipoleoutersextupolelhc);}
        }
    }

  if (bpInfo)
    {outerField->SetBeamPipeRadius(bpInfo->IndicativeRadius());}
  return outerField;
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(const G4String& elementNameIn,
								const Element* el,
								const BDSMagnetStrength* st,
								const BDSBeamPipeInfo* beamPipe,
								G4double defaultHorizontalWidth,
								G4double defaultVHRatio,
								G4double defaultCoilWidthFraction,
								G4double defaultCoilHeightFraction)
{
  G4bool yokeOnLeft = YokeOnLeft(el,st);
  G4double    angle = (*st)["angle"];
  
  return PrepareMagnetOuterInfo(elementNameIn, el, 0.5*angle, 0.5*angle, beamPipe, yokeOnLeft,
				defaultHorizontalWidth, defaultVHRatio, defaultCoilWidthFraction,
				defaultCoilHeightFraction);
}

BDSMagnetGeometryType BDSComponentFactory::MagnetGeometryType(const Element* el)
{
  BDSMagnetGeometryType result;
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  // magnet geometry type
  if (el->magnetGeometryType.empty() || globals->IgnoreLocalMagnetGeometry())
    {result = globals->MagnetGeometryType();}
  else
    {result = BDS::DetermineMagnetGeometryType(el->magnetGeometryType);}
  return result;
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(const G4String& elementNameIn,
								const Element*  el,
								const G4double  angleIn,
								const G4double  angleOut,
								const BDSBeamPipeInfo* beamPipe,
								const G4bool    yokeOnLeft,
								G4double        defaultHorizontalWidth,
								G4double        defaultVHRatio,
								G4double        defaultCoilWidthFraction,
								G4double        defaultCoilHeightFraction)
{
  BDSMagnetOuterInfo* info = new BDSMagnetOuterInfo();

  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  info->name = elementNameIn;
  
  // magnet geometry type
  info->geometryType = MagnetGeometryType(el);
  if (! (el->magnetGeometryType.empty() || globals->IgnoreLocalMagnetGeometry()) )
    {
      info->geometryTypeAndPath = el->magnetGeometryType;
      if (el->stripOuterVolume)
        {BDS::Warning(__METHOD_NAME__, "stripOuterVolume for element \"" + el->name + "\" will have no effect");}
    }

  // set face angles w.r.t. chord
  info->angleIn  = angleIn;
  info->angleOut = angleOut;
  
  // horizontal width
  info->horizontalWidth = PrepareHorizontalWidth(el, defaultHorizontalWidth);

  // inner radius of magnet geometry - TODO when poles can be built away from beam pipe
  info->innerRadius = beamPipe->IndicativeRadius();

  // outer material
  G4Material* outerMaterial;
  if (el->material.empty())
    {
      G4String defaultMaterialName = globals->OuterMaterialName();
      outerMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
    }
  else
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(el->material);}
  info->outerMaterial = outerMaterial;

  // yoke direction
  info->yokeOnLeft = yokeOnLeft;

  if (el->hStyle < 0) // it's unset
    {info->hStyle = globals->HStyle();}
  else
    {info->hStyle = G4bool(el->hStyle);} // convert from int to bool

  if (el->vhRatio > 0)
    {info->vhRatio = G4double(el->vhRatio);}
  else if (globals->VHRatio() > 0)
    {info->vhRatio = globals->VHRatio();}
  else if (defaultVHRatio > 0) // allow calling function to supply optional default
    {info->vhRatio = defaultVHRatio;}
  else
    {info->vhRatio = info->hStyle ? 0.8 : 1.0;} // h default : c default
  
  if (el->coilWidthFraction > 0)
    {info->coilWidthFraction = G4double(el->coilWidthFraction);}
  else if (globals->CoilWidthFraction() > 0)
    {info->coilWidthFraction = globals->CoilWidthFraction();}
  else if (defaultCoilHeightFraction > 0) // allow calling function to supply optional default
    {info->coilWidthFraction = defaultCoilWidthFraction;}
  else
    {info->coilWidthFraction = info->hStyle ? 0.8 : 0.65;} // h default : c default

  if (el->coilHeightFraction > 0)
    {info->coilHeightFraction = G4double(el->coilHeightFraction);}
  else if (globals->CoilHeightFraction() > 0)
    {info->coilHeightFraction = globals->CoilHeightFraction();}
  else if (defaultCoilHeightFraction > 0) // allow calling function to supply optional default
    {info->coilHeightFraction = defaultCoilHeightFraction;}
  else
    {info->coilHeightFraction = 0.8;} // default for both h and c type

  info->colour = PrepareColour(el);
  info->autoColour = el->autoColour;
  
  return info;
}

G4double BDSComponentFactory::PrepareHorizontalWidth(Element const* el,
						     G4double defaultHorizontalWidth)
{
  G4double horizontalWidth = el->horizontalWidth*CLHEP::m;
  if (horizontalWidth < 1e-6)
    {//horizontalWidth not set - use either global or specified default
      if (defaultHorizontalWidth > 0)
	{horizontalWidth = defaultHorizontalWidth;}
      else
	{horizontalWidth = BDSGlobalConstants::Instance()->HorizontalWidth();}
    }
  return horizontalWidth;
}

G4Material* BDSComponentFactory::PrepareVacuumMaterial(Element const* el) const
{
  G4Material* result = nullptr;
  if (el->vacuumMaterial.empty())
    {result = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial());}
  else
    {result = BDSMaterials::Instance()->GetMaterial(element->vacuumMaterial);}
  return result;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* el,
							  const G4ThreeVector& inputFaceNormalIn,
							  const G4ThreeVector& outputFaceNormalIn)
{
  BDSBeamPipeInfo* defaultModel = BDSGlobalConstants::Instance()->DefaultBeamPipeModel();
  BDSBeamPipeInfo* result; 
  if (!BDSGlobalConstants::Instance()->IgnoreLocalAperture())
    {
      try
	{
	  result = new BDSBeamPipeInfo(defaultModel,
				       el->apertureType,
				       el->aper1 * CLHEP::m,
				       el->aper2 * CLHEP::m,
				       el->aper3 * CLHEP::m,
				       el->aper4 * CLHEP::m,
				       el->vacuumMaterial,
				       el->beampipeThickness * CLHEP::m,
				       el->beampipeMaterial,
				       inputFaceNormalIn,
				       outputFaceNormalIn);
	}
      catch (BDSException& e)
	{
	  G4String msg = "\nProblem in element: \"" + el->name + "\"";
	  e.AppendToMessage(msg);
	  throw e;
	}
    }
  else
    {// ignore the aperture model from the element and use the global one
      result = new BDSBeamPipeInfo(*defaultModel); // ok as only pointers to materials
      result->inputFaceNormal  = inputFaceNormalIn;
      result->outputFaceNormal = outputFaceNormalIn;
    }
  return result;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* el,
							  const G4double angleIn,
							  const G4double angleOut)
{
  auto faces = BDS::CalculateFaces(angleIn, angleOut);
  BDSBeamPipeInfo* info = PrepareBeamPipeInfo(el, faces.first, faces.second);
  return info;
}

BDSTiltOffset* BDSComponentFactory::CreateTiltOffset(Element const* el)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "offsetX,Y: " << el->offsetX << " " << el->offsetY << " tilt: " << el->tilt << G4endl;
#endif
  G4double xOffset = el->offsetX * CLHEP::m;
  G4double yOffset = el->offsetY * CLHEP::m;
  G4double tilt    = el->tilt    * CLHEP::rad;

  BDSTiltOffset* result = nullptr;
  if (BDS::IsFinite(xOffset) || BDS::IsFinite(yOffset) || BDS::IsFinite(tilt))
    {result = new BDSTiltOffset(xOffset, yOffset, tilt);}
  return result;
}

G4Transform3D BDSComponentFactory::CreateFieldTransform(const BDSTiltOffset* tiltOffset)
{
  return tiltOffset ? tiltOffset->Transform3D() : G4Transform3D();
}

G4Transform3D BDSComponentFactory::CreateFieldTransform(Element const* el)
{
  BDSTiltOffset* to = CreateTiltOffset(el);
  G4Transform3D trans = CreateFieldTransform(to);
  delete to;
  return trans;
}

void BDSComponentFactory::CheckBendLengthAngleWidthCombo(G4double arcLength,
							 G4double angle,
							 G4double horizontalWidth,
							 const G4String& name)
{
  G4double radiusFromAngleLength =  std::abs(arcLength / angle); // s = r*theta -> r = s/theta
  if (horizontalWidth > 2*radiusFromAngleLength)
    {
      G4cerr << "Error: the combination of length, angle and horizontalWidth in element named \"" << name
	     << "\" will result in overlapping faces!" << G4endl << "Please reduce the horizontalWidth" << G4endl;
      throw BDSException(__METHOD_NAME__, "");
    }
}

void BDSComponentFactory::PrepareCavityModels()
{  
  for (const auto& model : BDSParser::Instance()->GetCavityModels())
    {
      // material can either be specified in 
      G4Material* material = nullptr;
      if (!model.material.empty())
	{material = BDSMaterials::Instance()->GetMaterial(model.material);}
	  
      auto info = new BDSCavityInfo(BDS::DetermineCavityType(model.type),
				    material,
				    model.irisRadius*CLHEP::m,
				    model.thickness*CLHEP::m,
				    model.equatorRadius*CLHEP::m,
				    model.halfCellLength*CLHEP::m,
				    model.numberOfPoints,
				    model.numberOfCells,
				    model.equatorHorizontalAxis*CLHEP::m,
				    model.equatorVerticalAxis*CLHEP::m,
				    model.irisHorizontalAxis*CLHEP::m,
				    model.irisVerticalAxis*CLHEP::m,
				    model.tangentLineAngle);
      
      cavityInfos[model.name] = info;
    }
}

void BDSComponentFactory::PrepareColours()
{
  if (!coloursInitialised)
    {
      BDSColours* allColours = BDSColours::Instance();
      for (const auto& colour : BDSParser::Instance()->GetColours())
	{
	  allColours->DefineColour(G4String(colour.name),
				   (G4double) colour.red,
				   (G4double) colour.green,
				   (G4double) colour.blue,
				   (G4double) colour.alpha);
	}
      coloursInitialised = true;
  }
}

void BDSComponentFactory::PrepareCrystals()
{
  for (auto& model : BDSParser::Instance()->GetCrystals())
    {
      G4Material* material = BDSMaterials::Instance()->GetMaterial(model.material);

      auto info = new BDSCrystalInfo(material,
				     G4String(model.data),
				     BDS::DetermineCrystalType(model.shape),
				     G4double(model.lengthX)*CLHEP::m,
				     G4double(model.lengthY)*CLHEP::m,
				     G4double(model.lengthZ)*CLHEP::m,
				     G4double(model.sizeA)*CLHEP::m,
				     G4double(model.sizeB)*CLHEP::m,
				     G4double(model.sizeC)*CLHEP::m,
				     G4double(model.alpha)*CLHEP::halfpi,
				     G4double(model.beta)*CLHEP::halfpi,
				     G4double(model.gamma)*CLHEP::halfpi,
				     G4int   (model.spaceGroup),
				     G4double(model.bendingAngleYAxis)*CLHEP::rad,
				     G4double(model.bendingAngleZAxis)*CLHEP::rad,
				     G4double(model.miscutAngleY)*CLHEP::rad);
      crystalInfos[model.name] = info;
    }

}

BDSCrystalInfo* BDSComponentFactory::PrepareCrystalInfo(const G4String& crystalName) const
{
  auto result = crystalInfos.find(crystalName);
  if (result == crystalInfos.end())
    {
      G4cout << "Defined crystals are:" << G4endl;
      for (const auto& kv : crystalInfos)
	{G4cout << kv.first << G4endl;}
      throw BDSException(__METHOD_NAME__, "unknown crystal \"" + crystalName + "\" - please define it");
    }

  // prepare a copy so the component can own that recipe
  BDSCrystalInfo* info = new BDSCrystalInfo(*(result->second));
  return info;
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfo(Element const* el,
							   G4double frequency) const
{
  // If the cavity model name (identifier) has been defined, return a *copy* of
  // that model - so that the component will own that info object.
  G4String modelName = G4String(el->cavityModel);

  // no specific model - prepare a default based on element parameters
  if (modelName.empty())
    {return PrepareCavityModelInfoForElement(el, frequency);}

  // cavity model name specified - match up with parser object already translated here
  auto result = cavityInfos.find(modelName);
  if (result == cavityInfos.end())
    {throw BDSException(__METHOD_NAME__, "unknown cavity model identifier \"" + el->cavityModel + "\" - please define it");}

  // we make a per element copy of the definition
  BDSCavityInfo* info = new BDSCavityInfo(*(result->second));

  // check cavity radius against horizontal width. Cavity radius calculation same as that in CavityFactory classes.
  G4double cavityRadius = info->equatorRadius + info->thickness + lengthSafety;
  G4double horizontalWidth = PrepareHorizontalWidth(el);
  if (cavityRadius > horizontalWidth)
    {
      G4String msg = "Cavity horizontalWidth for element \"" + elementName + "\" is smaller " + "than the cavity model radius.";
      throw BDSException(__METHOD_NAME__, msg);
    }
  
  // If no material specified, we take the material from the element. If no material at
  // all, we exit with warning.
  if (!info->material)
    {
      if (el->material.empty())
	{
	  G4String msg = "cavity material is not defined for cavity \"" + elementName + "\"";
	  msg += " or for cavity model \"" + el->cavityModel + "\" - please define it";
	  throw BDSException(__METHOD_NAME__, msg);
	}
      else
	{info->material = BDSMaterials::Instance()->GetMaterial(el->material);}
    }

  return info;
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfoForElement(Element const* el,
								     G4double frequency) const
{
  /// prepare aperture information for this element to base default cavity on.
  BDSBeamPipeInfo* aperture = PrepareBeamPipeInfo(el);

  G4double aper1     = aperture->aper1;
  G4double horizontalWidth = PrepareHorizontalWidth(el);
  
  G4double defaultHorizontalWidth = 20*CLHEP::cm;
  if (aper1 < defaultHorizontalWidth) // only do if the aperture will fit
    {horizontalWidth = std::min(defaultHorizontalWidth, horizontalWidth);} // better default
  G4double thickness = aperture->beamPipeThickness;
  G4double equatorRadius = horizontalWidth - thickness;
  if (equatorRadius <= 0)
    {
      G4String msg = "horizontalWidth - beampipeThickness <= 0 for element \"" + el->name + "\" -> this quantity must be positive";
      throw BDSException(__METHOD_NAME__, msg);
    }

  // assume single cell cavity
  G4double length     = el->l * CLHEP::m;
  G4double cellLength = length;
  G4int nCells        = 1;

  // calculate number of cells if frequency is finite -
  // frequency can be zero in which case only build 1 cell
  if (BDS::IsFinite(frequency))
    {
      cellLength = 2*CLHEP::c_light / frequency; // half wavelength
      G4double nCavities  = length / cellLength;
      nCells = G4int(std::floor(nCavities));
    }

  if (nCells == 0) // protect against long wavelengths or cavities
    {
      nCells = 1;
      cellLength = length;
    }
  
  BDSCavityInfo* defaultCI = new BDSCavityInfo(BDSCavityType::pillbox,
					       BDSMaterials::Instance()->GetMaterial("Copper"),
					       aperture->aper1,
					       thickness,
					       equatorRadius,
					       cellLength*0.5);

  delete aperture;
  return defaultCI;
}

G4double BDSComponentFactory::EFieldFromElement(Element const* el,
                                                BDSFieldType fieldType,
                                                G4double cavityLength,
                                                const BDSParticleDefinition& incomingParticle)
{
  G4double eField = 0; // - result variable
  G4double scaling = el->scaling;
  G4double frequency = el->frequency * CLHEP::rad;
  G4double phase = el->phase * CLHEP::rad;
  
  // the sign of the field to be accelerating is handled here - each field class just uses the value
  G4int acceleratingFieldDirectionFactor = BDS::Sign(incomingParticle.BRho());
  
  switch (fieldType.underlying())
    {
    case BDSFieldType::rfconstantinx:
    case BDSFieldType::rfconstantiny:
      {// voltage is not used for these
        eField = scaling * el->gradient * CLHEP::volt / CLHEP::m;
        break;
      }
    case BDSFieldType::rfconstantinz:
      {
        if (BDS::IsFinite(el->gradient))
          {eField = scaling * el->gradient * CLHEP::volt / CLHEP::m;}
        else
          {eField = scaling * el->E * CLHEP::volt / cavityLength;}
        break;
      }
    case BDSFieldType::rfpillbox:
      {
        if (BDS::IsFinite(el->gradient))
          {eField = scaling * el->gradient * CLHEP::volt / CLHEP::m;}
        else
          {
            G4double transitTimeFactor = BDSFieldEMRFCavity::TransitTimeFactor(frequency, phase, cavityLength, incomingParticle.Beta());
            eField = scaling * el->E * CLHEP::volt / cavityLength;
            // divide by the transit time factor so the total energy gain comes out as expected
            eField /= transitTimeFactor;
          }
        break;
      }
    default:
      {break;} // shouldn't happen
    }
  eField *= acceleratingFieldDirectionFactor;
  return eField;
}

void BDSComponentFactory::CalculateAngleAndFieldRBend(const Element* el,
                                                      G4double brhoIn,
                                                      G4double& arcLength,
                                                      G4double& chordLength,
                                                      G4double& field,
                                                      G4double& angle)
{
  // 'l' in the element represents the chord length for an rbend - must calculate arc length
  // for the field calculation and the accelerator component.
  chordLength = el->l * CLHEP::m;
  G4double arcLengthLocal = chordLength; // default for no angle

  if (BDS::IsFinite(el->B) && el->angleSet)
    {// both are specified and should be used - under or overpowered dipole by design
      // the angle and the length are set, so we ignore the beam definition and its bending radius
      field = el->B * CLHEP::tesla;
      angle = el->angle * CLHEP::rad;
      // protect against bad calculation from 0 angle and finite field
      if (BDS::IsFinite(angle))
        {
          G4double radiusOfCurvatureOfDipole = 0.5 * chordLength / std::sin(0.5 * angle);
          arcLengthLocal = radiusOfCurvatureOfDipole * angle;
        }
    else
      {arcLengthLocal = chordLength;}
    }
  else if (BDS::IsFinite(el->B))
    {// only B field - calculate angle
      field = el->B * CLHEP::tesla;
      G4double bendingRadius = brhoIn / field; // in mm as brho already in g4 units
      angle = 2.0*std::asin(chordLength*0.5 / bendingRadius);
      if (std::isnan(angle))
        {throw BDSException("Field too strong for element " + el->name + ", magnet bending angle will be greater than pi.");}

      arcLengthLocal = bendingRadius * angle;
    }
  else
    {// (assume) only angle - calculate B field
      angle = el->angle * CLHEP::rad;
      if (BDS::IsFinite(angle))
        {
        // sign for bending radius doesn't matter (from angle) as it's only used for arc length.
        // this is the inverse equation of that in BDSAcceleratorComponent to calculate
        // the chord length from the arclength and angle.
        G4double bendingRadius = chordLength * 0.5 / std::sin(std::abs(angle) * 0.5);
        arcLengthLocal = bendingRadius * angle;
        field = brhoIn * angle / std::abs(arcLengthLocal);
      }
    else
      {field = 0;} // 0 angle -> chord length and arc length the same; field 0
  }

  // Ensure positive length despite sign of angle.
  arcLength = std::abs(arcLengthLocal);

  if (std::abs(angle) > CLHEP::pi/2.0)
    {throw BDSException("Error: the rbend " + el->name + " cannot be constructed as its bending angle is defined to be greater than pi/2.");}
}

BDSMagnetStrength* BDSComponentFactory::PrepareCavityStrength(Element const*      el,
							      BDSFieldType        fieldType,
							      G4double            cavityLength,
							      BDSMagnetStrength*& fringeIn,
							      BDSMagnetStrength*& fringeOut) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double chordLength   = cavityLength; // length may be reduced for fringe placement.
  (*st)["equatorradius"] = 1*CLHEP::m; // to prevent 0 division - updated later in createRF
  (*st)["length"]        = chordLength;
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;
  
  switch (fieldType.underlying())
    {
    case BDSFieldType::rfpillbox:
    case BDSFieldType::rfconstantinz:
      {(*st)["ez"] = 1.0; break;}
    case BDSFieldType::rfconstantinx:
      {(*st)["ex"] = 1.0; break;}
    case BDSFieldType::rfconstantiny:
      {(*st)["ey"] = 1.0; break;}
    default:
      {(*st)["ez"] = 1.0; break;}
    }
    
  // scale factor to account for reduced body length due to fringe placement.
  G4double lengthScaling = cavityLength / (element->l * CLHEP::m);
  
  if ((fieldType == BDSFieldType::rfconstantinx || fieldType == BDSFieldType::rfconstantiny) && BDS::IsFinite(el->E) )
    {throw BDSException(__METHOD_NAME__, "only \"gradient\" is accepted for rfconstantinx or rfconstantiny components and not \"E\"");}

  G4double eField = EFieldFromElement(el, fieldType, chordLength, integralUpToThisComponent->designParticle); // includes scaling
  (*st)["efield"] = eField / lengthScaling;
  
  G4double frequency = std::abs(el->frequency * CLHEP::hertz);
  (*st)["frequency"] = frequency;

  // set the phase from the element even if zero frequency, field should be cos(0 + phi) = constant.
  G4double phase = el->phase * CLHEP::rad;
  if (BDS::IsFinite(el->tOffset))
    {
      if (BDS::IsFinite(el->phase))
        {throw BDSException(__METHOD_NAME__, "component: " + el->name + " has both tOffset and phase specified - only one can be used.");}
      phase = el->tOffset*CLHEP::s * frequency * CLHEP::twopi;
    }
  (*st)["phase"] = phase;

  // fringe strengths
  fringeIn  = new BDSMagnetStrength(*st);
  fringeOut = new BDSMagnetStrength(*st);
  // fringe synchronousT0 - should be the same as the centre of the component they're linked to
  // fringe phase - here the values are copied into the fringe strengths - so only the raw input
  // phase that are provided from the input that generally modulate the fringe

  // if frequency is 0, don't update phase with offset. Fringes should have the same phase.
  if (!BDS::IsFinite(frequency))
    {return st;}
  
  return st;
}

G4Colour* BDSComponentFactory::PrepareColour(Element const* el, const G4Material* material)
{
  G4String colour = el->colour;
  if (material && el->autoColour)
    {return BDSColourFromMaterial::Instance()->GetColour(material);}
  else if (colour.empty())
    {return BDSColours::Instance()->GetColour(GMAD::typestr(el->type));}
  else
    {return BDSColours::Instance()->GetColour(colour);}
}

G4Material* BDSComponentFactory::PrepareMaterial(Element const* el,
                                                 const G4String& defaultMaterialName)
{
  G4String materialName = el->material;
  if (materialName.empty())
    {return BDSMaterials::Instance()->GetMaterial(defaultMaterialName);}
  else
    {return BDSMaterials::Instance()->GetMaterial(materialName);}
}

G4Material* BDSComponentFactory::PrepareMaterial(Element const* el)
{
  G4String materialName = el->material;
  if (materialName.empty())
    {throw BDSException(__METHOD_NAME__, "element \"" + el->name + "\" has no material specified.");}
  else
    {return BDSMaterials::Instance()->GetMaterial(materialName);}
}

void BDSComponentFactory::SetFieldDefinitions(Element const* el,
                                              BDSAcceleratorComponent* component) const
{
  // Test for a line. And if so apply to each sub-component.
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  if (BDSLine* line = dynamic_cast<BDSLine*>(component))
    {
      for (auto comp : *line)
        {SetFieldDefinitions(el, comp);}
    }
  // RF can ues fieldVacuum for the cavity field model but this would overwrite it which is wrong
  BDSMagnet* mag = dynamic_cast<BDSMagnet*>(component);
  if (mag && el->type != ElementType::_RF)
    {
      if (!(el->fieldAll.empty()))
        {
          G4cerr << "Error: Magnet named \"" << elementName
                 << "\" is a magnet, but has fieldAll defined." << G4endl
                 << "Can only have fieldOuter and or fieldVacuum specified." << G4endl;
          throw BDSException(__METHOD_NAME__, "");
        }
      if (!(el->fieldOuter.empty()))
        {
          BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldOuter)));
          if (info->ProvideGlobal())
            {info->SetTransformBeamline(fieldTrans);}
          info->CompoundBScaling(ScalingFieldOuter(el));
          SetModulatorDefinition(el, info);
          mag->SetOuterField(info);
        }
      if (!(el->fieldVacuum.empty()))
        {
          BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldVacuum)));
          if (info->ProvideGlobal())
            {info->SetTransformBeamline(fieldTrans);}
          SetModulatorDefinition(el, info);
          mag->SetVacuumField(info);
        }
    }
  else
    {
      if (!(el->fieldAll.empty()))
        {
          BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldAll)));
          if (info->ProvideGlobal())
            {info->SetTransformBeamline(fieldTrans);}
          SetModulatorDefinition(element, info);
          if (el->scalingFieldOuter != 1)
            {BDS::Warning("component \"" + el->name + "\" has \"scalingFieldOuter\" != 1.0 -> this will have no effect for \"fieldAll\"");}
          component->SetField(info);
        }
    }
}

void BDSComponentFactory::SetModulatorDefinition(Element const* el,
                                                 BDSFieldInfo* info) const
{
  if (!el->fieldModulator.empty())
    {
      if (info->ModulatorInfo()) // already exists
        {throw BDSException(__METHOD_NAME__, "\""+elementName+"\" uses a field map with a modulator but also a modulator\ndouble modulation is not allowed");}
      else
        {
          auto modDef = ModulatorDefinition(el);
          info->SetModulatorInfo(modDef); // works even if none
        }
    }
}

BDSMagnetStrength* BDSComponentFactory::PrepareMagnetStrengthForMultipoles(Element const* el) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double scaling = el->scaling;
  G4double arcLength = el->l * CLHEP::m;
  (*st)["length"] = arcLength; // length needed for thin multipoles
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;
  // component strength is only normalised by length for thick multipoles
  if (el->type == ElementType::_THINMULT || (el->type == ElementType::_MULT && !BDS::IsFinite(el->l)))
    {(*st)["length"] = 1*CLHEP::m;}
  auto kn = el->knl.begin();
  auto ks = el->ksl.begin();
  std::vector<G4String> normKeys = BDSMagnetStrength::NormalComponentKeys();
  std::vector<G4String> skewKeys = BDSMagnetStrength::SkewComponentKeys();
  auto nkey = normKeys.begin();
  auto skey = skewKeys.begin();
  // Separate loops for kn and ks. The length of knl and ksl are determined by the input in the gmad file.
  for (; kn != el->knl.end(); kn++, nkey++)
    {(*st)[*nkey] = scaling * (*kn);}
  for (; ks != el->ksl.end(); ks++, skey++)
    {(*st)[*skey] = scaling * (*ks);}

  return st;
}

BDSMagnetStrength* BDSComponentFactory::PrepareMagnetStrengthForRMatrix(Element const* el) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["synchronousT0"] = synchronousTAtMiddleOfThisComponent;
  SetBeta0(st);
  G4double scaling = el->scaling;
  // G4double length  = el->l;

  (*st)["kick1"] = scaling * el->kick1;
  (*st)["kick2"] = scaling * el->kick2;
  (*st)["kick3"] = scaling * el->kick3;
  (*st)["kick4"] = scaling * el->kick4;

  (*st)["rmat11"] = scaling * el->rmat11;
  (*st)["rmat12"] = scaling * el->rmat12;
  (*st)["rmat13"] = scaling * el->rmat13;
  (*st)["rmat14"] = scaling * el->rmat14;

  (*st)["rmat21"] = scaling * el->rmat21;
  (*st)["rmat22"] = scaling * el->rmat22;
  (*st)["rmat23"] = scaling * el->rmat23;
  (*st)["rmat24"] = scaling * el->rmat24;

  (*st)["rmat31"] = scaling * el->rmat31;
  (*st)["rmat32"] = scaling * el->rmat32;
  (*st)["rmat33"] = scaling * el->rmat33;
  (*st)["rmat34"] = scaling * el->rmat34;

  (*st)["rmat41"] = scaling * el->rmat41;
  (*st)["rmat42"] = scaling * el->rmat42;
  (*st)["rmat43"] = scaling * el->rmat43;
  (*st)["rmat44"] = scaling * el->rmat44;

  return st;
}

G4double BDSComponentFactory::FieldFromAngle(const G4double angle,
                                             const G4double arcLength) const
{
  if (!BDS::IsFinite(angle))
    {return 0;}
  else
    {return BRho() * angle / arcLength;}
}

G4double BDSComponentFactory::AngleFromField(const G4double field,
                                             const G4double arcLength) const
{
  if (!BDS::IsFinite(field))
    {return 0;}
  else
    {return field * arcLength / BRho();}
}

void BDSComponentFactory::CalculateAngleAndFieldSBend(Element const* el,
                                                      G4double&      angle,
                                                      G4double&      field) const
{
  G4double arcLength = el->l * CLHEP::m;
  if (BDS::IsFinite(el->B) && el->angleSet)
    {// both are specified and should be used - under or overpowered dipole by design
      field = el->B * CLHEP::tesla;
      angle = el->angle * CLHEP::rad;
    }
  else if (BDS::IsFinite(el->B))
    {// only B field - calculate angle
      field = el->B * CLHEP::tesla;
      angle = AngleFromField(field, arcLength); // length is arc length for an sbend
    }
  else
    {// only angle - calculate B field
      angle = el->angle * CLHEP::rad;
      field = FieldFromAngle(angle, arcLength);
    }
  // un-split sbends are effectively rbends - don't construct a >pi/2 degree rbend
  // also cannot construct sbends with angles > pi/2
  if (BDSGlobalConstants::Instance()->DontSplitSBends() && (std::abs(angle) > CLHEP::pi/2.0))
    {throw BDSException("Error: the unsplit sbend "+ el->name + " cannot be constructed as its bending angle is defined to be greater than pi/2.");}

  else if (std::abs(angle) > CLHEP::pi*2.0)
    {throw BDSException("Error: the sbend "+ el->name +" cannot be constructed as its bending angle is defined to be greater than 2 pi.");}
}

G4double BDSComponentFactory::BendAngle(const Element* el) const
{
  G4double bendAngle = 0;
  if (el->type == ElementType::_RBEND)
    {
      G4double arcLength = 0, chordLength = 0, field = 0;
      CalculateAngleAndFieldRBend(el, BRho(), arcLength, chordLength, field, bendAngle);
    }
  else if (el->type == ElementType::_SBEND)
    {
      G4double field = 0; // required by next function.
      CalculateAngleAndFieldSBend(el, bendAngle, field);
    }
  else if (el->type == ElementType::_ELEMENT)
    {bendAngle = -1*el->angle*CLHEP::rad;}
  // else the default is 0
  return bendAngle;
}

G4double BDSComponentFactory::OutgoingFaceAngle(const Element* el) const
{
  if (!el)
    {return 0;}
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a previous element
  // - it'll be skipped while looking backwards.
  G4double outgoingFaceAngle = 0;
  G4double bendAngle         = BendAngle(el);

  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e2 = el->e2*CLHEP::rad;
  if (el->type == ElementType::_ELEMENT)
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      outgoingFaceAngle += factor * e2;
      return outgoingFaceAngle;
    }
  else if (el->type == ElementType::_RBEND)
    {
      if (integratorSet->IsMatrixIntegratorSet())
        {return outgoingFaceAngle;}
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      outgoingFaceAngle += 0.5 * bendAngle;
    }

  // if we're using the matrix integrator set, we build the bends flat irrespective of parameters
  // if we have a finite k1 for a bend, we're forced to use the bdsimmatrix integrator set
  if (integratorSet->IsMatrixIntegratorSet() || BDS::IsFinite(el->k1))
    {return outgoingFaceAngle;}

  // for an sbend, the output face is nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes
  
  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  if (BDS::IsFinite(e2) && BDSGlobalConstants::Instance()->BuildPoleFaceGeometry())
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      outgoingFaceAngle += factor * e2;
    }
  
  return outgoingFaceAngle;
}

G4double BDSComponentFactory::IncomingFaceAngle(const Element* el) const
{
  if (!el)
    {return 0;}
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a next element
  // - it'll be skipped while looking forwards.
  G4double incomingFaceAngle = 0;
  G4double bendAngle = BendAngle(el);

  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e1 = el->e1*CLHEP::rad;
  if (el->type == ElementType::_ELEMENT)
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      incomingFaceAngle += factor * e1;
      return incomingFaceAngle;
    }
  else if (el->type == ElementType::_RBEND)
    {
      if (integratorSet->IsMatrixIntegratorSet())
        {return incomingFaceAngle;}
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      incomingFaceAngle += 0.5 * bendAngle;
    }

  // if we're using the matrix integrator set, we build the bends flat irrespective of parameters
  // if we have a finite k1 for a bend, we're forced to use the bdsimmatrix integrator set
  if (integratorSet->IsMatrixIntegratorSet() || BDS::IsFinite(el->k1))
    {return incomingFaceAngle;}

  // for an sbend, the output face or nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes

  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  if (BDS::IsFinite(e1) && BDSGlobalConstants::Instance()->BuildPoleFaceGeometry())
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      incomingFaceAngle += factor * e1;
    }
  
  return incomingFaceAngle;
}

BDSModulatorInfo* BDSComponentFactory::ModulatorDefinition(const GMAD::Element* el,
                                                           G4bool inDevelopment) const
{
  if (inDevelopment)
    {INDEVELOPMENTERROR();}
  return el->fieldModulator.empty() ? defaultModulator : BDSFieldFactory::Instance()->GetModulatorDefinition(el->fieldModulator);
}

void BDSComponentFactory::INDEVELOPMENTERROR() const
{
  if (!element->fieldModulator.empty())
    {throw BDSException(__METHOD_NAME__, "fieldModulator is currently in development for element \"" + elementName + "\"");}
}
