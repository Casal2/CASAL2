/**
 * @file ProcessProportionsByCategory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_PROCESSPROPORTIONSBYCATEGORY_H_
#define OBSERVATIONS_PROCESSPROPORTIONSBYCATEGORY_H_

// headers
#include <Observations/Children/ProportionsByCategory/ProportionsByCategory.h>

// namespaces
namespace niwa {
namespace observations {

/**
 * Class definition
 */
class ProcessProportionsByCategory : public observations::ProportionsByCategory {
public:
  // methods
  ProcessProportionsByCategory(Model* model);
  virtual                     ~ProcessProportionsByCategory() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* PROCESSPROPORTIONSBYCATEGORY_H_ */
