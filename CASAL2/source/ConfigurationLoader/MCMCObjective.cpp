/**
 * @file MCMCObjective.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
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
  while (line != "starting_covariance_matrix {m}") {
    if (!std::getline(file, line)) {
      LOG_ERROR() << "Failed to read a line from the MCMC Objective file when looking for 'covariance_matrix:'";
      return false;
    }
  }

  if (line != "starting_covariance_matrix {m}") {
    LOG_ERROR() << "Could not file 'starting_covariance_matrix {m}' string in MCMC objective file: " << file_name;
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
    vector<string> addressable_values;
    boost::split(addressable_values, line, boost::is_any_of(" "), boost::token_compress_on);
    if (estimate_count != addressable_values.size()) {
      LOG_ERROR() << "Line " << i+1 << " of the covariance matrix had " << addressable_values.size()
          << " values when we expected " << estimate_count << " to match number of estimates";
      return false;
    }

    for (unsigned j = 0; j < estimate_count; ++j) {
      LOG_FINE() << "i: " << i << ", j: " << j << ", value: " << addressable_values[j];
      Double value = 0;
      if (!utilities::To<string, Double>(addressable_values[j], value)) {
        LOG_ERROR() << "MCMC Objective file " << file_name << " is not in the correct format."
            << " Value " << addressable_values[j] << " could not be converted to a numeric";
        return false;
      }

      covariance_matrix(i, j) = value;
    }
  }

  /**
   * Find the last line of the file to find the iteration number
   */
  string last_line = "";
  while(getline(file, line)) {
    last_line = line;
  }

  vector<string> Chain_info;
  boost::split(Chain_info, last_line, boost::is_any_of(" "), boost::token_compress_on);

  unsigned iteration_number = 0;
  if (!utilities::To<string, unsigned>(Chain_info[0], iteration_number)) {
    LOG_ERROR() << "Could not convert " << Chain_info[0] << " to an unsigned";
    return false;
  }

  // Acceptance rate
  Double AR = 0;
  if (!utilities::To<string, Double>(Chain_info[8], AR)) {
    LOG_ERROR() << "Could not convert " << Chain_info[8] << " to a Double";
    return false;
  }
  LOG_FINE() << "Acceptance rate = " << AR;
  // Now calculate successful jumps
  Double succesful_jumps = (Double)iteration_number * AR;
  LOG_FINE() << "Successful jumps = " << succesful_jumps;

  unsigned success_jump;
  if (!utilities::To<Double, unsigned>(succesful_jumps, success_jump)) {
    LOG_ERROR() << "Could not convert " << succesful_jumps << " to an unsigned";
    return false;
  }
  // Acceptance rate since last adapt
   Double AR_since_last_adapt = 0;
   if (!utilities::To<string, Double>(Chain_info[9], AR_since_last_adapt)) {
     LOG_ERROR() << "Could not convert " << Chain_info[9] << " to a Double";
     return false;
   }

  // step size
  Double step_size = 0;
  if (!utilities::To<string, Double>(Chain_info[7], step_size)) {
    LOG_ERROR() << "Could not convert " << Chain_info[7] << " to an Double";
    return false;
  }
  LOG_FINE() << "step size = " << step_size;

  model_->managers().mcmc()->active_mcmc()->set_starting_iteration(iteration_number);
  model_->managers().mcmc()->active_mcmc()->set_succeful_jumps(success_jump);
  model_->managers().mcmc()->active_mcmc()->set_step_size(step_size);
  model_->managers().mcmc()->active_mcmc()->set_acceptance_rate_from_last_adapt(AR_since_last_adapt);


  file.close();
  return true;
}

} /* namespace configuration */
} /* namespace niwa */
