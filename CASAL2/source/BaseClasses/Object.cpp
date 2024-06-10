/**
 * @file BaseObject.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Object.h"

#include <iostream>

#include "../Logging/Logging.h"
#include "../Utilities/To.h"

namespace niwa {
namespace base {

using std::cout;
using std::endl;

// Friend Operators
bool inline operator==(const Object* o, string_view label) {
  if (o == nullptr)
    LOG_CODE_ERROR() << "o == nullptr";
  return label == o->label();
}

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
 * Does the target addressable have the specified usage flag?
 *
 * @param label The label of the estimate to look for
 * @param flag The flag of addressable usage type
 * @return true if found, false if not
 */
bool Object::HasAddressableUsage(const string& label, const addressable::Usage& flag) const {
  if (addressable_types_.find(label) == addressable_types_.end()) {
    for (auto container : unnamed_addressable_s_map_vector_) {
      if (container->find(label) != container->end())
        return true;  // by default, we allow all usage. Overrises will show up in the types_
    }
    LOG_CODE_ERROR() << "The addressable " << label << " has not been registered for the object " << block_type_ << ".type=" << type_;
  }

  addressable::Usage flags = allowed_addressable_usage_.find(label)->second;
  return (flags & flag) == flag;
}

/**
 * @brief This method will store the usage for an addressable that is going to be used
 * by the model. This allows us to check later if we're using addressables for estimations
 * or time varying etc.
 *
 * @param label label of the parameter
 */
void Object::SetAddressableIsUsed(const string& label, const addressable::Usage& usage) {
  // if we don't have any usage yet, just add it.
  if (actual_addressable_usage_.find(label) == actual_addressable_usage_.end()) {
    actual_addressable_usage_[label] = usage;
    return;
  }

  // if we have usage, then we need to combine the values
  addressable::Usage new_usage     = actual_addressable_usage_[label];
  new_usage                        = (addressable::Usage)(new_usage | usage);
  actual_addressable_usage_[label] = new_usage;
}

/**
 * @brief Check if the addressable is actually being used for the usage parameter.
 *
 * @param label label of the parameter
 * @param usage usage of the parameter
 * @return true
 * @return false
 */
bool Object::IsAddressableUsedFor(const string& label, const addressable::Usage& usage) {
  if (actual_addressable_usage_.find(label) == actual_addressable_usage_.end())
    return false;
  LOG_FINE() << "IsAddressableUsedFor: " << label << " usage = " << usage;
  return usage == (addressable::Usage)(actual_addressable_usage_[label] & usage);
}

/**
 * This method checks if the addressable label passed
 * in is registered as part of a vector or not.
 *
 * @param label The label of the addressable to check
 * @return true if addressable is a vector, false if not
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
 * This method returns the number of values that have been registered as an
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
 * This method finds the addressable with the matching
 * label and return it.
 *
 * @param label The label of the addressable to find
 * @return A pointer to the addressable to be used by the Estimate object
 */
Double* Object::GetAddressable(const string& label) {
  for (auto addressables : addressable_types_) LOG_FINEST() << addressables.first;

  if (addressable_types_.find(label) == addressable_types_.end())
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";
  return addressables_[label];
}

/**
 * This method finds the addressable with the matching
 * label and index and return it.
 *
 * @param label The label of the addressable to find
 * @param index The index within the addressable
 * @return A pointer to the addressable vector element to be used by the Estimate object
 */
