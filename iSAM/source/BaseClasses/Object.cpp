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
  if (estimable_u_maps_.find(label) != estimable_u_maps_.end())
      return estimable_u_maps_.find(label)->second->size();
  if (estimable_s_maps_.find(label) != estimable_s_maps_.end())
    return estimable_s_maps_.find(label)->second->size();

  if (estimables_.find(label) == estimables_.end())
    LOG_CODE_ERROR("The estimable " << label << " has not been registered for the object " << block_type_ << ".type=" << type_);

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
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR("estimable_types_.find(" << label << ") == estimable_types_.end()");
  return estimables_[label];
}

Double* Object::GetEstimable(const string& label, const string& index) {
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR("estimable_types_.find(" << label << ") == estimable_types_.end()");

  if (estimable_types_[label] == Estimable::kStringMap) {
    if (estimable_s_maps_[label]->find(index) == estimable_s_maps_[label]->end())
      LOG_CODE_ERROR("estimable_s_maps_[" << label << "].find(" << index << ") == estimable_s_maps_.end()");

    return &(*estimable_s_maps_[label])[index];

  } else if (estimable_types_[label] == Estimable::kUnsignedMap) {
    unsigned value = 0;
    bool success = utilities::To<unsigned>(index, value);
    if (!success)
      LOG_CODE_ERROR("bool success = util::To<unsigned>(" << index << ", value);");

    if (estimable_u_maps_[label]->find(value) == estimable_u_maps_[label]->end())
      LOG_CODE_ERROR("estimable_u_maps[" << label << "].find(" << value << ") == estimable_u_maps_.end()");

    return &(*estimable_u_maps_[label])[value];

  } else if (estimable_types_[label] == Estimable::kVector) {
    unsigned value = 0;
    bool success = utilities::To<unsigned>(index, value);
    if (!success)
      LOG_CODE_ERROR("bool success = util::To<unsigned>(" << index << ", value);");

    if (estimable_vectors_[label]->size() >= value)
      LOG_CODE_ERROR("estimable_vectors_[" << label << "]->size() >= " << value);

    return &(*estimable_vectors_[label])[value];

  } else if (estimable_types_[label] != Estimable::kSingle)
    LOG_CODE_ERROR("estimable_types_[" << label << "] != Estimable::kSingle");

  return estimables_[label];
}

/**
 * This method will return a pointer to a map of estimables that have
 * been indexed by unsigned. The majority of these estimables will
 * be indexed by year.
 *
 * @param label The label of the estimable to find
 * @return a pointer to the map to use
 */
map<unsigned, Double>* Object::GetEstimableUMap(const string& label) {
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR("estimable_types_.find(" << label << ") == estimable_types_.end()");
  if (estimable_types_[label] != Estimable::kUnsignedMap)
    LOG_CODE_ERROR("estimable_types_[" << label << "] != Estimable::kUnsignedMap");

  return estimable_u_maps_[label];
}

/**
 *
 */
map<string, Double>* Object::GetEstimableSMap(const string& label) {
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR("estimable_types_.find(" << label << ") == estimable_types_.end()");
  if (estimable_types_[label] != Estimable::kStringMap)
    LOG_CODE_ERROR("estimable_types_[" << label << "] != Estimable::kStringMap");

  return estimable_s_maps_[label];
}

/**
 * This method will return a pointer to a vector of estimables
 */
vector<Double>* Object::GetEstimableVector(const string& label) {
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR("estimable_types_.find(" << label << ") == estimable_types_.end()");
  if (estimable_types_[label] != Estimable::kVector)
      LOG_CODE_ERROR("estimable_types_[" << label << "] != Estimable::kVector");

  return estimable_vectors_[label];
}

/**
 *
 */
Estimable::Type Object::GetEstimableType(const string& label) const {
  if (estimable_types_.find(label) == estimable_types_.end()) {
    LOG_CODE_ERROR("Unable to find the estimable type with the label: " << label);
  }

  return estimable_types_.find(label)->second;
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
  estimable_types_[label] = Estimable::kSingle;
}

/**
 * This method will register a vector of variables as an object
 * that can be targeted by an estimate to be used as part of an
 * estimation process or MCMC.
 *
 * @param label The label to register the estimable under
 * @param variables Vector containing all the elements to register
 */
void Object::RegisterAsEstimable(const string& label, vector<Double>* variables) {
  estimable_vectors_[label] = variables;
  estimable_types_[label]   = Estimable::kVector;
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
void Object::RegisterAsEstimable(const string& label, map<string, Double>* variables) {
  estimable_s_maps_[label]  = variables;
  estimable_types_[label]   = Estimable::kStringMap;
}
void Object::RegisterAsEstimable(const string& label, map<unsigned, Double>* variables) {
  estimable_u_maps_[label]  = variables;
  estimable_types_[label]   = Estimable::kUnsignedMap;
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
