
/**
 * @file Engine.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
#ifndef MINIMISERS_ADOLC_ENGINE_H_
#define MINIMISERS_ADOLC_ENGINE_H_

// Global Headers
#include <Minimisers/Common/ADOLC/Callback.h>

#include <vector>

// namespaces
namespace niwa {
namespace minimisers {
namespace adolc {

using std::vector;

/**
 * Class definition
 */
class Engine {
public:
  Engine();
  virtual                     ~Engine();
  Double optimise(adolc::CallBack& objective,
      vector<Double>& start_values, vector<double>& lower_bounds,
      vector<double>& upper_bounds, int& convergence, int& max_iterations,
      int& max_evaluations, double gradient_tolerance, double **out_hessian,
      int untransformed_hessians, double step_size);
  int                          get_convergence_status() { return convergence_; }
  int                          get_iterations_used() { return iterations_used_; }
  int                          get_evaluations_used() { return evaluations_used_; }

private:
  // members
  int                         convergence_;
  int                         iterations_used_;
  int                         evaluations_used_;
  vector<Double>              final_candidates_;
};

} /* namespace adolc */
} /* namespace minimisers */
} /* namesapce niwa */

#endif /* MINIMISERS_ADOLC_ENGINE_H_ */
#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
