/**
 * @file Exponential.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// headers
#include "Exponential.h"

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
Exponential::Exponential(shared_ptr<Model> model) : GrowthIncrement(model) {
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
void Exponential::DoValidate() {

}

/*
* @param length length to calculate growth increment from
* 
*/
Double Exponential::get_mean_increment(double length) { 
    return g_a_ * pow(g_b_ / g_a_, (length - l_a_)/(l_b_ - l_a_));
};

} /* namespace lengthweights */
} /* namespace niwa */
