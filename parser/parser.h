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
#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "aperture.h"
#include "atom.h"
#include "beam.h"
#include "blmplacement.h"
#include "cavitymodel.h"
#include "newcolour.h"
#include "coolingchannel.h"
#include "crystal.h"
#include "element.h"
#include "elementtype.h"
#include "field.h"
#include "fastlist.h"
#include "material.h"
#include "modulator.h"
#include "options.h"
#include "parameters.h"
#include "physicsbiasing.h"
#include "placement.h"
#include "query.h"
#include "region.h"
#include "scorer.h"
#include "scorermesh.h"
#include "samplerplacement.h"
#include "symbolmap.h"
#include "tunnel.h"

/// parser error message, defined in parser.y
int yyerror(const char *);
int yyerror2(const char *s); ///< Similar but without advice about semicolon
/// declaration needed by bison
extern int yylex();

/** 
 * @brief Parser namespace for GMAD language. Combination of Geant4 and MAD.
 *
 * @author Jochem Snuverink
 */

namespace GMAD
{
  class Array;
  class Symtab;
  /**
   * @brief Parser class
   * 
   * Parser class that holds all objects and relevant methods
   *
   * Singleton pattern
   *
   * @author Jochem Snuverink
   */
  
  class Parser
  {
  public:
    /// No default constructor
    Parser() = delete;
    /// Constructor method
    static Parser* Instance(const std::string& filename);
    /// Access method
    static Parser* Instance();
    /// Destructor
    virtual ~Parser();

  protected:
    /// Constructor from filename
    explicit Parser(std::string filename);
  private:
    /// Instance
    static Parser* instance;
    /// Initialisation of parser functions and constants
    void Initialise();
    /// Parse the input file and construct beamline_list and options 
    void ParseFile(FILE *f);

  public:
    /// Exit method
    void quit();
    /// Method that transfers parameters to element properties
    void write_table(std::string* name, ElementType type, bool isLine=false);

    /// Expand a sequence by name from start to end into the target list. This
    /// removes sublines from the beamline into one LINE.
    void expand_line(FastList<Element>& target,
                     const std::string& name,
                     const std::string& start = "",
                     const std::string& end   = "");

    /// Expand the main beamline as defined by the use command.
    void expand_line(const std::string& name,
                     const std::string& start,
                     const std::string& end);

    /// Find the sequence defined in the parser and expand it if not already
    /// done so. Cache result in map of fastlists.
    const FastList<Element>& get_sequence(const std::string& name);
  
    /// Add a particle set for a sampler and return a unique integer ID for that set. If no list
    /// or empty list given, returns -1, the default for 'no filter'.
    int add_sampler_partIDSet(std::list<int>* samplerPartIDListIn);
    /// insert a sampler into beamline_list
    void add_sampler(const std::string& name, int count, ElementType type, std::string samplerType, std::list<int>* samplerPartIDListIn = nullptr);
    /// Insert global object of parser class C in Container class
    template <class C, class Container=FastList<C>>
    void Add();
    template <class C, class Container=FastList<C>>
    void Add(bool unique, const std::string& className);
    /// Get global object of parser class C
    template <class C>
    C& GetGlobal();
    /// Get list for parser class C
    template <class C, class Container=FastList<C>>
    Container& GetList();
  
    const std::set<std::set<int>>& GetSamplerFilters() const {return samplerFilters;}
    const std::map<int, std::set<int>>& GetSamplerFilterIDToSet() const {return samplerFilterIDToSet;}

    /// find element
    Element& find_element(const std::string& element_name);
    /// find element (const) 
    const Element& find_element(const std::string& element_name) const;
    /// find element by pointer - nullptr if not found - searches element_list
    const Element* find_element_safe(const std::string& element_name) const;
    /// search placement_element
    const Element* find_placement_element_safe(const std::string& element_name) const;
    /// access property of Element with element_name
    double property_lookup(const std::string& element_name, const std::string& property_name) const;
    /// add element to temporary element sequence tmp_list
    void add_element_temp(const std::string& name, int number, bool pushfront, ElementType linetype);
    /// copy properties from Element into params, returns element type as integer, returs _NONE if not found
    int copy_element_to_params(const std::string& elementName);

    bool InvalidSymbolName(const std::string& s, std::string& errorReason);
    /// create new parser symbol
    Symtab * symcreate(const std::string& s);
    /// look up parser symbol
    Symtab * symlook(const std::string& s);

    ///@{ Add value to front of temporary list
    void Store(double value);
    void Store(const std::string& name);
    ///@}
    ///@{ Fill array object from temporary list and clear temporary list
    void FillArray(Array*);
    void FillString(Array*);
    ///@}
    /// Reset parameters
    void ClearParams();
    /// Set value for parser class
    template <class C, typename T>
    void SetValue(std::string property, T value);
    /// Get value for parser class (only for doubles)
    template <class C>
    double GetValue(std::string property);
    
    template<typename T>
    std::list<T>* ArrayToList(Array*);

    /// Add value to be extended to object
    template <typename T>
    void ExtendValue(const std::string& property, T value);

