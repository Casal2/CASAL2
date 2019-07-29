/**
 * @file Addressable.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Addressable.h"

#include "Model/Model.h"
#include "Model/Objects.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace asserts {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Addressable::Addressable(Model* model) : Assert(model) {
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Addressable to check", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to check addressable", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time step to execute after", "");
  parameters_.Bind<unsigned>(PARAM_VALUES, &values_, "Values to check against the addressable", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void Addressable::DoValidate() {
  if (parameter_ == "")
    parameter_ = label_;

  if (years_.size() != values_.size())
    LOG_ERROR_P(PARAM_YEARS) << "count (" << years_.size() << ") must match the number of values provided (" << values_.size() << ")";

  vector<unsigned> model_years = model_->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_YEARS) << "(" << year << ") is not a valid year in the model.";
  }

  for (unsigned i = 0; i < years_.size(); ++i)
    year_values_[years_[i]] = values_[i];
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void Addressable::DoBuild() {
  /**
   * subscribe this assert to the target time step in all years that were specified.
   */
  TimeStep* time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_ERROR_P(PARAM_TIME_STEP) << "(" << time_step_label_ << ") does not exist. Have you defined it?";
  for (unsigned year : years_)
    time_step->Subscribe(this, year);

  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in assert.addressable. Error was " << error;
  }

  addressable_ = model_->objects().GetAddressable(parameter_);
}

/**
 * Execute/Run/Process the object.
 */
void Addressable::Execute() {
  Double expected = year_values_[model_->current_year()];
  if (*addressable_ != expected)
    LOG_ERROR() << "Assert Failure: Addressable: " << parameter_ << " had actual value " << *addressable_ << " when we expected " << expected;
}

} /* namespace asserts */
} /* namespace niwa */
