/**
 * @file TimeStep.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#include "TimeStep.h"

#include "DerivedQuantities/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "Model/Model.h"
#include "Observations/Manager.h"
#include "Processes/Manager.h"

namespace isam {

/**
 * Default Constructor
 */
TimeStep::TimeStep() {
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
  processes::Manager& process_manager = processes::Manager::Instance();
  for (string process_name : process_names_) {
    ProcessPtr process = process_manager.GetProcess(process_name);
    if (!process)
      LOG_ERROR(parameters_.location(PARAM_PROCESSES) << ": process " << process_name << " does not exist. Have you defined it?");

    processes_.push_back(process);
  }

  /**
   * Get any derived quantities we have specified for this time step
   */
  derivedquantities::Manager& derived_quantities_manager = derivedquantities::Manager::Instance();
  initialisation_derived_quantities_ = derived_quantities_manager.GetForInitialisationTimeStep(label_);
  derived_quantities_                = derived_quantities_manager.GetForTimeStep(label_);

  LOG_INFO("Time step " << label_ << " has " << processes_.size() << " processes");
  LOG_INFO("Time step " << label_ << " has " << initialisation_derived_quantities_.size() << " initialisation derived quantities");
  LOG_INFO("Time step " << label_ << " has " << derived_quantities_.size() << " derived quantities");

  /**
   * If we have a block build it
   */
  block_start_process_index_ = processes_.size();
  block_end_process_Index_ = processes_.size() - 1;
  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->mortality_process()) {
      block_start_process_index_ = block_start_process_index_ == processes_.size() ? i : block_start_process_index_;
      block_end_process_Index_ = i;
    }
  }

  block_start_process_index_ = block_start_process_index_ == processes_.size() ? 0 : block_start_process_index_;
}

/**
 * Execute the time step during the initialisation phases
 */
void TimeStep::ExecuteForInitialisation(unsigned phase) {
  for (unsigned index = 0; index < processes_.size(); ++index) {
    processes_[index]->Execute();

    if (index == block_end_process_Index_) {
      ExecuteInitialisationDerivedQuantities(phase);
    }
  }
}

/**
 * Execute the time step
 */
void TimeStep::Execute(unsigned year) {
  observations::Manager& observations_manager = observations::Manager::Instance();

  for (unsigned index = 0; index < processes_.size(); ++index) {
    if (index == block_start_process_index_)
      observations_manager.PreExecute(year, label_);

    processes_[index]->Execute();

    if (index == block_end_process_Index_) {
      ExecuteDerivedQuantities();
      observations_manager.Execute(year, label_);
    }
  }
}

/**
 *
 */
void TimeStep::ExecuteInitialisationDerivedQuantities(unsigned phase) {
  LOG_TRACE();
  for (DerivedQuantityPtr derived_quantity : initialisation_derived_quantities_) {
    if (!derived_quantity)
      LOG_CODE_ERROR("derived_quantity == 0");
    derived_quantity->Calculate(phase);
  }
}

/**
 *
 */
void TimeStep::ExecuteDerivedQuantities() {
  LOG_TRACE();
  for (DerivedQuantityPtr derived_quantity : derived_quantities_)
    derived_quantity->Calculate();
}


} /* namespace isam */
