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
#include "element.h"
#include "elementtype.h"
#include "parameters.h"
#include "parser.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace GMAD;

namespace
{
  /// Helper method
  void print(const std::list<Element>& l, int ident=0)
  {
    if (ident == 0)
      {std::cout << "using line " << Parser::Instance()->current_line << std::endl;}

    for (auto& el : l)
      {el.print();}
  }
}

Element::Element():
  lst(nullptr)
{
  flush();

  PublishMembers();
}

void Element::PublishMembers()
{
  publish("userTypeName",   &Element::userTypeName);
  publish("userParameters", &Element::userParameters);
  
  publish("l",         &Element::l);
  publish("scaling",   &Element::scaling);
  publish("scalingFieldOuter", &Element::scalingFieldOuter);
  publish("ks",        &Element::ks);
  publish("k1",        &Element::k1);
  publish("k2",        &Element::k2);
  publish("k3",        &Element::k3);
  publish("k4",        &Element::k4);
  publish("angle",     &Element::angle);
  publish("B",         &Element::B);
  publish("e1",        &Element::e1);
  publish("e2",        &Element::e2);
  publish("fint",      &Element::fint);
  publish("fintx",     &Element::fintx);
  publish("fintK2",    &Element::fintK2);
  publish("fintxK2",   &Element::fintxK2);
  publish("hgap",      &Element::hgap);
  publish("h1",        &Element::h1);
  publish("h2",        &Element::h2);
  publish("kick",      &Element::kick);
  publish("hkick",     &Element::hkick);
  publish("vkick",     &Element::vkick);
  publish("knl",       &Element::knl);
  publish("ksl",       &Element::ksl);
  publish("gradient",  &Element::gradient);
  publish("E",         &Element::E);
  publish("frequency", &Element::frequency);
  publish("phase",     &Element::phase);
  publish("tOffset",   &Element::tOffset);
  publish("fieldModulator", &Element::fieldModulator);
  publish("kg",        &Element::kg);

  // rmatrix elements, only 4x4
  publish("kick1",     &Element::kick1);
  publish("kick2",     &Element::kick2);
  publish("kick3",     &Element::kick3);
  publish("kick4",     &Element::kick4);
  publish("rmat11",     &Element::rmat11);
  publish("rmat12",     &Element::rmat12);
  publish("rmat13",     &Element::rmat13);
  publish("rmat14",     &Element::rmat14);
  publish("rmat21",     &Element::rmat21);
  publish("rmat22",     &Element::rmat22);
  publish("rmat23",     &Element::rmat23);
  publish("rmat24",     &Element::rmat24);
  publish("rmat31",     &Element::rmat31);
  publish("rmat32",     &Element::rmat32);
  publish("rmat33",     &Element::rmat33);
  publish("rmat34",     &Element::rmat34);
  publish("rmat41",     &Element::rmat41);
  publish("rmat42",     &Element::rmat42);
  publish("rmat43",     &Element::rmat43);
  publish("rmat44",     &Element::rmat44);

  // beampipe information, new aperture model
  publish("beampipeThickness",&Element::beampipeThickness);
  publish("aper1",            &Element::aper1);
  publish("aper",             &Element::aper1);
  alternativeNames["aper"] = "aper1";
  publish("aperture",         &Element::aper1);
  alternativeNames["aperture"] = "aper1";
  publish("aperture1",        &Element::aper1);
  alternativeNames["aperture1"] = "aper1";
  publish("beampipeRadius",   &Element::aper1);
  alternativeNames["beampipeRadius"] = "aper1";
  publish("aper2",            &Element::aper2);
  publish("aperture2",        &Element::aper2);
  alternativeNames["aperture2"] = "aper2";
  publish("aper3",            &Element::aper3);
  publish("aperture3",        &Element::aper3);
  alternativeNames["aperture3"] = "aper3";
  publish("aper4",            &Element::aper4);
  publish("aperture4",        &Element::aper4);
  alternativeNames["aperture4"] = "aper4";
  publish("apertureType",        &Element::apertureType);
  publish("beampipeMaterial",    &Element::beampipeMaterial);
  publish("vacuumMaterial",      &Element::vacuumMaterial);

  // magnet geometry
  publish("magnetGeometryType",  &Element::magnetGeometryType);
  publish("horizontalWidth",     &Element::horizontalWidth);
  publish("outerDiameter",       &Element::horizontalWidth);
  alternativeNames["outerDiameter"] = "horizontalWidth";
  publish("yokeOnInside",        &Element::yokeOnInside);
  publish("hStyle",              &Element::hStyle);
  publish("vhRatio",             &Element::vhRatio);
  publish("coilWidthFraction",   &Element::coilWidthFraction);
  publish("coilHeightFraction",  &Element::coilHeightFraction);
  publish("tilt",             &Element::tilt);
  publish("xsize",            &Element::xsize);
  publish("ysize",            &Element::ysize);
  publish("xsizeOut",         &Element::xsizeOut);
  publish("ysizeOut",         &Element::ysizeOut);
  publish("xsizeLeft",        &Element::xsizeLeft);
  publish("xsizeRight",       &Element::xsizeRight);
  publish("offsetX",     &Element::offsetX);
  publish("offsetY",     &Element::offsetY);
  publish("jawTiltLeft",     &Element::jawTiltLeft);
  publish("jawTiltRight",     &Element::jawTiltRight);

  // screen parameters
  publish("tscint",          &Element::tscint);
  publish("twindow",         &Element::twindow);
  publish("tmount",          &Element::tmount);
  publish("windowScreenGap", &Element::windowScreenGap);
  publish("screenXSize",     &Element::screenXSize);
  publish("screenYSize",     &Element::screenYSize);
  publish("layerThicknesses",&Element::layerThicknesses);
  publish("layerMaterials",  &Element::layerMaterials);
  publish("layerIsSampler",  &Element::layerIsSampler);

  // for AWAKE spectrometer
  publish("screenPSize",        &Element::screenPSize);
  publish("screenEndZ",         &Element::screenEndZ);
  publish("poleStartZ",         &Element::poleStartZ);
  publish("screenWidth",        &Element::screenWidth);
  publish("awakeMagnetOffsetX", &Element::awakeMagnetOffsetX);
  publish("windowmaterial",      &Element::windowmaterial);
  publish("scintmaterial",       &Element::scintmaterial);
  publish("mountmaterial",       &Element::mountmaterial);

  // for 3d transform and laser
  publish("x",           &Element::xdir);
  alternativeNames["x"] = "xdir";
  publish("y",           &Element::ydir);
  alternativeNames["y"] = "ydir";
  publish("z",           &Element::zdir);
  alternativeNames["z"] = "zdir";
  publish("xdir",        &Element::xdir);
  publish("ydir",        &Element::ydir);
  publish("zdir",        &Element::zdir);
  publish("waveLength",  &Element::waveLength);
  publish("phi",         &Element::phi);
  publish("theta",       &Element::theta);
  publish("psi",         &Element::psi);
  publish("axisX",       &Element::axisX);
  publish("axisY",       &Element::axisY);
  publish("axisZ",       &Element::axisZ);
  publish("axisAngle",   &Element::axisAngle);

  // for degrader
  publish("numberWedges",      &Element::numberWedges);
  publish("wedgeLength",       &Element::wedgeLength);
  publish("degraderHeight",    &Element::degraderHeight);
  publish("materialThickness", &Element::materialThickness);
  publish("degraderOffset",    &Element::degraderOffset);

  // for wirescanner
  publish("wireDiameter",      &Element::wireDiameter);
  publish("wireLength",        &Element::wireLength);
  publish("wireOffsetX",       &Element::wireOffsetX);
  publish("wireOffsetY",       &Element::wireOffsetY);
  publish("wireOffsetZ",       &Element::wireOffsetZ);
  publish("wireAngle",         &Element::wireAngle);

  // for undulator
  publish("undulatorPeriod",       &Element::undulatorPeriod);
  publish("undulatorGap",          &Element::undulatorGap);
  publish("undulatorMagnetHeight", &Element::undulatorMagnetHeight);

  // for gabor lens
  publish("anodeRadius",         &Element::anodeRadius);
  publish("anodeLength",         &Element::anodeLength);
  publish("anodeThickness",      &Element::anodeThickness);
  publish("electrodeRadius",     &Element::electrodeRadius);
  publish("electrodeLength",     &Element::electrodeLength);
  publish("electrodeThickness",  &Element::electrodeThickness);

  // bias
  publish("bias",                &Element::bias);
  publish("biasMaterial",        &Element::biasMaterial);
  publish("biasVacuum",          &Element::biasVacuum);

  publish("minimumKineticEnergy",&Element::minimumKineticEnergy);

  publish("samplerName",         &Element::samplerName);
  publish("samplerType",         &Element::samplerType);
  publish("r",                   &Element::samplerRadius); // historic
  publish("samplerRadius",       &Element::samplerRadius);
  alternativeNames["r"] ="samplerRadius";

  publish("region",      &Element::region);
  publish("fieldOuter",  &Element::fieldOuter);
  publish("fieldVacuum", &Element::fieldVacuum);
  publish("fieldAll",    &Element::fieldAll);
  publish("bmap",        &Element::fieldAll);
  alternativeNames["bmap"] = "fieldAll";
  
  publish("geometryFile",        &Element::geometryFile);
  publish("geometry",            &Element::geometryFile);
  alternativeNames["geometry"] = "geometryFile"; // backwards compatibility
  publish("stripOuterVolume",    &Element::stripOuterVolume);
  publish("autoColour",          &Element::autoColour);
  publish("elementLengthIsArcLength", &Element::elementLengthIsArcLength);
  publish("material",            &Element::material);
  publish("outerMaterial",       &Element::material);
  alternativeNames["outerMaterial"] = "material";
  publish("namedVacuumVolumes",  &Element::namedVacuumVolumes);
  publish("markAsCollimator",    &Element::markAsCollimator);
  publish("spec",                &Element::spec);
  publish("cavityModel",         &Element::cavityModel);
  publish("cavityFieldType",     &Element::cavityFieldType);

  publish("dicomDataPath",       &Element::dicomDataPath);
  publish("dicomDataFile",       &Element::dicomDataFile);

  publish("colour",              &Element::colour);
  
  publish("crystalLeft",            &Element::crystalLeft);
  publish("crystalRight",           &Element::crystalRight);
  publish("crystalBoth",            &Element::crystalBoth);
  publish("crystalAngleYAxisLeft" , &Element::crystalAngleYAxisLeft);
  publish("crystalAngleYAxisRight", &Element::crystalAngleYAxisRight);
}

