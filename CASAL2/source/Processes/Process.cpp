/**
 * @file Process.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Process.h"

#include "Model/Managers.h"
#include "Model/Model.h"
#include "Reports/Manager.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Process::Process(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the process", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of process", "", "");

}

/**
 * Call the validation method for the child object of this process and
 * set some generic variables.
 */
void Process::Validate() {
  LOG_TRACE();
  parameters_.Populate(model_);
  LOG_FINEST() << "Validating process " << label_;

  if (block_type_ != PARAM_PROCESS && block_type_ != PARAM_PROCESSES) {
    if (type_ != "")
      type_ = block_type_ + "_" + type_;
    else
      type_ = block_type_;

    block_type_ = PARAM_PROCESS;
  }

  if (process_type_ == ProcessType::kUnknown)
    LOG_CODE_ERROR() << "process_type_ == ProcessType::kUnknown for label: " << label();

  LOG_FINEST() << "Exit parent validation and into child validation";
  DoValidate();
}

/**
 * Store the label and type in our print values and
 * then call the child build method.
 */
void Process::Build() {

  DoBuild();
}

/**
 * Flush the print_values_ that are created in the StoreForReport() method. To ensure when CASAL2 is in an iterative state the reports dosen't keep appending information
 */
void Process::Reset(){
	LOG_TRACE();
  DoReset();
}

/**
 * Execute our process and any executors
 */
void Process::Execute(unsigned year, const string& time_step_label) {
  LOG_FINEST() << label_;
  for (auto executor : executors_[year][time_step_label])
    executor->PreExecute();

  LOG_TRACE();
  DoExecute();
  LOG_TRACE();

  for (auto executor : executors_[year][time_step_label])
    executor->Execute();
}

/**
 *
 */
void Process::Subscribe(unsigned year, const string& time_step_label, Executor* executor) {
  executors_[year][time_step_label].push_back(executor);
}



} /* namespace Casal2 */
