/**
 * @file Basic.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// headers
#include "Basic.h"

// namespaces
namespace niwa {
namespace growthincrements {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
Basic::Basic(shared_ptr<Model> model) : GrowthIncrement(model) {
  parameters_.Bind<Double>(PARAM_G_ALPHA, &g_a_, "Growth alpha parameter", "");
  parameters_.Bind<Double>(PARAM_G_BETA, &g_b_, "Growth beta parameter", "");
  parameters_.Bind<Double>(PARAM_L_ALPHA, &l_a_, "Reference length for growth alpha", "");
  parameters_.Bind<Double>(PARAM_L_BETA, &l_b_, "Reference length for growth beta", "");

  RegisterAsAddressable(PARAM_G_ALPHA, &g_a_);
  RegisterAsAddressable(PARAM_G_BETA, &g_b_);
  RegisterAsAddressable(PARAM_L_ALPHA, &l_a_);
  RegisterAsAddressable(PARAM_L_BETA, &l_b_);
}

/*
 * Validate child class
 *
 */
void Basic::DoValidate() {
  //  bool has_one_time_step_with_all_growth = false; // can only apply annual growth
  //  for(auto time_value : time_step_proportions_) {
  //    if(utilities::math::IsOne(time_value))
  //      has_one_time_step_with_all_growth = true;
  //  }
  //  if(!has_one_time_step_with_all_growth)
  //    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "This growth model has not been validated to have multiple length increments within a year. This growth model requires a
  //    time_step_proportions value of one for the time-step to apply annual growth and zero in all others";
}
/*
 * @param length length to calculate growth increment from
 *
 */
Double Basic::get_mean_increment(double length, double time_step_proportion) {
  Double increment = g_a_ + (g_b_ - g_a_) * (length - l_a_) / (l_b_ - l_a_);
  LOG_FINEST() << "length = " << length << " increment = " << increment;
  return increment;
};

}  // namespace growthincrements
} /* namespace niwa */
