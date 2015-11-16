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

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Process::Process(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "", "");
  parameters_.Bind<bool>(PARAM_PRINT_REPORT, &print_report_, "Print parameter report", "", false);

}

/**
 * Call the validation method for the child object of this process and
 * set some generic variables.
 */
void Process::Validate() {
  parameters_.Populate();

  if (process_type_ == ProcessType::kUnknown)
    LOG_CODE_ERROR() << "process_type_ == ProcessType::kUnknown for label: " << label();

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
 * Execute our process and any executors
 */
void Process::Execute(unsigned year, const string& time_step_label) {
  for (auto executor : executors_[year][time_step_label])
    executor->PreExecute();

  DoExecute();

  for (auto executor : executors_[year][time_step_label])
    executor->Execute();
}

/**
 *
 */
void Process::Subscribe(unsigned year, const string& time_step_label, Executor* executor) {
  executors_[year][time_step_label].push_back(executor);
}


/**
 * Print the stored values and parameter values for this object.
 */
void Process::Print() {
  // Console::RedirectOutput();

//
//  cout << "*process_report: " << label_ << "\n";
//  if (type_ == "")
//    cout << "type: " << type_ << "\n";
//
//  if (print_values_.size() > 0) {
//    cout << "-- print values\n";
//
//    auto iter = print_values_.begin();
//    for (; iter != print_values_.end(); ++iter) {
//      cout << iter->first << ": ";
//      for (unsigned i = 0; i < iter->second.size(); ++i)
//        cout << iter->second[i] << " ";
//
//      cout << "\n";
//    }
//  }
//
//  cout << "-- parameters\n";
//  const map<string, ParameterPtr>& parameters = parameters_.parameters();
//
//  auto iter = parameters.begin();
//  for (; iter != parameters.end(); ++iter) {
//    cout << iter->first << ": ";
//    const vector<string>& values = (*iter).second->values();
//    for (unsigned i = 0; i < values.size(); ++i)
//      cout << values[i] << " ";
//    cout << "\n";
//  }
//  cout << "*end\n" << endl;


  // Console::Reset();
}

} /* namespace iSAM */
