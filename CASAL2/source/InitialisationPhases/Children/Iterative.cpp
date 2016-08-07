/**
 * @file Iterative.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Iterative.h"

#include <algorithm>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Partition/Accessors/Categories.h"
#include "Processes/Manager.h"
#include "TimeSteps/Factory.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace initialisationphases {

namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;

/**
 * Default constructor
 */
Iterative::Iterative(Model* model)
  : InitialisationPhase(model),
    cached_partition_(model),
    partition_(model) {
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The number of iterations (years) over which to execute this initialisation phase", "");
  parameters_.Bind<string>(PARAM_INSERT_PROCESSES, &insert_processes_, "Additional processes not defined in the annual cycle, that are to beinserted into this initialisation phase", "", true);
  parameters_.Bind<string>(PARAM_EXCLUDE_PROCESSES, &exclude_processes_, "Processes in the annual cycle to be excluded from this initialisation phase", "", true);
  parameters_.Bind<unsigned>(PARAM_CONVERGENCE_YEARS, &convergence_years_, "The iteration (year) when the test for converegence (lambda) is evaluated", "", true);
  parameters_.Bind<Double>(PARAM_LAMBDA, &lambda_, "The maximum value of the absolute sum of differences (lambda) between the partition at year-1 and year that indicates successfull convergence", "", Double(0.0));
}

/**
 *
 */
void Iterative::DoValidate() {
  for (string insert : insert_processes_) {
    vector<string> pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);
    if (pieces.size() != 2 && pieces.size() != 3)
      LOG_ERROR_P(PARAM_INSERT_PROCESSES) << " value " << insert << " does not match the format time_step(process)=new_process = " << pieces.size();
  }
}

/**
 *
 */
void Iterative::DoBuild() {
  time_steps_ = model_->managers().time_step()->ordered_time_steps();

  // Set the default process labels for the time step for this phase
  for (auto time_step : time_steps_)
    time_step->SetInitialisationProcessLabels(label_, time_step->process_labels());

  // handle any new processes we want to insert
  for (string insert : insert_processes_) {
    vector<string> pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);

    string target_process   = pieces.size() == 3 ? pieces[1] : "";
    string new_process      = pieces.size() == 3 ? pieces[2] : pieces[1];

    auto time_step = model_->managers().time_step()->GetTimeStep(pieces[0]);
    vector<string> process_labels = time_step->initialisation_process_labels(label_);

    if (target_process == "") {
      process_labels.insert(process_labels.begin(), new_process);
    } else {
      vector<string>::iterator iter = std::find(process_labels.begin(), process_labels.end(), target_process);
      if (iter == process_labels.end())
        LOG_ERROR_P(PARAM_INSERT_PROCESSES) << " process " << target_process << " does not exist in time step " << time_step->label();
      process_labels.insert(iter, new_process);
    }

    time_step->SetInitialisationProcessLabels(label_, process_labels);
  }

  // handle the excludes we've specified
  for (string exclude : exclude_processes_) {
    unsigned count = 0;
    for (auto time_step : time_steps_) {
      vector<string> process_labels = time_step->initialisation_process_labels(label_);
      unsigned size_before = process_labels.size();
      process_labels.erase(std::remove_if(process_labels.begin(), process_labels.end(), [exclude](string& ex) { return exclude == ex; }), process_labels.end());
      unsigned diff = size_before - process_labels.size();

      time_step->SetInitialisationProcessLabels(label_, process_labels);
      count += diff;
    }

    if (count == 0)
      LOG_ERROR_P(PARAM_EXCLUDE_PROCESSES) << " process " << exclude << " does not exist in any time steps to be excluded. Please check your spelling";
  }

  if (convergence_years_.size() != 0) {
    std::sort(convergence_years_.begin(), convergence_years_.end());
    if ((*convergence_years_.rbegin()) != years_)
      convergence_years_.push_back(years_);
  }

  // Build our partition
  vector<string> categories = model_->categories()->category_names();
  partition_.Init(categories);
  cached_partition_.Init(categories);


}

/**
 * Execute our iterative initialisation phases.
 */
void Iterative::Execute() {
  if (convergence_years_.size() == 0) {
    timesteps::Manager& time_step_manager = *model_->managers().time_step();
    time_step_manager.ExecuteInitialisation(label_, years_);

  } else {
    unsigned total_years = 0;
    for (unsigned years : convergence_years_) {
      timesteps::Manager& time_step_manager = *model_->managers().time_step();
      time_step_manager.ExecuteInitialisation(label_, years - (total_years + 1));


      total_years += years - (total_years + 1);
      if ((total_years + 1) >= years_) {
        time_step_manager.ExecuteInitialisation(label_, 1);
        break;
      }

      cached_partition_.BuildCache();
      time_step_manager.ExecuteInitialisation(label_, 1);
      ++total_years;

      if (CheckConvergence()) {
        LOG_FINEST() << " year Convergence was reached = " << years;
        break;
      }
      LOG_FINEST() << "Initial year = " << years;
    }
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
