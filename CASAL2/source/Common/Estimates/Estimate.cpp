/**
 * @file Estimate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Estimate.h"

#include "Common/EstimateTransformations/EstimateTransformation.h"
#include "Common/EstimateTransformations/Factory.h"
#include "Common/Model/Factory.h"
#include "Common/Model/Model.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/Utilities/To.h"

// Namespaces
namespace niwa {

/**
 * Default constructor
 */
Estimate::Estimate(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label of the estimate", "", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The prior type for the estimate", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the parameter to estimate in the model", "");
  parameters_.Bind<Double>(PARAM_LOWER_BOUND, &lower_bound_, "The lower bound for the parameter", "");
  parameters_.Bind<Double>(PARAM_UPPER_BOUND, &upper_bound_, "The upper bound for the parameter", "");
//  parameters_.Bind<string>(PARAM_PRIOR, &prior_label_, "TBA", "", ""); // This is type
  parameters_.Bind<string>(PARAM_SAME, &same_labels_, "List of parameters that are constrained to have the same value as this parameter", "", "");
  parameters_.Bind<unsigned>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "The first estimation phase to allow this to be estimated", "", 1);
  parameters_.Bind<bool>(PARAM_MCMC, &mcmc_fixed_, "Indicates if this parameter is fixed at the point estimate during an MCMC run", "", false);
  parameters_.Bind<string>(PARAM_TRANSFORMATION, &transformation_type_, "Type of simple transformation to apply to estimate", "", "");
  parameters_.Bind<bool>(PARAM_TRANSFORM_WITH_JACOBIAN, &transform_with_jacobian_, "Apply jacobian during transformation", "", false);
  parameters_.Bind<bool>(PARAM_PRIOR_APPLIES_TO_TRANSFORM, &transform_for_objective_function_, "Does the prior apply to the transformed parameter? a legacy switch, see Manual for more information", "", false);
}

/**
 * Validate our estimate. Some of the
 * validation was done by the
 * estimates::Info object before the
 * estimate was created so we can skip that.
 */
void Estimate::Validate() {
  if (transform_with_jacobian_ & transform_for_objective_function_)
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "You cannot specify an estimate that has a jacobian contributing to the objective function and define the prior for the transformed variable together. See the manual for more info";
  DoValidate();
}

void Estimate::Build() {
  if (transform_with_jacobian_ & transform_for_objective_function_)
    LOG_ERROR_P(PARAM_TRANSFORM_WITH_JACOBIAN) << "You cannot specify both " << PARAM_TRANSFORM_WITH_JACOBIAN << " and " << PARAM_PRIOR_APPLIES_TO_TRANSFORM << " to be true. Please check the manual for more info";

  if (!transform_for_objective_function_) {
    // only check bounds if prior on untransformed variable.
    if (*target_ < lower_bound_)
      LOG_ERROR() << location() <<  "the initial value(" << AS_DOUBLE((*target_)) << ") on the estimate " << parameter_
          << " is lower than the lower_bound(" << lower_bound_ << ")";
    if (*target_ > upper_bound_)
      LOG_ERROR() << location() << "the initial value(" << AS_DOUBLE((*target_)) << ") on the estimate " << parameter_
          << " is greater than the upper_bound(" << upper_bound_ << ")";
  }

  transform_with_jacobian_is_defined_ = parameters_.Get(PARAM_PRIOR_APPLIES_TO_TRANSFORM)->has_been_defined();

  // allow users to specify none
  if (transformation_type_ == PARAM_NONE)
    transformation_type_ = "";
  /**
   * If we have a transformation declared, we need to create it here and
   * let it know if it needs to calculate a Jacobian.
   */
  if (transformation_type_ != "") {
    // Check to see if it is a simple transformation

    LOG_FINEST() << "Applying transformaton to @estimate: " << label_ << ", label of estimate transformation = " <<  transformation_type_ + "_" + label_ << ", transformation type = " << transformation_type_;
    string boolean_value = "";
    boolean_value = utilities::ToInline<bool, string>(transform_with_jacobian_);
    EstimateTransformation* transformation = estimatetransformations::Factory::Create(model_, PARAM_ESTIMATE_TRANSFORMATION, transformation_type_);
    if(!transformation)
      LOG_ERROR_P(PARAM_TRANSFORMATION) << "Wrong transformation type, check the manual for available types.";
    if (!transformation->is_simple())
      LOG_FATAL_P(PARAM_TRANSFORMATION) << "Transformation type is not simple, only univariate (simple) transfomrations can be applied with this functionality. See the manual for information on how to apply more complex transformations";

    transformation->parameters().Add(PARAM_LABEL, transformation_type_ + "_" + label_, __FILE__, __LINE__);
    transformation->parameters().Add(PARAM_TYPE, transformation_type_, __FILE__, __LINE__);
    transformation->parameters().Add(PARAM_ESTIMATE_LABEL, label_, __FILE__, __LINE__);
    transformation->parameters().Add(PARAM_TRANSFORM_WITH_JACOBIAN, boolean_value, __FILE__, __LINE__);
    transformation->Validate();
    transformation->Build();

    if (transform_for_objective_function_) {
      // if prior on transformed varible check to see if the bounds make sense once the transformation has occured.
      transformation->Transform();
      if (*target_ < lower_bound_)
        LOG_ERROR() << location() <<  "the initial value(" << AS_DOUBLE((*target_)) << ") on the estimate " << parameter_
            << " is lower than the lower_bound(" << lower_bound_ << ")";
      if (*target_ > upper_bound_)
        LOG_ERROR() << location() << "the initial value(" << AS_DOUBLE((*target_)) << ") on the estimate " << parameter_
            << " is greater than the upper_bound(" << upper_bound_ << ")";
      transformation->Restore();
    }
  }

  Reset();
}


/**
 *
 */
void Estimate::Reset() {
  /**
   * Reset the value if the bounds are the same so we can ensure all of the
   * "same" parameters are aligned
   */
  if (utilities::doublecompare::IsEqual(lower_bound_, upper_bound_))
    set_value(value());
}

/**
 * This method will add all of the "sames" to this object.
 * We add the labels as well for reporting and debugging
 * purposes.
 *
 * @param label The label of the same to add
 * @param target The target value to modify when we set a new value
 */
void Estimate::AddSame(const string& label, Double* target) {
  same_labels_.push_back(label);
  sames_.push_back(target);
}

/**
 * Assign a new value to the object pointed to by
 * this estimate. We will also iterate over any
 * "sames" and assign the value to them as well.
 *
 * @param new_value The new value to assign.
 */
void Estimate::set_value(Double new_value) {
  *target_ = new_value;
  for (Double* same : sames_)
    *same = new_value;
}


} /* namespace niwa */

