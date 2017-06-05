/**
 * @file DerivedQuantity.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 4/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "DerivedQuantity.h"

#include <limits>

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {

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
DerivedQuantity::DerivedQuantity(Model* model)
  : model_(model),
    partition_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label of the derived quantity", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of derived quantity", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The time step in which to calculate the derived quantity after", "");
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories to use when calculating the derived quantity", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "A list of one selectivity", "");
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "Proportion through the mortality block of the time step when calculated", "", Double(0.5))->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_TIME_STEP_PROPORTION_METHOD, &proportion_method_, "Method for interpolating for the proportion through the mortality block", "", PARAM_WEIGHTED_SUM)
      ->set_allowed_values({ PARAM_WEIGHTED_SUM, PARAM_WEIGHTED_PRODUCT });

  RegisterAsEstimable(PARAM_VALUES, &override_values_);
  create_missing_estimables_[PARAM_VALUES] = true;

  mean_proportion_method_ = true;
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void DerivedQuantity::Validate() {
  parameters_.Populate();
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  // Validate Categories
  auto categories = model_->categories();
  for (const string& label : category_labels_) {
    if (!categories->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }



  if (proportion_method_ == PARAM_WEIGHTED_PRODUCT)
    mean_proportion_method_ = false;

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << " count (" << selectivity_labels_.size() << ") "
        << " is not the same as the categories count (" << category_labels_.size() << ")";
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void DerivedQuantity::Build() {
  LOG_TRACE();


  partition_.Init(category_labels_);

  selectivities::Manager& selectivity_manager = *model_->managers().selectivity();
  for (string label : selectivity_labels_) {
    Selectivity* selectivity = selectivity_manager.GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << " (" << label << ") could not be found. Have you defined it?";

    selectivities_.push_back(selectivity);
  }

  /**
   * ensure the time steps we have are valid
   */
  TimeStep* time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step)
    LOG_ERROR_P(PARAM_TIME_STEP) << " (" << time_step_label_ << ") could not be found. Have you defined it?";
  time_step->SubscribeToBlock(this);
  time_step->SubscribeToInitialisationBlock(this);
}

/**
 * Reset our derived quantity values
 */
void DerivedQuantity::Reset() {
  initialisation_values_.clear();

  // initialise the values variable
  for (unsigned year : model_->years())
    values_[year] = 0.0;
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
 * @return The derived quantity value
 */
Double DerivedQuantity::GetValue(unsigned year) {
  LOG_FINEST() << "get value for year: " << year;
  if (override_values_.find(year) != override_values_.end()) {
    values_[year] = override_values_[year];
    return override_values_[year];
  }
  if (values_.find(year) != values_.end())
    return values_[year];
  if (initialisation_values_.size() == 0)
    return 0.0;

  // Calculate how many years to go back. At this point
  // either we're in the init phases or we're going back
  // in to the init phases.
  unsigned years_to_go_back = model_->start_year() - year;

  Double result = 0.0;
  if (years_to_go_back == 0) {
    LOG_WARNING() << "Years to go back is 0 in derived quantity " << label_ << " when it shouldn't be";
    result = (*initialisation_values_.rbegin()->rbegin());
  } else if (initialisation_values_.rbegin()->size() > years_to_go_back) {
    result = initialisation_values_.rbegin()->at(initialisation_values_.rbegin()->size() - years_to_go_back);
  } else if (initialisation_values_.size() == 1) {
    result = (*initialisation_values_.rbegin()->begin()); // first value of last init phase
  } else {
    result = (*(initialisation_values_.rbegin() + 1)->begin()); // first value of last init phase
  }

  // Make an exception for intialisation phases such as derived which only requires to go back one year
  if (model_->b0_initialised(label_)) {
    result = (*initialisation_values_.rbegin()->rbegin());
  }

  LOG_FINEST() << "years_to_go_back: " << years_to_go_back
      << "; year: " << year
      << "; result: " << result
      << "; .begin(): " << (*initialisation_values_.rbegin()->rbegin())
      << ": .size(): " << initialisation_values_.rbegin()->size();

  return result;
}

/**
 * Return the last value stored for the target initialisation phase
 *
 * @param phase The index of the phase
 * @return The derived quantity value
 */
Double DerivedQuantity::GetLastValueFromInitialisation(unsigned phase) {
  if (initialisation_values_.size() <= phase)
    LOG_ERROR() << "No values have been calculated for the initialisation value in phase: " << phase;
  if (initialisation_values_[phase].size() == 0)
    LOG_ERROR() << "No values have been calculated for the initialisation value in phase: " << phase;

  return *initialisation_values_[phase].rbegin();
}

/**
 * Return an initialisation value from a target phase with an index.
 * If no phases exist we return 0.0
 * If the phase contains no values return 0.0
 * If the phase doesn't have enough values for the index return the last value
 *
 * @param phase The index of the phase
 * @param index The index of the value in the phase
 * @return derived quantity value
 */
Double DerivedQuantity::GetInitialisationValue(unsigned phase, unsigned index) {
  LOG_FINEST() << "phase = " << phase << "; index = " << index << "; initialisation_values_.size() = " << initialisation_values_.size();
  if (initialisation_values_.size() <= phase) {
    if (initialisation_values_.size() == 0)
      return 0.0;

    return (*initialisation_values_.rbegin()->rbegin());
  }

  LOG_FINEST() << "initialisation_values_[" << phase << "].size() = " << initialisation_values_[phase].size();
  if (initialisation_values_[phase].size() <= index) {
    if (initialisation_values_[phase].size() == 0)
      return 0.0;
    else
      return *initialisation_values_[phase].rbegin();
  }

  return initialisation_values_[phase][index];
}

} /* namespace niwa */
