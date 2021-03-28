


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 955058

  This file contains the code responsible for opening and closing file
  streams. The actual handling of the data from that stream is handled
  by the functions in data.cpp. See the header file for additional comments.
  
  Each function you must implement has a TODO in its comment block. You may
  have to implement additional functions. You may implement additional
  functions not specified.
 */

#include "input.h"

#include <utility>

/*
  TODO: InputSource::InputSource(source)

  Constructor for an InputSource.

  @param source
    A unique identifier for a source (i.e. the location).
*/
InputSource::InputSource(std::string _source) : source(std::move(_source)) {}

/*
  TODO: InputSource::getSource()

  This function should be callable from a constant context.

  @return
    A non-modifable value for the source passed into the construtor.
*/
std::string InputSource::getSource() const noexcept {
  return source;
}


/*
  TODO: InputFile:InputFile(path)

  Constructor for a file-based source.

  @param path
    The complete path for a file to import.

  @example
    InputFile input("data/areas.csv");
*/
InputFile::InputFile(std::string filePath) : InputSource(std::move(filePath)), inputStream() {
}


InputFile::~InputFile() {
  if (inputStream.is_open()) {
    inputStream.close();
  }
}


/*
  TODO: InputFile::open()

  Open a file stream to the file path retrievable from getSource()
  and return a reference to the stream.

  @return
    A standard input stream reference

  @throws
    std::runtime_error if there is an issue opening the file, with the message:
    InputFile::open: Failed to open file <file name>

  @example
    InputFile input("data/areas.csv");
    input.open();
*/
std::istream& InputFile::open() noexcept(false) {
  if (inputStream.is_open()) {
    return inputStream;
  }

  inputStream.open(getSource(), std::ifstream::in);

  if (!inputStream) {
    throw std::runtime_error("InputFile::open: Failed to open file " + getSource());
  }

  return inputStream;
}