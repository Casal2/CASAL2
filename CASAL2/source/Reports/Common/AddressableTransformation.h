/**
 * @file AddressableTransformation.h
 * @author  C. Marsh
 * @date 5/11/2021
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print an addressable out during a year/time step
 * so you can track the value of it over time (useful when it's
 * been modified during a projection etc)
 */
#ifndef REPORTS_ADDRESSABLETRANSFORMATION_H_
#define REPORTS_ADDRESSABLETRANSFORMATION_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
class AddressableTransformation;

namespace reports {

/**
 * class definition
 */
class AddressableTransformation : public niwa::Report {
public:
  // methods
  AddressableTransformation();
  virtual ~AddressableTransformation() noexcept(true) = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;

  void DoPrepareTabular(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;

private:
  // members
  string  addressable_label_ = "";
  niwa::AddressableTransformation* transformation_       = nullptr;
  bool print_all_transformations_ = true;
  bool first_run_ = true;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* REPORTS_ADDRESSABLETRANSFORMATION_H_ */
