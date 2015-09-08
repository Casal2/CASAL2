/**
 * @file EstimateValuesLoader.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The estimable values loader is responsible for loading a text file (csv)
 * of estimable values to do multiple iterations.
 *
 * Depending on the run mode the estimable values will be used different.
 * For a standard run the model will do N iterations where N is the amount
 * of values specified for the estimables.
 */
#ifndef CONFIGURATION_ESTIMABLEVALUESLOADER_H_
#define CONFIGURATION_ESTIMABLEVALUESLOADER_H_

// headers
#include <string>

#include "Model/Model.h"

// namespaces
namespace niwa {
namespace configuration {

using std::string;

// classes
class EstimableValuesLoader {
public:
  // methods
  EstimableValuesLoader(ModelPtr model) : model_(model) { }
  virtual                     ~EstimableValuesLoader() = default;
  void                        LoadValues(const string& file_name);

private:
  // members
  ModelPtr                    model_;
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* ESTIMATEVALUESLOADER_H_ */
