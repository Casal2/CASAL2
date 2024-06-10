/**
 * @file UserDefined.h
 * @author  C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * UserDefined is a class that uses the equation parser to define future parameters. Primarily for setting future catches based on variables in the system like Harvest control
 * rules.
 */
#ifndef PROJECTS_USER_DEFINED_H_
#define PROJECTS_USER_DEFINED_H_

// headers
#include "../../Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class UserDefined : public niwa::Project {
public:
  explicit UserDefined(shared_ptr<Model> model);
  virtual ~UserDefined() = default;
  void DoValidate() override final{};
  void DoBuild() override final;
  void DoReset() override final{};
  void DoUpdate() override final;

private:
  // members
  vector<string> equation_input_;
  string         equation_ = "";
  Double         value_;
};

} /* namespace projects */
} /* namespace niwa */

#endif /* PROJECTS_USER_DEFINED_H_ */
