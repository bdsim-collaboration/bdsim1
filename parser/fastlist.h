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
#ifndef FASTLIST_H
#define FASTLIST_H

#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

namespace GMAD
{
  /**
   * @brief List with Efficient Lookup
   *
   * This class keeps a list of elements
   * It has efficient lookup on an element's name (log n) by having a multimap between name and list position
   *
   * Used for beamline
   *
   * @author Jochem Snuverink
   */
  template<typename T>
    class FastList {

  public:
    /// for ease of reading
    using FastListIterator         = typename std::list<T>::iterator;
    using FastListConstIterator    = typename std::list<T>::const_iterator;
    using FastMapIterator          = typename std::multimap<std::string, FastListIterator>::iterator;
    using FastMapConstIterator     = typename std::multimap<std::string, FastListIterator>::const_iterator;
    using FastMapIteratorPair      = std::pair<FastMapIterator,FastMapIterator>;
    using FastMapConstIteratorPair = std::pair<FastMapConstIterator,FastMapConstIterator>;
    ///@{
    /// insert options (classic list inserts):
    /// inputiterator templated to account for reverse iterators
    template <typename FastListInputIterator>
      FastListIterator insert(FastListInputIterator position, const T& val);
    template <typename FastListInputIterator>
      void insert (FastListConstIterator position, FastListInputIterator first, const FastListInputIterator last);
    ///@}
    /// insert element before all elements with given name
    /// exits if no element with name
    void insert_before(const std::string& name, const T& val);

    /// insert element at end of list
    /// option to check for unique element name (exits in case name is not unique), false by default
    void push_back(const T& el, bool unique=false, const std::string& objectName="element");

    inline bool empty() const {return itsList.empty();} ///< Whether the list is empty.

    /// size of list
    int size()const;
    /// empty lists
    void clear();
    ///@{ erase elements
    void erase();
    FastListConstIterator erase (const FastListConstIterator position);
    FastListConstIterator erase (const FastListConstIterator first, const FastListConstIterator last);
    ///@}
    ///@{ non-const begin/end iterator:
    FastListIterator begin();
    FastListIterator end();
    ///@}
    ///@{ const begin/end iterator:
    FastListConstIterator begin()const;
    FastListConstIterator end()const;
    ///@}
    
    /// Get a vector version of this list.
    std::vector<T> getVector() const {return std::vector<T>(begin(), end());}

    /// Get a map of name to object of this list.
    std::map<std::string, T> getMap() const;

    ///@{ lookup method, returns iterator of list pointing to element with name
    /// second argument is instance number, default first instance
    /// both const and non-const version
    FastListConstIterator find(std::string name,unsigned int count=1)const;
    FastListIterator find(std::string name,unsigned int count=1);
    ///@}
    /// lookup method, returns pair of iterators of list pointing (similar to std::multimap::equal_range)
    FastMapIteratorPair equal_range(std::string name);
    FastMapConstIteratorPair equal_range(std::string name)const;

    /// print method
    void print(int ident=0)const;

  private:
    /// the element list
    /// a list is chosen since insertion is fast and iterators are not invalidated
    typename std::list<T> itsList;
    /// multimap for name lookup
    std::multimap<std::string, FastListIterator> itsMap;
  };

  /// template definitions need to be in header
  template <typename T>
    template <typename FastListInputIterator>
    typename FastList<T>::FastListIterator FastList<T>::insert(FastListInputIterator position, const T& val) {
    // insert in list
    FastListIterator it = itsList.insert(position,val);
    // insert iterator in map with key name
    itsMap.insert(std::pair<std::string,FastListIterator>(val.name,it));
    return it;
  }

  template <typename T>
    template <typename FastListInputIterator>
    void FastList<T>::insert(FastListConstIterator position, FastListInputIterator first, const FastListInputIterator last) {
    for (;first!=last; ++first) {
      // insert one by one before position
      FastList<T>::insert(position,*first);
    }
  }

  template <typename T>
    void FastList<T>::insert_before(const std::string& name, const T& val) {
    FastMapIteratorPair itPair = equal_range(name);
    if (itPair.first==itPair.second) {
      std::cerr<<"current list doesn't contain element "<< name << std::endl;
      exit(1);
    }
    // first insert into list and only then into map, otherwise map iterators are invalidated(!)
    std::vector<FastListIterator> listIterators;
    for (FastMapIterator it = itPair.first; it != itPair.second; ++it)
      {
        FastListIterator listIt = itsList.insert(it->second,val);
        listIterators.push_back(listIt);
      }
    for (FastListIterator it : listIterators) {
      itsMap.insert(std::pair<std::string,FastListIterator>(val.name,it));
    }
  }

