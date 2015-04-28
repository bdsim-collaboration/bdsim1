#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
#include "BDSTunnelFactory.hh"
#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactoryElliptical.hh"
#include "BDSTunnelType.hh"

#include "globals.hh"                        // geant4 globals / types

BDSTunnelFactory* BDSTunnelFactory::_instance = 0;

BDSTunnelFactory* BDSTunnelFactory::Instance()
{
  if (_instance == 0)
    {_instance = new BDSTunnelFactory();}
  return _instance;
}

BDSTunnelFactory::BDSTunnelFactory()
{;}

BDSTunnelFactoryBase* BDSTunnelFactory::GetAppropriateFactory(BDSTunnelType tunnelType)
{
  switch(tunnelType.underlying()){

  case BDSTunnelType::elliptical:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " circular beampipe factory" << G4endl;
#endif
    return BDSTunnelFactoryElliptial::Instance();
    break;
  default:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "unknown tunnel type \"" << tunnelType
	   << "\" - elliptical tunnel factory by default" << G4endl;
#endif
    return BDSTunnelFactoryElliptical::Instance();
    break;
  }
}
  
BDSGeometryComponent* BDSTunnelFactory::CreateTunnelSection(BDSTunnelType tunnelType,
							    G4String      name,
							    G4double      length,
							    G4double      tunnelThickness,
							    G4double      tunnelSoilThickness,
							    G4Material*   tunnelMaterial,
							    G4Material*   tunnelSoilMaterial,
							    G4bool        tunnelFloor,
							    G4double      tunnelFloorOffset,
							    G4double      tunnel1,
							    G4double      tunnel2);
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSection(name, length, tunnelThickness,
				      tunnelSoilThickness, tunnelMaterial,
				      tunnelSoilMaterial, tunnelFloor,
				      tunnelFloorOffset, tunnel1, tunnel2);
}

BDSGeometryComponent* BDSTunnelFactory::CreateTunnelSectionAngledIn(BDSTunnelType tunnelType,
								    G4String      name,
								    G4double      length,
								    G4double      angleIn,
								    G4double      tunnelThickness,
								    G4double      tunnelSoilThickness,
								    G4Material*   tunnelMaterial,
								    G4Material*   tunnelSoilMaterial,
								    G4bool        tunnelFloor,
								    G4double      tunnelFloorOffset,
								    G4double      tunnel1,
								    G4double      tunnel2);
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSection(name, length, angleIn, tunnelThickness,
				      tunnelSoilThickness, tunnelMaterial,
				      tunnelSoilMaterial, tunnelFloor,
				      tunnelFloorOffset, tunnel1, tunnel2);
}

BDSGeometryComponent* BDSTunnelFactory::CreateTunnelSectionAngledOut(BDSTunnelType tunnelType,
								     G4String      name,
								     G4double      length,
								     G4double      angleOut,
								     G4double      tunnelThickness,
								     G4double      tunnelSoilThickness,
								     G4Material*   tunnelMaterial,
								     G4Material*   tunnelSoilMaterial,
								     G4bool        tunnelFloor,
								     G4double      tunnelFloorOffset,
								     G4double      tunnel1,
								     G4double      tunnel2);
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSection(name, length, angleOut, tunnelThickness,
				      tunnelSoilThickness, tunnelMaterial,
				      tunnelSoilMaterial, tunnelFloor,
				      tunnelFloorOffset, tunnel1, tunnel2);
}

BDSGeometryComponent* BDSTunnelFactory::CreateTunnelSectionAngledInOut(BDSTunnelType tunnelType,
								       G4String      name,
								       G4double      length,
								       G4double      angleIn,
								       G4double      angleOut,
								       G4double      tunnelThickness,
								       G4double      tunnelSoilThickness,
								       G4Material*   tunnelMaterial,
								       G4Material*   tunnelSoilMaterial,
								       G4bool        tunnelFloor,
								       G4double      tunnelFloorOffset,
								       G4double      tunnel1,
								       G4double      tunnel2);
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSection(name, length, angleIn, angleOut,
				      tunnelThickness, tunnelSoilThickness, tunnelMaterial,
				      tunnelSoilMaterial, tunnelFloor,
				      tunnelFloorOffset, tunnel1, tunnel2);
}
