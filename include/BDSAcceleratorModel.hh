#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "BDSBeamlineSet.hh"

#include "globals.hh"         // geant4 globals / types

#include <map>
#include <vector>

class BDSBeamline;
class BDSFieldObjects;
class G4LogicalVolume;
class G4ProductionCuts;
class G4Region;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief A holder class for all representations of the
 * accelerator model created in BDSIM. 
 *
 * This can be extended to 
 * allow inspection of the model. Holds the readout geometry
 * physical world in a location independent of detector 
 * construction.
 * 
 * In future, there may be several more beamlines than just
 * a flat one, and perhaps grouped into a more hierarchical
 * version.  These can be contained here and this class can
 * be extended as required.
 * 
 * @author Laurie Nevay
 */

class BDSAcceleratorModel
{
public:
  static BDSAcceleratorModel* Instance();
  ~BDSAcceleratorModel();

  /// @{ Register consituent of world.
  inline void RegisterWorldPV(G4VPhysicalVolume* worldIn) {worldPV = worldIn;}
  inline void RegisterWorldLV(G4LogicalVolume*   worldIn) {worldLV = worldIn;}
  inline void RegisterWorldSolid(G4VSolid*       worldIn) {worldSolid = worldIn;}
  /// @}
  
  /// Access the physical volume of the world
  inline G4VPhysicalVolume* GetWorldPV() const {return worldPV;}

  /// Register the flat beam line - flat means that each element in the beamline represents
  /// one element in the accelerator lattice
  inline void RegisterFlatBeamline(BDSBeamline* beamlineIn) {flatBeamline = beamlineIn;}

  /// Access flat beam line
  inline BDSBeamline* GetFlatBeamline() const {return flatBeamline;}

  /// Register the curvilinear geometry beam line.
  inline void RegisterCurvilinearBeamline(BDSBeamline* beamlineIn)
  {curvilinearBeamline = beamlineIn;}

  /// Register the curvilinear bridging geometry beam line.
  inline void RegisterCurvilinearBridgeBeamline(BDSBeamline* beamlineIn)
  {curvilinearBridgeBeamline = beamlineIn;}

  /// @{ Accessor.
  inline BDSBeamline* GetCurvilinearBeamline() const {return curvilinearBeamline;}
  inline BDSBeamline* GetCurvilinearBridgeBeamline() const {return curvilinearBridgeBeamline;}
  /// @}
  
  /// Register the beam line containing all the magnet supports
  inline void RegisterSupportsBeamline(BDSBeamline* beamlineIn) {supportsBeamline = beamlineIn;}

  /// Access the beam line containing all the magnet supports
  inline BDSBeamline* GetSupportsBeamline() const {return supportsBeamline;}

  /// Register the beam line containing all the tunnel segments
  inline void RegisterTunnelBeamline(BDSBeamline* beamlineIn) {tunnelBeamline = beamlineIn;}

  /// Access the beam line containing all the tunnel segments
  inline BDSBeamline* GetTunnelBeamline() const {return tunnelBeamline;}

  /// Register the beam line of end pieces.
  inline void RegisterEndPieceBeamline(BDSBeamline* beamlineIn) {endPieceBeamline = beamlineIn;}

  /// Access the beam line of end pieces.
  inline BDSBeamline* GetEndPieceBeamline() const {return endPieceBeamline;}

  /// Register a 'beam line' of discontinuous elements for placement in the world as
  /// separate placement objects.
  inline void RegisterPlacementBeamline(BDSBeamline* beamlineIn) {placementBeamline = beamlineIn;}

  /// Access a discontinuous beam line of placements.
  inline BDSBeamline* GetPlacementBeamline() const {return placementBeamline;}
  
  /// Register all field objects
  inline void RegisterFields(std::vector<BDSFieldObjects*>& fieldsIn){fields = fieldsIn;}

  /// Register a region and associated production cut as G4Region doesn't seem to delete
  /// it - note, no checking for double registration.
  void RegisterRegion(G4Region* region, G4ProductionCuts* cut);

  /// Register a temporary file for possible deletion at the deletion of the accelerator model
  /// based on the option from BDSGlobalConstants.
  void RegisterTemporaryFile(G4String fileName);

  /// Register a set of beam lines to be managed and cleared up at the end of the simulation.
  void RegisterExtraBeamline(BDSBeamlineSet set);

  /// Access the vector of extra beam line sets.
  inline const std::vector<BDSBeamlineSet>& ExtraBeamlines() const {return extraBeamlines;}

  /// Access region information. Will exit if not found.
  G4Region*         Region(G4String name) const;

  /// Simpler accessor for production cuts vs regions.
  G4ProductionCuts* ProductionCuts(G4String name) {return cuts.at(name);}

private:
  BDSAcceleratorModel(); ///< Default constructor is private as singleton.

  static BDSAcceleratorModel* instance;

  G4bool removeTemporaryFiles;

  G4VPhysicalVolume* worldPV;              ///< Physical volume of the mass world.
  G4LogicalVolume*   worldLV;
  G4VSolid*          worldSolid;

  BDSBeamline* flatBeamline;              ///< Flat beam line.
  BDSBeamline* curvilinearBeamline;       ///< Curvilinear geometry beamline.
  BDSBeamline* curvilinearBridgeBeamline; ///< Curvilinear bridging volumes beamline.
  BDSBeamline* supportsBeamline;          ///< Element supports beam line.
  BDSBeamline* tunnelBeamline;            ///< Tunnel segments beam line.
  BDSBeamline* endPieceBeamline;          ///< End Pieces beam line.
  BDSBeamline* placementBeamline;         ///< Placement geometry beam line.

  std::vector<BDSBeamlineSet> extraBeamlines; ///< Extra beamlines.

  std::vector<BDSFieldObjects*> fields;       ///< All field objects.
  std::map<G4String, G4Region*> regions;      ///< All regions.
  std::map<G4String, G4ProductionCuts*> cuts; ///< Cuts corresponding to the regions.
  std::vector<G4String> temporaryFiles;       ///< All temporary file paths.
};

#endif
