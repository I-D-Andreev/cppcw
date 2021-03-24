


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  The file contains the Areas class implementation. Areas are the top
  level of the data structure in Beth Yw? for now.

  Areas is also responsible for importing data from a stream (using the
  various populate() functions) and creating the Area and Measure objects.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <iostream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <typeinfo>

#include "lib_json.hpp"

#include "datasets.h"
#include "areas.h"
#include "measure.h"
#include "bethyw.h"

// Anonymous namespace for helper functions. Private to areas.cpp
namespace {
  bool shouldIncludeYear(unsigned int year, const YearFilterTuple* const yearsFilter) {
    if (yearsFilter == nullptr) {
      return true;
    }

    unsigned int year1 = std::get<0>(*yearsFilter);
    unsigned int year2 = std::get<1>(*yearsFilter);

    if (year1 == 0 || year2 == 0) {
      return true;
    }

    return (year >= std::min(year1, year2) && year <= std::max(year1, year2));
  }

  // copy the filter into a lowercase set so that we can 
  // check in constant time for case-insensitive filtering
  StringFilterSet lowerCaseFilter(const StringFilterSet* const filter) {
    StringFilterSet lowerCaseFilter;

    if (filter == nullptr) {
      return lowerCaseFilter;
    }

    for (const std::string& el : *filter) {
      lowerCaseFilter.insert(helpers::stringToLower(el));
    }

    return lowerCaseFilter;
  }

  // check if a lowercase filter contains a certain code (area code/measure code)
  // if the filter contains the code or is empty, return true
  // otherwise return false
  bool filterContains(const StringFilterSet& lowerCaseFilter, const std::string& code) {
    std::string lowerCaseCode = helpers::stringToLower(code);

    if(!lowerCaseFilter.empty() && lowerCaseFilter.count(lowerCaseCode) == 0) {
      return false;
    }

    return true;
  }

} // end of anonymous namespace


/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  TODO: Areas::Areas()

  Constructor for an Areas object.

  @example
    Areas data = Areas();
*/
Areas::Areas() : areas() {
}

/*
  TODO: Areas::setArea(localAuthorityCode, area)

  Add a particular Area to the Areas object.

  If an Area already exists with the same local authority code, overwrite all
  data contained within the existing Area with those in the new
  Area (i.e. they should be combined, but the new Area's data should take
  precedence, e.g. replace a name with the same language identifier).

  @param localAuthorityCode
    The local authority code of the Area

  @param area
    The Area object that will contain the Measure objects

  @return
    void

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
*/
void Areas::setArea(const std::string& localAuthorityCode, const Area& area) {
  const std::string lowerCaseCode = helpers::stringToLower(localAuthorityCode);
  auto it = areas.find(lowerCaseCode);

  if(it == areas.end()) {
    areas[lowerCaseCode] = area;
  } else {
    it->second.combineArea(area);
  }
}


/*
  TODO: Areas::getArea(localAuthorityCode)

  Retrieve an Area instance with a given local authority code.

  @param localAuthorityCode
    The local authority code to find the Area instance of

  @return
    An Area object

  @throws
    std::out_of_range if an Area with the set local authority code does not
    exist in this Areas instance

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    ...
    Area area2 = areas.getArea("W06000023");
*/
Area& Areas::getArea(const std::string& localAuthorityCode) {
  const std::string lowerCaseCode = helpers::stringToLower(localAuthorityCode);

  auto it = areas.find(lowerCaseCode);

  if(it == areas.end()){
    throw std::out_of_range("No area found matching " + localAuthorityCode);
  }

  return it->second;
}


/*
  TODO: Areas::size()

  Retrieve the number of Areas within the container. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The number of Area instances

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    
    auto size = areas.size(); // returns 1
*/
size_t Areas::size() const noexcept {
  return areas.size();
}


