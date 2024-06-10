/**
 * @file Constant.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/01/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef TIMEVARYING_CONSTANT_H_
#define TIMEVARYING_CONSTANT_H_

// headers
#include "../../TimeVarying/TimeVarying.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Class definition
 */
class Constant : public niwa::TimeVarying {
public:
  // methods
  explicit Constant(shared_ptr<Model> model);
  virtual ~Constant() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoUpdate() override final;

protected:
  // members
  vector<Double> values_;
};

} /* namespace timevarying */
} /* namespace niwa */

#endif /* TIMEVARYING_CONSTANT_H_ */
