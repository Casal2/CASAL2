/**
 * @file EstimateValuesLoader.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/12/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef CONFIGURATION_ESTIMATEVALUESLOADER_H_
#define CONFIGURATION_ESTIMATEVALUESLOADER_H_

// headers
#include <string>

// namespaces
namespace niwa {
namespace configuration {

using std::string;

/**
 * Class Definition
 */
class EstimateValuesLoader {
public:
  EstimateValuesLoader() = default;
  virtual                     ~EstimateValuesLoader() = default;
  void                        LoadValues(const string& file_name);
};

} /* namespace configuration */
} /* namespace niwa */

#endif /* ESTIMATEVALUESLOADER_H_ */
