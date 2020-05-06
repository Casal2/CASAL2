/**
 * @file Derived.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Derived.h"

#include <algorithm>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Categories/Categories.h"
#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "TimeSteps/Manager.h"
#include "Processes/Age/RecruitmentBevertonHolt.h"
#include "Processes/Age/RecruitmentBevertonHoltWithDeviations.h"

// namespaces
namespace niwa {
namespace initialisationphases {
namespace age {

/**
 * Default constructor
 */
Derived::Derived(Model* model) :
    InitialisationPhase(model), cached_partition_(model), partition_(model) {
  parameters_.Bind<string>(PARAM_INSERT_PROCESSES, &insert_processes_, "Additional processes not defined in the annual cycle that are to be inserted into this initialisation phase", "", true);
  parameters_.Bind<string>(PARAM_EXCLUDE_PROCESSES, &exclude_processes_, "Processes in the annual cycle to be excluded from this initialisation phase", "", true);
  parameters_.Bind<bool>(PARAM_CASAL_INITIALISATION, &casal_initialisation_phase_, "Run an extra annual cycle to evalaute equilibrium SSBs. Warning - if true, this may not correctly evaluate the equilibrium state. Set to true if replicating a CASAL model", "", false);

}

/**
 * Validate the format of insert_processes if any are given
 */
void Derived::DoValidate() {
  for (string insert : insert_processes_) {
    vector<string> pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);
    if (pieces.size() != 2 && pieces.size() != 3)
      LOG_ERROR_P(PARAM_INSERT_PROCESSES) << " value " << insert << " does not match the format time_step(process)=new_process = " << pieces.size();
  }

}

/**
 * Build any runtime relationships needed for execution
 */
void Derived::DoBuild() {
  time_steps_ = model_->managers().time_step()->ordered_time_steps();

  // handle any new processes we want to insert
  for (string insert : insert_processes_) {
    vector < string > pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);

    string target_process = pieces.size() == 3 ? pieces[1] : "";
    string new_process = pieces.size() == 3 ? pieces[2] : pieces[1];

    auto time_step = model_->managers().time_step()->GetTimeStep(pieces[0]);
    vector < string > process_labels = time_step->initialisation_process_labels(label_);

    if (target_process == "")
      process_labels.push_back(new_process);
    else {
      auto iter = std::find(process_labels.begin(), process_labels.end(), target_process);
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
      vector < string > process_labels = time_step->initialisation_process_labels(label_);
      unsigned size_before = process_labels.size();
      process_labels.erase(std::remove_if(process_labels.begin(), process_labels.end(), [exclude](string& ex) {return exclude == ex;}),
          process_labels.end());
      unsigned diff = size_before - process_labels.size();

      time_step->SetInitialisationProcessLabels(label_, process_labels);
      count += diff;
    }

    if (count == 0)
      LOG_ERROR_P(PARAM_EXCLUDE_PROCESSES) << " process " << exclude << " does not exist in any time steps to be excluded";
  }

  // Build our partition
  vector < string > categories = model_->categories()->category_names();

  partition_.Init(categories);
  cached_partition_.Init(categories);

  // Find out the recruitment and ageing order
  vector<ProcessType> process_types = model_->managers().time_step()->GetOrderedProcessTypes();
  unsigned ageing_index = std::numeric_limits<unsigned>::max();
  unsigned recruitment_index = std::numeric_limits<unsigned>::max();
  for (unsigned i = 0; i < process_types.size(); ++i) {
    if (process_types[i] == ProcessType::kAgeing) {
      ageing_index = i;
    } else if (process_types[i] == ProcessType::kRecruitment) {
      recruitment_index = i;
    }
  }
  if (ageing_index == std::numeric_limits<unsigned>::max())
    LOG_ERROR() << location() << "Unable to calculate the recruitment index because there is no ageing process";
  if (recruitment_index < ageing_index)
    recruitment_ = true;

  // Find any BH_recruitment process in the annual cycle
  unsigned i = 0;
  for (auto time_step : model_->managers().time_step()->ordered_time_steps()) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kRecruitment && process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT) {
        LOG_FINEST() << "Found a BevertonHolt process";
        recruitment_process_.push_back(dynamic_cast<RecruitmentBevertonHolt*>(process));
        if (!recruitment_process_[i])
          LOG_CODE_ERROR() << "BevertonHolt Recruitment exists but dynamic cast pointer cannot be made, if (!recruitment) ";
        i++;
      } else if (process->process_type() == ProcessType::kRecruitment && process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT_WITH_DEVIATIONS) {
        LOG_FINEST() << "Found a BevertonHolt process";
        recruitment_process_with_devs_.push_back(dynamic_cast<RecruitmentBevertonHoltWithDeviations*>(process));
        if (!recruitment_process_with_devs_[i])
          LOG_CODE_ERROR() << "BevertonHolt Recruitment with deviations exists but dynamic cast pointer cannot be made, if (!recruitment) ";
        i++;
      }
    }
  }
}

/**
 * Execute the Derived Initialisation phase
 */
