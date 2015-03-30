/**
 * @file Iterative.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Iterative.h"

#include <algorithm>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Partition/Accessors/Age/Categories.h"
#include "Processes/Manager.h"
#include "TimeSteps/Factory.h"

// namespaces
namespace niwa {
namespace initialisationphases {

namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;

/**
 * Default constructor
 */
Iterative::Iterative() {
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The number of iterations to execute this phase for", "");
  parameters_.Bind<string>(PARAM_INSERT_PROCESSES, &insert_processes_, "The processes to insert in to target time steps", "", true);
  parameters_.Bind<string>(PARAM_EXCLUDE_PROCESSES, &exclude_processes_, "The processes to exclude from all time steps", "", true);
  parameters_.Bind<unsigned>(PARAM_CONVERGENCE_YEARS, &convergence_years_, "The years to test for convergence", "", true);
  parameters_.Bind<Double>(PARAM_LAMBDA, &lambda_, "Lambda", "", Double(0.0));
}

/**
 *
 */
void Iterative::DoValidate() {
  for (string insert : insert_processes_) {
    vector<string> pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);
    if (pieces.size() != 2 && pieces.size() != 3)
      LOG_ERROR(parameters_.location(PARAM_INSERT_PROCESSES) << " value " << insert << " does not match the format time_step(process)=new_process = " << pieces.size());
  }
}

/**
 *
 */
void Iterative::DoBuild() {
  time_steps_ = timesteps::Manager::Instance().time_steps();

  // Set the default process labels for the time step for this phase
  for (TimeStepPtr time_step : time_steps_)
    time_step->SetInitialisationProcessLabels(label_, time_step->process_labels());

  // handle any new processes we want to insert
  for (string insert : insert_processes_) {
    vector<string> pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);

    string target_process   = pieces.size() == 3 ? pieces[1] : "";
    string new_process      = pieces.size() == 3 ? pieces[2] : pieces[1];

    TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(pieces[0]);
    vector<string> process_labels = time_step->initialisation_process_labels(label_);

    if (target_process == "") {
      process_labels.insert(process_labels.begin(), new_process);
    } else {
      vector<string>::iterator iter = std::find(process_labels.begin(), process_labels.end(), target_process);
      if (iter == process_labels.end())
        LOG_ERROR(parameters_.location(PARAM_INSERT_PROCESSES) << " process " << target_process << " does not exist in time step " << time_step->label());
      process_labels.insert(iter, new_process);
    }

    time_step->SetInitialisationProcessLabels(label_, process_labels);
  }

  // handle the excludes we've specified
  for (string exclude : exclude_processes_) {
    unsigned count = 0;
    for (TimeStepPtr time_step : time_steps_) {
      vector<string> process_labels = time_step->initialisation_process_labels(label_);
      unsigned size_before = process_labels.size();
      process_labels.erase(std::remove_if(process_labels.begin(), process_labels.end(), [exclude](string& ex) { return exclude == ex; }), process_labels.end());
      unsigned diff = size_before - process_labels.size();

      time_step->SetInitialisationProcessLabels(label_, process_labels);
      count += diff;
    }

    if (count == 0)
      LOG_ERROR(parameters_.location(PARAM_EXCLUDE_PROCESSES) << " process " << exclude << " does not exist in any time steps to be excluded. Please check your spelling");
  }

  if (convergence_years_.size() == 0)
    convergence_years_.push_back(years_);
  else
    std::sort(convergence_years_.begin(), convergence_years_.end());

  // Build our partition
  vector<string> categories = Categories::Instance()->category_names();
  partition_.Init(categories);
  cached_partition_.Init(categories);


}

/**
 * Execute our iterative initialisation phases.
 */
void Iterative::Execute() {

  unsigned total_years = 0;
  for (unsigned years : convergence_years_) {
    timesteps::Manager& time_step_manager = timesteps::Manager::Instance();
    time_step_manager.ExecuteInitialisation(label_, (years - total_years) - 1);
    LOG_INFO("Exec: " << (years - total_years) << " years");

    total_years += (years - total_years);
    LOG_INFO("Total years: " << total_years);
    if (total_years >= years_)
      break;

    cached_partition_.BuildCache();
    time_step_manager.ExecuteInitialisation(label_, 1);
    if (CheckConvergence())
      break;
  }
}

/**
 * Check for convergence on our partition and return true if it exceeds the
 * lambda threshold so we can quit early and save time.
 *
 * @return True if convergence, False otherwise
 */
bool Iterative::CheckConvergence() {
  LOG_TRACE();

  Double variance = 0.0;

  auto cached_category = cached_partition_.begin();
  auto category = partition_.begin();

  for (; category != partition_.end(); ++cached_category, ++category) {
    Double sum = 0.0;
    for (Double value : (*category)->data_)
      sum += value; // Can't use std::accum because of AutoDiff
    if (sum == 0.0)
      return false;

    for (unsigned i = 0; i < (*category)->data_.size(); ++i)
      variance += fabs(cached_category->data_[i] - (*category)->data_[i]) / sum;
  }

  if (variance < lambda_)
    return true;

  return false;
}


} /* namespace initialisationphases */
} /* namespace niwa */
