// Geometry Interface Class
// ========================
//
// A class of functions to output Geant4/Mokka/BDSIM parameters for elements
// - to include geometry, optics, fields, etc.

#include "BDSGlobalConstants.hh"
#include "BDSGeometryInterface.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <string>

using std::setw;

BDSGeometryInterface::BDSGeometryInterface(G4String filename):
  itsFileName(filename)
{}

BDSGeometryInterface::~BDSGeometryInterface()
{}

void BDSGeometryInterface::Optics()
{
  std::ofstream optics;
  
  G4cout << "Generating Optics file to: " << itsFileName << " ..." << G4endl;

  optics.open(itsFileName);

  time_t currenttime;
  time(&currenttime);
  std::string timestring = asctime(localtime(&currenttime));
  timestring = timestring.substr(0,timestring.size()-1);

  optics << "### BDSIM output - created "<< timestring << G4endl;
  optics << std::left
	 << setw(15) << "Type       "
         << setw(40) << "Name       " 
	 << setw(15) << "Length[m]  " 
	 << setw(15) << "S[m]       " 
	 << setw(15) << "Angle[rad] " 
	 << setw(15) << "K1[m^-2]   " 
	 << setw(15) << "K2[m^-3]   " 
	 << setw(15) << "K3[m^-4]   "
	 << setw(15) << "TILT       "
	 << setw(15) << "AperX[m]   "
	 << setw(15) << "AperY[m]   "
	 << setw(15) << "Aper_Type  "
	 << G4endl;

  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    { 
      BDSAcceleratorComponent* thecurrentitem = BDSBeamline::Instance()->currentItem();
      G4int aper_type; //1 = rect, 2 = circ, 3 = ellipse
      if(thecurrentitem->GetType() == "rcol" ) //RCOL
	aper_type=1;
      else if(thecurrentitem->GetType() == "ecol") //ECOL
	{
	  if(thecurrentitem->GetAperX()==thecurrentitem->GetAperY()) 
	    aper_type=2;
	  else aper_type=3;
	}
      else aper_type=2; // circular is default
      
      optics.setf(std::ios::fixed, std::ios::floatfield);
      optics.setf(std::ios::showpoint);
      
      optics.precision(8);
      
      optics << std::left 
	     << setw(15) << thecurrentitem->GetType() << " "
	     << setw(40) << thecurrentitem->GetName() << " "
	     << setw(15) << thecurrentitem->GetChordLength()/CLHEP::m  << " "
	     << setw(15) << BDSBeamline::Instance()->positionS()/CLHEP::m  << " "
	     << setw(15) << thecurrentitem->GetAngle()   << " "
	     << setw(15) << thecurrentitem->GetK1()   << " "
	     << setw(15) << thecurrentitem->GetK2()   << " "
	     << setw(15) << thecurrentitem->GetK3()   << " "
	     << setw(15) << thecurrentitem->GetTilt() << " "
	     << setw(15) << thecurrentitem->GetAperX()/CLHEP::m   << " "
	     << setw(15) << thecurrentitem->GetAperY()/CLHEP::m   << " "
	     << setw(15) << aper_type   << " "
	     << G4endl;
	}
      optics.close();

}


