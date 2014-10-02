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

#include "DerivedQuantities/Manager.h"
#include "TimeSteps/Factory.h"
#include "Processes/Manager.h"

// namespaces
namespace isam {
namespace initialisationphases {

/**
 * Default constructor
 */
Iterative::Iterative() {
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The number of iterations to execute this phase for", "");
  parameters_.Bind<string>(PARAM_INSERT_PROCESSES, &insert_processes_, "The processes to insert in to target time steps", "", true);
  parameters_.Bind<string>(PARAM_EXCLUDE_PROCESSES, &exclude_processes_, "The processes to exclude from all time steps", "", true);
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
}

/**
 *
 */
void Iterative::Execute() {
  for (unsigned year = 0; year < years_; ++year) {
    for (TimeStepPtr time_step : time_steps_) {
      time_step->ExecuteForInitialisation(label_);
    }
  }
}


} /* namespace initialisationphases */
} /* namespace isam */
