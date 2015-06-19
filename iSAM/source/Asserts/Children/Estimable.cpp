/**
 * @file Estimable.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Estimable.h"

#include "Model/Model.h"
#include "ObjectsFinder/ObjectsFinder.h"
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
Estimable::Estimable() {
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Estimable to check", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to check estimable", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time step to execute after", "");
  parameters_.Bind<unsigned>(PARAM_VALUES, &values_, "Values to check against the estimable", "");
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void Estimable::DoValidate() {
  if (parameter_ == "")
    parameter_ = label_;

  if (years_.size() != values_.size())
    LOG_ERROR_P(PARAM_YEARS) << "count (" << years_.size() << ") must match the number of values provided (" << values_.size() << ")";

  vector<unsigned> model_years = Model::Instance()->years();
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
void Estimable::DoBuild() {
  /**
   * subscribe this assert to the target time step in all years that were specified.
   */
  TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_ERROR_P(PARAM_TIME_STEP) << "(" << time_step_label_ << ") does not exist. Have you defined it?";
  for (unsigned year : years_)
    time_step->Subscribe(shared_ptr(), year);

  estimable_ = objects::FindEstimable(parameter_);
  if (estimable_ == 0)
    LOG_ERROR_P(PARAM_PARAMETER) << "(" << parameter_ << ") could not be found. Have you defined it properly?";
}

/**
 * Execute/Run/Process the object.
 */
void Estimable::Execute() {
  Double expected = year_values_[Model::Instance()->current_year()];
  if (*estimable_ != expected)
    LOG_ERROR() << "Assert Failure: Estimable: " << parameter_ << " had actual value " << *estimable_ << " when we expected " << expected;
}

} /* namespace asserts */
} /* namespace niwa */