void BDSGeometryInterface::Survey()
{
  std::ofstream survey;

  G4cout << "Generating Survey: " << itsFileName << " ..." << G4endl;

  survey.open(itsFileName);

  time_t currenttime;
  time(&currenttime);
  std::string timestring = asctime(localtime(&currenttime));
  timestring = timestring.substr(0,timestring.size()-1);

  survey << "### BDSIM output - created "<< timestring << G4endl;
  survey << std::left 
	 << setw(15) << "Type        " << " "
	 << setw(40) << "Name        " << " "
	 << setw(12) << "SStart[m]   " << " "
	 << setw(12) << "SMid[m]     " << " "
	 << setw(12) << "SEnd[m]     " << " "
	 << setw(12) << "Chord_len[m]" << " "
	 << setw(12) << "Arc_len[m]  " << " "
	 << setw(12) << "X[m]        " << " "
	 << setw(12) << "Y[m]        " << " "
	 << setw(12) << "Z[m]        " << " "
	 << setw(12) << "Phi[rad]    " << " "
	 << setw(12) << "Theta[rad]  " << " "
	 << setw(12) << "Psi[rad]    " << " "
	 << setw(12) << "AperX[m]    " << " "
	 << setw(12) << "AperY[m]    " << " "
	 << setw(8)  << "Aper_Type   " << " "
	 << setw(12) << "Angle[rad]  " << " "
	 << setw(12) << "K1[m^-2]    " << " "
	 << setw(12) << "K2[m^-3]    " << " "
	 << setw(12) << "K3[m^-4]    " << " "
	 << G4endl;
  
  G4double lengthTotal(0.0);
  G4double arc_lengthTotal(0.0);
  G4double length(0);
  G4double spos(0);
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    {
      BDSAcceleratorComponent* thecurrentitem = BDSBeamline::Instance()->currentItem();
      G4int aper_type; //1 = rect, 2 = circ, 3 = ellipse
      if(thecurrentitem->GetType() == "rcol" ) //RCOL
	aper_type=1;
      else if(thecurrentitem->GetType() == "ecol" ) //ECOL
	if(thecurrentitem->GetAperX()==thecurrentitem->GetAperY()) 
	  aper_type=2;
	else aper_type=3;
      else aper_type=1;
      
      //G4double phi=0.0, theta=0.0, psi=0.0;
      G4double phi   = BDSBeamline::Instance()->rotation()->getPhi();
      G4double theta = BDSBeamline::Instance()->rotation()->getTheta();
      G4double psi   = BDSBeamline::Instance()->rotation()->getPsi();
      /*
      if(thecurrentitem->GetRotation())
	{
	  phi = thecurrentitem->GetRotation()->getPhi();
	  theta = thecurrentitem->GetRotation()->getTheta();
	  psi = thecurrentitem->GetRotation()->getPsi();
	}
      */
      survey.setf(std::ios::fixed, std::ios::floatfield);
      survey.setf(std::ios::showpoint);
      
      survey.precision(7);
      
      length = thecurrentitem->GetArcLength()/CLHEP::m;
      spos   = thecurrentitem->GetSPos()/CLHEP::m;
      
      survey << std::left << std::setprecision(6) << std::fixed
	     << setw(15) << thecurrentitem->GetType()                   << " "
	     << setw(40) << thecurrentitem->GetName()                   << " "
	     << setw(12) << spos - (length/2.0)                         << " " /*SStart*/
	     << setw(12) << spos                                        << " " /*SMid*/
	     << setw(12) << spos + (length/2.0)                         << " " /*SEnd*/
	     << setw(12) << thecurrentitem->GetChordLength()/CLHEP::m   << " "
	     << setw(12) << thecurrentitem->GetArcLength()/CLHEP::m     << " "
	     << setw(12) << BDSBeamline::Instance()->position()->x()/CLHEP::m  << " "
	     << setw(12) << BDSBeamline::Instance()->position()->y()/CLHEP::m  << " "
	     << setw(12) << BDSBeamline::Instance()->position()->z()/CLHEP::m  << " "
	     << setw(12) << phi/CLHEP::radian                           << " "
	     << setw(12) << theta/CLHEP::radian                         << " "
	     << setw(12) << psi/CLHEP::radian                           << " "
	     << setw(12) << thecurrentitem->GetAperX()/CLHEP::m         << " "
	     << setw(12) << thecurrentitem->GetAperY()/CLHEP::m         << " "
	     << setw(8)  << aper_type                                   << " "
	     << setw(12) << thecurrentitem->GetAngle()                  << " "
	     << setw(12) << thecurrentitem->GetK1()                     << " "
	     << setw(12) << thecurrentitem->GetK2()                     << " "
	     << setw(12) << thecurrentitem->GetK3()                     << " "
	     << G4endl;
      lengthTotal+=thecurrentitem->GetChordLength()/CLHEP::m;
      arc_lengthTotal+=thecurrentitem->GetArcLength()/CLHEP::m;
    }
  survey << "### Total length = " << lengthTotal << "m" << G4endl;
  survey << "### Total arc length = " <<  arc_lengthTotal << "m" << G4endl;
  survey.close();
}
