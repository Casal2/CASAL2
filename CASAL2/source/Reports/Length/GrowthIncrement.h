/**
 * @file GrowthIncrement.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */

#ifndef SOURCE_LENGTH_REPORTS_GROWTHINCREMENT_H_
#define SOURCE_LENGTH_REPORTS_GROWTHINCREMENT_H_

// Headers
#include "../../Reports/Report.h"

namespace niwa {
class GrowthIncrement;
namespace reports {
namespace length {

class GrowthIncrement : public niwa::Report {
public:
  // methods
  GrowthIncrement();
  virtual ~GrowthIncrement() = default;
  void DoValidate(shared_ptr<Model> model) override final;
  void DoBuild(shared_ptr<Model> model) override final;
  void DoExecute(shared_ptr<Model> model) override final;
  void DoPrepareTabular(shared_ptr<Model> model) override final;

private:
  string           growth_label_;
  niwa::GrowthIncrement* growth_increment_ = nullptr;
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_LENGTH_REPORTS_GROWTHINCREMENT_H_ */
