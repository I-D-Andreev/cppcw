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

#include "measure.h"

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
  const std::string localAuthorityCode;

  // language code -> name in the specified language
  std::unordered_map<std::string, std::string> names;

  // measure code -> Measure
  std::unordered_map<std::string, Measure> measures;
public:
  Area(const std::string& localAuthorityCode_);
  std::string getLocalAuthorityCode() const;
  std::string getName(const std::string& langCode) const;
  void setName(const std::string& langCode, const std::string& name);
};

#endif // AREA_H_