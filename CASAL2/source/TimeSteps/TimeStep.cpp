/**
 * @file TimeStep.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#include "TimeStep.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include "InitialisationPhases/Manager.h"
#include "Model/Model.h"
#include "Observations/Manager.h"
#include "Processes/Manager.h"

namespace niwa {

/**
 * Default Constructor
 */
TimeStep::TimeStep(Model* model) : model_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_PROCESSES, &process_names_, "Processes", "");
}

/**
 * Validate our time step
 */
void TimeStep::Validate() {
  parameters_.Populate();
}

/**
 * Build our time step
 */
void TimeStep::Build() {

  // Get the pointers to our processes
  processes::Manager& process_manager = *model_->managers().process();
  for (string process_name : process_names_) {
    Process* process = process_manager.GetProcess(process_name);
    if (!process) {
      LOG_ERROR_P(PARAM_PROCESSES) << ": process " << process_name << " does not exist. Have you defined it?";
    } else
      processes_.push_back(process);
  }
  LOG_FINE() << "Time step " << label_ << " has " << processes_.size() << " processes";

  /**
   * Find the range of our mortality block. This block encompasses the
   * first continuous collection of mortality processes within the time
   * step.
   */
  mortality_block_.first  = processes_.size();
  mortality_block_.second = processes_.size() - 1;
  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->process_type() == ProcessType::kMortality) {
      mortality_block_.first = mortality_block_.first == processes_.size() ? i : mortality_block_.first;
      mortality_block_.second = i;
    } else if (mortality_block_.first != processes_.size())
      break;
  }

  mortality_block_.first = mortality_block_.first == processes_.size() ? 0 : mortality_block_.first;
}

/**
 * Execute the time step during the initialisation phases
 */
void TimeStep::ExecuteForInitialisation(const string& phase_label) {
  LOG_FINEST() << "Executing for initialisation phase: " << phase_label << " with " << initialisation_block_executors_.size() << " executors";
  for (unsigned index = 0; index < initialisation_processes_[phase_label].size(); ++index) {
    if (initialisation_mortality_blocks_[phase_label].first == index) {
      for (auto executor : initialisation_block_executors_) {
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
}

/**
 * Execute the time step
 */
void TimeStep::Execute(unsigned year) {
  LOG_TRACE();

  for (auto executor : executors_[year])
      executor->PreExecute();

  for (unsigned index = 0; index < processes_.size(); ++index) {
    if (index == mortality_block_.first) {
      for (auto executor : block_executors_[year])
        executor->PreExecute();
    }

    for(auto executor : process_executors_[year][index])
      executor->PreExecute();

    processes_[index]->Execute(year, label_);

    for(auto executor : process_executors_[year][index])
      executor->Execute();

    if (index == mortality_block_.second) {
      for (auto executor : block_executors_[year])
        executor->Execute();
    }
  }

  for (auto executor : executors_[year])
    executor->Execute();
}

/**
 *
 */
void TimeStep::SubscribeToBlock(Executor* executor) {
  vector<unsigned> years = model_->years();
  for (unsigned year : years)
    block_executors_[year].push_back(executor);
}

/**
 *
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
 *
 */
Process* TimeStep::SubscribeToProcess(Executor* executor, const vector<unsigned>& years, string process_label) {
  LOG_TRACE();

  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->label() == process_label) {
      for (unsigned year : years)
        process_executors_[year][i].push_back(executor);
      return processes_[i];
    }
  }

  LOG_FATAL() << executor->location() << "the process " << process_label << " could not be found in the time step " << label_;
  return nullptr;
}

/**
 *
 */
void TimeStep::SetInitialisationProcessLabels(const string& initialisation_phase_label, vector<string> process_labels_) {
  initialisation_process_labels_[initialisation_phase_label] = process_labels_;
}

/**
 *
 */
void TimeStep::BuildInitialisationProcesses() {
  LOG_TRACE();
  for (auto iter : initialisation_process_labels_) {
    for (string process_label : iter.second) {
      auto process = model_->managers().process()->GetProcess(process_label);
      if (!process)
        return;
      initialisation_processes_[iter.first].push_back(process);
    }

    initialisation_mortality_blocks_[iter.first].first  = processes_.size();
    initialisation_mortality_blocks_[iter.first].second = processes_.size() - 1;
    for (unsigned i = 0; i < initialisation_processes_[iter.first].size(); ++i) {
      if (initialisation_processes_[iter.first][i]->process_type() == ProcessType::kMortality) {
        initialisation_mortality_blocks_[iter.first].first = initialisation_mortality_blocks_[iter.first].first == processes_.size() ? i : initialisation_mortality_blocks_[iter.first].first;
        initialisation_mortality_blocks_[iter.first].second = i;
      } else if (initialisation_mortality_blocks_[iter.first].first != processes_.size())
        break;
    }

    initialisation_mortality_blocks_[iter.first].first = initialisation_mortality_blocks_[iter.first].first == processes_.size() ? 0 : initialisation_mortality_blocks_[iter.first].first;
  }
}
} /* namespace niwa */
