/**
 * @file Profile.h
 * @author C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will take a target a project and print all of
 * the values that have been assigned to it.
 */
#ifndef SOURCE_REPORTS_CHILDREN_PROFILE_H_
#define SOURCE_REPORTS_CHILDREN_PROFILE_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
class Profile;
namespace reports {

// classes
class Profile : public Report {
public:
  Profile();
  virtual ~Profile() = default;
  void DoValidate(shared_ptr<Model> model) override final;
  void DoBuild(shared_ptr<Model> model) override final;
  void DoExecute(shared_ptr<Model> model) override final;
  void DoPrepareTabular(shared_ptr<Model> model) override final;

private:
  niwa::Profile* profile_       = nullptr;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_PROFILE_H_ */
