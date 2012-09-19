/**
 * @file ParameterList-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 19/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARAMETERLIST_INL_H_
#define PARAMETERLIST_INL_H_

// Headers
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <exception>

#include "../Utilities/Exception.h"

// Using
using std::cout;
using std::endl;

namespace iSAM {
/**
 * This method will fill the vector <list> with any values stored
 * against the parameter <name>
 *
 * @param list This is the vector that is filled with the values
 * @param name The name of the attribute to load the values for
 * @param optional Specifies if an exception is thrown if the value is missing
 */
template<class Type>
void ParameterList::fillVector(vector<Type> &list, string name, bool optional) {
  if (!hasParameter(name)) {
    if (optional) {
      return;
    } else {
      THROW_EXCEPTION("Parameter has not been defined and is not optional: " + name);
    }
  }

  // Clear the list
  list.clear();

  vector<string>::iterator iter = parameters_[name].begin();
  while (iter != parameters_[name].end()) {
    try {
      list.push_back(boost::lexical_cast<Type>((*iter)));
    } catch (const std::bad_cast &ex) {
      THROW_EXCEPTION("Failed to convert type: " + *iter + ". Reason: " + ex.what());
    }
    iter++;
  }
}

} /* namespace iSAM */
#endif /* PARAMETERLIST_INL_H_ */
