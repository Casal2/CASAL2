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
#ifndef USE_AUTODIFF
#ifndef MINIMISERS_GAMMADIFF_CALLBACK_H_
#define MINIMISERS_GAMMADIFF_CALLBACK_H_

// Headers
#include <vector>

#include "Model/Model.h"

// namespaces
namespace isam {
namespace minimisers {
namespace gammadiff {

using std::vector;

/**
 * Class definition
 */
class CallBack {
public:
  CallBack();
  virtual                     ~CallBack() = default;
  double                      operator()(const vector<double>& Parameters);

private:
  ModelPtr                    model_;
};

} /* namespace gammadiff */
} /* namespace minimiser */
} /* namespace isam */

#endif /* MINIMISERS_GAMMADIFF_CALLBACK_H_ */
#endif
