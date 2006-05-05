/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed StringFromInt to be BDSGlobals Version
*/
#include "BDSGlobalConstants.hh" // must be first in include list

#include "BDSQuadrupole.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"

#include <map>

const int DEBUG = 0;


//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

extern BDSMaterials* theMaterials;
//============================================================

BDSQuadrupole::BDSQuadrupole(G4String aName,G4double aLength, 
			     G4double bpRad,G4double FeRad,
			     G4double bGrad, G4double tilt, G4double outR):
  BDSMultipole(aName,aLength, bpRad, FeRad,SetVisAttributes()),
  itsBGrad(bGrad)
{
  SetOuterRadius(outR);
 
  itsType="quad";

  itsTilt = tilt;
  
  if (!(*LogVolCount)[itsName])
    {
      BuildBPFieldAndStepper();
      BuildBPFieldMgr(itsStepper,itsMagField);
      BuildDefaultMarkerLogicalVolume();
	  
      BuildBeampipe(itsLength);
      BuildDefaultOuterLogicalVolume(itsLength);

      SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
      //SetSensitiveVolume(itsBeampipeLogicalVolume);// for synchrotron
      //SetSensitiveVolume(itsOuterLogicalVolume);// for laserwire

      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(0., 0., 0));
      VisAtt->SetForceSolid(true);
      itsInnerBPLogicalVolume->SetVisAttributes(VisAtt);
      
      G4VisAttributes* VisAtt1 = 
	new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
      VisAtt1->SetForceSolid(true);
      itsBeampipeLogicalVolume->SetVisAttributes(VisAtt1);
      
      G4VisAttributes* VisAtt2 = 
	new G4VisAttributes(G4Colour(1., 0., 0.));
      VisAtt2->SetForceSolid(true);
      itsOuterLogicalVolume->SetVisAttributes(VisAtt2);
      
      if(BDSGlobals->GetIncludeIronMagFields())
	{
	  G4double polePos[4];
	  G4double Bfield[3];

	  polePos[0]=-BDSGlobals->GetMagnetPoleRadius()/sqrt(2.0);
	  polePos[1]=BDSGlobals->GetMagnetPoleRadius()/sqrt(2.0);
	  polePos[2]=0.;
	  polePos[3]=-999.;//flag to use polePos rather than local track
	                   //coordinate in GetFieldValue
	    
	    
	  itsMagField->GetFieldValue(polePos,Bfield);
	  G4double BFldIron=
	    sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	    BDSGlobals->GetMagnetPoleSize()/
	    (BDSGlobals->GetComponentBoxSize()/2-
	     BDSGlobals->GetMagnetPoleRadius());
	  // Magnetic flux from a pole is divided in two directions
	  BFldIron/=2.;

	  BuildOuterFieldManager(4, BFldIron,pi/4);
	}
	  
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
      if(BDSGlobals->GetSynchRadOn()&& BDSGlobals->GetSynchRescale())
	{
	  // with synchrotron radiation, the rescaled magnetic field
	  // means elements with the same name must have different
	  //logical volumes, becuase they have different fields
	  itsName+=BDSGlobals->StringFromInt((*LogVolCount)[itsName]);
	  BuildBPFieldAndStepper();
	  BuildBPFieldMgr(itsStepper,itsMagField);
	  BuildDefaultMarkerLogicalVolume();
	  
	  BuildBeampipe(itsLength);
	  BuildDefaultOuterLogicalVolume(itsLength);

	  SetSensitiveVolume(itsBeampipeLogicalVolume);// for synchrotron
	  //SetSensitiveVolume(itsOuterLogicalVolume);// for laserwire      
	  (*LogVol)[itsName]=itsMarkerLogicalVolume;
	}
      else
	{
	  itsMarkerLogicalVolume=(*LogVol)[itsName];
	}      
    }


}
  
void BDSQuadrupole::SynchRescale(G4double factor)
{
  itsStepper->SetBGrad(factor*itsBGrad);
  itsMagField->SetBGrad(factor*itsBGrad);
  if(DEBUG) G4cout << "Quad " << itsName << " has been scaled" << G4endl;
}


G4VisAttributes* BDSQuadrupole::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,0,0));
  return itsVisAttributes;
}


void BDSQuadrupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  //  G4ThreeVector Bfield(0.,0.,0.);
  //  itsMagField=new G4UniformMagField(Bfield);
  itsMagField=new BDSQuadMagField(itsBGrad);

  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  
  itsStepper=new BDSQuadStepper(itsEqRhs);
  itsStepper->SetBGrad(itsBGrad);
  itsStepper->SetVolLength(itsLength);
}

BDSQuadrupole::~BDSQuadrupole()
{
  delete itsVisAttributes;
  delete itsMarkerLogicalVolume;
  delete itsOuterLogicalVolume;
  delete itsPhysiComp;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
