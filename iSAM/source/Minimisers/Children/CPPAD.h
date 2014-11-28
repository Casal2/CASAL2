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
namespace isam {
namespace minimisers {

using isam::utilities::Double;

/**
 * class definition
 */
class CPPAD : public Minimiser {
public:
  CPPAD();
  virtual                     ~CPPAD() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoReset() override final { };
  void                        Execute() override final;
};

} /* namespace minimisers */
} /* namespace isam */

#endif /* MNINIMISERS_CPPAD_H_ */
#endif /* USE_CPPAD */
#endif /* USE_AUTODIFF */
