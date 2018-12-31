/**
 * @file DLib.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * DLib is minimiser available from dlib.net.
 * Implementation is designed around http://dlib.net/optimization_ex.cpp.html
 */
#ifndef USE_AUTODIFF
#ifndef MINIMISERS_DLIB_H_
#define MINIMISERS_DLIB_H_

// headers
#include "Minimisers/Minimiser.h"

// namespaces
namespace niwa {
namespace minimisers {




/**
 * Class definition
 */
class DLib : public niwa::Minimiser {
public:
  // methods
  DLib(Model* model);
  virtual                     ~DLib() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoReset() override final { };
  void                        Execute() override final;

private:
  // members
  string                      minimisation_type_ = "";
  string                      search_strategy_ = "";
  Double                      gradient_tolerance_ = 0;
  unsigned                    lbfgs_max_size_ = 0;
  unsigned                    bobyqa_interpolation_points_ = 0;
  Double                      bobyqa_initial_trust_radius_ = 0.0;
  Double                      bobyqa_stopping_trust_radius_ = 0.0;
  Double                      bobyqa_max_evaluations_ = 0.0;
};

} /* namespace minimisers */
} /* namespace niwa */
#endif /* MINIMISERS_DLIB_H_ */
#endif /* NOT USE_AUTODIFF */
