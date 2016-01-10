/**
 * @file DLib.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef USE_AUTODIFF

// headers
#include "DLib.h"

#include <dlib/optimization.h>

#include "Estimates/Manager.h"
#include "Minimisers/Children/DLib/CallBack.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
namespace minimisers {

using namespace dlib;

typedef ::dlib::matrix<double,0,1> column_vector;

/**
 * default constructor
 */
DLib::DLib(Model* model) : Minimiser(model) {
}

/**
 *
 */
void DLib::Execute() {
  cout << "Executing DLib Minimiser" << endl;
  // Variables
  dlib::Callback  call_back(model_);

  estimates::Manager& estimate_manager = *model_->managers().estimate();
  vector<Estimate*> estimates = estimate_manager.GetEnabled();

  ::dlib::matrix<double, 0, 1> start_values(estimates.size());
  ::dlib::matrix<double, 0, 1> lower_bounds(estimates.size());
  ::dlib::matrix<double, 0, 1> upper_bounds(estimates.size());

//  double smallest_lower_bound = 9999999.9;

  unsigned i = 0;
  for (Estimate* estimate : estimates) {
    if (!estimate->enabled())
      continue;

    lower_bounds(i) = estimate->lower_bound();
    upper_bounds(i) = estimate->upper_bound();
    start_values(i) = estimate->value(); // utilities::math::scaleValue(estimate->value(), estimate->lower_bound(), estimate->upper_bound());
//    if (start_values(i) < lower_bounds(i))
//      start_values(i) = lower_bounds(i);
//    if (start_values(i) < upper_bounds(i))
//      start_values(i) = upper_bounds(i);
//
//    smallest_lower_bound = smallest_lower_bound < estimate->lower_bound() ? smallest_lower_bound : estimate->lower_bound();

//    start_values.push_back((double)estimate->value());

    if (estimate->value() < estimate->lower_bound()) {
      LOG_ERROR() << "When starting the DLib minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_ERROR() << "When starting the DLib minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")";
    }

    ++i;
  }

  // cg_search_strategy()
  // lbfgs_search_strategy(10)
  // bfgs_search_strategy()
  ::dlib::find_min_using_approximate_derivatives(::dlib::bfgs_search_strategy(),
                                                 ::dlib::objective_delta_stop_strategy(1e-7),
                                                  dlib::Callback(model_), start_values, -1);

//  ::dlib::find_min_bobyqa(dlib::Callback(),
//                  start_values,
//                  (start_values.size()+1)*(start_values.size()+2)/2,    // number of interpolation points
//                  lower_bounds,  // lower bound constraint
//                  upper_bounds,   // upper bound constraint
//                  1e-2,    // initial trust region radius
//                  1e-6,  // stopping trust region radius
//                  100000    // max number of objective function evaluations
//                  );
}


} /* namespace minimisers */
} /* namespace niwa */

#endif /* NOT USE_AUTODIFF */
