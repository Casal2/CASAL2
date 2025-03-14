/**
 * @file TimeStep.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#include "TimeStep.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include "../InitialisationPhases/Manager.h"
#include "../Model/Model.h"
#include "../Observations/Manager.h"
#include "../Processes/Manager.h"

namespace niwa {

/**
 * Default Constructor
 */
TimeStep::TimeStep(shared_ptr<Model> model) : model_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the time step", "");
  parameters_.Bind<string>(PARAM_PROCESSES, &process_names_, "The labels of the processes that occur in this time step, in the order that they occur", "");
}

/**
 * Validate the time step
 */
void TimeStep::Validate() {
  parameters_.Populate(model_);
}

/**
 * Build the time step
 */
void TimeStep::Build() {
  // Get the pointers to our processes
  processes::Manager& process_manager = *model_->managers()->process();
  for (string process_name : process_names_) {
    Process* process = process_manager.GetProcess(process_name);
    if (!process) {
      LOG_ERROR_P(PARAM_PROCESSES) << ": process " << process_name << " was not found.";
    } else
      processes_.push_back(process);
  }
  LOG_FINE() << "Time step " << label_ << " has " << processes_.size() << " processes";

  // Check if this time-step was in the annual cycle.
  // if not flag an info to user
  vector<string> model_time_steps = model_->time_steps();
  if(find(model_time_steps.begin(), model_time_steps.end(), label_) == model_time_steps.end()) {
    LOG_INFO() << "time_step " << label_ << " was defined but not included in the annual cycle.";
  } 

  /**
   * Find the range of our mortality block. This block encompasses the
   * first continuous collection of mortality processes within the time
   * step.
   */
  mortality_block_.first        = processes_.size();
  mortality_block_.second       = processes_.size() - 1;
  bool finished_mortality_block = false;
  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->process_type() == ProcessType::kMortality && !finished_mortality_block) {
      mortality_block_.first  = mortality_block_.first == processes_.size() ? i : mortality_block_.first;
      mortality_block_.second = i;
    } else if (processes_[i]->process_type() == ProcessType::kMortality && finished_mortality_block) {
      LOG_FATAL() << "Mortality processes within a time step need to be consecutive (there can be one mortality block within each time step)";
    } else if (mortality_block_.first != processes_.size())
      finished_mortality_block = true;
  }

  mortality_block_.second = mortality_block_.first == processes_.size() ? mortality_block_.first : mortality_block_.second;
}
 /**
 * Execute the time step during the initialisation phases
 */
void     TimeStep::Verify(shared_ptr<Model> model) {

}
/**
 * Execute the time step during the initialisation phases
 */
void TimeStep::ExecuteForInitialisation(const string& phase_label) {
  LOG_FINEST() << "Executing for initialisation phase: " << phase_label << " with " << initialisation_block_executors_.size() << " executors";
  LOG_FINEST() << "initialisation_processes_[phase_label].size(): " << initialisation_processes_[phase_label].size();
  for (unsigned index = 0; index < initialisation_processes_[phase_label].size(); ++index) {
    if (initialisation_mortality_blocks_[phase_label].first == index) {
      for (auto executor : initialisation_block_executors_) {
        LOG_FINEST() << "Executing executor: " << executor->label();
        executor->PreExecute();
      }
    }

    initialisation_processes_[phase_label][index]->Execute(0u, "");

    if (initialisation_mortality_blocks_[phase_label].second == index) {
      for (auto executor : initialisation_block_executors_) {
        executor->Execute();
      }
    }
  }

  if (initialisation_mortality_blocks_[phase_label].first == processes_.size()) {
    for (auto executor : initialisation_block_executors_) {
      executor->PreExecute();
      executor->Execute();
    }
  }
}

/**
 * Execute the time step
 */