void Derived::Execute() {
  unsigned year_range = model_->age_spread();

  if (recruitment_)
    year_range -= 1;

  vector<string> categories = model_->categories()->category_names();

  LOG_FINEST() << "running intialisation for " << year_range << " years";
  timesteps::Manager* time_step_manager = model_->managers().time_step();
  time_step_manager->ExecuteInitialisation(label_, year_range);

  // a shortcut to avoid running the model over more years to get the plus group right
  // calculate the annual change c for each element of the plus group
  if (model_->age_plus()) {
    cached_partition_.BuildCache();
    // Run the model for an extra year
    time_step_manager->ExecuteInitialisation(label_, 1);
    Double c;
    auto cached_category = cached_partition_.begin();
    auto category = partition_.begin();
    for (; category != partition_.end(); ++cached_category, ++category) {
      // find the element in the data container (numbers at age) that contains the plus group for each category
      // We are assuming it is the last element, If categories can have different age ranges, this will have to change.
      unsigned plus_index = (*category)->data_.size() - 1;

      LOG_FINEST() << "Cached plus group " << cached_category->data_[plus_index] << " 1 year plus group " << (*category)->data_[plus_index];
      //if (the plus group has been populated)
      if (cached_category->data_[plus_index] > 0) {
        c = (*category)->data_[plus_index] / cached_category->data_[plus_index] - 1; //zero fun
        LOG_FINEST() << "The value of c = " << c;
        if (c > 0.99) {
          c = 0.99;
        } else if (c < 0.0)
          c = 0.0;
        // reset the partition back to the original Cached partition
        (*category)->data_ = cached_category->data_;
        LOG_FINEST() << "plus group pre adjustment = " << (*category)->data_[plus_index];
        // now multiply the approximated change to the plus group
        (*category)->data_[plus_index] *= 1 / (1 - c);
        LOG_FINEST() << "Adjustment based an approximation for the plus group = " << (*category)->data_[plus_index];
      } else {
        // reset the partition back to the original Cached partition
        (*category)->data_ = cached_category->data_;
      }
    }
  }

  Double max_rel_diff = 1e18;
  vector<Double> plus_group(categories.size(), 1);
  vector<Double> old_plus_group(categories.size(), 1);

  auto category = partition_.begin();
  unsigned iter = 0;
  for (; category != partition_.end(); ++category, ++iter) {
    if (iter >= old_plus_group.size()) {
      LOG_CODE_ERROR()<< "if (iter >= old_plus_group.size())";
    }
    old_plus_group[iter] = (*category)->data_[(*category)->data_.size() - 1];
  }

  LOG_FINEST() << "check relative difference";
  while (max_rel_diff > 0.005) {
    time_step_manager->ExecuteInitialisation(label_, 1);
    max_rel_diff = 0;
    category = partition_.begin();

    for (unsigned iter = 0; category != partition_.end(); ++category, ++iter) {
      plus_group[iter] = (*category)->data_[(*category)->data_.size() - 1];
      LOG_FINEST() << " plus group " << plus_group[iter] << " Old plus group " << old_plus_group[iter];
      if (old_plus_group[iter] != 0) {
        if (fabs((plus_group[iter] - old_plus_group[iter]) / old_plus_group[iter]) > max_rel_diff)
          max_rel_diff = fabs((plus_group[iter] - old_plus_group[iter]) / old_plus_group[iter]);
        LOG_FINEST() << " max difference " << max_rel_diff;
      }
    }
    old_plus_group = plus_group;
  }

  LOG_FINEST() << "Number of Beverton-Holt recruitment processes in annual cycle = " << recruitment_process_.size();

  LOG_FINEST() << "Number of Beverton-Holt recruitment processes with deviations in annual cycle = " << recruitment_process_with_devs_.size();

  // We are at Equilibrium state here
  // Check if we have B0 initialised or R0 initialised recruitment
  bool B0_intial_recruitment = false;
  for (auto recruitment_process : recruitment_process_) {
    if (recruitment_process->b0_initialised()) {
      LOG_FINEST() << PARAM_B0 << " has been defined for process labelled " << recruitment_process->label();
      recruitment_process->ScalePartition();
      B0_intial_recruitment = true;
    }
  }

  for (auto recruitment_process_with_devs : recruitment_process_with_devs_) {
    if (recruitment_process_with_devs->b0_initialised()) {
      LOG_FINEST() << PARAM_B0 << " has been defined for process labelled " << recruitment_process_with_devs->label();
      recruitment_process_with_devs->ScalePartition();
      B0_intial_recruitment = true;
    }
  }

  if (B0_intial_recruitment) {
    LOG_FINE() << "B0 initialised";
    // Calculate derived quanitities in the right space if we have a B0 initialised model
    time_step_manager->ExecuteInitialisation(label_, 1);
  }

  // Add a switch for to replicate CASAL output if this method does not reach an equilibrium State
  if (casal_initialisation_phase_) {
    LOG_FINEST() << "Legacy CASAL type initialisation has been executed";
    cached_partition_.BuildCache();
    // Run a annual cycle once to populate derived quantities
    time_step_manager->ExecuteInitialisation(label_, 1);

    auto cached_category = cached_partition_.begin();
    auto category = partition_.begin();
    for (; category != partition_.end(); ++category, ++cached_category) {
      for(unsigned n_age = 0; n_age <  (*category)->data_.size(); ++n_age)
        LOG_FINEST() << "new part = " << (*category)->data_[n_age] << " old part = " << cached_category->data_[n_age];
      (*category)->data_ = cached_category->data_;
    }
  }
}

} /* namespace age */
} /* namespace initialisationphases */
} /* namespace niwa */
