/**
 * @file AgeLength.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */

#ifndef SOURCE_AGE_REPORTS_CHILDREN_AGELENGTH_H_
#define SOURCE_AGE_REPORTS_CHILDREN_AGELENGTH_H_

// Headers
#include "Common/Reports/Report.h"

namespace niwa {
namespace age {
namespace reports {

class AgeLength: public niwa::Report {
public:
  // methods
  AgeLength(Model* model);
  virtual                     ~AgeLength() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  string                      age_length_label_;
  string                      category_;
};

} /* namespace reports */
}
} /* namespace niwa */

#endif /* SOURCE_AGE_REPORTS_CHILDREN_AGELENGTH_H_ */
