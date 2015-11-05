/**
 * @file MCMCObjective.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "MCMCObjective.h"

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
MCMCObjective::MCMCObjective(Model* model) : model_(model) {
}

/**
 *
 */
bool MCMCObjective::LoadFile(const string& file_name) {
  // open file
  ifstream  file(file_name.c_str());
  if (file.fail() || !file.is_open()) {
    LOG_ERROR() << file_name  << " does not exist or could not be opened";
    return false;
  }

  /**
   * look for the text "covariance_matrix:"
   * as this is the expected line before our variance matrix
   */
  string line = "";
  while (line != "covariance_matrix:") {
    if (!std::getline(file, line)) {
      LOG_ERROR() << "Failed to read a line from the MCMC Objective file when looking for 'covariance_matrix:'";
      return false;
    }
  }

  if (line != "covariance_matrix:") {
    LOG_ERROR() << "Could not file 'covariance_matrix:' string in MCMC objective file: " << file_name;
    return false;
  }

  auto estimate_count      = model_->managers().estimate()->GetEnabledCount();
  auto& covariance_matrix  = model_->managers().mcmc()->active_mcmc()->covariance_matrix();
  covariance_matrix.resize(estimate_count, estimate_count);
  for (unsigned i = 0; i < estimate_count; ++i) {
    if (!getline(file, line)) {
      LOG_ERROR() << "Failed to load line " << i+1 << " of the covariance matrix from the file: " << file_name;
    }

    // split the line
    vector<string> estimable_values;
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
        LOG_ERROR() << "MCMC Objective file " << file_name << " is not in the correct format."
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
