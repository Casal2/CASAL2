/**
 * @file Model.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Model.h"

#include <iostream>

#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {

using std::cout;
using std::endl;


Model::Model() {
  LOG_TRACE();
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
}

/**
 * Our singleton accessor method
 *
 * @return singleton shared ptr
 */
shared_ptr<Model> Model::Instance() {
  static ModelPtr model = ModelPtr(new Model());
  return model;
}

void Model::Run() {

  cout << "Running the Model!!" << endl;
}

} /* namespace isam */