std::string Element::getPublishedName(const std::string& nameIn) const
{
  auto it = alternativeNames.find(nameIn);
  if (it != alternativeNames.end())
    {return it->second;}
  // if not found return name
  return nameIn;
}

bool Element::isSpecial() const
{
  return (type == ElementType::_TRANSFORM3D ||
          type == ElementType::_MARKER ||
          type == ElementType::_LINE ||
          type == ElementType::_REV_LINE);
}

void Element::print(int ident) const
{
  for(int i=0;i<ident;i++)
    {std::cout << "--";}

  std::cout << name << " : " << type << std::endl;
  if (l > 0.0)
    {std::cout << "l = " << l << "m" << std::endl;}
  if (horizontalWidth > 0)
    {std::cout << "horizontalWidth = " << horizontalWidth << "m" << std::endl;}
  if (samplerType != "none")
    {
      std::cout << "samplerType = " << samplerType << "\n"
                << "samplerRadius = " << samplerRadius << "\n"
                << "samplerarticleSetID = " << samplerParticleSetID << std::endl;
    }

  switch(type)
    {
    case ElementType::_SBEND:
    case ElementType::_RBEND:
      {
        std::cout << "B     = " << B     << std::endl
                  << "angle = " << angle << std::endl
                  << "k1    = " << k1    << std::endl;
        break;
      }
    case ElementType::_QUAD:
      {std::cout << "k1    = " << k1 << std::endl; break;}
    case ElementType::_SEXTUPOLE:
      {std::cout << "k2    = " << k2 << std::endl; break;}
    case ElementType::_OCTUPOLE:
      {std::cout << "k3    = " << k3 << std::endl; break;}
    case ElementType::_DECAPOLE:
      {std::cout << "k4    = " << k4 << std::endl; break;}
    case ElementType::_SOLENOID:
      {std::cout << "ks    = " << ks << std::endl; break;}
    case ElementType::_MULT:
    case ElementType::_THINMULT:
      {
        std::cout << " , knl={";
        for (auto value : knl)
          {std::cout << value << ", ";}
        std::cout << "},  ksl={";
        for (auto value : ksl)
          {std::cout << value << ", ";}
        std::cout << "}" << std::endl;
        break;
      }
    case ElementType::_ECOL:
    case ElementType::_RCOL:
    case ElementType::_JCOL:
      {
        std::cout << "x half aperture = " << xsize <<" m" << std::endl
                  << "y half aperture = " << ysize <<" m" << std::endl
                  << "material = \""      << material << "\"" << std::endl;
        break;
      }
    case ElementType::_ELEMENT:
      {
        std::cout << "horizontalWidth: " << horizontalWidth << "m" << std::endl
                  << "geometryFile:    " << geometryFile << std::endl
                  << "fieldAll:        " << fieldAll     << std::endl;
        break;
      }
    case ElementType::_RF:
    case ElementType::_RFX:
    case ElementType::_RFY:
      {
        std::cout << "E (voltage): "        << E << " V" << std::endl
		  << "gradient (e field): " << gradient << " V/m" << std::endl
		  << "cavityFieldType: "    << cavityFieldType << std::endl
		  << "cavityModel: "        << cavityModel << std::endl
		  << "frequency: "          << frequency << " Hz" << std::endl
		  << "phase: "              << phase << " rad" << std::endl;
        break;
      }
    case ElementType::_CT:
      {
        std::cout << "dicomDataPath: " << dicomDataPath << std::endl;
        std::cout << "dicomDataFile: " << dicomDataFile << std::endl;
        break;
      }
    case ElementType::_AWAKESCREEN:
      {
        std::cout << "twindow         = " << twindow*1e6         << " um" << std::endl
                  << "tscint          = " << tscint*1e6          << " um" << std::endl
                  << "windowScreenGap = " << windowScreenGap*1e6 << " um" << std::endl
                  << "windowmaterial  = " << windowmaterial      << std::endl
                  << "scintmaterial   = " << scintmaterial       << std::endl;
        break;
      }
    case ElementType::_AWAKESPECTROMETER:
      {
        std::cout << "twindow         = " << twindow*1e6         << " um" << std::endl
                  << "tscint          = " << tscint*1e6          << " um" << std::endl
                  << "screenPSize     = " << screenPSize*1e6     << " um" << std::endl
                  << "windowScreenGap = " << windowScreenGap*1e6 << " um" << std::endl
                  << "windowmaterial  = " << windowmaterial      << std::endl
                  << "tmount          = " << tmount*1e6          << " um" << std::endl
                  << "mountmaterial   = " << mountmaterial       << std::endl
                  << "scintmaterial   = " << scintmaterial       << std::endl;
        break;
      }
    case ElementType::_LASER:
      {
        std::cout << "lambda= " << waveLength << "m" << std::endl
                  << "xSigma= " << xsize << "m" << std::endl
                  << "ySigma= " << ysize << "m" << std::endl
                  << "xdir= "   << xdir << std::endl
                  << "ydir= "   << ydir << std::endl
                  << "zdir= "   << zdir << std::endl;
        break;
      }
    case ElementType::_SCREEN:
      {
        std::cout << "angle= " << angle <<"rad" << std::endl;
        break;
      }
    case ElementType::_TRANSFORM3D:
      {
        std::cout << "xdir=  " << xdir  << "m" << std::endl
                  << "ydir=  " << ydir  << "m" << std::endl
                  << "zdir=  " << zdir  << "m" << std::endl
                  << "phi=   " << phi   << "rad" << std::endl
                  << "theta= " << theta << "rad" << std::endl
                  << "psi=   " << psi   << "rad" << std::endl;
        break;
      }
    case ElementType::_GABORLENS:
      {
        std::cout << "b=  " << B  << "T" << std::endl
                  << "kg=  " << kg  << std::endl;
        break;
      }
    default:
      {break;}
    }

  switch (type)
    {
    case ElementType::_RBEND:
    case ElementType::_SBEND:
    case ElementType::_QUAD:
    case ElementType::_SEXTUPOLE:
    case ElementType::_OCTUPOLE:
    case ElementType::_DECAPOLE:
    case ElementType::_SOLENOID:
    case ElementType::_MULT:
    case ElementType::_THINMULT:
    case ElementType::_AWAKESPECTROMETER:
    case ElementType::_MUONSPOILER:
    case ElementType::_HKICKER:
    case ElementType::_VKICKER:
    case ElementType::_KICKER:
    case ElementType::_TKICKER:
    case ElementType::_UNDULATOR:
    case ElementType::_RF:
    case ElementType::_RFX:
    case ElementType::_RFY:
      {
        std::cout << "scaling = " << scaling << std::endl;
        if (scalingFieldOuter != 1)
          {std::cout << "scalingFieldOuter = " << scalingFieldOuter << std::endl;}
            std::cout << "fieldModulator = \"" << fieldModulator << "\"" << std::endl;
        break;
      }
    default:
      {break;}
    }
  
  if (lst)
    {::print(*lst,++ident);}
}

