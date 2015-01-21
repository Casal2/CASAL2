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
 * << Add Description >>
 */
#ifndef CONFIGURATION_ESTIMABLEVALUESLOADER_H_
#define CONFIGURATION_ESTIMABLEVALUESLOADER_H_

// headers
#include <string>

// namespaces
namespace niwa {
namespace configuration {

using std::string;

/**
 * Class Definition
 */
class EstimableValuesLoader {
public:
  EstimableValuesLoader() = default;
  virtual                     ~EstimableValuesLoader() = default;
  void                        LoadValues(const string& file_name);
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* ESTIMATEVALUESLOADER_H_ */
