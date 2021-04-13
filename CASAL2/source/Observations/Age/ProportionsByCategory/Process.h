/**
 * @file ProcessProportionsByCategory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_PROCESSPROPORTIONSBYCATEGORY_H_
#define AGE_OBSERVATIONS_PROCESSPROPORTIONSBYCATEGORY_H_

// headers
#include "Observations/Age/ProportionsByCategory.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Class definition
 */
class ProcessProportionsByCategory : public observations::age::ProportionsByCategory {
public:
  // methods
  ProcessProportionsByCategory(shared_ptr<Model> model);
  virtual                     ~ProcessProportionsByCategory() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESSPROPORTIONSBYCATEGORY_H_ */
