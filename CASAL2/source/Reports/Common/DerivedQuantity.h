/*
 * DerivedQuantity.h
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#ifndef REPORTS_DERIVEDQUANTITY_H_
#define REPORTS_DERIVEDQUANTITY_H_

// headers
#include "../../DerivedQuantities/Manager.h"
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

class DerivedQuantity;

/**
 *
 */
class DerivedQuantity : public niwa::Report {
public:
  DerivedQuantity();
  virtual ~DerivedQuantity() = default;
  void DoValidate(shared_ptr<Model> model) final;
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;

private:
  bool                   first_run_ = true;
  string                 unit_;
  string                 derived_quantity_label_ = "";
  niwa::DerivedQuantity* derived_quantity_       = nullptr;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* DERIVEDQUANTITY_H_ */
