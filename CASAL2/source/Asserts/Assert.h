/**
 * @file Assert.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * An assert is a runtime check done in the model. If the check
 * fails then the model will stop and the system will return a non-zero value.
 *
 * This is primarily designed for use within unit tests or automated testing of
 * models to ensure the model outputs do not change over time.
 */
#ifndef ASSERT_H_
#define ASSERT_H_

// headers
#include "../BaseClasses/Executor.h"
#include "../Translations/Translations.h"

// namespaces
namespace niwa {
class Model;

// classes
class Assert : public niwa::base::Executor {
public:
  // methods
  Assert() = delete;
  explicit Assert(shared_ptr<Model> model);
  virtual ~Assert() = default;
  void Validate();
  void Build() { DoBuild(); };
  void Verify(shared_ptr<Model> model){};
  void Reset(){};
  void PreExecute() final{};

protected:
  // methods
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;

  // members
  shared_ptr<Model> model_ = nullptr;
  string            error_type_;
  Double            tol_ = 1e-5;
};
} /* namespace niwa */

#endif /* ASSERT_H_ */
