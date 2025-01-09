/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "coolingchannel.h"

using namespace GMAD;

CoolingChannel::CoolingChannel()
{
  clear();
  PublishMembers();
}

void CoolingChannel::clear()
{
  name = "";
  surroundingMaterial = "vacuum";

  nCells = 0;
  cellLengthZ = 0;

  nCoils = 0;
  coilInnerRadius.clear();
  coilRadialThickness.clear();
  coilLengthZ.clear();
  coilCurrent.clear();
  coilOffsetZ.clear();
  coilMaterial.clear();
  mirrorCoils = false;
  onAxisTolerance = 0;
  nAbsorbers = 0;
  nSheets = 0;
  absorberType.clear();
  absorberMaterial.clear();
  absorberOffsetZ.clear();
  absorberCylinderLength.clear();
  absorberCylinderRadius.clear();
  absorberWedgeOpeningAngle.clear();
  absorberWedgeHeight.clear();
  absorberWedgeRotationAngle.clear();
  absorberWedgeOffsetX.clear();
  absorberWedgeOffsetY.clear();
  absorberWedgeApexToBase.clear();
  nRFCavities = 0;
  rfOffsetZ.clear();
  rfLength.clear();
  rfVoltage.clear();
  rfPhase.clear();
  rfFrequency.clear();
  rfWindowThickness.clear();
  rfWindowMaterial.clear();
  rfWindowRadius.clear();
  rfCavityMaterial.clear();
  rfCavityVacuumMaterial.clear();
  rfCavityRadius.clear();
  rfCavityThickness.clear();
  rfTimeOffset.clear();
  integrator = "g4classicalrk4";
  magneticFieldModel = "solenoidblock";
  electricFieldModel = "rfcavity";
}

