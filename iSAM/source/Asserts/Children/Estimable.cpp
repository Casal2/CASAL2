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
#include "TimeSteps/Manager.h"

// namespaces
namespace isam {
namespace asserts {

/**
 * Default constructor
 */
Estimable::Estimable() {
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to check estimable", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time step to execute after", "");
  parameters_.Bind<unsigned>(PARAM_VALUES, &values_, "Values to check against the estimable", "");
}

/**
 *
 */
void Estimable::DoValidate() {
  if (years_.size() != values_.size())
    LOG_ERROR(parameters_.location(PARAM_YEARS) << " count (" << years_.size() << ") must match the number of values provided (" << values_.size() << ")");

  vector<unsigned> model_years = Model::Instance()->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR(parameters_.location(PARAM_YEARS) << "(" << year << ") is not a valid year in the model.")
  }
}

/**
 *
 */
void Estimable::DoBuild() {
  /**
   * subscribe this assert to the target time step in all years that were specified.
   */
  TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP) << "(" << time_step_label_ << ") does not exist. Have you defined it?");
  for (unsigned year : years_)
    time_step->Subscribe(shared_ptr(), year);
}

/**
 *
 */
void Estimable::Execute() {

}

} /* namespace asserts */
} /* namespace isam */
