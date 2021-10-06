/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "../Categories/Categories.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * Return an age length smart_ptr based on label
 *
 * @param label The label of the age length object
 * @return age length smart_ptr
 */
AgeLength* Manager::FindAgeLength(const string& label) {
  for (auto length : objects_) {
    if (length->label() == label)
      return length;
  }

  return nullptr;
}

/**
 * This function is executed each year. It calls age-length classes that are type data
 * and updates the mean_length and mean_weight containers which are called throughout the year.
 * It is a pretty in expensive process copying values from data object on to the age-length class mean weight and mean length 
 */
void Manager::UpdateDataType() { 
  for (auto agelength : objects_) { 
    if (agelength ->type() == PARAM_DATA) 
      agelength->UpdateYearContainers(); 
  } 
} 
} /* namespace agelengths */
} /* namespace niwa */
