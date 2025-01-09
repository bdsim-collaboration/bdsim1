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
#include "parameters.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include "array.h"

using namespace GMAD;

Parameters::Parameters()
{
  // fill setMap, needs to match published member names
  // usually same but some exceptions, those are commented
  setMap["userTypeName"]   = false;
  setMap["userParameters"] = false;
  
  setMap["l"]         = false;
  setMap["scaling"]   = false;
  setMap["scalingFieldOuter"] = false;
  setMap["ks"]        = false;
  setMap["k1"]        = false;
  setMap["k2"]        = false;
  setMap["k3"]        = false;
  setMap["k4"]        = false;
  setMap["angle"]     = false;
  setMap["B"]         = false;
  setMap["e1"]        = false;
  setMap["e2"]        = false;
  setMap["fint"]      = false;
  setMap["fintx"]     = false;
  setMap["fintK2"]    = false;
  setMap["fintxK2"]   = false;
  setMap["h1"]        = false;
  setMap["h2"]        = false;
  setMap["hgap"]      = false;
  setMap["kick"]      = false;
  setMap["hkick"]     = false;
  setMap["vkick"]     = false;
  setMap["knl"]       = false;
  setMap["ksl"]       = false;
  setMap["gradient"]  = false;
  setMap["E"]         = false;
  setMap["gradient"]  = false;
  setMap["frequency"] = false;
  setMap["phase"]     = false;
  setMap["tOffset"]   = false;
  setMap["fieldModulator"] = false;

  setMap["kick1"]     = false;
  setMap["kick2"]     = false;
  setMap["kick3"]     = false;
  setMap["kick4"]     = false;
  setMap["rmat11"]     = false;
  setMap["rmat12"]     = false;
  setMap["rmat13"]     = false;
  setMap["rmat14"]     = false;
  setMap["rmat21"]     = false;
  setMap["rmat22"]     = false;
  setMap["rmat23"]     = false;
  setMap["rmat24"]     = false;
  setMap["rmat31"]     = false;
  setMap["rmat32"]     = false;
  setMap["rmat33"]     = false;
  setMap["rmat34"]     = false;
  setMap["rmat41"]     = false;
  setMap["rmat42"]     = false;
  setMap["rmat43"]     = false;
  setMap["rmat44"]     = false;

  setMap["beampipeThickness"]  = false;
  setMap["aper1"]              = false;
  setMap["aper2"]              = false;
  setMap["aper3"]              = false;
  setMap["aper4"]              = false;
  setMap["apertureType"]       = false;
  setMap["beampipeMaterial"]   = false;
  setMap["vacuumMaterial"]     = false;
  
  setMap["magnetGeometryType"] = false;
  setMap["horizontalWidth"]    = false;
  setMap["yokeOnInside"]       = false;
  setMap["hStyle"]             = false;
  setMap["vhRatio"]            = false;
  setMap["coilWidthFraction"]  = false;
  setMap["coilHeightFraction"] = false;
  setMap["tilt"]               = false;
  setMap["xsize"]              = false;
  setMap["ysize"]              = false;
  setMap["xsizeOut"]           = false;
  setMap["ysizeOut"]           = false;
  setMap["xsizeLeft"]          = false;
  setMap["xsizeRight"]         = false;
  setMap["offsetX"]            = false;
  setMap["offsetY"]            = false;
  
  setMap["tscint"]             = false;
  setMap["twindow"]            = false;
  setMap["tmount"]             = false;
  setMap["windowScreenGap"]    = false;
  setMap["screenXSize"]        = false;
  setMap["screenYSize"]        = false;
  setMap["layerThicknesses"]   = false;
  setMap["layerMaterials"]     = false;
  setMap["layerIsSampler"]     = false;

  setMap["screenPSize"]        = false;
  setMap["screenEndZ"]         = false;
  setMap["poleStartZ"]         = false;
  setMap["screenWidth"]        = false;
  setMap["awakeMagnetOffsetX"] = false;
  setMap["scintmaterial"]      = false;
  setMap["windowmaterial"]     = false;
  setMap["mountmaterial"]      = false;
  
  setMap["xdir"]               = false;
  setMap["ydir"]               = false;
  setMap["zdir"]               = false; 
  setMap["waveLength"]         = false;
  setMap["phi"]                = false;
  setMap["theta"]              = false;
  setMap["psi"]                = false;
  setMap["axisX"]              = false;
  setMap["axisY"]              = false;
  setMap["axisZ"]              = false;
  setMap["axisAngle"]          = false;

  setMap["numberWedges"]      = false;
  setMap["wedgeLength"]       = false;
  setMap["degraderHeight"]    = false;
  setMap["materialThickness"] = false;
  setMap["degraderOffset"]    = false;

  setMap["wireDiameter"]      = false;
  setMap["wireLength"]        = false;
  setMap["wireOffsetX"]       = false;
  setMap["wireOffsetY"]       = false;
  setMap["wireOffsetZ"]       = false;
  setMap["wireAngle"]         = false;
  
  setMap["undulatorPeriod"]       = false;
  setMap["undulatorGap"]          = false;
  setMap["undulatorMagnetHeight"] = false;

  setMap["bias"]                 = false;
  setMap["biasMaterial"]         = false;
  setMap["biasVacuum"]           = false;
  
  setMap["minimumKineticEnergy"] = false;
  
  setMap["samplerName"]          = false;
  setMap["samplerType"]          = false;
  setMap["samplerRadius"]        = false;
  setMap["region"]               = false;
  setMap["fieldOuter"]           = false;
  setMap["fieldVacuum"]          = false;
  setMap["fieldAll"]             = false;

  setMap["geometryFile"]   = false;
  setMap["stripOuterVolume"] = false;
  setMap["autoColour"]     = false;
  setMap["elementLengthIsArcLength"] = false;
  setMap["material"]       = false;
  setMap["namedVacuumVolumes"] = false;
  setMap["markAsCollimator"] = false;
  setMap["spec"]           = false;
  setMap["cavityModel"]    = false;
  setMap["cavityFieldType"] = false;

  setMap["dicomDataPath"]  = false;
  setMap["dicomDataFile"]  = false;

  setMap["colour"] = false;

  setMap["jawTiltLeft"] = false;
  setMap["jawTiltRight"] = false;

  setMap["crystalLeft"]            = false;
  setMap["crystalRight"]           = false;
  setMap["crystalBoth"]            = false;
  setMap["crystalAngleYAxisLeft"]  = false;
  setMap["crystalAngleYAxisRight"] = false;

  setMap["coolingDefinition"] = false;
}

void Parameters::flush()
{
  Element::flush();

  for (auto& i : setMap)
    {i.second = false;}
}

void Parameters::inherit_properties(const Element& e)
{
  // copy parameters into temporary buffer params from element e
  // parameters already set in params have priority and are not overridden
  // this is used for the inheritance / newinstance mechanism

  for (auto& i : setMap)
    {
      if(i.second == false)
	{
	  std::string property = i.first;
          // method can in theory throw runtime_error (shouldn't happen), catch and exit gracefully
	  try
	    {Published<Element>::set(this,(Element*)&e,property);}
	  catch(const std::runtime_error&)
	    {
	      std::cerr << "Error: element> unknown property \"" << property
			<< "\" from element " << e.name  << std::endl;
	      exit(1);
	    }
	  i.second = true;
	}
    }
}
