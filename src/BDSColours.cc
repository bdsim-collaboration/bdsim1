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
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Colour.hh"

#include <iomanip>
#include <map>
#include <sstream>

BDSColours* BDSColours::instance = nullptr;

BDSColours* BDSColours::Instance()
{
  if (!instance)
    {instance = new BDSColours();}
  return instance;
}

BDSColours::~BDSColours()
{
  instance = nullptr;
}

BDSColours::BDSColours()
{
  magnetName[1] = "sectorbend";
  magnetName[2] = "quadrupole";
  magnetName[3] = "sextupole";
  magnetName[4] = "octupole";
  magnetName[5] = "decapole";

  // colour definitions (red,green,blue)
  // some additional info:
  // http://geant4.web.cern.ch/geant4/G4UsersDocuments/UsersGuides/ForApplicationDeveloper/html/Visualization/attributes.html
  // https://en.wikipedia.org/wiki/Web_colors

  // special
  colours["default"]          = new G4Colour(0.9,   0.9,   0.9);   // almost white
  colours["warning"]          = new G4Colour(1,     0.078, 0.576); // hot warning pink
  
  // visualisation of trajectories
  colours["traj_neutral"]     = new G4Colour(0.2, 0.7, 0.0, 0.2); // 0  charge - nicer green 0, semi-transparent as tonnes of photons usually
  colours["traj_positive"]    = new G4Colour(0.0, 0.2, 0.9); // nicer blue
  colours["traj_negative"]    = new G4Colour(0.8, 0.0, 0.0); // nicer red

  // tunnel
  colours["tunnel"]           = new G4Colour(0.545, 0.533, 0.470); // a nice grey
  colours["tunnelfloor"]      = new G4Colour(0.5,   0.5,   0.45);  // another grey
  colours["soil"]             = new G4Colour(0.545, 0.353, 0, 0.4);// brown 
  
  // elements - one for each type in parser/elementtype.cc
  colours["marker"]           = colours["default"];
  colours["beampipe"]         = new G4Colour(0.4,   0.4,   0.4);   // dark grey
  colours["drift"]            = colours["beampipe"];
  colours["sectorbend"]       = new G4Colour(0,     0.4,   0.8);   // blue
  colours["sbend"]            = colours["sectorbend"];
  colours["rectangularbend"]  = colours["sectorbend"];
  colours["rbend"]            = colours["sectorbend"];
  colours["quadrupole"]       = new G4Colour(0.82,  0.1,   0.1);   // red
  colours["sextupole"]        = new G4Colour(1,     0.8,   0);     // yellow
  colours["octupole"]         = new G4Colour(0,     0.6,   0.3);   // green
  colours["decapole"]         = new G4Colour(0.3,   0.2,   0.7);   // purple
  colours["solenoid"]         = new G4Colour(1,     0.549, 0, 0.7);// orange - semi transparent to see coil
  colours["multipole"]        = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["rfcavity"]         = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["rf"]               = colours["rfcavity"];
  colours["rfx"]              = colours["rf"];
  colours["rfy"]              = colours["rf"];
  colours["srfcavity"]        = new G4Colour(0.69,  0.769, 0.871); // light steel blue
  colours["collimator"]       = new G4Colour(0.25,  0.4,   0.2);   // dark green
  colours["ecol"]             = colours["collimator"];
  colours["jcol"]             = colours["collimator"];
  colours["rcol"]             = colours["collimator"];
  colours["jcol"]             = colours["collimator"];
  colours["bmcol"]            = colours["collimator"];
  colours["target"]           = colours["collimator"];
  colours["muonspoiler"]      = new G4Colour(0,     0.807, 0.819); // "light blue" / tab blue
  colours["vkicker"]          = new G4Colour(0.73,  0.33,  0.83);  // blue
  colours["hkicker"]          = new G4Colour(0.3,   0.2,   0.7);   // blue
  colours["kicker"]           = colours["sectorbend"];
  colours["tkicker"]          = colours["kicker"];
  colours["degrader"]         = new G4Colour(0.625, 0.625, 0.625); // silver
  colours["wirescanner"]      = colours["tunnel"];                 // grey
  colours["undulator"]        = new G4Colour(0.625, 0.625, 0.625); // silver
  colours["shield"]           = colours["tunnel"];
  colours["crystal"]          = colours["srfcavity"];
  colours["thinmultipole"]    = colours["default"];
  colours["dipolefringe"]     = colours["default"];
  colours["rmatrix"]          = colours["default"];
  colours["thinrmatrix"]      = colours["rmatrix"];
  colours["paralleltransporter"] = colours["default"];
  colours["element"]          = colours["default"];
  colours["screen"]           = new G4Colour(0.69,  0.769, 0.871); // slate grey
  colours["awakescreen"]      = colours["screen"];
  colours["awakespectrometer"]= colours["sectorbend"];
  colours["gap"]              = colours["default"];
  colours["opaquebox"]        = new G4Colour(0.2, 0.2, 0.2, 0.2); // dark grey but mostly transparent
  
  // some specific materials
  colours["iron"]             = new G4Colour(0.509, 0.321, 0.294);
  colours["kapton"]           = new G4Colour(0.929, 0.380, 0.082, 0.5);
  colours["lead"]             = new G4Colour(0.377, 0.408, 0.451);
  colours["water"]            = new G4Colour(0, 0.4, 0.8, 0.5);
  
  // element parts
  colours["coil"]             = new G4Colour(0.722, 0.451, 0.2);   // copper
  colours["copper"]           = colours["coil"];
  colours["LHCcopperskin"]    = new G4Colour(0.722, 0.525, 0.043); // copper
  colours["LHCcoil"]          = new G4Colour(0.9,   0.75,  0);     // gold-ish
  colours["LHCcollar"]        = new G4Colour(0.9,   0.9,   0.9);   // gainsboro
  colours["LHCyoke"]          = new G4Colour(0,     0.5,   1.0);   // LHC blue
  colours["LHCyokered"]       = new G4Colour(*(colours["quadrupole"]));// quadrupole red
  colours["gdml"]             = new G4Colour(0.4,   0.2,   0);     // poo brown
  colours["screenframe"]      = new G4Colour(0.7,   0.7,   0.7, 0.4);// light grey, semi-transparent
  colours["capillaryouter"]   = colours["collimator"];
  colours["capillarygas"]    = new G4Colour(0.82,  0.1,   0.1, 0.4); // red, semi-transparent
  colours["electrode"]        = colours["screen"];

  // general
  colours["white"]   = new G4Colour(G4Colour::White());
  colours["gray"]    = new G4Colour(G4Colour::Gray());
  colours["grey"]    = new G4Colour(G4Colour::Grey());
  colours["black"]   = new G4Colour(G4Colour::Black());
  colours["brown"]   = new G4Colour(G4Colour::Brown());
  colours["red"]     = new G4Colour(G4Colour::Red());
  colours["green"]   = new G4Colour(G4Colour::Green());
  colours["blue"]    = new G4Colour(G4Colour::Blue());
  colours["cyan"]    = new G4Colour(G4Colour::Cyan());
  colours["magenta"] = new G4Colour(G4Colour::Magenta());
  colours["yellow"]  = new G4Colour(G4Colour::Yellow());
  colours["reallyreallydarkgrey"] = new G4Colour(0.2, 0.2, 0.2);

#ifdef BDSDEBUG
  Print();
#endif
}