void CoolingChannel::PublishMembers()
{
  publish("name",                 &CoolingChannel::name);

  publish("surroundingMaterial",  &CoolingChannel::surroundingMaterial);
  publish("nCells",               &CoolingChannel::nCells);
  publish("cellLengthZ",          &CoolingChannel::cellLengthZ);
  publish("nCoils",               &CoolingChannel::nCoils);
  publish("coilInnerRadius",      &CoolingChannel::coilInnerRadius);
  publish("coilRadialThickness",  &CoolingChannel::coilRadialThickness);
  publish("coilLengthZ",          &CoolingChannel::coilLengthZ);
  publish("coilCurrent",          &CoolingChannel::coilCurrent);
  publish("coilOffsetZ",          &CoolingChannel::coilOffsetZ);
  publish("coilMaterial",         &CoolingChannel::coilMaterial);
  publish("mirrorCoils",          &CoolingChannel::mirrorCoils);
  publish("onAxisTolerance",      &CoolingChannel::onAxisTolerance);
  publish("nSheets",             &CoolingChannel::nSheets);



  publish("nAbsorbers",                 &CoolingChannel::nAbsorbers);
  publish("absorberType",               &CoolingChannel::absorberType);
  publish("absorberMaterial",           &CoolingChannel::absorberMaterial);
  publish("absorberOffsetZ",            &CoolingChannel::absorberOffsetZ);
  publish("absorberCylinderLength",     &CoolingChannel::absorberCylinderLength);
  publish("absorberCylinderRadius",     &CoolingChannel::absorberCylinderRadius);
  publish("absorberWedgeOpeningAngle",  &CoolingChannel::absorberWedgeOpeningAngle);
  publish("absorberWedgeHeight",        &CoolingChannel::absorberWedgeHeight);
  publish("absorberWedgeRotationAngle", &CoolingChannel::absorberWedgeRotationAngle);
  publish("absorberWedgeOffsetX",       &CoolingChannel::absorberWedgeOffsetX);
  publish("absorberWedgeOffsetY",       &CoolingChannel::absorberWedgeOffsetY);
  publish("absorberWedgeApexToBase",    &CoolingChannel::absorberWedgeApexToBase);

  publish("nRFCavities",       &CoolingChannel::nRFCavities);
  publish("rfOffsetZ",         &CoolingChannel::rfOffsetZ);
  publish("rfLength",          &CoolingChannel::rfLength);
  publish("rfVoltage",         &CoolingChannel::rfVoltage);
  publish("rfPhase",           &CoolingChannel::rfPhase);
  publish("rfFrequency",       &CoolingChannel::rfFrequency);
  publish("rfWindowThickness", &CoolingChannel::rfWindowThickness);
  publish("rfWindowMaterial",  &CoolingChannel::rfWindowMaterial);
  publish("rfWindowRadius",    &CoolingChannel::rfWindowRadius);
  publish("rfCavityMaterial",  &CoolingChannel::rfCavityMaterial);
  publish("rfCavityVacuumMaterial", &CoolingChannel::rfCavityVacuumMaterial);
  publish("rfCavityRadius",    &CoolingChannel::rfCavityRadius);
  publish("rfCavityThickness", &CoolingChannel::rfCavityThickness);
  publish("rfTimeOffset",      &CoolingChannel::rfTimeOffset);

  publish("integrator",        &CoolingChannel::integrator);
  publish("magneticFieldModel",&CoolingChannel::magneticFieldModel);
  publish("electricFieldModel",&CoolingChannel::electricFieldModel);

  attribute_map_list_double["coilInnerRadius"]     = &coilInnerRadius;
  attribute_map_list_double["coilRadialThickness"] = &coilRadialThickness;
  attribute_map_list_double["coilLengthZ"]         = &coilLengthZ;
  attribute_map_list_double["coilCurrent"]         = &coilCurrent;
  attribute_map_list_double["coilOffsetZ"]         = &coilOffsetZ;
  attribute_map_list_string["coilMaterial"]        = &coilMaterial;
  attribute_map_list_string["absorberType"]        = &absorberType;
  attribute_map_list_string["absorberMaterial"]    = &absorberMaterial;
  attribute_map_list_double["absorberOffsetZ"]     = &absorberOffsetZ;
  attribute_map_list_double["absorberCylinderLength"]     = &absorberCylinderLength;
  attribute_map_list_double["absorberCylinderRadius"]     = &absorberCylinderRadius;
  attribute_map_list_double["absorberWedgeOpeningAngle"]  = &absorberWedgeOpeningAngle;
  attribute_map_list_double["absorberWedgeHeight"]        = &absorberWedgeHeight;
  attribute_map_list_double["absorberWedgeRotationAngle"] = &absorberWedgeRotationAngle;
  attribute_map_list_double["absorberWedgeOffsetX"]       = &absorberWedgeOffsetX;
  attribute_map_list_double["absorberWedgeOffsetY"]       = &absorberWedgeOffsetY;
  attribute_map_list_double["absorberWedgeApexToBase"]    = &absorberWedgeApexToBase;
  attribute_map_list_double["rfOffsetZ"]           = &rfOffsetZ;
  attribute_map_list_double["rfLength"]            = &rfLength;
  attribute_map_list_double["rfVoltage"]           = &rfVoltage;
  attribute_map_list_double["rfPhase"]             = &rfPhase;
  attribute_map_list_double["rfFrequency"]         = &rfFrequency;
  attribute_map_list_double["rfWindowThickness"]   = &rfWindowThickness;
  attribute_map_list_string["rfWindowMaterial"]    = &rfWindowMaterial;
  attribute_map_list_string["rfCavityVacuumMaterial"] = &rfCavityVacuumMaterial;
  attribute_map_list_double["rfWindowRadius"]      = &rfWindowRadius;
  attribute_map_list_string["rfCavityMaterial"]    = &rfCavityMaterial;
  attribute_map_list_double["rfCavityRadius"]      = &rfCavityRadius;
  attribute_map_list_double["rfCavityThickness"]   = &rfCavityThickness;
  attribute_map_list_double["rfTimeOffset"]        = &rfTimeOffset;
  // integrator, magneticFieldModel and electricFieldModel can't be lists or arrays, so don't include here
}

