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

#include "Common/Model/Managers.h"
#include "Common/Model/Model.h"
#include "Common/Processes/Manager.h"
#include "Common/Processes/Process.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * @param model Pointer to the current model context
 */
Process::Process(Model* model) : Report(model) {
  model_state_ = State::kPostExecute;
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kSimulation | RunMode::kProjection);

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
  LOG_FINE() <<" printing report " << label_ << " of type " << process_->type();
  bool is_BH_recruitment = (process_->type() == PARAM_RECRUITMENT_BEVERTON_HOLT) | (process_->type() == PARAM_BEVERTON_HOLT);
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "process_type: " << process_->type() << "\n";
  cache_ << "process: " << process_label_ << "\n";

  cache_ << "parameters:\n";
  auto parameters = process_->parameters().parameters();
  for (auto parameter : parameters) {
    if(!(is_BH_recruitment & ((parameter.first == PARAM_YCS_YEARS ||  parameter.first  == PARAM_YCS_VALUES)))) {
      // if this process is a beverton holt process don't print the parameters ycs_years or ycs_values. The reason is, this is printed in the storeForReport Function within the process
      // The reason this was done was, we can't update the input parameters to include future years in projection mode, specifically we push back on a vector and becomes a non-sensical vector (when doing multiple projections), thus we went down the
      // store for report method.
      cache_ << parameter.first << ": ";
      string line = boost::algorithm::join(parameter.second->current_values(), " ");
      cache_ << line << "\n";
    }
  }

  auto print_values = process_->print_values();
  for (auto element : print_values) {
    cache_ << element.first;
    string line = boost::algorithm::join(element.second, " ");
    cache_ << line << "\n";
  }

  ready_for_writing_ = true;
}

/**
 * Execute this report
 */
void Process::DoExecuteTabular() {
  auto print_tabular_values = process_->print_tabular_values();
  if (first_run_) {
    first_run_ = false;
    cache_ << "*" << label_ << " " << "(" << type_ << ")" << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    for (auto Reports : print_tabular_values)
      cache_ << Reports.first << " ";
    cache_ << "\n";
  }
  for (auto Reports : print_tabular_values)
    cache_ << Reports.second << " ";
  cache_ << "\n";
}

/**
 *  End report signature
 */
void Process::DoFinaliseTabular() {
  ready_for_writing_ = true;
}
} /* namespace reports */
} /* namespace niwa */
