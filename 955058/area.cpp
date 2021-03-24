


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  This file contains the implementation for the Area class. Area is a relatively
  simple class that contains a local authority code, a container of names in
  different languages (perhaps stored in an associative container?) and a series
  of Measure objects (also in some form of container).

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>

#include "area.h"
#include "bethyw.h"


Area::Area() : Area("") {}
/*
  TODO: Area::Area(localAuthorityCode)

  Construct an Area with a given local authority code.

  @param localAuthorityCode
    The local authority code of the Area

  @example
    Area("W06000023");
*/
Area::Area(const std::string& localAuthorityCode_) : 
    localAuthorityCode(localAuthorityCode_),
    names(),
    measures() {}

/*
  TODO: Area::getLocalAuthorityCode()

  Retrieve the local authority code for this Area. This function should be 
  callable from a constant context and not modify the state of the instance.
  
  @return
    The Area's local authority code

  @example
    Area area("W06000023");
    ...
    auto authCode = area.getLocalAuthorityCode();
*/
std::string Area::getLocalAuthorityCode() const {
  return localAuthorityCode;
}

/*
  TODO: Area::getName(lang)

  Get a name for the Area in a specific language.  This function should be 
  callable from a constant context and not modify the state of the instance.

  @param lang
    A three-leter language code in ISO 639-3 format, e.g. cym or eng

  @return
    The name for the area in the given language

  @throws
    std::out_of_range if lang does not correspond to a language of a name stored
    inside the Area instance

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);
    ...
    auto name = area.getName(langCode);
*/
std::string Area::getName(const std::string& langCode) const {
  std::string lowerCaseCode = helpers::stringToLower(langCode);

  auto it = names.find(lowerCaseCode);

  if(it == names.end()) {
    throw std::out_of_range("A name in language {" + langCode + "} does not exist!");
  }

  return it->second;
}


/*
  Either get the name if it exists or return empty if it doesn't.
*/
std::string Area::getNameOrEmpty(const std::string& langCode) const {
  try {
    return getName(langCode);
  }
  catch(const std::out_of_range& ex) {
    return std::string();
  }
}

/*
  TODO: Area::setName(lang, name)

  Set a name for the Area in a specific language.

  @param lang
    A three-letter (alphabetical) language code in ISO 639-3 format,
    e.g. cym or eng, which should be converted to lowercase

  @param name
    The name of the Area in `lang`

  @throws
    std::invalid_argument if lang is not a three letter alphabetic code

  @example
    Area area("W06000023");
    std::string langCodeEnglish  = "eng";
    std::string langValueEnglish = "Powys";
    area.setName(langCodeEnglish, langValueEnglish);

    std::string langCodeWelsh  = "cym";
    std::string langValueWelsh = "Powys";
    area.setName(langCodeWelsh, langValueWelsh);
*/
void Area::setName(const std::string& langCode, const std::string& name) {
  constexpr int LANG_CODE_LENGTH_REQUIREMENT = 3;

  if(!helpers::isWord(langCode) || langCode.length() != LANG_CODE_LENGTH_REQUIREMENT) {
    throw std::invalid_argument("Area::setName: Language code must be three alphabetical letters only");
  }

  std::string lowerCaseCode = helpers::stringToLower(langCode);

  names[lowerCaseCode] = name;
}


/*
  TODO: Area::getMeasure(key)

  Retrieve a Measure object, given its codename. This function should be case
  insensitive when searching for a measure.

  @param key
    The codename for the measure you want to retrieve

  @return
    A Measure object

  @throws
    std::out_of_range if there is no measure with the given code, throwing
    the message:
    No measure found matching <codename>

  @example
    Area area("W06000023");
    Measure measure("Pop", "Population");
    area.setMeasure("Pop", measure);
    ...
    auto measure2 = area.getMeasure("pop");
*/
Measure& Area::getMeasure(const std::string& measureCode) {
  const std::string lowerCaseCode = helpers::stringToLower(measureCode);

  auto it = measures.find(lowerCaseCode);
  if(it == measures.end()) {
    throw std::out_of_range("No measure found matching " + measureCode);
  }

  return it->second;
}