void Element::flush()
{
  type = ElementType::_NONE;
  name = "";
  userTypeName = "";
  userParameters = "";
  l = 0;
  scaling = 1;
  scalingFieldOuter = 1;
  ks = 0;
  k1 = 0;
  k2 = 0;
  k3 = 0;
  k4 = 0;
  angle = 0;
  B = 0;
  e1 = 0;
  e2 = 0;
  fint = 0;
  fintx = -1;
  fintK2 = 0;
  fintxK2 = 0;
  hgap  = 0;
  h1 = 0;
  h2 = 0;
  kick  = 0;
  hkick = 0;
  vkick = 0;
  knl.clear();
  ksl.clear();
  gradient  = 0;
  E         = 0;
  frequency = 0;
  phase     = 0;
  tOffset   = 0;
  fieldModulator = "";
  kg = 0;

  // rmatrix
  kick1 = 0;
  kick2 = 0;
  kick3 = 0;
  kick4 = 0;
  rmat11= 1.0;
  rmat12= 0;
  rmat13= 0;
  rmat14= 0;
  rmat21= 0;
  rmat22= 1.0;
  rmat23= 0;
  rmat24= 0;
  rmat31= 0;
  rmat32= 0;
  rmat33= 1.0;
  rmat34= 0;
  rmat41= 0;
  rmat42= 0;
  rmat43= 0;
  rmat44= 1.0;

  // new aperture model
  beampipeThickness = 0;
  aper1 = 0;
  aper2 = 0;
  aper3 = 0;
  aper4 = 0;
  apertureType = "";
  beampipeMaterial = "";
  vacuumMaterial = "";

  // magnet geometry
  magnetGeometryType  = "";
  horizontalWidth = 0;
  yokeOnInside  = true;
  hStyle             = -1;
  vhRatio            = -1;
  coilWidthFraction  = -1;
  coilHeightFraction = -1; // signifies use default in factory
  tilt = 0;
  xsize = 0;
  ysize = 0;
  xsizeOut = 0;
  ysizeOut = 0;
  xsizeLeft = 0;
  xsizeRight = 0;
  offsetX = 0;
  offsetY = 0;
  jawTiltLeft = 0;
  jawTiltRight = 0;

  // screen parameters
  tscint = 0.0003;
  twindow = 0;
  tmount = 0;
  windowScreenGap = 0;
  screenXSize = 0;
  screenYSize = 0;
  layerThicknesses.clear();
  layerMaterials.clear();
  layerIsSampler.clear();

  // for AWAKE spectrometer
  screenPSize        = 0;
  screenEndZ         = 0;
  poleStartZ         = 0;
  screenWidth        = 0;
  awakeMagnetOffsetX = 0.13;
  windowmaterial     = "vacuum";
  scintmaterial      = "";
  mountmaterial      = "";

  // for 3d transform and laser
  xdir = 0;
  ydir = 0;
  zdir = 0;
  waveLength = 0;
  gradient = 0;
  phi = 0;
  theta = 0;
  psi = 0;
  axisX = 0;
  axisY = 0;
  axisZ = 0;
  axisAngle = false;

  // for degrader
  numberWedges = 1;
  wedgeLength = 0;
  degraderHeight = 0;
  materialThickness = 0;
  degraderOffset = 0;

  // for wirescanner
  wireDiameter = 0;
  wireLength   = 0;
  wireOffsetX  = 0;
  wireOffsetY  = 0;
  wireOffsetZ  = 0;
  wireAngle    = 0;

  // undulator
  undulatorPeriod = 1;
  undulatorGap = 0;
  undulatorMagnetHeight = 0;

  // gabor lens
  anodeLength = 0;
  anodeRadius = 0;
  anodeThickness = 0;
  electrodeLength = 0;
  electrodeRadius = 0;
  electrodeThickness = 0;

  // bias
  bias         = "";
  biasMaterial = "";
  biasVacuum   = "";
  biasMaterialList.clear();
  biasVacuumList.clear();
  
  minimumKineticEnergy = 0;

  samplerName = "";
  samplerType = "none"; // allowed "none", "plane", "cylinder"
  samplerRadius = 0;
  samplerParticleSetID = -1;   // -1 is code for none
  
  region      = "";
  fieldOuter  = "";
  fieldVacuum = "";
  fieldAll    = "";

  geometryFile = "";
  stripOuterVolume = false;
  autoColour   = true;
  elementLengthIsArcLength = false;
  material="";
  namedVacuumVolumes = "";
  markAsCollimator = false;
  spec = "";
  cavityModel = "";
  cavityFieldType = "";
  
  dicomDataFile = "";
  dicomDataPath = "";
  
  colour = "";

  crystalLeft            = "";
  crystalRight           = "";
  crystalBoth            = "";
  crystalAngleYAxisLeft  = 0;
  crystalAngleYAxisRight = 0;
  
  angleSet = false;
  scalingFieldOuterSet = false;

  lst = nullptr;
}

