/**
 * @file CPPAD.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifdef USE_AUTODIFF
#ifdef USE_CPPAD
#ifndef MNINIMISERS_CPPAD_H_
#define MNINIMISERS_CPPAD_H_

// headers
#include "Minimisers/Minimiser.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace minimisers {

using niwa::utilities::Double;

/**
 * class definition
 */
class CPPAD : public Minimiser {
public:
  explicit CPPAD(Model* model);
  virtual                     ~CPPAD() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoReset() override final { };
  void                        Execute() override final;

private:
  // members
  string                      retape_ = "";
  unsigned                    print_level_ = 0;
  string                      sb_ = "";
  unsigned                    max_iter_ = 0;
  Double                      tol_ = 0.0;
  Double                      acceptable_tol_ = 0.0;
  Double                      acceptable_obj_change_tol_ = 0.0;
  string                      derivative_test_ = "";
  Double                      point_perturbation_radius_ = 0.0;
};

} /* namespace minimisers */
} /* namespace niwa */

#endif /* MNINIMISERS_CPPAD_H_ */
#endif /* USE_CPPAD */
#endif /* USE_AUTODIFF */
