/**
 * @file Manager-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 */

#include "../Logging/Logging.h"
#include "../Model/Model.h"

// Namespaces
namespace niwa {
namespace base {

/**
 * Validate the objects stored in the manager
 */
template <class ClassType, class StoredType>
void Manager<ClassType, StoredType>::Validate() {
  map<string, StoredType*> duplicates;

  for (auto stored_object : objects_) {
    stored_object->Validate();
    if (stored_object->label() != "" && duplicates.find(stored_object->label()) != duplicates.end()) {
      LOG_FATAL() << "Two " << stored_object->block_type() << " objects with the same label " << stored_object->label() << " have been declared. Found at "
                  << stored_object->location() << "and " << duplicates[stored_object->label()]->location();
    }
    duplicates[stored_object->label()] = stored_object;
  }
}

/**
 * Build the objects stored in the manager
 */
template <class ClassType, class StoredType>
void Manager<ClassType, StoredType>::Build() {
  LOG_FINEST() << "Starting Build... with " << objects_.size() << " objects";
  for (auto stored_object : objects_) {
    stored_object->Build();
  }

  LOG_FINEST() << "Build Finished";
}

/**
 * Reset all of the stored objects for the
 * next iteration
 */
template <class ClassType, class StoredType>
void Manager<ClassType, StoredType>::Reset() {
  for (auto stored_object : objects_) {
    stored_object->Reset();
  }
}

/**
 * This method will iterate over all of the objects
 * held by this manager looking for a specific type.
 *
 */
template <class ClassType, class StoredType>
bool Manager<ClassType, StoredType>::HasType(const std::string_view type) {
  for (auto stored_object : objects_) {
    if (stored_object->type() == type)
      return true;
  }
  return false;
}

/**
 * Get an object from our manager based on it's label.
 * As we can compare Object to label this will work nicely.
 */
template <class ClassType, class StoredType>
StoredType* Manager<ClassType, StoredType>::get(string_view label) {
  for (auto stored_object : objects_) {
    if (label == stored_object->label())
      return stored_object;
  }

  LOG_FATAL() << "Object " << label << " could not be found. Please check it has been defined correctly";
  return nullptr;
}

}  // namespace base
} /* namespace niwa */
