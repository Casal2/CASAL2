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

#include <iostream>

#include "Logging/Logging.h"
#include "Utilities/To.h"

namespace niwa {
namespace base {

using std::cout;
using std::endl;

/**
 * This method will check to see if the estimable has been registered
 * or not
 *
 * @param label of the estimate we are looking for
 * @return True if found, false if not
 */
bool Object::HasEstimable(const string& label) const {
  bool result = !(estimable_types_.find(label) == estimable_types_.end());

  bool result2 = false;
  for (auto unnamed : unnamed_estimable_s_map_vector_) {
    if (unnamed->find(label) != unnamed->end()) {
      result2 = true;
      break;
    }
  }

  return result || result2;
}

/**
 * This method will check to see if the estimable label passed
 * in is registered as part of a vector or not.
 *
 * @param label The label of the estimable to check
 * @return True if estimable is a vector, false if not
 */
bool Object::IsEstimableAVector(const string& label) const {
  bool result = !(estimable_vectors_.find(label) == estimable_vectors_.end());

  bool result2 = false;
  for (auto unnamed : unnamed_estimable_s_map_vector_) {
    if (unnamed->find(label) != unnamed->end()) {
      result2 = true;
      break;
    }
  }

  return result || result2;
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
  for (auto unnamed : unnamed_estimable_s_map_vector_) {
    if (unnamed->find(label) != unnamed->end())
      return unnamed->find(label)->second.size();
  }

  if (estimables_.find(label) == estimables_.end())
    LOG_CODE_ERROR() << "The estimable " << label << " has not been registered for the object " << block_type_ << ".type=" << type_;

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
    LOG_CODE_ERROR() << "estimable_types_.find(" << label << ") == estimable_types_.end()";
  return estimables_[label];
}

Double* Object::GetEstimable(const string& label, const string& index) {
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR() << "estimable_types_.find(" << label << ") == estimable_types_.end()";

  if (estimable_types_[label] == Estimable::kStringMap) {
    if (estimable_s_maps_[label]->find(index) == estimable_s_maps_[label]->end())
      LOG_CODE_ERROR() << "estimable_s_maps_[" << label << "].find(" << index << ") == estimable_s_maps_.end()";

    return &(*estimable_s_maps_[label])[index];

  } else if (estimable_types_[label] == Estimable::kUnsignedMap) {
    unsigned value = 0;
    bool success = utilities::To<unsigned>(index, value);
    if (!success)
      LOG_CODE_ERROR() << "bool success = util::To<unsigned>(" << index << ", value);";

    if (estimable_u_maps_[label]->find(value) == estimable_u_maps_[label]->end())
      LOG_CODE_ERROR() << "estimable_u_maps[" << label << "].find(" << value << ") == estimable_u_maps_.end()";

    return &(*estimable_u_maps_[label])[value];

  } else if (estimable_types_[label] == Estimable::kVector) {
    unsigned value = 0;
    bool success = utilities::To<unsigned>(index, value);
    if (!success)
      LOG_CODE_ERROR() << "bool success = util::To<unsigned>(" << index << ", value);";

    if (value == 0)
      LOG_FATAL() << "Estimable " << label << " is a vector and must be indexed from 1, not 0";
    if (estimable_vectors_[label]->size() < value)
      LOG_CODE_ERROR() << "estimable_vectors_[" << label << "]->size() " << estimable_vectors_[label]->size() << " < " << value;

    return &(*estimable_vectors_[label])[value - 1];

  } else if (estimable_types_[label] != Estimable::kSingle)
    LOG_CODE_ERROR() << "estimable_types_[" << label << "] != Estimable::kSingle";

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
  bool dummy =  false;
  return GetEstimableUMap(label, dummy);
}

map<unsigned, Double>* Object::GetEstimableUMap(const string& label, bool& create_missing) {
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR() << "estimable_types_.find(" << label << ") == estimable_types_.end()";
  if (estimable_types_[label] != Estimable::kUnsignedMap)
    LOG_CODE_ERROR() << "estimable_types_[" << label << "] != Estimable::kUnsignedMap";

  create_missing = create_missing_estimables_.find(label) != create_missing_estimables_.end();
  return estimable_u_maps_[label];
}

/**
 * Get the estimable as that is a string/double map
 *
 * @param label of the estimable
 * @return An ordered map of estimables
 */
OrderedMap<string, Double>* Object::GetEstimableSMap(const string& label) {
  if (estimable_types_.find(label) == estimable_types_.end())
    LOG_CODE_ERROR() << "estimable_types_.find(" << label << ") == estimable_types_.end()";
  if (estimable_types_[label] != Estimable::kStringMap)
    LOG_CODE_ERROR() << "estimable_types_[" << label << "] != Estimable::kStringMap";

  return estimable_s_maps_[label];
}

/**
 * This method will return a pointer to a vector of estimables
 *
 * @param label The label of the estimable we want
 * @return vector pointer of estimables
 */
vector<Double>* Object::GetEstimableVector(const string& label) {
  LOG_FINEST() << "finding object with label " << label;
  if (estimable_types_.find(label) == estimable_types_.end()) {
    /**
     * It's a VectorStringMap estimable, so we'll go looking for it
     */
    for (auto container : unnamed_estimable_s_map_vector_) {
      if (container->find(label) != container->end()) {
        return &(*container)[label];
      }
    }
    LOG_CODE_ERROR() << "estimable_types_.find(" << label << ") == estimable_types_.end()";
  }
  if (estimable_types_[label] != Estimable::kVector)
    LOG_CODE_ERROR() << "estimable_types_[" << label << "] != Estimable::kVector";

  return estimable_vectors_[label];
}

/**
 *
 */
Estimable::Type Object::GetEstimableType(const string& label) const {
  if (estimable_types_.find(label) == estimable_types_.end()) {
    for (auto container : unnamed_estimable_s_map_vector_) {
      if (container->find(label) != container->end())
        return Estimable::kVectorStringMap;
    }
    LOG_CODE_ERROR() << "Unable to find the estimable type with the label: " << label;
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
void Object::RegisterAsEstimable(const string& label, OrderedMap<string, Double>* variables) {
  estimable_s_maps_[label]  = variables;
  estimable_types_[label]   = Estimable::kStringMap;
}
void Object::RegisterAsEstimable(const string& label, map<unsigned, Double>* variables) {
  estimable_u_maps_[label]  = variables;
  estimable_types_[label]   = Estimable::kUnsignedMap;
}

/**
 *
 */
void Object::RegisterAsEstimable(map<string, vector<Double>>* variables) {
  unnamed_estimable_s_map_vector_.push_back(variables);
}

/**
 * This method will print the same value as the locations() method on the ParameterList for a given
 * parameter except it'll doing it for the whole base object
 *
 * @return a string containing the configuration file location data for debugging/logging
 */
string Object::location() {
  string line_number;
  niwa::utilities::To<unsigned, string>(parameters_.defined_line_number(), line_number);

  string location = "At line " + line_number + " in " + parameters_.defined_file_name() + " ";
  if (label_ != "")
    location += "for @" + block_type_ + "." + label_ + " ";

  return location;
}

/**
 * This method will print the object and block types and all parameters with documentation
 * to the screen
 */
void Object::PrintParameterQueryInfo() {
  cout << "Parameters:" << endl;

  auto map = parameters_.parameters();
  for (auto iter : map) {
    cout << iter.first << "<" << iter.second->stored_type() << ">\n";
    cout << "  -- Description: " << iter.second->description() << endl;
  }

}

} /* namespace base */
} /* namespace niwa */