  template <typename T>
    void FastList<T>::push_back(const T& el, bool unique, const std::string& className) {
    // better to search in map (faster)
    if (unique && itsMap.find(el.name) != itsMap.end()) {
      std::cout << "ERROR: " << className << " with name \"" << el.name << "\" already defined." << std::endl;
      exit(1);
    }
    // insert at back of list (insert() instead of push_back() to get iterator for map):
    FastListIterator it = itsList.insert(end(),el);
    itsMap.insert(std::pair<std::string,FastListIterator>(el.name,it));
  }

  template <typename T>
    int FastList<T>::size()const {
    return itsList.size();
  }

  template <typename T>
    void FastList<T>::clear() {
    itsList.clear();
    itsMap.clear();
  }

  template <typename T>
    void FastList<T>::erase() {
    FastListIterator it = begin();
    for(;it!=end();++it) {
      delete (*it).lst;
    }
    clear();
  }

  template <typename T>
    typename FastList<T>::FastListConstIterator FastList<T>::erase(const FastList<T>::FastListConstIterator it) {

    // find entry in map to erase:
    std::string name = (*it).name;
    if (itsMap.count(name) == 1) {
      itsMap.erase(name);
    }
    else { // more than one entry with same name
      FastMapIteratorPair ret = itsMap.equal_range(name);
      for (FastMapIterator emit = ret.first; emit!=ret.second; ++emit) {
        if ((*emit).second == it) // valid comparison? if not, how to find correct element?
          {
            itsMap.erase(emit);
            break;
          }
      }
    }
    return itsList.erase(it);
  }

  template <typename T>
    typename FastList<T>::FastListConstIterator FastList<T>::erase(const FastListConstIterator first, const FastListConstIterator last) {
    FastListConstIterator it=first;
    while (it!=last) {
      // erase one by one
      it = erase(it);
    }
    return it;
  }

  template <typename T>
    typename FastList<T>::FastListIterator FastList<T>::begin() {
    return itsList.begin();
  }

  template <typename T>
    typename FastList<T>::FastListIterator FastList<T>::end() {
    return itsList.end();
  }

  template <typename T>
    typename FastList<T>::FastListConstIterator FastList<T>::begin()const {
    return itsList.begin();
  }

  template <typename T>
    typename FastList<T>::FastListConstIterator FastList<T>::end()const {
    return itsList.end();
  }

  template <typename T>
    typename FastList<T>::FastMapIteratorPair FastList<T>::equal_range(std::string name) {
    return itsMap.equal_range(name);
  }

  template <typename T>
    typename FastList<T>::FastMapConstIteratorPair FastList<T>::equal_range(std::string name) const {
    return itsMap.equal_range(name);
  }

  template <typename T>
  std::map<std::string, T> FastList<T>::getMap() const
  {
    std::map<std::string, T> result;
    for (auto kv: itsMap)
      {result[kv.first] = *(kv.second);}
    return result;
  }

  template <typename T>
    typename FastList<T>::FastListConstIterator FastList<T>::find(std::string name,unsigned int count)const {
    if (count==1) {
      FastMapConstIterator emit = itsMap.find(name);
      if (emit==itsMap.end()) return itsList.end();
      return (*emit).second;
    } else {
      // if count > 1
      FastMapConstIteratorPair ret = itsMap.equal_range(name);
      unsigned int i=1;
      for (FastMapConstIterator emit = ret.first; emit!=ret.second; ++emit, i++) {
        if (i==count) {
          return (*emit).second;
        }
      }
      return itsList.end();
    }
  }

  template <typename T>
    typename FastList<T>::FastListIterator FastList<T>::find(std::string name,unsigned int count) {
    if (count==1) {
      FastMapIterator emit = itsMap.find(name);
      if (emit==itsMap.end()) return itsList.end();
      // remove constness trick, call erase with empty range
      FastListIterator it = itsList.erase((*emit).second, (*emit).second);
      return it;
    } else {
      // if count > 1
      FastMapIteratorPair ret = itsMap.equal_range(name);
      unsigned int i=1;
      for (FastMapIterator emit = ret.first; emit!=ret.second; ++emit, i++) {
        if (i==count) {
          // remove constness trick, call erase with empty range
          FastListIterator it = itsList.erase((*emit).second, (*emit).second);
          return it;
        }
      }
      return itsList.end();
    }
  }

  template <typename T>
    void FastList<T>::print(int ident)const {
    for(FastListConstIterator it=begin();it!=end();++it)
      {
        (*it).print(ident);
      }
  }
}

#endif