    /// Overwrite object with current values
    void Overwrite(const std::string& objectName);
    /// Add variable memory to variable list for memory management
    void AddVariable(std::string* name);
    ///@{ Print methods
    void PrintBeamline()const;
    void PrintElements()const;
    void PrintOptions()const;
    ///@}
    /// Search each member vector for an object with the matching name.
    /// Return true if successfully printed.
    bool TryPrintingObject(const std::string& objectName) const;
    
    ///@{ Name of beamline
    std::string current_line;
    std::string current_start;
    std::string current_end;
    ///@}
    /// Beamline Access.
    const FastList<Element>& GetBeamline() const;
    
  private:
    /// Set sampler
    void set_sampler(const std::string& name,
                     int count, ElementType type,
                     const std::string& samplerType,
                     double samplerRadius=0,
                     int particleSetID = -1);
    /// Add function to parser
    void add_func(std::string name, double (*func)(double));
    void add_var(std::string name, double value, int is_reserved = 0);

    /// Expand all sequences define with 'line' into FastLists.
    void expand_sequences();

    // protected implementation (for inheritance to BDSParser - hackish)
  protected:
    /// Beam instance;
    Beam beam;
    /// General options
    Options options;
    /// Beamline
    FastList<Element>   beamline_list;
    /// @{ List of parser defined instances of that object.
    FastList<Atom>   atom_list;
    FastList<NewColour> colour_list;
    FastList<Crystal> crystal_list;
    FastList<CoolingChannel> coolingchannel_list;
    FastList<Field>  field_list;
    FastList<Material> material_list;
    FastList<Query> query_list;
    FastList<Region> region_list;
    FastList<Tunnel> tunnel_list;
    FastList<PhysicsBiasing> xsecbias_list;
    FastList<Placement> placement_list;
    FastList<CavityModel> cavitymodel_list;
    FastList<SamplerPlacement> samplerplacement_list;
    FastList<Scorer> scorer_list;
    FastList<ScorerMesh> scorermesh_list;
    FastList<Aperture> aperture_list;
    FastList<BLMPlacement> blm_list;
    FastList<Modulator> modulator_list;
    /// @}

  private:
    // *****************
    // Private members *
    // *****************
    /// maximum number of nested lines
    const int MAX_EXPAND_ITERATIONS = 50;

    ///@{ temporary list for reading of arrays in parser
    std::list<double> tmparray;
    std::list<std::string> tmpstring;
    ///@}
    /// vector of defined lines for memory management
    std::vector<std::list<Element>*> allocated_lines;

    /// @{ The one instance we fill before appending to a list.
    Parameters params;
    Atom atom;
    NewColour colour;
    Crystal crystal;
    CoolingChannel coolingchannel;
    Field field;
    Material material;
    PhysicsBiasing xsecbias;
    Placement placement;
    Query query;
    Region region;
    Tunnel tunnel;
    CavityModel cavitymodel;
    SamplerPlacement samplerplacement;
    Scorer scorer;
    ScorerMesh scorermesh;
    Aperture aperture;
    BLMPlacement blm;
    Modulator modulator;
    /// @}
    
    /// Find object by name in list
    template <class C>
    bool FindAndExtend(const std::string& objectName);
    /// Extend object with maps
    template <class C>
    void ExtendObject(C& object);

    /// Map for options of type double for extending objects
    std::map<std::string, double> extendedNumbers;
    /// Map for options of type string for extending objects
    std::map<std::string, std::string> extendedStrings;
    /// Map for options of type vector for extending objects
    std::map<std::string, Array*> extendedVectors;

    /// List of all encountered elements
    FastList<Element> element_list;
    
    /// List of element definitions that are used in placements - keep separately a copy
    /// so that when we clear all the lists after expanding the lines we still have the
    /// element definitions we need
    FastList<Element> placement_elements;
    
    /// Temporary list
    std::list<Element> tmp_list;
    
    /// Names of all defined sequences in the parser with 'line'.
    std::vector<std::string> sequences;

    /// Cached copy of expanded sequences.
    std::map<std::string, FastList<Element>*> expandedSequences;

    /// Parser symbol map
    SymbolMap symtab_map;
    /// Variable vector for memory storage
    std::vector<std::string*> var_list;
    
    /// Set of unique sets of particle IDs. This will allow us to build up unique
    /// Sensitive detectors for particles later on.
    std::set<std::set<int>> samplerFilters;
    std::map<int, std::set<int>> samplerFilterIDToSet;
    std::map<std::set<int>, int> setToSamplerFilterID;
  };

  template <class C, typename T>
  void Parser::SetValue(std::string property, T value)
  {
    GetGlobal<C>().set_value(property, value);
  }
  
  template <class C>
  double Parser::GetValue(std::string property)
  {
    return GetGlobal<C>().get_value(property);
  }
  
  template<typename T>
  std::list<T>* Parser::ArrayToList(Array* arrayIn)
  {
    if (!arrayIn)
      {return nullptr;}
    else
    {
      std::list<T>* result = new std::list<T>();
      const auto& doubleData = arrayIn->GetDataList();
      for (auto& value : doubleData)
      {result->push_back((T)value);}
      return result;
    }
  }
}

#endif
