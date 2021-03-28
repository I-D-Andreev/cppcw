


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  This file contains all the helper functions for initialising and running
  Beth Yw? In languages such as Java, this would be a class, but we really
  don't need a class here. Classes are for modelling data, and so forth, but
  here the code is pretty much a sequential block of code (BethYw::run())
  calling a series of helper functions.

  TODO: This file contains numerous functions you must implement. Each one
  is denoted with a TODO in the block comment. Note that some code has been
  provided in some functions to get you started, but you should read through
  this code and make sure it is safe and complete. You may need to remove or
  modify the provided code to get your program to work fully. You may implement
  additional functions not specified.
*/

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iomanip>


#include "lib_cxxopts.hpp"

#include "datasets.h"
#include "bethyw.h"
#include "input.h"

// Anonymous namespace for helper functions private to bethyw.cpp.
namespace {
  /*
    Parses a command line argument, which is optional. If it doesn't 
    exist or exists and contains "all" as value (any case), all areas should be
    imported, i.e., the filter should be an empty set.

    Does not check validity of arguments.
    Filtering is case insensitive.

    ! Helper function for parseAreasArg and parseMeasuresArg as they work the same.

    @param args
      Parsed program arguments
    
    @param argName
      The name of the argument we want to parse. E.g. "areas"

    @return
      An std::unordered_set of std::strings corresponding to specific argument
      to import, or an empty set if all should be imported.

    // @throws todo1?
    //   std::invalid_argument if the argument contains an invalid areas value with
    //   message: Invalid input for area argument
  */
  // todo1: should this throw
  StringFilterSet parseStringArg(
      const cxxopts::ParseResult& args,
      const std::string& argName){
    
    // The set to return. 
    // Does not deal with repetition when args have different letter case.
    StringFilterSet parsedArgs;

    if(args.count(argName) == 0){
      return StringFilterSet();
    }

    const auto& inputArgs = args[argName].as<std::vector<std::string>>();

    for(const std::string& arg : inputArgs){
      if(string_operations::stringToLower(arg) == BethYw::IMPORT_ALL_ARG){
        return StringFilterSet();
      }

      parsedArgs.insert(arg);
    }

    return parsedArgs;
  }

} // end of anonymous namespace


/*
  Run Beth Yw?, parsing the command line arguments, importing the data,
  and outputting the requested data to the standard output/error.

  Hint: cxxopts.parse() throws exceptions you'll need to catch. Read the cxxopts
  documentation for more information.

  @param argc
    Number of program arguments

  @param argv
    Program arguments

  @return
    Exit code
*/
int BethYw::run(int argc, char *argv[]) {
  
  try {
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);

    // Print the help usage if requested
    if (args.count("help")) {
      std::cerr << cxxopts.help() << std::endl;
      return 0;
    }

    // Parse data directory argument
    std::string dir = args["dir"].as<std::string>() + DIR_SEP;
    
    // Parse other arguments and import data
    std::vector<BethYw::InputFileSource> datasetsToImport = BethYw::parseDatasetsArg(args);
    StringFilterSet areasFilter = BethYw::parseAreasArg(args);
    StringFilterSet measuresFilter = BethYw::parseMeasuresArg(args);
    YearFilterTuple yearsFilter = BethYw::parseYearsArg(args);

    Areas data = Areas();

    BethYw::loadAreas(data, dir, areasFilter);
  

    BethYw::loadDatasets(data,
                        dir,
                        datasetsToImport,
                        areasFilter,
                        measuresFilter,
                        yearsFilter);
    
    if (args.count("json")) {
      // The output as JSON
      std::cout << data.toJSON() << std::endl;
    } else {
      // The output as tables
      std::cout << data << std::endl;
    }
  }
  catch(const cxxopts::OptionException& ex){
    // args exceptions
    std::cerr << "Program argument error: " << std::endl;
    std::cerr << ex.what() << std::endl;
    return 1;
  }
  catch(const std::exception& ex) {
    // argument loading / runtime exceptions
    std::cerr << ex.what() << std::endl;
    return 1;
  }
  
  return 0;
}

