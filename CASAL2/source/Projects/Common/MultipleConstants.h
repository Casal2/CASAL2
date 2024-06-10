/**
 * @file MultipleConstants.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROJECTS_MULTIPLECONSTANT_H_
#define PROJECTS_MULTIPLECONSTANT_H_

// headers
#include "../../Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class MultipleConstants : public niwa::Project {
public:
  explicit MultipleConstants(shared_ptr<Model> model);
  virtual ~MultipleConstants();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoUpdate() override final;

protected:
  // members
  parameters::Table*                                  data_table_ = nullptr;
  vector<map<unsigned, Double>>                       projection_values_; // dash_i x n_years
  Double                                              value_;
};

} /* namespace projects */
} /* namespace niwa */

#endif /* PROJECTS_MULTIPLECONSTANT_H_ */
