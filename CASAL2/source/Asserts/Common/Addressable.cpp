/**
 * @file Addressable.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Addressable.h"

#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace asserts {

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
Addressable::Addressable(shared_ptr<Model> model) : Assert(model) {
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The addressable to check", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years to check addressable", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The time step to execute after", "");
  parameters_.Bind<Double>(PARAM_VALUES, &values_, "The values to check against the addressable", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void Addressable::DoValidate() {
  if (parameter_ == "")
    parameter_ = label_;

  if (years_.size() != values_.size())
    LOG_ERROR_P(PARAM_YEARS) << " year count (" << years_.size() << ") must match the number of values provided (" << values_.size() << ")";

  vector<unsigned> model_years = model_->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_YEARS) << "year (" << year << ") is not a valid year in the model";
  }

  for (unsigned i = 0; i < years_.size(); ++i) year_values_[years_[i]] = values_[i];
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void Addressable::DoBuild() {
  /**
   * subscribe this assert to the target time step in all years that were specified.
   */
  TimeStep* time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_ERROR_P(PARAM_TIME_STEP) << "time step label (" << time_step_label_ << ") was not found";
  for (unsigned year : years_) time_step->Subscribe(this, year);

  string error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be found. Error: " << error;
  }

  addressable_ = model_->objects().GetAddressable(parameter_);
}

/**
 * Execute/Run/Process the object.
 */
void Addressable::Execute() {
  std::streamsize prec = std::cout.precision();
  std::cout.precision(12);

  if (model_->run_mode() == RunMode::kBasic) {
    Double expected = year_values_[model_->current_year()];
    Double diff     = *addressable_ - expected;
    if (error_type_ == PARAM_ERROR && !utilities::math::IsBasicallyEqual(*addressable_, expected, tol_)) {
      LOG_ERROR() << "Assert Failure: The Addressable with label '" << label_ << "' and parameter " << parameter_ << " has value " << *addressable_ << " and " << expected
                  << " was expected. The difference was " << diff;
    } else if (!utilities::math::IsBasicallyEqual(*addressable_, expected, tol_)) {
      LOG_WARNING() << "Assert Failure: The Addressable with label '" << label_ << "' and parameter " << parameter_ << " has value " << *addressable_ << " and " << expected
                    << " was expected. The difference was " << diff;
    } else {
      LOG_INFO() << "Assert Passed: The Addressable with label '" << label_ << "' and parameter " << parameter_ << " has value " << *addressable_ << " and " << expected
                 << " was expected.";
    }
  }
  std::cout.precision(prec);
}

} /* namespace asserts */
} /* namespace niwa */
