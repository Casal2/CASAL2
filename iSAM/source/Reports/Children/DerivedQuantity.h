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
namespace isam {
namespace reports {

/**
 *
 */
class DerivedQuantity : public isam::Report {
public:
  DerivedQuantity();
  virtual                     ~DerivedQuantity() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
};

} /* namespace reports */
} /* namespace isam */
#endif /* DERIVEDQUANTITY_H_ */