Double* Object::GetAddressable(const string& label, const string& index) {
  if (addressable_types_.find(label) == addressable_types_.end())
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";

  if (addressable_types_[label] == addressable::kStringMap) {
    if (addressable_s_maps_[label]->find(index) == addressable_s_maps_[label]->end())
      LOG_CODE_ERROR() << "addressable_s_maps_[" << label << "].find(" << index << ") == addressable_s_maps_.end()";

    return &(*addressable_s_maps_[label])[index];

  } else if (addressable_types_[label] == addressable::kUnsignedMap) {
    unsigned value   = 0;
    bool     success = utilities::To<unsigned>(index, value);
    if (!success)
      LOG_CODE_ERROR() << "bool success = util::To<unsigned>(" << index << ", value);";

    LOG_FINEST() << "looking for " << label << " with index " << index;

    if (addressable_u_maps_[label]->find(value) == addressable_u_maps_[label]->end())
      LOG_CODE_ERROR() << "addressable_u_maps[" << label << "].find(" << value << ") == addressable_u_maps_.end()";

    return &(*addressable_u_maps_[label])[value];

  } else if (addressable_types_[label] == addressable::kVector) {
    unsigned value   = 0;
    bool     success = utilities::To<unsigned>(index, value);
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
 * This method returns a vector of addressables for use. This is required
 * when requesting a subset of a vector or map.
 *
 * @param The absolute label for this (e.g., ycs_years{1973:2014})
 * @param A vector of the index values to find (already exploded with utilities::string::explode()
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
 * This method returns a pointer to a map of addressables that have
 * been indexed by unsigned. The majority of these addressables will
 * be indexed by year.
 *
 * @param label The label of the addressable to find
 * @return a pointer to the map to use
 */
map<unsigned, Double>* Object::GetAddressableUMap(const string& label) {
  bool dummy = false;
  return GetAddressableUMap(label, dummy);
}

/**
 * Get the addressable as that is an unsigned/Double map
 *
 * @param label The label of the addressable
 * @param create_missing The flag to indicate whether this addressable was created
 * @return A map of addressables
 */
map<unsigned, Double>* Object::GetAddressableUMap(const string& label, bool& create_missing) {
  if (addressable_types_.find(label) == addressable_types_.end())
    LOG_CODE_ERROR() << "addressable_types_.find(" << label << ") == addressable_types_.end()";
  if (addressable_types_[label] != addressable::kUnsignedMap)
    LOG_CODE_ERROR() << "addressable_types_[" << label << "] != Addressable::kUnsignedMap";

  create_missing = create_missing_addressables_.find(label) != create_missing_addressables_.end();
  return addressable_u_maps_[label];
}

/**
 * Get the addressable as that is a string/Double map
 *
 * @param label The label of the addressable
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
 * This method returns a pointer to a vector of addressables
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
 * @param label The label of the addressable
 * @return the addressable type
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
 * This method registers a variable as an object
 * that can be targeted by an estimate to be used as part of an
 * estimation process or MCMC.
 *
 * @param label The label to register the addressable under
 * @param variable The variable to register as an addressable
 * @param usage The Usage enum for this addressable
 */
void Object::RegisterAsAddressable(const string& label, Double* variable, addressable::Usage usage) {
  LOG_FINE() << "Registering addressable " << label << " for useage = " << usage;
  addressables_[label]              = variable;
  addressable_types_[label]         = addressable::kSingle;
  allowed_addressable_usage_[label] = usage;
}

/**
 * This method registers a vector of variables as an object
 * that can be targeted by an estimate to be used as part of an
 * estimation process or MCMC.
 *
 * @param label The label to register the addressable under
 * @param variables Vector containing all the elements to register
 * @param usage The Usage enum for this addressable
 */
void Object::RegisterAsAddressable(const string& label, vector<Double>* variables, addressable::Usage usage) {
  LOG_FINE() << "Registering addressable " << label << " for useage = " << usage;
  addressable_vectors_[label]       = variables;
  addressable_types_[label]         = addressable::kVector;
  allowed_addressable_usage_[label] = usage;
}

/**
 * This method registers a map of variables as addressables.
 * To register each variable:
 *
 * process_label.variable(map.string)
 *
 * @param label The label for the process
 * @param variables The OrderedMap containing the string and double values to store
 * @param usage The Usage enum for this addressable
 */
void Object::RegisterAsAddressable(const string& label, OrderedMap<string, Double>* variables, addressable::Usage usage) {
  LOG_FINE() << "Registering addressable " << label << " for useage = " << usage;
  addressable_s_maps_[label]        = variables;
  addressable_types_[label]         = addressable::kStringMap;
  allowed_addressable_usage_[label] = usage;
}

/**
 * This method registers a map of variables as addressables.
 * To register each variable:
 *
 * process_label.variable(map.string)
 *
 * @param label The label for the process
 * @param variables The map containing the unsigned integer and double values to store
 * @param usage The Usage enum for this addressable
 */
void Object::RegisterAsAddressable(const string& label, map<unsigned, Double>* variables, addressable::Usage usage) {
  LOG_FINE() << "Registering addressable " << label << " for useage = " << usage;
  addressable_u_maps_[label]        = variables;
  addressable_types_[label]         = addressable::kUnsignedMap;
  allowed_addressable_usage_[label] = usage;
}

/**
 * Register a map as an addressable
 *
 * @param variables The map containing the string and vector of double values to store
 */
void Object::RegisterAsAddressable(map<string, vector<Double>>* variables) {
  unnamed_addressable_s_map_vector_.push_back(variables);
}

/**
 * This method prints the same value as the locations() method on the ParameterList for a given
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
 * This method prints the object and block types and all parameters with documentation
 * to the screen
 */
void Object::PrintParameterQueryInfo() {
  cout << "Parameters:" << endl;

  auto map = parameters_.parameters();
  for (auto iter : map) {
    cout << iter.first << " (" << iter.second->stored_type() << ") - " << iter.second->description() << endl;
  }
}

/**
 * The default Rebuild cache calls the rebuild cache
 * for all subscribers so that a cascading cache rebuild will
 * take effect when something like a time-varying parameter is rebuilt.
 */
void Object::RebuildCache() {}

/**
 * This method allows one object to subscribe to the RebuildCache of another. This is used with things like
 * time varying parameters where a process can subscribe to a selectivity. The selectivity will notify the
 * process it has changed and the process can handle the updates.
 *
 * @param subscriber The object that will subscribe to this object
 */
void Object::SubscribeToRebuildCache(Object* subscriber) {
  rebuild_cache_subscribers_.push_back(subscriber);
}

/**
 * This method notifies subscribers of a cache rebuild event
 */
void Object::NotifySubscribers() {
  for (auto subscriber : rebuild_cache_subscribers_) {
    subscriber->RebuildCache();
    // Add a second tier
    subscriber->NotifySubscribers();
  }
}

} /* namespace base */
} /* namespace niwa */