/*
  TODO: Areas::populateFromAuthorityCodeCSV(is, cols, areasFilter)

  This function specifically parses the compiled areas.csv file of local 
  authority codes, and their names in English and Welsh.

  This is a simple dataset that is a comma-separated values file (CSV), where
  the first row gives the name of the columns, and then each row is a set of
  data.

  For this coursework, you can assume that areas.csv will always have the same
  three columns in the same order.

  Once the data is parsed, you need to create the appropriate Area objects and
  insert them in to a Standard Library container within Areas.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/areas.csv");
    auto is = input.open();

    auto cols = InputFiles::AREAS.COLS;

    auto areasFilter = BethYw::parseAreasArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityCodeCSV(
    std::istream &is,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter) {
  // todo1: wrap errors

  // Copy the areas filter so that we can actually do 
  // case-insensitive lookup.
  StringFilterSet caseInsensitiveAreasFilter = ::lowerCaseFilter(areasFilter);

  const std::string LANG_CODE_ENG = "eng";
  const std::string LANG_CODE_CYM = "cym";

  std::string line;
  std::getline(is, line);

  auto elements = helpers::splitString(line, ',');

  if(elements.size() > cols.size()){
    throw std::out_of_range("The parsed files contains more columns than the mapping");
  }

  while(std::getline(is, line)){
    if(line.empty()){
      continue;
    }

    elements = helpers::splitString(line, ',');
    if(elements.size() != 3){
      throw std::runtime_error("Error parsing areas.csv. Three args per line expected.");
    }

    const std::string& code = elements[0];
    const std::string& nameEng = elements[1];
    const std::string& nameCym = elements[2];

    if (::filterContains(caseInsensitiveAreasFilter, code)) {
      Area area = Area(code);
      area.setName(LANG_CODE_ENG, nameEng);
      area.setName(LANG_CODE_CYM, nameCym);

      setArea(code, area);
    }
  }
}

/*
  TODO: Areas::populateFromWelshStatsJSON(is,
                                          cols,
                                          areasFilter,
                                          measuresFilter,
                                          yearsFilter)

  Data from StatsWales is in the JSON format, and contains three
  top-level keys: odata.metadata, value, odata.nextLink. value contains the
  data we need. Rather than been hierarchical, it contains data as a
  continuous list (e.g. as you would find in a table). For each row in value,
  there is a mapping of various column headings and their respective vaues.

  Therefore, you need to go through the items in value (in a loop)
  using a JSON library. To help you, I've selected the nlohmann::json
  library that you must use for your coursework. Read up on how to use it here:
  https://github.com/nlohmann/json

  Example of using this library:
    - Reading/parsing in from a stream is very simply using the >> operator:
        json j;
        stream >> j;

    - Looping through parsed JSON is done with a simple for each loop. Inside
      the loop, you can access each using the array syntax, with the key/
      column name, e.g. data["Localauthority_ItemName_ENG"] gives you the
      local authority name:
        for (auto& el : j["value"].items()) {
           auto &data = el.value();
           std::string localAuthorityCode = data["Localauthority_ItemName_ENG"];
           // do stuff here...
        }

  In this function, you will have to parse the JSON datasets, extracting
  the local authority code, English name (the files only contain the English
  names), and each measure by year.

  If you encounter an Area that does not exist in the Areas container, you
  should create the Area object

  If areasFilter is a non-empty set only include areas matching the filter. If
  measuresFilter is a non-empty set only include measures matching the filter.
  If yearsFilter is not equal to <0,0>, only import years within the range
  specified by the tuple (inclusive).

  I've provided the column names for each JSON file that you need to parse
  as std::strings in datasets.h. This mapping should be passed through to the
  cols parameter of this function.

  Note that in the JSON format, years are stored as strings, but we need
  them as ints. When retrieving values from the JSON library, you will
  have to cast them to the right type.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings of areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings of measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as the range of years to be imported (inclusively)

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populateFromWelshStatsJSON(
      is,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/

void Areas::populateFromWelshStatsJSON(
  std::istream& is,
  const BethYw::SourceColumnMapping& cols,
  const StringFilterSet* const areasFilter,
  const StringFilterSet* const measuresFilter,
  const YearFilterTuple* const yearsFilter 
) noexcept(false) {
  using SC = BethYw::SourceColumn;

  // If any of the .at() function calls fail they will throw out_of_range exception
  // as is expected by the function, so we don't need to wrap in try-catch. 
  const std::string& areaCodeIdx = cols.at(SC::AUTH_CODE);
  const std::string& nameEngIdx = cols.at(SC::AUTH_NAME_ENG);
  

  bool singleMeasureCode = (cols.count(SC::SINGLE_MEASURE_CODE) > 0);

  std::string measureCodeIdx = "";
  std::string measureNameIdx = "";

  if (!singleMeasureCode) {
    measureCodeIdx = cols.at(SC::MEASURE_CODE);
    measureNameIdx = cols.at(SC::MEASURE_NAME);
  }

  const std::string& yearIdx = cols.at(SC::YEAR);
  const std::string& valueIdx = cols.at(SC::VALUE);


  // Copy the filters in lowercase so that we can do case-insensitive
  // checks for areas and measures.
  StringFilterSet areasFilterLowercase = ::lowerCaseFilter(areasFilter);
  StringFilterSet measuresFilterLowercase = ::lowerCaseFilter(measuresFilter);


  // todo2: do better? error handling
  try {
    json json;
    is >> json;

    for(const auto& arrElement : json["value"].items()) {
      const auto& obj = arrElement.value();

      std::string areaCode = obj[areaCodeIdx];
      if(!::filterContains(areasFilterLowercase, areaCode)) {
        continue;
      }
      std::string nameEng = obj[nameEngIdx];

      std::string measureCode;
      std::string measureLabel;
      
      if (singleMeasureCode) {
        measureCode = cols.at(SC::SINGLE_MEASURE_CODE);
        measureLabel = cols.at(SC::SINGLE_MEASURE_NAME);
      } else {
        measureCode = obj[measureCodeIdx];
        measureLabel = obj[measureNameIdx];
      }
      
      if(!::filterContains(measuresFilterLowercase, measureCode)) {
        continue;
      }


      std::string yearData = obj[yearIdx];
      int year = helpers::stringToNumber(yearData); 
      if(!::shouldIncludeYear(year, yearsFilter)) {
        continue;
      }

      // todo4: FIX!
      double value = obj[valueIdx].get<double>();

      // Not as slow as it seems.
      // The "combining" logic only loops through the "other" (second)
      // objects variables so it will only check 1 measure and 1 value.
      Area area {areaCode};
      area.setName("eng", nameEng);
      Measure measure { measureCode, measureLabel};
      measure.setValue(year, value);

      area.setMeasure(measureCode, measure);
      setArea(areaCode, area);
    }
  }
  catch(const std::exception& ex) {
    throw std::runtime_error("Failure parsing JSON file: " + std::string(ex.what()) + "\n");
  }
}

/*
  TODO: Areas::populateFromAuthorityByYearCSV(is,
                                              cols,
                                              areasFilter,
                                              yearFilter)

  This function imports CSV files that contain a single measure. The 
  CSV file consists of columns containing the authority code and years.
  Each row contains an authority code and values for each year (or no value
  if the data doesn't exist).

  Note that these files do not include the names for areas, instead you 
  have to rely on the names already populated through 
  Areas::populateFromAuthorityCodeCSV();

  The datasets that will be parsed by this function are
   - complete-popu1009-area.csv
   - complete-popu1009-pop.csv
   - complete-popu1009-opden.csv

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of strings for measures to import, or an empty 
    set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/complete-popu1009-pop.csv");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["complete-pop"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto yearsFilter = BethYw::parseYearsArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter, &yearsFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/

void Areas::populateFromAuthorityByYearCSV(
  std::istream& is,
  const BethYw::SourceColumnMapping& cols,
  const StringFilterSet* const areasFilter,
  const StringFilterSet* const measuresFilter,
  const YearFilterTuple* const yearsFilter 
  ) {
  
  // Copy the case-sensitive filter and convert it to lowercase
  // as our input args should be case-insensitive.
  // This will allow us to do case-insensitive search. 
  StringFilterSet measuresFilterLowercase = ::lowerCaseFilter(measuresFilter);
  StringFilterSet areasFilterLowercase = ::lowerCaseFilter(areasFilter);
  std::vector <int> years;

  // firtly check that this measure/file should be imported at all
  const std::string& fileMeasure = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE);
  if (!::filterContains(measuresFilterLowercase, fileMeasure)) {
    return;
  } 

  // a single line in the file.
  std::string line;

  // the elements of each line, when separated by ','
  std::vector <std::string> lineElements;


  // parse first line
  std::getline(is, line);
  lineElements = helpers::splitString(line, ',');

  if (lineElements.size() <= 2) {
    throw std::runtime_error("Expected AuthorityCode and at least one year");
  }

  for(size_t i=1; i<lineElements.size(); i++) {
    try {
      int year = helpers::stringToNumber(lineElements[i]);
      years.push_back(year);
    }
    catch(const std::exception& ex) {
      // may throw std::invalid_argument or std::out_of_range, but
      // in both cases we will just rethrow, so catch a general type exception 
      throw std::runtime_error("Failed to parse year " + std::string(ex.what()));
    }
  }


  // parse lines 2nd to last
  while (std::getline(is, line)) {
    lineElements = helpers::splitString(line, ',');

    if(lineElements.size() <= 2){
      // disregard lines with only authority code or empty lines
      continue;
    }

    std::string areaCode = lineElements[0];

    // todo3: probably around here will be code for task8 (names)
    if(!::filterContains(areasFilterLowercase, areaCode)) {
      // This area should not be imported.
      continue;
    }


    Area area {areaCode};
    
    std::string measureCode = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE);
    std::string measureLabel = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_NAME);
    Measure measure {measureCode, measureLabel};
    
    // Parse the values on the line
    // years in years vector start from 0
    // values in the lineElements vector start from 1 as the authority code is the 0th element
    for(size_t yearsIndex=0, valuesIndex=1; 
      yearsIndex < years.size() && valuesIndex < lineElements.size();
      yearsIndex++, valuesIndex++) {
    
      int year = years[yearsIndex];
      std::string value = lineElements[valuesIndex];
      
      if(::shouldIncludeYear(year, yearsFilter) && value != "") {
        try {
          double valueParsed = helpers::stringToFloatingPointNumber(value);
          measure.setValue(year, valueParsed);
        }
        catch(const std::exception& ex) {
          // may throw std::invalid_argument or std::out_of_range, but
          // in both cases we will just rethrow, so catch a general type exception
          throw std::runtime_error("Failed to parse measurement " + std::string(ex.what()));
        }
      }
    }

    area.setMeasure(measureCode, measure);
    setArea(areaCode, area);
  }
}


/*
  TODO: Areas::populate(is,
                        type,
                        cols,
                        areasFilter,
                        measuresFilter,
                        yearsFilter)

  Parse data from an standard input stream, that is of a particular type,
  and with a given column mapping, filtering for specific areas, measures,
  and years, and fill the container.

  This function should look at the `type` and hand off to one of the three 
  functions you've implemented above.

  The function must check if the stream is in working order and has content.

  This overloaded function includes pointers to the three filters for areas,
  measures, and years.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings for measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variables areasFilter, measuresFilter,
    and yearsFilter are created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populate(
    std::istream &is,
    const BethYw::SourceDataType &type,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter,
    const StringFilterSet * const measuresFilter,
    const YearFilterTuple * const yearsFilter)
     {
  // todo1: check if stream is in working order and has content
  if (type == BethYw::SourceDataType::AuthorityCodeCSV) {
    populateFromAuthorityCodeCSV(is, cols, areasFilter);
  } 
  else if (type == BethYw::SourceDataType::AuthorityByYearCSV) {
    populateFromAuthorityByYearCSV(is, cols, areasFilter, measuresFilter, yearsFilter);
  }
  else if (type == BethYw::SourceDataType::WelshStatsJSON) {
    populateFromWelshStatsJSON(is, cols, areasFilter, measuresFilter, yearsFilter);
  }
  else {
    throw std::runtime_error("Areas::populate: Unexpected data type");
  }
}

/*
  TODO: Areas::toJSON()

  Convert this Areas object, and all its containing Area instances, and
  the Measure instances within those, to values.

  Use the sample JSON library as above to create this. Construct a blank
  JSON object:
    json j;

  Convert this json object to a string:
    j.dump();

  You then need to loop through your areas, measures, and years/values
  adding this data to the JSON object.

  Read the documentation for how to convert your outcome code to JSON:
    https://github.com/nlohmann/json#arbitrary-types-conversions
  
  The JSON should be formatted as such:
    {
    "<localAuthorityCode1>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               },
    "<localAuthorityCode2>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    ...
    "<localAuthorityCodeN>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    }

  An empty JSON is "{}" (without the quotes), which you must return if your
  Areas object is empty.
  
  @return
    std::string of JSON

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    std::cout << data.toJSON();
*/
std::string Areas::toJSON() const {
  json j;
  
  return j.dump();
}

