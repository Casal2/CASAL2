/**
 * @file AddressableInputValueLoader.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/12/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */

// headers
#include "AddressableInputValueLoader.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#include "../AddressableInputLoader/AddressableInputLoader.h"
#include "../Logging/Logging.h"
#include "../Model/Managers.h"
#include "../Utilities/To.h"
#include "../Utilities/Types.h"

// namespaces
namespace niwa {
namespace configuration {

using niwa::utilities::Double;
using std::cout;
using std::endl;
using std::ifstream;
using std::vector;

/**
 * Load the values of the addressable parameters from the file provided
 *
 * @param file_name The name of the file containing the values
 */
void AddressableInputValueLoader::LoadValues(const string& file_name) {
  LOG_FINE() << "Load values from file " << file_name;
  ifstream file_;
  file_.open(file_name.c_str());
  if (file_.fail() || !file_.is_open())
    LOG_FATAL() << "Unable to open the addressable input file " << file_name;

  /**
   * Get the first line which should contain a list of parameters
   */
  string         current_line = "";
  vector<string> parameters;
  unsigned       line_number = 0;
  if (!getline(file_, current_line) || current_line == "")
    LOG_FATAL() << "addressable input file appears to be empty, or the first line is blank. File: " << file_name;

  // Make an exception for MCMC_samples outputs that users will want to feed back into Casal2 using the -i functionality
  LOG_FINE() << " current_line.substr(0, 12) = " << current_line.substr(0, 12);
  if ("*mcmc_sample" == current_line.substr(0, 12)) {
    LOG_FINEST() << "skipping line as it is an input from an MCMC report " << current_line;
    getline(file_, current_line);
    ++line_number;
  }

  LOG_FINEST() << "current line: " << current_line;

  boost::replace_all(current_line, "\t", " ");
  boost::trim_all(current_line);
  boost::split(parameters, current_line, boost::is_any_of(" "));

  /**
   * Iterate through file
   */
  vector<string>          values;
  AddressableInputLoader& addressable_input_loader = *model_->managers()->addressable_input_loader();
  ++line_number;
  while (getline(file_, current_line)) {
    ++line_number;

    boost::replace_all(current_line, "\t", " ");
    boost::trim_all(current_line);
    LOG_FINEST() << "current_line " << line_number << " in addressable input file: " << current_line;

    boost::split(values, current_line, boost::is_any_of(" "));
    if (values.size() != parameters.size())
      LOG_FATAL() << "In addressable input file, line " << line_number << " has " << values.size() << " values when the number of parameters is " << parameters.size();

    double numeric = 0.0;
    for (unsigned i = 0; i < values.size(); ++i) {
      boost::trim_all(parameters[i]);
      boost::trim_all(values[i]);

      if (!utilities::To<double>(values[i], numeric))
        LOG_FATAL() << "In addressable input file could not convert the value " << values[i] << " to a double";
      addressable_input_loader.AddValue(parameters[i], numeric);
    }
  }

  file_.close();
}

} /* namespace configuration */
} /* namespace niwa */
