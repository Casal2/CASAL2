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

#include "Common/Estimates/Manager.h"
#include "Common/MCMCs/Manager.h"
#include "Common/Minimisers/Manager.h"
#include "Common/Model/Managers.h"
#include "Common/Model/Model.h"
#include "Common/Model/Objects.h"
#include "Common/Utilities/To.h"

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
  while (line != "*mcmc (mcmc_sample)") {
    if (!getline(file, line)) {
      LOG_ERROR() << "Failed to read a line from the MCMC Sample file when looking for '*mcmc (mcmc_sample)'";
      return false;
    }
  }
  LOG_FINEST() << "line = " << line;


  if (line != "*mcmc (mcmc_sample)") {
    LOG_ERROR() << "Could not read '*mcmc (mcmc_sample)' string in MCMC Sample file: " << file_name;
    return false;
  }
  LOG_FINEST() << "line = " << line;

  // Skip the next line which is needed for reading into R
  while (line != "values {d}") {
    if (!getline(file, line)) {
      LOG_ERROR() << "Failed to read a line from the MCMC Sample file when looking for 'values {d}'";
      return false;
    }
  }

  if (line != "values {d}") {
     LOG_ERROR() << "Could not read 'values {d}' string in MCMC Sample file: " << file_name;
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
  vector<string> addressable_values;
  boost::split(addressable_values, last_line, boost::is_any_of(" "), boost::token_compress_on);
  if (addressable_values.size() != columns.size()) {
    LOG_ERROR() << "Number of values provided in the sample file (" << addressable_values.size()
        << ") does not match the columns provided in the header line (" << columns.size() << ")";
    return false;
  }

  /**
   * Apply the values to our estimates
   */
  auto estimates     = model_->managers().estimate()->GetIsEstimated();
  if (estimates.size() != columns.size()) {
    LOG_ERROR() << "Model has " << estimates.size() << " estimates and the MCMC sample file has "
        << columns.size();
    return false;
  }

  for (unsigned i = 0; i < columns.size(); ++i) {
    Double value = 0.0;
    if (!utilities::To<string, Double>(addressable_values[i], value)) {
      LOG_ERROR() << "";
      return false;
    }

    if (estimates[i]->parameter() != columns[i]) {
      LOG_ERROR() << " parameter " << estimates[i]->parameter() << " was not found in column " << columns[i];
      return false;
    }

    estimates[i]->set_value(value);
  }

  file.close();
  return true;
}

} /* namespace configuration */
} /* namespace niwa */
