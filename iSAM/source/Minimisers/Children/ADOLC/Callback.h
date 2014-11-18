/**
 * @file CGammaDiffCallback.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
#ifndef MINIMISERS_ADOLC_CALLBACK_H_
#define MINIMISERS_ADOLC_CALLBACK_H_

// Headers
#include <vector>

#include "Model/Model.h"

// namespaces
namespace isam {
namespace minimisers {
namespace adolc {

using std::vector;

/**
 * Class definition
 */
class CallBack {
public:
  CallBack();
  virtual                     ~CallBack() = default;
  adouble                     operator()(const vector<adouble>& Parameters);

private:
  ModelPtr                    model_;
};

} /* namespace gammadiff */
} /* namespace minimiser */
} /* namespace isam */

#endif /* MINIMISERS_ADOLC_CALLBACK_H_ */
#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
