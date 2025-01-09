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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSIonDefinition.hh"

#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <exception>
#include <ostream>
#include <regex>
#include <stdexcept>

BDSIonDefinition::BDSIonDefinition(const G4String& definition):
  a(1),
  z(1),
  charge(1),
  energy(0),
  overrideCharge(false),
  nElectrons(0)
{
  Parse(definition);

  if (a < z)
    {
      G4String message("Invalid ion definition: \"" + definition + "\" -> A is less than Z");
      throw BDSException(__METHOD_NAME__, message);
    }
  if (charge < 0)
    {G4cout << __METHOD_NAME__ << "Using ion with -ve charge -> implies at least 1 extra electron." << G4endl;}
}

BDSIonDefinition::BDSIonDefinition(G4int    aIn,
				   G4int    zIn,
				   G4double qIn):
  a(aIn),
  z(zIn),
  charge(qIn),
  energy(0),
  overrideCharge(false),
  nElectrons(0)
{
  if (a < z)
    {
      G4String message("Invalid ion definition: A is less than Z");
      throw BDSException(__METHOD_NAME__, message);
    }
  if (charge > z)
    {
        G4String message("Invalid ion definition: Charge is greater than Z");
        throw BDSException(__METHOD_NAME__, message);
    }
  if (z != charge)
    {
      nElectrons = z - (G4int)charge;
      G4cout << __METHOD_NAME__ << nElectrons << " bound electrons to ion inferred from charge, A and Z." << G4endl;
    }
  if (charge < 0)
    {G4cout << __METHOD_NAME__ << "Using ion with -ve charge -> implies at least 1 extra electron." << G4endl;}
}

std::ostream& operator<< (std::ostream& out, BDSIonDefinition const& io)
{
  out << "A: " << io.a << " Z: " << io.z << " Q: " << io.charge << " E: "
      << io.energy << G4endl;
  return out;
}

void BDSIonDefinition::Parse(const G4String& definition)
{
  std::regex wspace("\\s+"); // any whitespace

  // A Z Q E
  G4int counter = 0;
  std::vector<G4int*> vals = {&a, &z};
  // split on whitespace - "-1" here means split on the gap, not the token, ie the word not the whitespace
  std::sregex_token_iterator wordsBegin(definition.begin(), definition.end(), wspace, -1);
  std::sregex_token_iterator wordsEnd;
  for (auto i = wordsBegin; i != wordsEnd; ++i, ++counter)
    {
      if (counter == 0)
        {continue;}// the first 'word' is expected to be "ion"
      std::string word = (*i).str();
      if (counter == 3) // ie > 2
	{overrideCharge = true;} // charge is specified
      try
	{
	  if (counter > 2)
	    {// Q or E are doubles
	      G4double value = std::stod(word);
	      if (counter == 4)
		{energy = value*CLHEP::keV;}
	      else
		{charge = value*CLHEP::eplus;}
	    }
	  else
	    {// A and Z are integers
	      // check for decimal point -> should not be a floating point number
	      if (word.find(".") != std::string::npos)
		{throw BDSException(__METHOD_NAME__, "value in beam ion definition \"" + word + "\" must be an integer");}
	      (*vals[counter-1]) = std::stoi(word);
	      if (counter == 2) // by default copy Z as value of Q
		{charge = (G4double)*vals[counter-1] * CLHEP::eplus;}
	    }
	}
      catch (const std::invalid_argument&) // if stod/i can't convert number to double / int
	{throw BDSException(__METHOD_NAME__, "Invalid ion definition " + definition );}
    }

  if (z != charge)
    {
      nElectrons = z - charge;
      G4cout << __METHOD_NAME__ << nElectrons << " bound electrons to ion inferred from charge, A and Z." << G4endl;
    }
  if (charge > z)
    {
      G4String message("Invalid ion definition: Charge is greater than Z");
      throw BDSException(__METHOD_NAME__, message);
    }
    auto dummy = 1;
}
