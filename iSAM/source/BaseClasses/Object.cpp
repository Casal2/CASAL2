/**
 * @file BaseObject.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Object.h"

#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

namespace isam {
namespace base {

/**
 * Print the description and usage details for this object.
 */
void Object::PrintDescription() const {
  cout << "Object Description: \n";
  cout << "Description: " << description_ << endl;

  // TODO: Add parameter printing
}

/**
 * This method will check to see if the estimable label passed
 * in is registered as part of a vector or not.
 *
 * @param label The label of the estimable to check
 */
bool Object::IsEstimableAVector(const string& label) const {
  if (estimable_vectors_.find(label) == estimable_vectors_.end())
    return false;

  return true;
}

/**
 * This method will return the number of values that have been registered as an
 * estimable.
 *
 * @param label The label of the registered parameter
 * @return The amount of values registered as estimable
 */
unsigned Object::GetEstimableSize(const string& label) const {
  if (estimable_vectors_.find(label) != estimable_vectors_.end())
    return estimable_vectors_.find(label)->second->size();
  if (estimable_maps_.find(label) != estimable_maps_.end())
    return estimable_maps_.find(label)->second->size();

  if (estimables_.find(label) == estimables_.end())
    LOG_CODE_ERROR("Unable to locate the label " << label << " in our estimables_ vector");

  return 1u;
}

/**
 * This method will find the estimable with the matching
 * label and return it.
 *
 * @param label The label of the estimable to find
 * @return A pointer to the estimable to be used by the Estimate object
 */
Double* Object::GetEstimable(const string& label) {
  if (estimables_.find(label) == estimables_.end()) {
    LOG_CODE_ERROR("Unable to find the estimable with the label: " << label);
  }

  return estimables_[label];
}

/**
 * This method will register a variable as an object
 * that can be targeted by an estimate to be used as part of an
 * estimation process or MCMC.
 *
 * @param label The label to register the estimable under
 * @param variable The variable to register as an estimable
 */
void Object::RegisterAsEstimable(const string& label, Double* variable) {
  estimables_[label]      = variable;
}

/**
 * This method will register a vector of variables as an object
 * that can be targeted by an estimate to be used as part of an
 * estimation process or MCMC.
 *
 * @param label The label to register the estimable under
 * @param variables Vector containing all the elements to register
 */
void Object::RegisterAsEstimable(const string& label, vector<Double>& variables) {
  estimable_vectors_[label] = &variables;
}

/**
 * This method will register a map of variables as estimables.
 * When register each variable it'll be done like:
 *
 * process_label.variable(map.string)
 *
 * @param label The label for the process
 * @param variables Map containing index and double values to store
 */
void Object::RegisterAsEstimable(const string& label, map<string, Double>& variables) {
  estimable_maps_[label] = &variables;
}

/**
 * This method will print the same value as the locations() method on the ParameterList for a given
 * parameter except it'll doing it for the whole base object
 */
string Object::location() {
  string line_number;
  isam::utilities::To<unsigned, string>(parameters_.defined_line_number(), line_number);

  string location = "At line " + line_number + " in file " + parameters_.defined_file_name();
  if (label_ != "")
    location += ": " + label_;

  return location;
}

} /* namespace base */
} /* namespace isam */
