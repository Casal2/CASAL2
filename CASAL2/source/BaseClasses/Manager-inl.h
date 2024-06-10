/**
 * @file Manager-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

#include "../Logging/Logging.h"
#include "../Model/Model.h"

// Namespaces
namespace niwa::base {

/**
 * @brief
 *
 * @tparam ClassType
 * @tparam StoredType
 */
template <class ClassType, class StoredType>
Manager<ClassType, StoredType>::~Manager() {
  for (StoredType* object : objects_) {
    if (object != nullptr)
      delete object;
    object = nullptr;
  }
}

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
 * Verify the objects stored in the manager.
 */
template <class ClassType, class StoredType>
void Manager<ClassType, StoredType>::Verify(shared_ptr<Model> model) {
  LOG_FINEST() << "Starting Verify... with " << objects_.size() << " objects";
  for (auto stored_object : objects_) {
    stored_object->Verify(model);
  }

  LOG_FINEST() << "Verify Finished";
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
    if (stored_object == nullptr)
      LOG_CODE_ERROR() << "stored_object == nullptr when looking for" << label;
    if (label == stored_object->label())
      return stored_object;
  }

  LOG_FATAL() << "Object " << label << " could not be found. Please check it has been defined correctly";
  return nullptr;
}

}  // namespace niwa::base
