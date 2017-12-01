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

#include "Common/Logging/Logging.h"
#include "Common/Utilities/To.h"

namespace niwa {
namespace base {

using std::cout;
using std::endl;

/**
 * This method will check to see if the addressable has been registered
 * or not
 *
 * @param label of the estimate we are looking for
 * @return True if found, false if not
 */
bool Object::HasAddressable(const string& label) const {
  bool result = !(addressable_types_.find(label) == addressable_types_.end());

  bool result2 = false;
  for (auto unnamed : unnamed_addressable_s_map_vector_) {
    if (unnamed->find(label) != unnamed->end()) {
      result2 = true;
      break;
    }
  }

  return result || result2;
}

/**
 * Does the target addressable have the usage flag we want?
 */
bool Object::HasAddressableUsage(const string& label, const addressable::Usage& flag) const {
  if (addressable_types_.find(label) == addressable_types_.end()) {
    for (auto container : unnamed_addressable_s_map_vector_) {
      if (container->find(label) != container->end())
        return true; // by default, we allow all usage. Overrises will show up in the types_
    }
    LOG_CODE_ERROR() << "The addressable " << label << " has not been registered for the object " << block_type_ << ".type=" << type_;
  }

  addressable::Usage flags = addressable_usage_.find(label)->second;
  return (flags & flag) == flag;
}

/**
 * This method will check to see if the addressable label passed
 * in is registered as part of a vector or not.
 *
 * @param label The label of the addressable to check
 * @return True if addressable is a vector, false if not
 */
bool Object::IsAddressableAVector(const string& label) const {
  bool result = !(addressable_vectors_.find(label) == addressable_vectors_.end());

  bool result2 = false;
  for (auto unnamed : unnamed_addressable_s_map_vector_) {
    if (unnamed->find(label) != unnamed->end()) {
      result2 = true;
      break;
    }
  }

  return result || result2;
}

/**
 * This method will return the number of values that have been registered as an
 * addressable.
 *
 * @param label The label of the registered parameter
 * @return The amount of values registered as addressable
 */
unsigned Object::GetAddressableSize(const string& label) const {
  if (addressable_vectors_.find(label) != addressable_vectors_.end())
    return addressable_vectors_.find(label)->second->size();
  if (addressable_u_maps_.find(label) != addressable_u_maps_.end())
      return addressable_u_maps_.find(label)->second->size();
  if (addressable_s_maps_.find(label) != addressable_s_maps_.end())
    return addressable_s_maps_.find(label)->second->size();
  for (auto unnamed : unnamed_addressable_s_map_vector_) {
    if (unnamed->find(label) != unnamed->end())
      return unnamed->find(label)->second.size();
  }

  if (addressables_.find(label) == addressables_.end())
    LOG_CODE_ERROR() << "The addressable " << label << " has not been registered for the object " << block_type_ << ".type=" << type_;

  return 1u;
}

/**
 * This method will find the addressable with the matching
 * label and return it.
 *
 * @param label The label of the addressable to find
 * @return A pointer to the addressable to be used by the Estimate object
 */
Double* Object::GetAddressable(const string& label) {
  for(auto addressables : addressable_types_)
    LOG_FINEST() << addressables.first;

  if (addressable_types_.find(label) == addressable_types_.end())
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";
  return addressables_[label];
}

Double* Object::GetAddressable(const string& label, const string& index) {
  if (addressable_types_.find(label) == addressable_types_.end())
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";

  if (addressable_types_[label] == addressable::kStringMap) {
    if (addressable_s_maps_[label]->find(index) == addressable_s_maps_[label]->end())
      LOG_CODE_ERROR() << "addressable_s_maps_[" << label << "].find(" << index << ") == addressable_s_maps_.end()";

    return &(*addressable_s_maps_[label])[index];

  } else if (addressable_types_[label] == addressable::kUnsignedMap) {
    unsigned value = 0;
    bool success = utilities::To<unsigned>(index, value);
    if (!success)
      LOG_CODE_ERROR() << "bool success = util::To<unsigned>(" << index << ", value);";

    LOG_FINEST() << "looking for " << label << " with index " << index;

    if (addressable_u_maps_[label]->find(value) == addressable_u_maps_[label]->end())
      LOG_CODE_ERROR() << "addressable_u_maps[" << label << "].find(" << value << ") == addressable_u_maps_.end()";

    return &(*addressable_u_maps_[label])[value];

  } else if (addressable_types_[label] == addressable::kVector) {
    unsigned value = 0;
    bool success = utilities::To<unsigned>(index, value);
    if (!success)
      LOG_CODE_ERROR() << "bool success = util::To<unsigned>(" << index << ", value);";

    if (value == 0)
      LOG_FATAL() << "Addressable " << label << " is a vector and must be indexed from 1, not 0";
    if (addressable_vectors_[label]->size() < value)
      LOG_CODE_ERROR() << "addressable_vectors_[" << label << "]->size() " << addressable_vectors_[label]->size() << " < " << value;

    return &(*addressable_vectors_[label])[value - 1];

  } else if (addressable_types_[label] != addressable::kSingle)
    LOG_CODE_ERROR() << "addressable_types_[" << label << "] != Addressable::kSingle";

  return addressables_[label];
}

/**
 * This method will return a vector of addressables for use. This is required
 * when we're asking for a subset of a vector or map.
 *
 * @param The absolute label for this (e.g ycs_years{1973:2014}
 * @param A vector of the indexes to find (already exploded with utilities::string::explode()
 * @return a Pointer to a vector of Double pointers
 */
vector<Double*>* Object::GetAddressables(const string& absolute_label, const vector<string> indexes) {
  if (addressable_custom_vectors_.find(absolute_label) != addressable_custom_vectors_.end())
    return &addressable_custom_vectors_[absolute_label];

  string short_label = absolute_label.substr(0, absolute_label.find_first_of('{'));
  for (auto& index : indexes) {
    addressable_custom_vectors_[absolute_label].push_back(GetAddressable(short_label, index));
  }

  LOG_FINE() << "Creating custom addressable vector " << absolute_label << " with " << indexes.size() << " values";
  return &addressable_custom_vectors_[absolute_label];
}

/**
 * This method will return a pointer to a map of addressables that have
 * been indexed by unsigned. The majority of these addressables will
 * be indexed by year.
 *
 * @param label The label of the addressable to find
 * @return a pointer to the map to use
 */
map<unsigned, Double>* Object::GetAddressableUMap(const string& label) {
  bool dummy =  false;
  return GetAddressableUMap(label, dummy);
}

map<unsigned, Double>* Object::GetAddressableUMap(const string& label, bool& create_missing) {
  if (addressable_types_.find(label) == addressable_types_.end())
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";
  if (addressable_types_[label] != addressable::kUnsignedMap)
    LOG_CODE_ERROR() << "addressable_types_[" << label << "] != Addressable::kUnsignedMap";

  create_missing = create_missing_addressables_.find(label) != create_missing_addressables_.end();
  return addressable_u_maps_[label];
}

/**
 * Get the addressable as that is a string/double map
 *
 * @param label of the addressable
 * @return An ordered map of addressables
 */
OrderedMap<string, Double>* Object::GetAddressableSMap(const string& label) {
  if (addressable_types_.find(label) == addressable_types_.end())
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";
  if (addressable_types_[label] != addressable::kStringMap)
    LOG_CODE_ERROR() << "addressable_types_[" << label << "] != Addressable::kStringMap";

  return addressable_s_maps_[label];
}

/**
 * This method will return a pointer to a vector of addressables
 *
 * @param label The label of the addressable we want
 * @return vector pointer of addressables
 */
vector<Double>* Object::GetAddressableVector(const string& label) {
  LOG_FINEST() << "finding object with label " << label;
  if (addressable_types_.find(label) == addressable_types_.end()) {
    /**
     * It's a VectorStringMap addressable, so we'll go looking for it
     */
    for (auto container : unnamed_addressable_s_map_vector_) {
      if (container->find(label) != container->end()) {
        return &(*container)[label];
      }
    }
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";
  }
  if (addressable_types_[label] != addressable::kVector)
    LOG_CODE_ERROR() << "addressable_types_[" << label << "] != Addressable::kVector";

  return addressable_vectors_[label];
}

/**
 *
 */

addressable::Type Object::GetAddressableType(const string& label) const {
  if (addressable_types_.find(label) == addressable_types_.end()) {
    for (auto container : unnamed_addressable_s_map_vector_) {
      if (container->find(label) != container->end())
        return addressable::kVectorStringMap;
    }
    LOG_CODE_ERROR() << "Unable to find the addressable type with the label: " << label;
  }

  return addressable_types_.find(label)->second;
}

/**
 * This method will register a variable as an object
 * that can be targeted by an estimate to be used as part of an
 * estimation process or MCMC.
 *
 * @param label The label to register the addressable under
 * @param variable The variable to register as an addressable
 */
void Object::RegisterAsAddressable(const string& label, Double* variable, addressable::Usage usage) {
  addressables_[label]      = variable;
  addressable_types_[label] = addressable::kSingle;
  addressable_usage_[label] = usage;
}

/**
 * This method will register a vector of variables as an object
 * that can be targeted by an estimate to be used as part of an
 * estimation process or MCMC.
 *
 * @param label The label to register the addressable under
 * @param variables Vector containing all the elements to register
 */
void Object::RegisterAsAddressable(const string& label, vector<Double>* variables, addressable::Usage usage) {
  addressable_vectors_[label] = variables;
  addressable_types_[label]   = addressable::kVector;
  addressable_usage_[label] = usage;
}

/**
 * This method will register a map of variables as addressables.
 * When register each variable it'll be done like:
 *
 * process_label.variable(map.string)
 *
 * @param label The label for the process
 * @param variables Map containing index and double values to store
 */
void Object::RegisterAsAddressable(const string& label, OrderedMap<string, Double>* variables, addressable::Usage usage) {
  addressable_s_maps_[label]  = variables;
  addressable_types_[label]   = addressable::kStringMap;
  addressable_usage_[label] = usage;
}
void Object::RegisterAsAddressable(const string& label, map<unsigned, Double>* variables, addressable::Usage usage) {
  addressable_u_maps_[label]  = variables;
  addressable_types_[label]   = addressable::kUnsignedMap;
  addressable_usage_[label] = usage;
}

/**
 *
 */
void Object::RegisterAsAddressable(map<string, vector<Double>>* variables) {
  unnamed_addressable_s_map_vector_.push_back(variables);
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
