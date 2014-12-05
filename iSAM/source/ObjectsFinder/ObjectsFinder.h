/**
 * @file ObjectFinder.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file is responsible for taking a string and then
 * finding the object within iSAM that matches it. This
 * class is used by the Estimate and Equation parsing systems
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBJECTS_FINDER_H_
#define OBJECTS_FINDER_H_

// Headers
#include <string>

#include "BaseClasses/Object.h"

// Namespaces
namespace niwa {
namespace objects {

using std::string;

/**
 * This method will take an input string and explode it returning the different values that are needed
 * to find an object within iSAM.
 *
 * @param parameter The full parameter loaded from the configuration file
 * @param type (out) The type of object that will need to be located
 * @param label (out) The label of the object that will need to be located
 * @param parameter (out) The estimable member of the object to locate
 * @param index (out) The index of the estimate (0 based) in to the vector/array
 */
void ExplodeString(const string& source_parameter, string &type, string& label, string& parameter, string& index);


void ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter);

/**
 * This method will find the object in iSAM and return an Object pointer to it.
 *
 * @param object_absolute_name The absolute name for the parameter. This includes the object details (e.g process[mortality].m
 */
niwa::base::ObjectPtr FindObject(const string& parameter_absolute_name);

/**
 * Find an estimable in the system using an absolute name
 */
Double* FindEstimable(const string& estimable_absolute_name);

} /* namespace objects */
} /* namespace niwa */
#endif /* OBJECTS_FINDER_H_ */
