/**
 * @file AllValues.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ALLVALUES_H_
#define ALLVALUES_H_

// Headers
#include "Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class AllValues : public niwa::Selectivity {
public:
  // Methods
  AllValues();
  explicit AllValues(Model* model);
  virtual                     ~AllValues() = default;
  void                        DoValidate() override final;
  void                        Reset() override final;

private:
  // Members
  vector<Double>              v_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* ALLVALUES_H_ */