/*
  TODO: operator<<(os, areas)

  Overload the << operator to print all of the imported data.

  Output should be formatted like the following to pass the tests. Areas should
  be printed, ordered alphabetically by their local authority code. Measures 
  within each Area should be ordered alphabetically by their codename.

  See the coursework specification for more information, although for reference
  here is a quick example of how output should be formatted:

    <English name of area 1> / <Welsh name of area 1> (<authority code 1>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>


    <English name of area 2> / <Welsh name of area 2> (<authority code 2>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

    ...

    <English name of area y> / <Welsh name of area y> (<authority code y>)
    <Measure 1 name> (<Measure 1 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x codename>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

  With real data, your output should start like this for the command
  bethyw --dir <dir> -p popden -y 1991-1993 (truncated for readability):

    Isle of Anglesey / Ynys Môn (W06000001)
    Land area (area) 
          1991       1992       1993    Average    Diff.  % Diff. 
    711.680100 711.680100 711.680100 711.680100 0.000000 0.000000 

    Population density (dens) 
         1991      1992      1993   Average    Diff.  % Diff. 
    97.126504 97.486216 98.038430 97.550383 0.911926 0.938905 

    Population (pop) 
            1991         1992         1993      Average      Diff.  % Diff. 
    69123.000000 69379.000000 69772.000000 69424.666667 649.000000 0.938906 


    Gwynedd / Gwynedd (W06000002)
    Land area (Area)
    ...

  @param os
    The output stream to write to

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream

  @example
    Areas areas();
    std::cout << areas << std::end;
*/
std::ostream& operator<<(std::ostream& os, Areas& areas) {
  for(auto& codeAreaPair : areas.areas) {
    os << codeAreaPair.second << std::endl;
  }

  return os;
}

// todo2: remove
// Area& Areas::getOrCreateArea(const std::string& localAuthorityCode, bool& created) {
//   try {
//     Area& area = getArea(localAuthorityCode);
//     created = false;
//     return area;
//   }
//   catch(const std::out_of_range& ex) {
//     setArea(localAuthorityCode, Area(localAuthorityCode));
//     created = true;
//     return getArea(localAuthorityCode);
//   }
// }
