#ifndef MEASURE_H_
#define MEASURE_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  This file contains the decalaration of the Measure class.

  TODO: Read the block comments with TODO in measure.cpp to know which 
  functions and member variables you need to declare in this class.
 */

#include <string>
#include <map>

/*
  The Measure class contains a measure code, label, and a container for readings
  from across a number of years.

  TODO: Based on your implementation, there may be additional constructors
  or functions you implement here, and perhaps additional operators you may wish
  to overload.
*/
class Measure {

private:
  std::string codename;
  std::string label;

  // year -> recorded value
  // use sorted map so that we can have the years in order
  std::map<size_t, double> values;
public:
  Measure(std::string codename_, const std::string& label_);

  std::string getCodeName() const noexcept;
  std::string getLabel() const noexcept;
  void setLabel(const std::string& newLabel);
  double getValue(size_t year) const;
  void setValue(size_t year, double val);
  size_t size() const noexcept;
  double getDifference() const noexcept;
  double getDifferenceAsPercentage() const noexcept;
  double getAverage() const noexcept;

  friend bool operator==(const Measure& lhs, const Measure& rhs);
};

#endif // MEASURE_H_