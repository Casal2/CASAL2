/*
 * DerivedQuantity.h
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#ifndef REPORTS_DERIVEDQUANTITY_H_
#define REPORTS_DERIVEDQUANTITY_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class DerivedQuantity : public niwa::Report {
public:
  DerivedQuantity();
  virtual                     ~DerivedQuantity() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* DERIVEDQUANTITY_H_ */
