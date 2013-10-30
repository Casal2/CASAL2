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
#include "Processes/Manager.h"

namespace isam {

/**
 * Default Constructor
 */
TimeStep::TimeStep() {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label");
  parameters_.Bind<string>(PARAM_PROCESSES, &process_names_, "Processes");
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
}

/**
 * Execute the time step
 */
void TimeStep::Execute() {
  for (ProcessPtr process : processes_)
    process->Execute();
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
