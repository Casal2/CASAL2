/**
 * @file CategoryList.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_REPORTS_CHILDREN_CATEGORYLIST_H_
#define SOURCE_REPORTS_CHILDREN_CATEGORYLIST_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

// classes
class CategoryList : public niwa::Report {
public:
  // methods
  CategoryList();
  virtual ~CategoryList() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final{};
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_CATEGORYLIST_H_ */
