/**
 * @file DerivedQuantity.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 4/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#include "DerivedQuantity.h"

#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"

namespace isam {

/**
 * default constructor
 */
DerivedQuantity::DerivedQuantity() {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The time step to calculate the derived quantity after", "");
  parameters_.Bind<string>(PARAM_INITIALISATION_TIME_STEPS, &initialisation_time_step_labels_, "The initialisation time steps to calculate the derived quantity after", "", true);
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories to use when calculating the derived quantity", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The list of selectivities to use when calculating the derived quantity. 1 per category", "");

  model_ = Model::Instance();
}

/**
 * Validate the parameters defined in the configuration
 * file for this derived quantity.
 */
void DerivedQuantity::Validate() {
  parameters_.Populate();

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << " count (" << selectivity_labels_.size() << ") "
        << " is not the same as the categories count (" << category_labels_.size() << ")");
}

/**
 * Build the run time relationships between the derived
 * quantity and other components in the model
 */
void DerivedQuantity::Build() {
  LOG_TRACE();

  partition_.Init(category_labels_);

  selectivities::Manager& selectivity_manager = selectivities::Manager::Instance();
  for (string label : selectivity_labels_) {
    SelectivityPtr selectivity = selectivity_manager.GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << " (" << label << ") could not be found. Have you defined it?");

    selectivities_.push_back(selectivity);
  }

  /**
   * ensure the time steps we have are valid
   */
  TimeStepPtr time_step = timesteps::Manager::Instance().GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP) << " (" << time_step_label_ << ") could not be found. Have you defined it?");
  time_step->SubscribeToBlock(shared_ptr());

  for (const string label : initialisation_time_step_labels_) {
    TimeStepPtr init_time_step = timesteps::Manager::Instance().GetTimeStep(label);
    if (!init_time_step)
      LOG_ERROR(parameters_.location(PARAM_INITIALISATION_TIME_STEPS) << " (" << label << ") could not be found. Have you defined it?");
    init_time_step->SubscribeToInitialisationBlock(shared_ptr());
  }
}

/**
 * Check if this derived quantity has the initialisation phase label
 * assigned too it. This is used to determine if we need to
 * bind this derived quantity to an initialisation phase
 * for calculation
 *
 * @param label The label of the initialisation time step to check
 * @return True if assigned to parameter initialisation time step, false otherwise
 */
bool DerivedQuantity::IsAssignedToInitialisationPhase(const string& label) {
  return std::find(initialisation_time_step_labels_.begin(), initialisation_time_step_labels_.end(), label)
    != initialisation_time_step_labels_.end();
}

/**
 * Return the calculated value stored in this derived quantity
 * for the parameter year. If the year does not exist as a standard
 * value we'll calculate how many years to go back in to
 * the initialisation phase values.
 *
 * Note: We cannot go back more than 1 phase. If this condition
 * is triggered we return the first value from the phase instead
 * of going back.
 *
 * @param year The year to get the derived quantity value for.
 */
Double DerivedQuantity::GetValue(unsigned year) {
  if (values_.find(year) != values_.end()) {
    return values_[year];
  }

  if (values_.size() > 0 && values_.rbegin()->first < year)
    LOG_ERROR("Trying to get a value from the derived quantity " << label_ << " for year " << year << " when the latest year calculated is "
        << values_.rbegin()->first);
  if (initialisation_values_.size() == 0)
    return 0.0;

  // Calculate how many years to go back. At this point
  // either we're in the init phases or we're going back
  // in to the init phases.
  unsigned years_to_go_back = model_->start_year() - year;

  Double result = 0.0;
  if (years_to_go_back == 0) {
    LOG_WARNING("Years to go back is 0 in derived quantity " << label_ << " when it shouldn't be");
    result = (*initialisation_values_.rbegin()->rbegin());
  } else if (initialisation_values_.rbegin()->size() > years_to_go_back) {
    result = initialisation_values_.rbegin()->at(initialisation_values_.rbegin()->size() - years_to_go_back);
  } else {
    LOG_CODE_ERROR("this should be going back in to another init phase if possible (" << label_ << ")");
    result = (*initialisation_values_.rbegin()->begin()); // first value of last init phase
  }

  LOG_INFO("years_to_go_back: " << years_to_go_back
      << "; year: " << year
      << "; result: " << result
      << "; .begin(): " << (*initialisation_values_.rbegin()->rbegin())
      << ": .size(): " << initialisation_values_.rbegin()->size());

  return result;
}

/**
 *
 */
Double DerivedQuantity::GetLastValueFromInitialisation(unsigned phase) {
  if (initialisation_values_.size() <= phase)
    LOG_ERROR("No values have been calculated for the initialisation value in phase: " << phase);
  if (initialisation_values_[phase].size() == 0)
    LOG_ERROR("No values have been calculated for the initialisation value in phase: " << phase);

  return *initialisation_values_[phase].rbegin();
}

/**
 *
 */
Double DerivedQuantity::GetInitialisationValue(unsigned phase, unsigned index) {
  LOG_INFO("phase = " << phase << "; index = " << index << "; initialisation_values_.size() = " << initialisation_values_.size());
  if (initialisation_values_.size() <= phase) {
    if (initialisation_values_.size() == 0)
      return 0.0;

    return (*initialisation_values_.rbegin()->rbegin());
  }

  LOG_INFO("initialisation_values_[" << phase << "].size() = " << initialisation_values_[phase].size());
  if (initialisation_values_[phase].size() <= index) {
    if (initialisation_values_[phase].size() == 0)
      return 0.0;
    else
      return *initialisation_values_[phase].rbegin();
  }

  return initialisation_values_[phase][index];
}

} /* namespace isam */
