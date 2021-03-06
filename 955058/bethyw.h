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
  int run(int argc, char* argv[]);

  /*
   Create a cxxopts instance.
  */
  cxxopts::Options cxxoptsSetup() noexcept(false);

  /*
   Parse the datasets argument and return a std::vector of all the datasets
   to import. InputFileSource is declared in datasets.h.
  */
  std::vector<BethYw::InputFileSource> parseDatasetsArg(
          cxxopts::ParseResult& args) noexcept(false);

  /*
   Parse the areas argument and return a std::unordered_set of all the
   areas to import, or an empty set if all areas should be imported.
  */
  StringFilterSet parseAreasArg(cxxopts::ParseResult& args) noexcept(false);


  /*
   Parse the measures argument and return a std::unordered_set of all the
   measures to import, or an empty set if all measures should be imported.
  */
  StringFilterSet parseMeasuresArg(cxxopts::ParseResult& args) noexcept(false);


  /*
   Parse the year argument and return a tuple
   containing the start and end (inclusive) of the year range.
  */
  YearFilterTuple parseYearsArg(cxxopts::ParseResult& args) noexcept(false);


  /*
   Load the areas.csv file.
  */
  void loadAreas(Areas& areas, const std::string& dir, const StringFilterSet& areasFilter) noexcept;

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


namespace string_operations {
  // Convert to lowerCase letters.
  std::string stringToLower(std::string str);

  // Split the string based on a provided delimiter. 
  std::vector<std::string> splitString(const std::string& str, char delimiter);

  // Check if a string contains only digits.
  bool isPositiveNumber(const std::string& numStr);

  // Check if a string contains only leters.
  bool isWord(const std::string& wordStr);

  // Convert a string to a number.
  int stringToNumber(const std::string& numStr);

  // The number of characters a double will take when printed to the screen.
  int charsInDouble(double num, size_t decimalPrecision);

  // Convert a string to a floating point number
  double stringToFloatingPointNumber(const std::string& numStr);
} // namespace string_operations

#endif // BETHYW_H_