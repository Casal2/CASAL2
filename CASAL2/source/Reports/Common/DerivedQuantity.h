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
  DerivedQuantity(Model* model);
  virtual                     ~DerivedQuantity() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final;
  void                        DoFinaliseTabular() override final;


private:
  bool						  first_run_ = true;
  string            unit_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* DERIVEDQUANTITY_H_ */
