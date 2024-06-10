/**
 * @file AgeLength.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */

#ifndef SOURCE_AGE_REPORTS_AGE_AGELENGTH_H_
#define SOURCE_AGE_REPORTS_AGE_AGELENGTH_H_

// Headers
#include "../../Reports/Report.h"

namespace niwa {
class AgeLength;
namespace reports {
namespace age {

class AgeLength : public niwa::Report {
public:
  // methods
  AgeLength();
  virtual ~AgeLength() = default;
  void DoValidate(shared_ptr<Model> model) override final;
  void DoBuild(shared_ptr<Model> model) override final;
  void DoExecute(shared_ptr<Model> model) override final;
  void DoPrepareTabular(shared_ptr<Model> model) override final;

private:
  string           age_length_label_;
  string           category_;
  niwa::AgeLength* age_length_ = nullptr;
};

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_AGE_REPORTS_AGE_AGELENGTH_H_ */