template <class T>
std::ostream& operator<<(std::ostream& out, const std::list<T>& l);

void CoolingChannel::print()const

{
  std::cout << "coolingchannel: "
            << "name "                       << name                       << std::endl
            << "nCells "                     << nCells                     << std::endl
            << "cellLengthZ "                << cellLengthZ                << std::endl
            << "nCoils "                     << nCoils                     << std::endl
            << "coilInnerRadius "            << coilInnerRadius            << std::endl
            << "coilRadialThickness "        << coilRadialThickness        << std::endl
            << "coilLengthZ "                << coilLengthZ                << std::endl
            << "coilCurrent "                << coilCurrent                << std::endl
            << "coilOffsetZ "                << coilOffsetZ                << std::endl
            << "coilMaterial "               << coilMaterial               << std::endl
            << "mirrorCoils "                << mirrorCoils                << std::endl
            << "nAbsorbers "                 << nAbsorbers                 << std::endl
            << "absorberType "               << absorberType               << std::endl
            << "absorberMaterial "           << absorberMaterial           << std::endl
            << "absorberOffsetZ "            << absorberOffsetZ            << std::endl
            << "absorberCylinderLength "     << absorberCylinderLength     << std::endl
            << "absorberCylinderRadius "     << absorberCylinderRadius     << std::endl
            << "absorberWedgeOpeningAngle "  << absorberWedgeOpeningAngle  << std::endl
            << "absorberWedgeHeight "        << absorberWedgeHeight        << std::endl
            << "absorberWedgeRotationAngle " << absorberWedgeRotationAngle << std::endl
            << "absorberWedgeOffsetX "       << absorberWedgeOffsetX       << std::endl
            << "absorberWedgeOffsetY "       << absorberWedgeOffsetY       << std::endl
            << "absorberWedgeApexToBase "    << absorberWedgeApexToBase    << std::endl
            << "nRFCavities "                << nRFCavities                << std::endl
            << "rfOffsetZ "                  << rfOffsetZ                  << std::endl
            << "rfLength"                    << rfLength                   << std::endl
            << "rfVoltage "                  << rfVoltage                  << std::endl
            << "rfPhase "                    << rfPhase                    << std::endl
            << "rfFrequency "                << rfFrequency                << std::endl
            << "rfWindowThickness "          << rfWindowThickness          << std::endl
            << "rfWindowMaterial "           << rfWindowMaterial           << std::endl
            << "rfCavityVacuumMaterial "     << rfCavityVacuumMaterial     << std::endl
            << "rfWindowRadius "             << rfWindowRadius             << std::endl
            << "rfCavityMaterial "           << rfCavityMaterial           << std::endl
            << "rfCavityRadius "             << rfCavityRadius             << std::endl
            << "rfCavityThickness "          << rfCavityThickness          << std::endl
            << "rfTimeOffset "               << rfTimeOffset               << std::endl
            << "integrator "                 << integrator                 << std::endl
            << "magneticFieldModel "         << magneticFieldModel         << std::endl
            << "electricFieldModel "         << electricFieldModel         << std::endl;
}

template <class T>
std::ostream& operator<<(std::ostream& out, const std::list<T>& l)
{
  out << "{";
  for (const auto& v : l)
    {out << v << ", ";}
  out << "}";
  return out;
}

void CoolingChannel::set_value(const std::string& property, Array* value)
{
  auto search = attribute_map_list_double.find(property);
  if (search != attribute_map_list_double.end())
    {value->set_vector(*search->second);}
  else
    {
      auto search2 = attribute_map_list_string.find(property);
      if (search2 != attribute_map_list_string.end())
        {value->set_vector(*search2->second);}
      else
        {
          std::cerr << "Error: parser> unknown coolingchannel option \"" << property << "\", or doesn't expect vector type" << std::endl;
          exit(1);
        }
    }
}
