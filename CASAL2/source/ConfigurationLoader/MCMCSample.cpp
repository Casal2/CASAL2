/**
 * @file MCMCSample.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

#include "MCMCSample.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "../Estimates/Manager.h"
#include "../MCMCs/Manager.h"
#include "../Minimisers/Manager.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Model/Objects.h"
#include "../Utilities/To.h"

// namespaces
namespace niwa {
namespace configuration {
namespace ublas = boost::numeric::ublas;

using std::cout;
using std::endl;
using std::ifstream;
using std::ios_base;
using std::ofstream;
using std::streambuf;

/**
 * Default constructor
 */
MCMCSample::MCMCSample(shared_ptr<Model> model) : model_(model) {}

/**
 * This method loads the 'mcmc_sample' file
 *
 * @param file_name The name of the file
 * @return true if the file was parsed, false if not
 */
bool MCMCSample::LoadFile(const string& file_name) {
  // open file
  ifstream file(file_name.c_str());
  if (file.fail() || !file.is_open()) {
    LOG_ERROR() << "File " << file_name << " does not exist or could not be opened";
    return false;
  }

  /**
   * look for the text "mcmc_sample:"
   * as this is the expected line before our sample lines
   */
  string line = "";
  while (line != "*mcmc_sample[mcmc]") {
    if (!getline(file, line)) {
      LOG_ERROR() << "Failed to read a line from the MCMC Sample file when looking for '*mcmc_sample[mcmc]'";
      return false;
    }
    boost::trim_right(line);
  }

  if (line != "*mcmc_sample[mcmc]") {
    LOG_ERROR() << "Could not read '*mcmc_sample[mcmc]' string in MCMC Sample file " << file_name;
    return false;
  }
  LOG_MEDIUM() << "line = " << line;

  /*
  // Skip the next line which is needed for reading into R
  while (line != ("values " + (string)REPORT_R_MATRIX) {
    if (!getline(file, line)) {
      LOG_ERROR() << "Failed to read a line from the MCMC Sample file when looking for 'values '" << REPORT_R_MATRIX << "'";
      return false;
    }
  }

  if (line != ("values " + (string)REPORT_R_MATRIX)) {
     LOG_ERROR() << "Could not read 'values '" << REPORT_R_MATRIX << "' string in MCMC Sample file " << file_name;
     return false;
   }

  */

  /**
   * read the columns from our header
   */
  if (!getline(file, line)) {
    LOG_ERROR() << "Could not read header line from MCMC sample file " << file_name;
    return false;
  }
  boost::trim_right(line);

  // Check the order of parameters
  auto estimate_count = model_->managers()->estimate()->GetIsEstimatedCount();
  auto estimates      = model_->managers()->estimate()->GetIsEstimated();
  LOG_MEDIUM() << "Check the order of parameters";
  vector<string> param_labels;
  boost::split(param_labels, line, boost::is_any_of(" "), boost::token_compress_on);
  if (estimate_count != param_labels.size()) {
    LOG_ERROR() << "The covariance parameter header labels had " << param_labels.size() << " values when the number of estimated parameters is " << estimate_count;
    return false;
  }

  for (unsigned i = 0; i < param_labels.size(); ++i) {
    if (param_labels[i] != estimates[i]->parameter())
      LOG_ERROR() << "parameter " << param_labels[i] << " is not matched with internal estimate parameters which expected " << estimates[i]->label();
  }

  vector<string> columns;
  boost::split(columns, line, boost::is_any_of(" "), boost::token_compress_on);

  /**
   * Find the last line of the file
   */
  string last_line = "";
  while (getline(file, line)) {
    boost::trim_right(line);
    last_line = line;
  }

  if (last_line == REPORT_END) {
    LOG_ERROR() << "MCMC sample file " << file_name << " found '" << REPORT_END << "'. Returning 'false'";
    return false;
  }

  /**
   * Split last line in to a vector
   */
  vector<string> addressable_values;
  boost::split(addressable_values, last_line, boost::is_any_of(" "), boost::token_compress_on);
  if (addressable_values.size() != columns.size()) {
    LOG_ERROR() << "Number of values provided in the sample file (" << addressable_values.size() << ") does not match the columns provided in the header line (" << columns.size()
                << ")";
    return false;
  }

  /**
   * Apply the values to our estimates
   */
  // auto estimates     = model_->managers().estimate()->GetIsEstimated();
  if (estimates.size() != columns.size()) {
    LOG_ERROR() << "Model has " << estimates.size() << " estimates and the MCMC sample file has " << columns.size() << " columns.";
    return false;
  }

  double value = 0.0;
  for (unsigned i = 0; i < columns.size(); ++i) {
    if (!utilities::To<string, double>(addressable_values[i], value)) {
      LOG_ERROR() << " Value " << addressable_values[i] << " could not be converted to a double";
      return false;
    }

    if (estimates[i]->parameter() != columns[i]) {
      LOG_ERROR() << " parameter " << estimates[i]->parameter() << " was not found in column " << columns[i];
      return false;
    }

    estimates[i]->set_value(value);
    LOG_MEDIUM() << "MCMC sample starting value for parameter " << estimates[i]->parameter() << " is " << value;
  }

  file.close();
  return true;
}

} /* namespace configuration */
} /* namespace niwa */
