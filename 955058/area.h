#ifndef AREA_H_
#define AREA_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  This file contains the Area class declaration. Area objects contain all the
  Measure objects for a given local area, along with names for that area and a
  unique authority code.

  TODO: Read the block comments with TODO in area.cpp to know which 
  functions and member variables you need to declare in this class.
 */

#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
#include <vector>

#include "lib_json.hpp"

#include "measure.h"

using json = nlohmann::json;

/*
  An Area object consists of a unique authority code, a container for names
  for the area in any number of different languages, and a container for the
  Measures objects.

  TODO: Based on your implementation, there may be additional constructors
  or functions you implement here, and perhaps additional operators you may wish
  to overload.
*/
class Area {
private:
  std::string localAuthorityCode;

  // language code -> name in the specified language
  // keep ordered by lang code
  std::map<std::string, std::string> names;

  // measure code -> Measure
  // Order by measures codename as required for operator<< and for nicer printing.
  std::map<std::string, Measure> measures;
public:
  Area();
  Area(const std::string& localAuthorityCode_);

  std::string getLocalAuthorityCode() const;
  std::string getName(const std::string& langCode) const;
  void setName(const std::string& langCode, const std::string& name);
  Measure& getMeasure(const std::string& measureCode);
  void setMeasure(const std::string& measureCode, const Measure& measure);
  size_t size() const noexcept;
  
  /* get a name given a lang code or return empty if it doesn't exist */
  std::string getNameOrEmpty(const std::string& langCode) const;

  /* get list of measures sorted by their codename */
  std::vector<std::string> getMeasureCodesSorted() const;

  /* get list of all names for the specified area */
  std::vector<std::string> getAllNames() const;
  
  /* Combine this Area with another one, updating the overlapping variables
  and adding/keeping non-overlapping ones. */
  void combineArea(const Area& other);

  friend std::ostream& operator<<(std::ostream& os, Area& area);
  friend bool operator==(const Area& lhs, const Area& rhs);

  json toJSON() const;
};

#endif // AREA_H_