/*
  TODO: Area::setMeasure(codename, measure)

  Add a particular Measure to this Area object. Note that the Measure's
  codename should be converted to lowercase.

  If a Measure already exists with the same codename in this Area, overwrite any
  values contained within the existing Measure with those in the new Measure
  passed into this function. The resulting Measure stored inside the Area
  instance should be a combination of the two Measures instances.

  @param codename
    The codename for the Measure

  @param measure
    The Measure object

  @return
    void

  @example
    Area area("W06000023");

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    double value = 12345678.9;
    measure.setValue(1999, value);

    area.setMeasure(codename, measure);
*/
void Area::setMeasure(const std::string& measureCode, const Measure& measure) {
  const std::string lowerCaseCode = helpers::stringToLower(measureCode);
  auto it = measures.find(measureCode);

  if(it == measures.end()) {
    measures[lowerCaseCode] = measure;
  } else {
    it->second.combineMeasure(measure);
  }
}


/*
  TODO: Area::size()

  Retrieve the number of Measures we have for this Area. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The size of the Area (i.e., the number of Measures)

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    area.setMeasure(code, measure);
    auto size = area.size();
*/
size_t Area::size() const noexcept {
    return measures.size();
}

/*
  Combine this area with another one.
  Overlapping values should be overriden,
  while non-overlapping ones should be kept/added.
*/
void Area::combineArea(const Area& other) {
  localAuthorityCode = other.localAuthorityCode;

  for(const auto& keyValPair : other.names) {
    names[keyValPair.first] = keyValPair.second;
  }

  for(const auto& keyValPair : other.measures) {
    setMeasure(keyValPair.first, keyValPair.second);
  }

}


std::vector<std::string> Area::getMeasureCodesSorted() const {
  std::vector<std::string> measureCodes;
  
  for(const auto& keyValPair : measures) {
    measureCodes.push_back(keyValPair.first);
  }
  
  return measureCodes;
}


/*
  TODO: operator<<(os, area)

  Overload the stream output operator as a free/global function.

  Output the name of the Area in English and Welsh, followed by the local
  authority code. Then output all the measures for the area (see the coursework
  worksheet for specific formatting).

  If the Area only has only one name, output this. If the area has no names,
  output the name "Unnamed".

  Measures should be ordered by their Measure codename. If there are no measures
  output the line "<no measures>" after you have output the area names.

  See the coursework specification for more examples.

  @param os
    The output stream to write to

  @param area
    Area to write to the output stream

  @return
    Reference to the output stream

  @example
    Area area("W06000023");
    area.setName("eng", "Powys");
    std::cout << area << std::endl;
*/
std::ostream& operator<<(std::ostream& os, Area& area) {
  std::string nameEng = area.getNameOrEmpty("eng");
  std::string nameCym = area.getNameOrEmpty("cym");
  std::string prettyCode = "(" + area.getLocalAuthorityCode() + ")";

  int namesCount = static_cast<int>(!nameEng.empty()) + static_cast<int>(!nameCym.empty());

  switch (namesCount)
  {
  case 2:
    os << nameEng << " / " << nameCym << " " << prettyCode << std::endl;
    break;
  
  case 1:
    // one of them is empty so we can concatenate
    os << nameEng << nameCym << " " << prettyCode << std::endl;
    break;
  
  case 0:
  default:
    os << "Unnamed " << prettyCode << std::endl;
    break;
  }


  if (area.size() == 0) {
    os << "<no measures>" << std::endl;
    return os;
  }


  std::vector<std::string> measureCodes = area.getMeasureCodesSorted();
  for(const std::string& code : measureCodes) {
    os << area.getMeasure(code) << std::endl;
  }

  return os;
}


/*
  TODO: operator==(lhs, rhs)

  Overload the == operator for two Area objects as a global/free function. Two
  Area objects are only equal when their local authority code, all names, and
  all data are equal.

  @param lhs
    An Area object

  @param rhs
    A second Area object

  @return
    true if both Area instanes have the same local authority code, names
    and data; false otherwise.

  @example
    Area area1("MYCODE1");
    Area area2("MYCODE1");

    bool eq = area1 == area2;
*/
bool operator==(const Area& lhs, const Area& rhs) {
  // todo1: should localAuthority check be case insensitive
  // names and measures saving is already lowercase
  return (lhs.localAuthorityCode == rhs.localAuthorityCode) && (lhs.names == rhs.names) && (lhs.measures == rhs.measures);
}


json Area::toJSON() const {
  json j;
  json measuresJson;
  json namesJson;

  for (const auto& keyValPair : names) {
    namesJson[keyValPair.first] = keyValPair.second;
  }

  for (const auto& keyValPair : measures) {
    measuresJson[keyValPair.second.getCodename()] = keyValPair.second.toJSON();
  }

  if (!measuresJson.empty()) {
    j["measures"] = measuresJson;
  }

  if (!namesJson.empty()) {
    j["names"] = namesJson;
  }

  return j;
}