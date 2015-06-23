/**
 * @file EstimateValuesLoader.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/12/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 */

// headers
#include "EstimableValuesLoader.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Estimables/Estimables.h"
#include "Logging/Logging.h"
#include "Utilities/To.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace configuration {

using std::ifstream;
using std::cout;
using std::endl;
using std::vector;
using niwa::utilities::Double;

/**
 * Load the values of our estimates from the file provided
 *
 * @param file_name The name of the file containing the values
 */
void EstimableValuesLoader::LoadValues(const string& file_name) {
  ifstream file_;
  file_.open(file_name.c_str());
  if (file_.fail() || !file_.is_open())
    LOG_FATAL() << "Unable to open the estimate_value file: " << file_name << ". Does this file exist?";

  /**
   * Get the first line which should contain a list of parameters
   */
  string    current_line        = "";
  vector<string> parameters;
  if (!getline(file_, current_line) || current_line == "")
    LOG_FATAL() << "estimable value file appears to be empty, or the first line is blank. File: " << file_name;

  boost::replace_all(current_line, "\t", " ");
  boost::trim_all(current_line);
  boost::split(parameters, current_line, boost::is_any_of(","));

  /**
   * Iterate through file
   */
  unsigned line_number = 1;
  vector<string> values;
  EstimablesPtr estimables = Estimables::Instance();

  while (getline(file_, current_line)) {
    ++line_number;

    boost::replace_all(current_line, "\t", " ");
    boost::trim_all(current_line);
    LOG_FINEST() << "current_line " << line_number << " in estimate_values: " << current_line;

    boost::split(values, current_line, boost::is_any_of(","));
    if (values.size() != parameters.size())
      LOG_FATAL() << "In estimate_value file, line " << line_number << " has " << values.size() << " values when we expected " << parameters.size();
    for (unsigned i = 0; i < values.size(); ++i) {
      boost::trim_all(parameters[i]);
      boost::trim_all(values[i]);

      Double numeric = 0.0;
      if (!utilities::To<Double>(values[i], numeric))
        LOG_FATAL() << "In estimate_value file could not convert the value " << values[i] << " to a double";

      estimables->AddValue(parameters[i], numeric);
    }
  }

  file_.close();
}

} /* namespace configuration */
} /* namespace niwa */
