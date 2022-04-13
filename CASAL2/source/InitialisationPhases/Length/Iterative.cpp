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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../../Categories/Categories.h"
#include "../../DerivedQuantities/Manager.h"
#include "../../Partition/Accessors/Categories.h"
#include "../../Processes/Manager.h"
#include "../../TimeSteps/Factory.h"
#include "../../TimeSteps/Manager.h"
#include "../../Processes/Length/RecruitmentBevertonHolt.h"

// namespaces
namespace niwa {
namespace initialisationphases {
namespace length {

namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;

/**
 * Default constructor
 */
Iterative::Iterative(shared_ptr<Model> model) : InitialisationPhase(model), cached_partition_(model), partition_(model) {
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The number of iterations (years) over which to execute this initialisation phase", "");
  parameters_.Bind<string>(PARAM_INSERT_PROCESSES, &insert_processes_, "The processes in the annual cycle to be included in this initialisation phase", "", true);
  parameters_.Bind<string>(PARAM_EXCLUDE_PROCESSES, &exclude_processes_, "The processes in the annual cycle to be excluded from this initialisation phase", "", true);
  parameters_.Bind<unsigned>(PARAM_CONVERGENCE_YEARS, &convergence_years_, "The iteration (year) when the test for converegence (lambda) is evaluated", "", true);
  parameters_.Bind<Double>(PARAM_LAMBDA, &lambda_,
                           "The maximum value of the absolute sum of differences (lambda) between the partition at year-1 and year that indicates successful convergence", "",
                           Double(0.0));
}

/**
 * Validate
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
 * Build
 */
void Iterative::DoBuild() {
  LOG_TRACE();
  time_steps_ = model_->managers()->time_step()->ordered_time_steps();

  // Set the default process labels for the time step for this phase
  for (auto time_step : time_steps_) time_step->SetInitialisationProcessLabels(label_, time_step->process_labels());

  // handle any new processes we want to insert
  for (string insert : insert_processes_) {
    vector<string> pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);

    string target_process = pieces.size() == 3 ? pieces[1] : "";
    string new_process    = pieces.size() == 3 ? pieces[2] : pieces[1];

    auto           time_step      = model_->managers()->time_step()->GetTimeStep(pieces[0]);
    vector<string> process_labels = time_step->initialisation_process_labels(label_);

    if (target_process == "") {
      process_labels.push_back(new_process);
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
      unsigned       size_before    = process_labels.size();
      process_labels.erase(std::remove_if(process_labels.begin(), process_labels.end(), [exclude](string& ex) { return exclude == ex; }), process_labels.end());
      unsigned diff = size_before - process_labels.size();

      time_step->SetInitialisationProcessLabels(label_, process_labels);
      count += diff;
    }

    if (count == 0)
      LOG_ERROR_P(PARAM_EXCLUDE_PROCESSES) << " process " << exclude << " does not exist in any time steps to be excluded.";
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

  // Find any BH_recruitment process in the annual cycle
  unsigned i = 0;
  for (auto time_step : model_->managers()->time_step()->ordered_time_steps()) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kRecruitment && process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT) {
        LOG_FINEST() << "Found a BevertonHolt process";
        recruitment_process_.push_back(dynamic_cast<length::RecruitmentBevertonHolt*>(process));
        if (!recruitment_process_[i])
          LOG_CODE_ERROR() << "BevertonHolt Recruitment exists but dynamic cast pointer cannot be made, if (!recruitment) ";
        i++;
      } 
    }
  }
}

/**
 * Execute the iterative initialisation phases
 */
void Iterative::Execute() {
  LOG_TRACE();
  timesteps::Manager& time_step_manager = *model_->managers()->time_step();
  if (convergence_years_.size() == 0) {
    time_step_manager.ExecuteInitialisation(label_, years_);

  } else {
    unsigned total_years = 0;
    for (unsigned years : convergence_years_) {
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
  // Check if we have B0 initialised or R0 initialised recruitment
  bool B0_initial_recruitment = false;
  for (auto recruitment_process : recruitment_process_) {
    if (recruitment_process->b0_initialised()) {
      LOG_FINEST() << PARAM_B0 << " has been defined for process labelled " << recruitment_process->label();
      recruitment_process->ScalePartition();
      B0_initial_recruitment = true;
    }
  }
  // if so re-run the model and populate model quantities with scaled values
  if (B0_initial_recruitment) {
    LOG_FINE() << "B0 initialised";
    // Calculate derived quantities in the right space if we have a B0 initialised model
    time_step_manager.ExecuteInitialisation(label_, 1);
  }
}

/**
 * Check for convergence on the partition and return true if it exceeds the
 * lambda threshold to quit early and save time
 *
 * @return True if convergence, false otherwise
 */
bool Iterative::CheckConvergence() {
  LOG_TRACE();
  Double variance = 0.0;
  auto cached_category = cached_partition_.begin();
  auto category        = partition_.begin();

  for (; category != partition_.end(); ++cached_category, ++category) {
    Double sum = (*category)->data_[(*category)->data_.size() - 1];
    if (sum == 0.0)
      return false;
    // focus on the plus group
    variance += fabs((*cached_category).data_[(*cached_category).data_.size() - 1] - (*category)->data_[(*category)->data_.size() - 1]) / sum;
  }

  if (variance < lambda_)
    return true;

  return false;
}

} /* namespace length */
}  // namespace initialisationphases
} /* namespace niwa */