/*
  This function sets up and returns a valid cxxopts object. You do not need to
  modify this function.

  @return
     A constructed cxxopts object

  @example
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);
*/
cxxopts::Options BethYw::cxxoptsSetup() {
  cxxopts::Options cxxopts(
        "bethyw",
        "Student ID: " + STUDENT_NUMBER + "\n\n"
        "This program is designed to parse official Welsh Government"
        " statistics data files.\n");
    
  cxxopts.add_options()(
      "dir",
      "Directory for input data passed in as files",
      cxxopts::value<std::string>()->default_value("datasets"))(

      "d,datasets",
      "The dataset(s) to import and analyse as a comma-separated list of codes "
      "(omit or set to 'all' to import and analyse all datasets)",
      cxxopts::value<std::vector<std::string>>())(

      "a,areas",
      "The areas(s) to import and analyse as a comma-separated list of "
      "authority codes (omit or set to 'all' to import and analyse all areas)",
      cxxopts::value<std::vector<std::string>>())(

      "m,measures",
      "Select a subset of measures from the dataset(s) "
      "(omit or set to 'all' to import and analyse all measures)",
      cxxopts::value<std::vector<std::string>>())(

      "y,years",
      "Focus on a particular year (YYYY) or "
      "inclusive range of years (YYYY-ZZZZ)",
      cxxopts::value<std::string>()->default_value("0"))(

      "j,json",
      "Print the output as JSON instead of tables.")(

      "h,help",
      "Print usage.");

  return cxxopts;
}

/*
  TODO: BethYw::parseDatasetsArg(args)

  Parse the datasets argument passed into the command line. 

  The datasets argument is optional, and if it is not included, all datasets 
  should be imported. If it is included, it should be a comma-separated list of 
  datasets to import. If the argument contains the value "all"
  (case-insensitive), all datasets should be imported.

  This function validates the passed in dataset names against the codes in
  DATASETS array in the InputFiles namespace in datasets.h. If an invalid code
  is entered, throw a std::invalid_argument with the message:
  No dataset matches key: <input code>
  where <input name> is the name supplied by the user through the argument.

  @param args
    Parsed program arguments

  @return
    A std::vector of BethYw::InputFileSource instances to import

  @throws
    std::invalid_argument if the argument contains an invalid dataset with
    message: No dataset matches key <input code>

  @example
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);

    auto datasetsToImport = BethYw::parseDatasetsArg(args);
 */
std::vector<BethYw::InputFileSource> BethYw::parseDatasetsArg(
    cxxopts::ParseResult& args) {

  // Create the container for the return type
  std::vector<InputFileSource> datasetsToImport;

  // Retrieve all valid datasets, see datasets.h
  size_t numDatasets = InputFiles::NUM_DATASETS;
  auto &allDatasets = InputFiles::DATASETS;

  // { dataset code : dataset position in array }
  // Save the position in the datasets array so that the dataset is not copied.
  std::unordered_map <std::string, unsigned int> datasets;

  for (unsigned int i = 0; i< numDatasets; i++){
    datasets[allDatasets[i].CODE] = i;
  }

  
  bool importAll = false;
  // The dataset strings as taken from the command line input.
  std::vector<std::string> inputDatasets;

  if(args.count("datasets")){
    inputDatasets = args["datasets"].as<std::vector<std::string>>();
  } 
  else {
    importAll = true;
  }

  // Check if we should import all datasets and whether they are all correct
  for(const std::string& code : inputDatasets){
    if(string_operations::stringToLower(code) == BethYw::IMPORT_ALL_ARG){
      importAll = true;
      break;
    }
  }

  if(!importAll) {
    for(const std::string& code : inputDatasets){
      if(datasets.count(code) == 0){
          throw std::invalid_argument("No dataset matches key: " + code);
      }
    }
  }


  if(importAll){
    datasetsToImport = std::vector<InputFileSource>(std::begin(allDatasets), std::end(allDatasets));
  }
  else {
    // Make sure that a dataset is not imported twice if it is repeated
    // in the command line input.
    std::unordered_set<unsigned int> alreadyImported;

    for(const std::string& code : inputDatasets){
      unsigned int idx = datasets[code];

      if(alreadyImported.count(idx) == 0){
        alreadyImported.insert(idx);
        datasetsToImport.push_back(allDatasets[idx]);
      }
    }

  }

  return datasetsToImport;
}

