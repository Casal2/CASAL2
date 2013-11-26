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
namespace isam {

/**
 * Default constructor
 */
Process::Process() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<bool>(PARAM_PRINT_REPORT, &print_report_, "Print parameter report", false);

}

/**
 * Call the validation method for the child object of this process and
 * set some generic variables.
 */
void Process::Validate() {
  parameters_.Populate();
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
 * Print the stored values and parameter values for this object.
 */
void Process::Print() {
  // Console::RedirectOutput();


  cout << "*process_report: " << label_ << "\n";
  if (type_ == "")
    cout << "type: " << type_ << "\n";

  if (print_values_.size() > 0) {
    cout << "-- print values\n";

    auto iter = print_values_.begin();
    for (; iter != print_values_.end(); ++iter) {
      cout << iter->first << ": ";
      for (unsigned i = 0; i < iter->second.size(); ++i)
        cout << iter->second[i] << " ";

      cout << "\n";
    }
  }

  cout << "-- parameters\n";
  const map<string, ParameterPtr>& parameters = parameters_.parameters();

  auto iter = parameters.begin();
  for (; iter != parameters.end(); ++iter) {
    cout << iter->first << ": ";
    const vector<string>& values = (*iter).second->values();
    for (unsigned i = 0; i < values.size(); ++i)
      cout << values[i] << " ";
    cout << "\n";
  }
  cout << "*end\n" << endl;


  // Console::Reset();
}

} /* namespace iSAM */
