


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  This file contains the implementation of the Measure class. Measure is a
  very simple class that needs to contain a few member variables for its name,
  codename, and a Standard Library container for data. The data you need to 
  store is values, organised by year. I'd recommend storing the values as 
  doubles.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <string>
#include <iomanip>

#include "measure.h"
#include "bethyw.h"

Measure::Measure() : Measure("", ""){}

/*
  TODO: Measure::Measure(codename, label);

  Construct a single Measure, that has values across many years.

  All StatsWales JSON files have a codename for measures. You should convert 
  all codenames to lowercase.

  @param codename
    The codename for the measure

  @param label
    Human-readable (i.e. nice/explanatory) label for the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);
*/
Measure::Measure(std::string codename_, const std::string& label_) :
  codename(helpers::stringToLower(codename_)),
  label(label_),
  values(){}

/*
  TODO: Measure::getCodename()

  Retrieve the code for the Measure. This function should be callable from a 
  constant context and must promise to not modify the state of the instance or 
  throw an exception.

  @return
    The codename for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto codename2 = measure.getCodename();
*/
std::string Measure::getCodename() const noexcept {
  return codename;
}


/*
  TODO: Measure::getLabel()

  Retrieve the human-friendly label for the Measure. This function should be 
  callable from a constant context and must promise to not modify the state of 
  the instance and to not throw an exception.

  @return
    The human-friendly label for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto label = measure.getLabel();
*/
std::string Measure::getLabel() const noexcept {
  return label;
}


/*
  TODO: Measure::setLabel(label)

  Change the label for the Measure.

  @param label
    The new label for the Measure

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    ...
    measure.setLabel("New Population");
*/
void Measure::setLabel(const std::string& newLabel) {
  label = newLabel;
}


/*
  TODO: Measure::getValue(key)

  Retrieve a Measure's value for a given year.

  @param key
    The year to find the value for

  @return
    The value stored for the given year

  @throws
    std::out_of_range if year does not exist in Measure with the message
    No value found for year <year>

  @return
    The value

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto value = measure.getValue(1999); // returns 12345678.9
*/
double Measure::getValue(size_t year) const {
  auto it = values.find(year);

  if(it == values.end()) {
    throw std::out_of_range("No value found for year " + year);
  }

  return it->second;
}


/*
  TODO: Measure::setValue(key, value)

  Add a particular year's value to the Measure object. If a value already
  exists for the year, replace it.

  @param key
    The year to insert a value at

  @param value
    The value for the given year

  @return
    void

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
*/
void Measure::setValue(size_t year, double val) {
  values[year] = val;
}


/*
  TODO: Measure::size()

  Retrieve the number of years data we have for this measure. This function
  should be callable from a constant context and must promise to not change
  the state of the instance or throw an exception.

  @return
    The size of the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    auto size = measure.size(); // returns 1
*/
size_t Measure::size() const noexcept {
  return values.size();
}

/*
  TODO: Measure::getDifference()

  Calculate the difference between the first and last year imported. This
  function should be callable from a constant context and must promise to not
  change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year, or 0 if it
    cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(1999, 12345679.9);
    auto diff = measure.getDifference(); // returns 1.0
*/
double Measure::getDifference() const noexcept{
  if(size() <= 1) {
    return 0;
  }

  double smallest = values.begin()->second;
  double biggest = values.rbegin()->second;

  return biggest - smallest;
}


/*
  TODO: Measure::getDifferenceAsPercentage()

  Calculate the difference between the first and last year imported as a 
  percentage. This function should be callable from a constant context and
  must promise to not change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year as a decminal
    value, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1990, 12345678.9);
    measure.setValue(2010, 12345679.9);
    auto diff = measure.getDifferenceAsPercentage();
*/
double Measure::getDifferenceAsPercentage() const noexcept {
  if(size() <= 1) {
      return 0;
    }

  double smallest = values.begin()->second;

  return getDifference() / smallest * 100;
}