/*
  TODO: BethYw::parseAreasArg(args)
  
  Parses the areas command line argument, which is optional. If it doesn't 
  exist or exists and contains "all" as value (any case), all areas should be
  imported, i.e., the filter should be an empty set.

  Unlike datasets we can't check the validity of the values as it depends
  on each individual file imported (which hasn't happened until runtime).
  Therefore, we simply fetch the list of areas and later pass it to the
  Areas::populate() function.

  The filtering of inputs should be case insensitive.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific areas
    to import, or an empty set if all areas should be imported.

  @throws
    std::invalid_argument if the argument contains an invalid areas value with
    message: Invalid input for area argument
*/
StringFilterSet BethYw::parseAreasArg(
    cxxopts::ParseResult& args) {
  
  return ::parseStringArg(args, "areas");
}


/*
  TODO: BethYw::parseMeasuresArg(args)

  Parse the measures command line argument, which is optional. If it doesn't 
  exist or exists and contains "all" as value (any case), all measures should
  be imported.

  Unlike datasets we can't check the validity of the values as it depends
  on each individual file imported (which hasn't happened until runtime).
  Therefore, we simply fetch the list of areas and later pass it to the
  Areas::populate() function.

  The filtering of inputs should be case insensitive.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific measures
    to import, or an empty set if all measures should be imported.

  @throws
    std::invalid_argument if the argument contains an invalid measures value
    with the message: Invalid input for measures argument
*/
StringFilterSet BethYw::parseMeasuresArg(
    cxxopts::ParseResult& args) {
  
  return ::parseStringArg(args, "measures");
}


/*
  TODO: BethYw::parseYearsArg(args)

  Parse the years command line argument. Years is either a four digit year 
  value, or two four digit year values separated by a hyphen (i.e. either 
  YYYY or YYYY-ZZZZ).

  This should be parsed as two integers and inserted into a std::tuple,
  representing the start and end year (inclusive). If one or both values are 0,
  then there is no filter to be applied. If no year argument is given return
  <0,0> (i.e. to import all years). You will have to search
  the web for how to construct std::tuple objects! 

  @param args
    Parsed program arguments

  @return
    A std::tuple containing two unsigned ints

  @throws
    std::invalid_argument if the argument contains an invalid years value with
    the message: Invalid input for years argument
*/

YearFilterTuple BethYw::parseYearsArg(cxxopts::ParseResult& args) {
  
  const std::string& yearArg = args["years"].as<std::string>();
  const std::string invalidArgMessage = "Invalid input for years argument";

  std::vector<std::string> years = string_operations::splitString(yearArg, '-');

  // Check that year input is correct
  
  // The arg should contain either 1 or 2 years
  if (years.size() > 2 || years.size() < 1) {
    throw std::invalid_argument(invalidArgMessage);
  }

  // All the years in the arg should be numbers
  for(const auto& year : years){
    if(!string_operations::isPositiveNumber(year)){
      throw std::invalid_argument(invalidArgMessage);
    }
  }

  // The years should have the correct number of digits.
  // The years are either "0" or a four digit number.
  for(const auto& year : years) {
    if (year == "0") {
      return {0, 0};
    }
  }

  for(const auto& year : years) {
    if(year.size() != 4) {
      throw std::invalid_argument(invalidArgMessage);
    }
  }

  // Due to our previous check, the year is guaranteed to be a non-negative number.
  std::vector<unsigned int> yearsAsNumbers;
  for(const auto& strYear: years) {
    yearsAsNumbers.push_back(static_cast<unsigned int>(string_operations::stringToNumber(strYear)));
  }

  return {yearsAsNumbers[0], yearsAsNumbers[yearsAsNumbers.size()-1]};
}


/*
  TODO: BethYw::loadAreas(areas, dir, areasFilter)

  Load the areas.csv file from the directory `dir`. Parse the file and
  create the appropriate Area objects inside the Areas object passed to
  the function in the `areas` argument.

  areas.csv is guaranteed to be formatted as:
    Local authority code,Name (eng),Name (cym)

  Hint: To implement this function. First you will need create an InputFile 
  object with the filename of the areas file, open it, and then pass reference 
  to the stream to the Areas::populate() function.

  Hint 2: you can retrieve the specific filename for a dataset, e.g. for the 
  areas.csv file, from the InputFileSource's FILE member variable

  @param areas
    An Areas instance that should be modified (i.e. the populate() function
    in the instance should be called)

  @param dir
    Directory where the areas.csv file is

  @param areasFilter
    An unordered set of areas to filter, or empty to import all areas

  @return
    void

  @example
    Areas areas();

    BethYw::loadAreas(areas, "data", BethYw::parseAreasArg(args));
*/
void BethYw::loadAreas(Areas& areas, const std::string& dir, const StringFilterSet& areasFilter) noexcept {
  try {
    const BethYw::InputFileSource& AREAS = InputFiles::AREAS;
    std::string areasFilePath = dir + AREAS.FILE;
    InputFile file { areasFilePath };
    
    areas.populate(file.open(), BethYw::SourceDataType::AuthorityCodeCSV, AREAS.COLS, &areasFilter);
  }
  catch(const std::exception& ex){
    std::cerr << "Error importing dataset:" << std::endl;
    std::cerr << ex.what() << std::endl;
    std::exit(1);
  }
}


