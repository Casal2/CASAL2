/**
 * @file Manager-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#include "Common/Logging/Logging.h"
#include "Common/Model/Model.h"

// Namespaces
namespace niwa {
namespace base {

/**
 * This method is our singleton instance method
 */
//template <class ClassType, class StoredType>
//ClassType& Manager<ClassType, StoredType>::Instance() {
//  static ClassType singleton;
//  return singleton;
//}

/**
 * Validate the objects stored in the manager
 */
template <class ClassType, class StoredType>
void Manager<ClassType, StoredType>::Validate() {
  map<string, StoredType*> duplicates;

  for(auto stored_object : objects_) {
    stored_object->Validate();
    if (stored_object->label() != "" && duplicates.find(stored_object->label()) != duplicates.end()) {
      LOG_FATAL() << "Two " << stored_object->block_type() << " objects with the same label "
          << stored_object->label() << " have been declared. At "
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
  for(auto stored_object : objects_) {
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
  for(auto stored_object : objects_) {
    stored_object->Reset();
  }
}


} /* namespae base */
} /* namespace niwa */
