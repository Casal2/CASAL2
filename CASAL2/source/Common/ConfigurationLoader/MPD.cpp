/**
 * @file MPD.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Feb 9, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

// headers
#include "MPD.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Common/Estimables/Estimables.h"
#include "Common/Estimates/Manager.h"
#include "Common/Logging/Logging.h"
#include "Common/MCMCs/Manager.h"
#include "Common/Model/Managers.h"
#include "Common/Model/Model.h"
#include "Common/Utilities/To.h"
#include "Common/Utilities/Types.h"

// namespaces
namespace niwa {
namespace configuration {
using std::ifstream;
using std::cout;
using std::endl;
using std::vector;
using niwa::utilities::Double;

/**
 * Load our MPD file
 */
bool MPD::LoadFile(const string& file_name) {
  ifstream file;
  file.open(file_name.c_str());
  if (file.fail() || !file.is_open())
    LOG_FATAL() << "Unable to open the estimate_value file: " << file_name << ". Does this file exist?";

  // first line should be: * MPD
  string line = "";
  if (!getline(file, line) || line == "")
    LOG_FATAL() << "MPD file appears to be empty, or the first line is blank. File: " << file_name;
  if (line != "* MPD")
    LOG_FATAL() << "MPD first line should be '* MPD'. But it was " << line << " in file: " << file_name;

  // estimate_values line
  if (!getline(file, line) || line == "")
    LOG_FATAL() << "MPD file appears to be empty, or the first line is blank. File: " << file_name;
  if (line != "estimate_values:")
    LOG_FATAL() << "MPD third line should be 'estimate_values:'. But it was " << line << " in file: " << file_name;

  /**
   * Get the first line which should contain a list of parameters
   */
  vector<string> parameters;
  if (!getline(file, line) || line == "")
    LOG_FATAL() << "MPD file appears to be empty, or the parameter line is blank. File: " << file_name;

  boost::replace_all(line, "\t", " ");
  boost::trim_all(line);
  boost::split(parameters, line, boost::is_any_of(" "));

  // load the values
  if (!getline(file, line) || line == "")
    LOG_FATAL() << "MPD file appears to be empty, or the parameter line is blank. File: " << file_name;

  boost::replace_all(line, "\t", " ");
  boost::trim_all(line);

  vector<string> values;
  boost::split(values, line, boost::is_any_of(" "));
  if (values.size() != parameters.size())
    LOG_FATAL() << "In estimate_value file, estimate values has " << values.size() << " values when we expected " << parameters.size();

  for (unsigned i = 0; i < values.size(); ++i) {
    boost::trim_all(parameters[i]);
    boost::trim_all(values[i]);

    Double numeric = 0.0;
    if (!utilities::To<Double>(values[i], numeric))
      LOG_FATAL() << "In estimate_value file could not convert the value " << values[i] << " to a double";

    auto estimate      = model_->managers().estimate()->GetEstimate(parameters[i]);
    if (!estimate)
      LOG_FATAL() << "Estimate " << parameters[i] << " was defined in MPD file but could not be found";

    estimate->set_value(numeric);
  }

  // Load the Covariance
  if (!getline(file, line) || line == "")
    LOG_FATAL() << "MPD file appears to be empty, or the covariance_matrix line is invalid. File: " << file_name;

  if (line != "covariance_matrix:") {
    LOG_ERROR() << "Could not file 'covariance_matrix:' string in MPD file: " << file_name;
    return false;
  }

  auto estimate_count      = model_->managers().estimate()->GetIsEstimatedCount();
  auto& covariance_matrix  = model_->managers().mcmc()->active_mcmc()->covariance_matrix();
  covariance_matrix.resize(estimate_count, estimate_count);
  for (unsigned i = 0; i < estimate_count; ++i) {
    if (!getline(file, line)) {
      LOG_ERROR() << "Failed to load line " << i+1 << " of the covariance matrix from the file: " << file_name;
    }

    // split the line
    vector<string> estimable_values;
    boost::trim_all(line);
    boost::split(estimable_values, line, boost::is_any_of(" "), boost::token_compress_on);
    if (estimate_count != estimable_values.size()) {
      LOG_ERROR() << "Line " << i+1 << " of the covariance matrix had " << estimable_values.size()
               << " values when we expected " << estimate_count << " to match number of estimates";
      return false;
    }

    for (unsigned j = 0; j < estimate_count; ++j) {
      LOG_FINE() << "i: " << i << ", j: " << j << ", value: " << estimable_values[j];
      Double value = 0;
      if (!utilities::To<string, Double>(estimable_values[j], value)) {
        LOG_ERROR() << "MPD file " << file_name << " is not in the correct format."
            << " Value " << estimable_values[j] << " could not be converted to a numeric";
        return false;
      }

      covariance_matrix(i, j) = value;
    }
  }

  file.close();
  return true;
}

} /* namespace configuration */
} /* namespace niwa */