/*
  TODO: Measure::getAverage()

  Calculate the average/mean value for all the values. This function should be
  callable from a constant context and must promise to not change the state of 
  the instance or throw an exception.

  @return
    The average value for all the years, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(1999, 12345679.9);
    auto diff = measure.getDifference(); // returns 1
*/
double Measure::getAverage() const noexcept {
  if(size() == 0) {
    return 0;
  }

  double sum = 0;
  for(const auto& keyValuePair : values) {
    sum+= keyValuePair.second;
  }

  return sum / size();
}

/*
  Combine a measure with another one.
  This will result in any overlapping values being overriden,
  while all non-overlapping ones will be included.
*/
void Measure::combineMeasure(const Measure& other) {
  // todo1: Is this correct?
  codename = other.codename;
  label = other.label;

  for(const auto& keyValuePair: other.values) {
    values[keyValuePair.first] = keyValuePair.second;
  }
}


/*
    Returns a vector of measurement readings.
    The first element of the pair is the measurement year, while the second is the reading itself.
  */
std::vector<std::pair<size_t, double>> Measure::getAllReadingsSorted() const {
  std::vector<std::pair<size_t, double>> readings;

  for (const auto& keyValPair : values) {
    readings.push_back(keyValPair);
  }

  return readings;
}



/*
  TODO: operator<<(os, measure)

  Overload the << operator to print all of the Measure's imported data.

  We align the year and value outputs by padding the outputs with spaces,
  i.e. the year and values should be right-aligned to each other so they
  can be read as a table of numerical values.

  Years should be printed in chronological order. Three additional columns
  should be included at the end of the output, correspodning to the average
  value across the years, the difference between the first and last year,
  and the percentage difference between the first and last year.

  If there is no data in this measure, print the name and code, and 
  on the next line print: <no data>

  See the coursework specification for more information.

  @param os
    The output stream to write to

  @param measure
    The Measure to write to the output stream

  @return
    Reference to the output stream

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    std::cout << measure << std::end;
*/
std::ostream& operator<< (std::ostream& os, const Measure& measure) {
  const int DECIMAL_PRECISION = 6; // chars after decimal point
  
  // Number of characters per column. Initially, the measure 
  // name "Average" (and "% Diff.") will take the most space at 7 chars.
  const int INITIAL_COLUMN_SPACING = 7; 
  const int SPACE_BETWEEN_COLUMNS = 2;
  std::string spaceBetweenColumnsStr = std::string(SPACE_BETWEEN_COLUMNS, ' ');

  os << measure.label << " (" << measure.codename << ")" << std::endl;

  std::vector<std::pair<size_t, double>> readings = measure.getAllReadingsSorted();

  // Check if any value will take more space than allocated
  int columnSpacing = INITIAL_COLUMN_SPACING;
  for(const auto& reading : readings) {
    columnSpacing = std::max(columnSpacing, helpers::charsInDouble(reading.second, DECIMAL_PRECISION));
  }

  // Measure Names Row
  for(const auto& reading : readings){
    os << std::setw(columnSpacing) << reading.first;
    os << spaceBetweenColumnsStr;
  }

  for(const auto& additionalName : {"Average", "Diff.", "% Diff."}){
    os << std::setw(columnSpacing) << additionalName;
    os << spaceBetweenColumnsStr;
  }

  os << std::endl;
  // Measure Values Row
  os << std::fixed;
  for(const auto& reading : readings){
    os << std::setw(columnSpacing) << std::setprecision(DECIMAL_PRECISION) << reading.second;
    os << spaceBetweenColumnsStr;
  }

  for(const auto& additionalVal : {measure.getAverage(), measure.getDifference(), measure.getDifferenceAsPercentage()}) {
    os << std::setw(columnSpacing) << std::setprecision(DECIMAL_PRECISION) << additionalVal;
    os << spaceBetweenColumnsStr;
  }
  os << std::endl;

  return os;
}

/*
  TODO: operator==(lhs, rhs)

  Overload the == operator for two Measure objects. Two Measure objects
  are only equal when their codename, label and data are all equal.

  @param lhs
    A Measure object

  @param rhs
    A second Measure object

  @return
    true if both Measure objects have the same codename, label and data; false
    otherwise
*/
bool operator==(const Measure& lhs, const Measure& rhs) {
  return (lhs.codename == rhs.codename) && (lhs.label == rhs.label) && (lhs.values == rhs.values);
}