/*
  TODO: BethYw::loadDatasets(areas,
                             dir,
                             datasetsToImport,
                             areasFilter,
                             measuresFilter,
                             yearsFilter)

  Import datasets from `datasetsToImport` as files in `dir` into areas, and
  filtering them with the `areasFilter`, `measuresFilter`, and `yearsFilter`.

  The actual filtering will be done by the Areas::populate() function, thus 
  you need to merely pass pointers on to these flters.

  This function should promise not to throw an exception. If there is an
  error/exception thrown in any function called by thus function, catch it and
  output 'Error importing dataset:', followed by a new line and then the output
  of the what() function on the exception.

  @param areas
    An Areas instance that should be modified (i.e. datasets loaded into it)

  @param dir
    The directory where the datasets are

  @param datasetsToImport
    A vector of InputFileSource objects

  @param areasFilter
    An unordered set of areas (as authority codes encoded in std::strings)
    to filter, or empty to import all areas

  @param measuresFilter
    An unordered set of measures (as measure codes encoded in std::strings)
    to filter, or empty to import all measures

  @param yearsFilter
    An two-pair tuple of unsigned ints corresponding to the range of years 
    to import, which should both be 0 to import all years.

  @return
    void

  @example
    Areas areas();

    BethYw::loadDatasets(
      areas,
      "data",
      BethYw::parseDatasetsArgument(args),
      BethYw::parseAreasArg(args),
      BethYw::parseMeasuresArg(args),
      BethYw::parseYearsArg(args));
*/
void BethYw::loadDatasets(Areas& areas,
 const std::string& dir,
 std::vector<BethYw::InputFileSource>& datasetsToImport,
 const StringFilterSet& areasFilter,
 const StringFilterSet& measuresFilter,
 const YearFilterTuple& yearsFilter
 ) noexcept {
 
  try {
    for(const InputFileSource& dataset : datasetsToImport) {
      std::string filePath = dir + dataset.FILE;
      InputFile file {filePath};

      areas.populate(file.open(), dataset.PARSER, dataset.COLS, &areasFilter, &measuresFilter, &yearsFilter);
    }
  }
  catch(const std::exception& ex) {
    std::cerr << "Error importing dataset:" << std::endl;
    std::cerr << ex.what() << std::endl;
    std::exit(1);
  }
}


std::string string_operations::stringToLower(std::string str) {
  for (char& c : str){
    c = std::tolower(c);
  }

  return str;
}

std::vector<std::string> string_operations::splitString(const std::string& str, char delimiter) {
  std::vector<std::string> stringParts;
  std::string temp;

  for (const char& c : str){
    if (c == delimiter){
      stringParts.push_back(temp);
      temp.clear();
    } 
    else {
      temp+= c;
    }
  }

  // Add the last element
  if(!temp.empty()){
    stringParts.push_back(temp);
  }

  return stringParts;
}


bool string_operations::isPositiveNumber(const std::string& numStr){
  for(char c : numStr){
    if(!std::isdigit(c)){
      return false;
    }
  }

  return true;
}


int string_operations::stringToNumber(const std::string& numStr) {
  return std::stoi(numStr);
}

bool string_operations::isWord(const std::string& wordStr) {
  for (char c : wordStr) {
    if(!isalpha(c)) {
      return false;
    }
  }

  return true;
}


int string_operations::charsInDouble(double num, size_t decimalPrecision) {
  std::stringstream ss;
  ss << std::fixed;
  ss << std::setprecision(decimalPrecision) << num;
  return ss.str().size();
}


double string_operations::stringToFloatingPointNumber(const std::string& numStr) {
  return std::stod(numStr);
}
