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
#include "DerivedQuantities/Manager.h"

// namespaces
namespace niwa {
namespace initialisationphases {

/**
 * Default constructor
 */
Derived::Derived(Model* model)
  : InitialisationPhase(model),
    cached_partition_(model),
    partition_(model) {
    parameters_.Bind<string>(PARAM_INSERT_PROCESSES, &insert_processes_, "The processes to insert in to target time steps", "", true);
    parameters_.Bind<string>(PARAM_EXCLUDE_PROCESSES, &exclude_processes_, "The processes to exclude from all time steps", "", true);
}

/*
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

/*
 * Build any runtime relationships needed for execution
 */

void Derived::DoBuild() {
  time_steps_ = model_->managers().time_step()->ordered_time_steps();

  // Set the default process labels for the time step for this phase
  for (auto time_step : time_steps_)
    time_step->SetInitialisationProcessLabels(label_, time_step->process_labels());

  // handle any new processes we want to insert
  for (string insert : insert_processes_) {
    vector < string > pieces;
    boost::split(pieces, insert, boost::is_any_of("()="), boost::token_compress_on);

    string target_process = pieces.size() == 3 ? pieces[1] : "";
    string new_process = pieces.size() == 3 ? pieces[2] : pieces[1];

    auto time_step = model_->managers().time_step()->GetTimeStep(pieces[0]);
    vector < string > process_labels = time_step->initialisation_process_labels(label_);

    if (target_process == "")
      process_labels.insert(process_labels.begin(), new_process);
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
      LOG_ERROR_P(PARAM_EXCLUDE_PROCESSES) << " process " << exclude
          << " does not exist in any time steps to be excluded. Please check your spelling";
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
    LOG_ERROR() << location() << " could not calculate the recruitment index because there is no ageing process";
  if (recruitment_index < ageing_index)
    recruitment_ = true;

  // Now we need to find ssb_offset


}

/*
 * Execute our Derived Initialisation phase
 */
void Derived::Execute() {
  unsigned year_range = model_->age_spread();

  if (recruitment_)
    year_range -= 1;

  vector<string> categories = model_->categories()->category_names();

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
        if (c > 0.9) {
          c = 0.9;
        } else if (c < 0.0)
          c = 0.0;
        // reset the partition back to the original Cached partition
        (*category)->data_ = cached_category->data_;
        // now multiply the approximated change to the plus group
        (*category)->data_[plus_index] *= 1 / (1 - c);
        LOG_FINEST() << "Adjustment based an approximation for the plus group = " <<  (*category)->data_[plus_index];
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
    if (iter >= old_plus_group.size())  {
       LOG_CODE_ERROR() << "if (iter >= old_plus_group.size())";
    }
    old_plus_group[iter] = (*category)->data_[(*category)->data_.size() - 1];
  }

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
        LOG_FINEST() << " max diff " << max_rel_diff;
      }
    }
    old_plus_group = plus_group;
  }

  // run the annual cycle for ssb_offset years
  time_step_manager->ExecuteInitialisation(label_, ssb_offset_);


}

} /* namespace initialisationphases */
} /* namespace niwa */
