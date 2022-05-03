/**
 * @file Addressable.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef ASSERT_ADDRESSABLE_H_
#define ASSERT_ADDRESSABLE_H_

// headers
#include "../../Asserts/Assert.h"

// namespaces
namespace niwa {
namespace asserts {

// classes
class Addressable : public niwa::Assert {
public:
  // methods
  Addressable(shared_ptr<Model> model);
  virtual ~Addressable() = default;
  void Execute() override final;

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;

private:
  // members
  string                parameter_ = "";
  vector<unsigned>      years_;
  string                time_step_label_ = "";
  vector<Double>        values_;
  map<unsigned, Double> year_values_;
  Double*               addressable_ = nullptr;
};

} /* namespace asserts */
} /* namespace niwa */

#endif /* ASSERT_ADDRESSABLE_H_ */