void BDSColours::DefineColour(const G4String& name,
			      G4double red,
			      G4double green,
			      G4double blue,
			      G4double alpha,
                              G4bool   normaliseTo255)
{
  if (colours.find(name) != colours.end())
    {
      G4cerr << "Colour \"" << name << "\" is already defined - clashing definitions" << G4endl;
      G4cout << "Already defined colours are " << G4endl;
      Print();
      throw BDSException(__METHOD_NAME__, "duplicate colour definition");
    }
  
  BDS::EnsureInLimits(red,0,255);
  BDS::EnsureInLimits(green,0,255);
  BDS::EnsureInLimits(blue,0,255);
  BDS::EnsureInLimits(alpha,0,1);
  G4Colour* newColour;
  if (normaliseTo255)
    {newColour = new G4Colour(red/255.,green/255.,blue/255.,alpha);}
  else
    {newColour = new G4Colour(red, green, blue, alpha);}
  colours[name] = newColour;
}

void BDSColours::Print()
{
  // auto-generate the manual colour table in rst syntax
  G4cout << __METHOD_NAME__ << "Colour Table" << G4endl;
  G4cout << "This is only the pre-defined BDSIM colours and not the user-defined ones." << G4endl;
  G4cout << "+---------------------+-----+-----+-----+------+" << G4endl;
  G4cout << "| Name                |  R  |  G  |  B  |  A   |" << G4endl;
  G4cout << "+=====================+=====+=====+=====+======+" << G4endl;
  for (const auto& col : colours)
    {
      int r = (int)(col.second->GetRed() * 255);
      int g = (int)(col.second->GetGreen() * 255);
      int b = (int)(col.second->GetBlue() * 255);
      double a = col.second->GetAlpha();
      G4cout << "| " << std::setw(20) << col.first << "| "
	     << std::setw(3) << r << " | "
	     << std::setw(3) << g << " | "
	     << std::setw(3) << b << " | "
	     << std::setw(4) << a << " |" << G4endl;
      G4cout << "+---------------------+-----+-----+-----+------+" << G4endl;
    }
}

G4Colour* BDSColours::GetColour(const G4String& type,
                                G4bool normaliseTo255)
{
  G4String colourName = type;
  G4bool   canDefine  = false;
  if (BDS::StrContains(type, ":"))
    {
      colourName = type.substr(0, type.find(":"));
      canDefine  = true;
    }
    
  auto it = colours.find(colourName);
  if (it != colours.end())
    {// colour must therefore exist
      return it->second;
    }
  else if (it == colours.end() && canDefine)
    {
      G4double r = 240;
      G4double g = 240;
      G4double b = 240; // default rgb is almost white but visible
      G4String rgb = type.substr(type.find(":")+1); // everything after ':'
      std::stringstream ss(rgb);
      G4double a = 1;
      ss >> r >> g >> b;
      if (ss.rdbuf()->in_avail() != 0)
	{ss >> a;}
      DefineColour(colourName, r, g, b, a, normaliseTo255);
      return colours[colourName];
    }
  else
    {throw BDSException(__METHOD_NAME__, "unknown colour \"" + type + "\"");}
}