double Element::property_lookup(std::string property_name) const
{
  double value;
  try
    {value = get<double>(this,property_name);}
  catch (const std::runtime_error&)
    {
      std::cerr << "element.cc> Error: unknown property \"" << property_name
                << "\" (only works on numerical properties)" << std::endl; 
      exit(1);
    }
  return value;
}

void Element::set(const Parameters& params, std::string nameIn, ElementType typeIn)
{
  // common parameters for all elements
  type = typeIn;
  name = nameIn;
  
  set(params);

  if (params.setMap.at("angle"))
    {angleSet = true;}
  else if (params.setMap.at("scalingFieldOuter"))
    {scalingFieldOuterSet = true;}
}

void Element::set(const Parameters& params)
{
  for (auto& i : params.setMap)
    {
      if (i.second)
        {
          std::string property = i.first;

          // method can in theory throw runtime_error (shouldn't happen), catch and exit gracefully
          try
            {Published<Element>::set(this,(Element*)&params,property);}
          catch(const std::runtime_error&)
            {
              std::cerr << "Error: parser> unknown property \"" << property
                        << "\" for element " << name  << std::endl;
              exit(1);
            }

          // split bias into tokens and add to both material and vacuum
          if (property == "bias")
            {
              std::stringstream ss(bias);
              std::string tok;
              while(ss >> tok)
                {
                  biasMaterialList.push_back(tok);
                  biasVacuumList.push_back(tok);
                }
            }
          else if (property == "biasMaterial")
            {
              std::stringstream ss(biasMaterial);
              std::string tok;
              while(ss >> tok) {biasMaterialList.push_back(tok);}
            }
          else if (property == "biasVacuum")
            {
              std::stringstream ss(biasVacuum);
              std::string tok;
              while(ss >> tok) {biasVacuumList.push_back(tok);}
            }
        }
    }
}

void Element::setSamplerInfo(std::string samplerTypeIn,
                             std::string samplerNameIn,
                             double samplerRadiusIn,
                             int particleSetIDIn)
{
  if (samplerType != "none")
    {std::cout << "WARNING: overwriting already defined sampler info for element: " << name << std::endl;}

  samplerType   = samplerTypeIn;
  samplerName   = samplerNameIn;
  samplerRadius = samplerRadiusIn;
  samplerParticleSetID = particleSetIDIn;
}