void TimeStep::Execute(unsigned year) {
  LOG_TRACE();

  for (auto executor : executors_[year]) executor->PreExecute();

  for (unsigned index = 0; index < processes_.size(); ++index) {
    if (index == mortality_block_.first) {
      for (auto executor : block_executors_[year]) executor->PreExecute();
    }

    for (auto executor : process_executors_[year][index]) executor->PreExecute();

    LOG_FINEST() << "Executing process: " << processes_[index]->label();
    processes_[index]->Execute(year, label_);

    for (auto executor : process_executors_[year][index]) executor->Execute();

    if (index == mortality_block_.second) {
      for (auto executor : block_executors_[year]) executor->Execute();
    }
  }
  if (mortality_block_.first == processes_.size()) {
    for (auto executor : block_executors_[year]) {
      executor->PreExecute();
      executor->Execute();
    }
  }

  for (auto executor : executors_[year]) executor->Execute();
}

/**
 * Subscribe an executor to this block
 *
 * @param executor The pointer to the Executor
 */
void TimeStep::SubscribeToBlock(Executor* executor) {
  vector<unsigned> years = model_->years();
  for (unsigned year : years) block_executors_[year].push_back(executor);
}

/**
 * Subscribe an executor to a specific process in a specific year
 *
 * @param executor The pointer to the Executor
 * @param year The year
 * @param process_label The label of the process
 * @return a pointer to a Process
 */
Process* TimeStep::SubscribeToProcess(Executor* executor, unsigned year, string process_label) {
  LOG_TRACE();

  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->label() == process_label) {
      process_executors_[year][i].push_back(executor);
      return processes_[i];
    }
  }

  LOG_FATAL() << executor->location() << "the process " << process_label << " could not be found in the time step " << label_;
  return nullptr;
}

/**
 * Subscribe an executor to a specific process for a vector of years
 *
 * @param executor The pointer to the Executor
 * @param year The vector of years
 * @param process_label The label of the process
 * @return a pointer to a Process
 */
Process* TimeStep::SubscribeToProcess(Executor* executor, const vector<unsigned>& years, string process_label) {
  LOG_TRACE();

  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->label() == process_label) {
      for (unsigned year : years) process_executors_[year][i].push_back(executor);
      return processes_[i];
    }
  }

  LOG_FATAL() << executor->location() << "the process " << process_label << " could not be found in the time step " << label_;
  return nullptr;
}

/**
 * Set the initialisation process labels
 *
 * @param initialisation_phase_label The initialisation phase label
 * @param process_labels The vector of process labels
 */
void TimeStep::SetInitialisationProcessLabels(const string& initialisation_phase_label, vector<string> process_labels_) {
  initialisation_process_labels_[initialisation_phase_label] = process_labels_;
}

/**
 * Build the initialisation process
 */
void TimeStep::BuildInitialisationProcesses() {
  LOG_TRACE();
  initialisation_processes_.clear();

  for (auto iter : initialisation_process_labels_) {
    for (string process_label : iter.second) {
      LOG_FINEST() << "Including " << process_label << " process in initialisation phase";
      auto process = model_->managers()->process()->GetProcess(process_label);
      if (!process)
        return;
      initialisation_processes_[iter.first].push_back(process);
    }

    initialisation_mortality_blocks_[iter.first].first  = processes_.size();
    initialisation_mortality_blocks_[iter.first].second = processes_.size() - 1;
    bool finished_mortality_block                       = false;
    for (unsigned i = 0; i < initialisation_processes_[iter.first].size(); ++i) {
      if (initialisation_processes_[iter.first][i]->process_type() == ProcessType::kMortality && !finished_mortality_block) {
        initialisation_mortality_blocks_[iter.first].first
            = initialisation_mortality_blocks_[iter.first].first == processes_.size() ? i : initialisation_mortality_blocks_[iter.first].first;
        initialisation_mortality_blocks_[iter.first].second = i;
      } else if (initialisation_processes_[iter.first][i]->process_type() == ProcessType::kMortality && finished_mortality_block) {
        LOG_FATAL() << "Mortality processes within a time step need to be consecutive (there can be one mortality block within each time step)";
      } else if (initialisation_mortality_blocks_[iter.first].first != processes_.size())
        finished_mortality_block = true;
    }
    mortality_block_.second = mortality_block_.first == processes_.size() ? mortality_block_.first : mortality_block_.second;

    initialisation_mortality_blocks_[iter.first].second = initialisation_mortality_blocks_[iter.first].first == processes_.size()
                                                              ? initialisation_mortality_blocks_[iter.first].first
                                                              : initialisation_mortality_blocks_[iter.first].second;
  }
}
} /* namespace niwa */
