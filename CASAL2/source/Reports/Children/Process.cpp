/**
 * @file Process.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Process.h"

#include <boost/algorithm/string/join.hpp>

#include "Model/Managers.h"
#include "Model/Model.h"
#include "Processes/Manager.h"
#include "Processes/Process.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * @param model Pointer to the current model context
 */
Process::Process(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kFinalise;

  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label that is reported", "", "");
}

/**
 * Build our relationships between this object and other objects
 */
void Process::DoBuild() {
  process_ = model_->managers().process()->GetProcess(process_label_);
  if (!process_) {
    LOG_ERROR_P(PARAM_PROCESS) << "process " << process_label_ << " could not be found. Have you defined it?";
  }

  process_->flag_print_report();
}

/**
 * Execute this report
 */
void Process::DoExecute() {
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "process: " << process_label_ << "\n";

  cache_ << "parameters:\n";
  auto parameters = process_->parameters().parameters();
  for (auto parameter : parameters) {
    cache_ << parameter.first << ": ";
    string line = boost::algorithm::join(parameter.second->current_values(), " ");
    cache_ << line << "\n";
  }

  cache_ << "stored values:\n";
  auto print_values = process_->print_values();
  for (auto element : print_values) {
    cache_ << element.first << ": ";
    string line = boost::algorithm::join(element.second, " ");
    cache_ << line << "\n";
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
