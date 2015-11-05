/**
 * @file MCMCSample.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

#include "MCMCSample.h"

#include <string>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "Estimates/Manager.h"
#include "MCMCs/Manager.h"
#include "Minimisers/Manager.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Model/Objects.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace configuration {
namespace ublas = boost::numeric::ublas;

using std::streambuf;
using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;
using std::ios_base;
/**
 *
 */
MCMCSample::MCMCSample(Model* model) : model_(model) {
}

/**
 *
 */
bool MCMCSample::LoadFile(const string& file_name) {
  // open file
  ifstream  file(file_name.c_str());
  if (file.fail() || !file.is_open()) {
    LOG_ERROR() << file_name  << " does not exist or could not be opened";
    return false;
  }

  /**
   * look for the text "mcmc_sample:"
   * as this is the expected line before our sample lines
   */
  string line = "";
  while (line != "*mcmc_sample") {
    if (!getline(file, line)) {
      LOG_ERROR() << "Failed to read a line from the MCMC Sample file when looking for '*mcmc_sample'";
      return false;
    }
  }

  if (line != "*mcmc_sample") {
    LOG_ERROR() << "Could not read '*mcmc_sample' string in MCMC Sample file: " << file_name;
    return false;
  }

  /**
   * read the columns from our header
   */
  if (!getline(file, line)) {
    LOG_ERROR() << "Could not read header line from MCMC sample file: " << file_name;
    return false;
  }

  vector<string> columns;
  boost::split(columns, line, boost::is_any_of(" "), boost::token_compress_on);

  /**
   * Find the last line of the file
   */
  string last_line = "";
  while(getline(file, line)) {
    last_line = line;
  }

  if (last_line == "*end") {
    LOG_ERROR() << "MCMC sample file " << file_name << " is complete. Cannot resume from a complete file";
    return false;
  }

  /**
   * Split last line in to a vector
   */
  vector<string> estimable_values;
  boost::split(estimable_values, last_line, boost::is_any_of(" "), boost::token_compress_on);
  if (estimable_values.size() != columns.size()) {
    LOG_ERROR() << "Number of values provided in the sample file (" << estimable_values.size()
        << ") does not match the columns provided in the header line (" << columns.size() << ")";
    return false;
  }

  /**
   * Apply the values to our estimates
   */
  auto estimates     = model_->managers().estimate()->GetEnabled();
  if (estimates.size() != columns.size() - 1) {
    LOG_ERROR() << "Model has " << estimates.size() << " estimates and the MCMC sample file has "
        << columns.size() - 1;
    return false;
  }

  for (unsigned i = 1; i < columns.size(); ++i) {
    Double value = 0.0;
    if (!utilities::To<string, Double>(estimable_values[i], value)) {
      LOG_ERROR() << "";
      return false;
    }

    if (estimates[i - 1]->parameter() != columns[i]) {
      LOG_ERROR() << " parameter " << estimates[i - 1]->parameter() << " was not found in column " << columns[i];
      return false;
    }

    estimates[i - 1]->set_value(value);
  }

  unsigned iteration_number = 0;
  if (!utilities::To<string, unsigned>(estimable_values[0], iteration_number)) {
    LOG_ERROR() << "Could not convert " << estimable_values[0] << " to an unsigned";
    return false;
  }

  model_->managers().mcmc()->active_mcmc()->set_starting_iteration(iteration_number);

  file.close();
  return true;
}

} /* namespace configuration */
} /* namespace niwa */
