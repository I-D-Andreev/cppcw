#ifndef BETHYW_H_
#define BETHYW_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  This file contains declarations for the helper functions for initialising and
  running Beth Yw?

  TODO: Read the block comments with TODO in bethyw.cpp to know which 
  functions you need to declare in this file.
 */

#include <string>
#include <unordered_set>
#include <vector>
#include <tuple>

#include "lib_cxxopts.hpp"

#include "datasets.h"
#include "areas.h"


const char DIR_SEP =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

namespace BethYw {

/*
  Student number!
*/
const std::string STUDENT_NUMBER = "955058";

const std::string IMPORT_ALL_ARG = "all";

/*
  Run Beth Yw?, parsing the command line arguments and acting upon them.
*/
int run(int argc, char *argv[]);

/*
  Create a cxxopts instance.
*/
cxxopts::Options cxxoptsSetup();

/*
  Parse the datasets argument and return a std::vector of all the datasets
  to import. InputFileSource is declared in datasets.h.
*/
std::vector<BethYw::InputFileSource> parseDatasetsArg(
  cxxopts::ParseResult& args);

/*
  Parse the areas argument and return a std::unordered_set of all the
  areas to import, or an empty set if all areas should be imported.
*/
std::unordered_set<std::string> parseAreasArg(cxxopts::ParseResult& args);


/*
  Parse the measures argument and return a std::unordered_set of all the
  measures to import, or an empty set if all measures should be imported.
*/
std::unordered_set<std::string> parseMeasuresArg(cxxopts::ParseResult& args);


/*
  Parse the year argument and return a tuple 
  containing the start and end (inclusive) of the year range.
*/
std::tuple<unsigned int, unsigned int> parseYearsArg(cxxopts::ParseResult& args);


/*
  Load the areas.csv file.
*/
void loadAreas(Areas& areas, const std::string& dir, const std::unordered_set<std::string>& areasFilter) noexcept;

/*
  Load the remaining datasets.
*/
void loadDatasets(Areas& areas,
 const std::string& dir,
 std::vector<BethYw::InputFileSource>& datasetsToImport,
 const StringFilterSet& areasFilter,
 const StringFilterSet& measuresFilter,
 const YearFilterTuple& yearsFilter
 ) noexcept;

} // namespace BethYw


namespace helpers {
  // Convert to lowerCase letters.
  std::string stringToLower(std::string str);
  
  // Split the string based on a provided delimiter. 
  std::vector<std::string> splitString(const std::string& str, char delimiter);
  
  // Check if a string contains only digits.
  bool isNumber(const std::string& numStr);

  // Check if a string contains only leters.
  bool isWord(const std::string& wordStr);

  // Convert a string to a number.
  int stringToNumber(const std::string& numStr);

  // The number of characters a double will take when printed to the screen.
  int charsInDouble(double num, size_t decimalPrecision);

  // Convert a string to a floating point number
  double stringToFloatingPointNumber(const std::string& numStr);
} // namespace helpers

#endif // BETHYW_H_