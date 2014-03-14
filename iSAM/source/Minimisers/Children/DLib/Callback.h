/**
 * @file Callback.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef CALLBACK_H_
#define CALLBACK_H_

// headers
#include <dlib/optimization.h>

#include "Model/Model.h"

// namespaces
namespace isam {
namespace minimisers {
namespace dlib {

/**
 *
 */
class Callback {
public:
  // methods
  Callback();
  virtual                     ~Callback() = default;
  double                      operator()(const ::dlib::matrix<double, 0, 1>& Parameters) const;

private:
  // members
  ModelPtr                    model_;
};

} /* namespace dlib */
} /* namespace minimisers */
} /* namespace isam */
#endif /* CALLBACK_H_ */
