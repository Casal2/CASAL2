/**
 * @file Observation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Observation.h"

#include "Categories/Categories.h"
#include "Likelihoods/Factory.h"
#include "Model/Model.h"
#include "Selectivities/Manager.h"

// Namespaces
namespace isam {

/**
 * Default Constructor
 */
Observation::Observation() {
  parameters_.RegisterAllowed(PARAM_LABEL);
  parameters_.RegisterAllowed(PARAM_TYPE);
  parameters_.RegisterAllowed(PARAM_YEAR);
  parameters_.RegisterAllowed(PARAM_TIME_STEP);
  parameters_.RegisterAllowed(PARAM_LIKELIHOOD);
  parameters_.RegisterAllowed(PARAM_TIME_STEP_PROPORTION);
  parameters_.RegisterAllowed(PARAM_TIME_STEP_PROPORTION_METHOD);
  parameters_.RegisterAllowed(PARAM_CATEGORIES);
  parameters_.RegisterAllowed(PARAM_SELECTIVITIES);
  parameters_.RegisterAllowed(PARAM_SIMULATION_LIKELIHOOD);

  mean_proportion_method_ = true;
  score_                  = 0.0;
}

/**
 * Destructor
 */
Observation::~Observation() {
}

/**
 * Validate the parameters passed in from the
 * configuration file
 */
void Observation::Validate() {
  CheckForRequiredParameter(PARAM_LABEL);
  CheckForRequiredParameter(PARAM_TYPE);
  CheckForRequiredParameter(PARAM_YEAR);
  CheckForRequiredParameter(PARAM_TIME_STEP);
  CheckForRequiredParameter(PARAM_LIKELIHOOD);

  label_                        = parameters_.Get(PARAM_LABEL).GetValue<string>();
  type_                         = parameters_.Get(PARAM_TYPE).GetValue<string>();
  year_                         = parameters_.Get(PARAM_YEAR).GetValue<unsigned>();
  time_step_label_              = parameters_.Get(PARAM_TIME_STEP).GetValue<string>();
  likelihood_type_              = parameters_.Get(PARAM_LIKELIHOOD).GetValue<string>();
  time_step_proportion_         = parameters_.Get(PARAM_TIME_STEP_PROPORTION).GetValue<double>(1.0);
  time_step_proportion_method_  = parameters_.Get(PARAM_TIME_STEP_PROPORTION_METHOD).GetValue<string>(PARAM_MEAN);
  category_labels_              = parameters_.Get(PARAM_CATEGORIES).GetValues<string>();
  selectivity_labels_           = parameters_.Get(PARAM_SELECTIVITIES).GetValues<string>();

  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    if (likelihood_type_ == PARAM_PSEUDO) {
      CheckForRequiredParameter(PARAM_SIMULATION_LIKELIHOOD);
      simulation_likelihood_label_ = parameters_.Get(PARAM_SIMULATION_LIKELIHOOD).GetValue<string>();
      likelihood_type_ = simulation_likelihood_label_;
    } else {
      simulation_likelihood_label_ = likelihood_type_;
    }
  }

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << time_step_proportion_ << ") must be between 0.0 and 1.0");

  string temp = time_step_proportion_method_;
  if (temp != PARAM_MEAN && temp != PARAM_DIFFERENCE)
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP_PROPORTION_METHOD) << ": time_step_proportion_method (" << temp <<") must be either difference or mean");
  if (temp != PARAM_MEAN)
    mean_proportion_method_ = false;

  if (category_labels_.size() != 1 && selectivity_labels_.size() == 1)
    selectivity_labels_.assign(category_labels_.size(), selectivity_labels_[0]);

  if (category_labels_.size() != selectivity_labels_.size()) {
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": number of selectivites (" << selectivity_labels_.size()
        << ") does not match the number of categories (" << category_labels_.size() << ")");
  }

  /**
   * TODO: Verify that the categories are valid during the year of this observation
   */
}

/**
 * Build all of the runtime relationships required for
 * this observation
 */
void Observation::Build() {
  for(string label : selectivity_labels_) {
    SelectivityPtr selectivity = selectivities::Manager::Instance().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": Selectivity " << label << " does not exist. Have you defined it?");
    selectivities_.push_back(selectivity);
  }

  if (selectivities_.size() == 1 && category_labels_.size() != 1)
    selectivities_.assign(category_labels_.size(), selectivities_[0]);

  likelihood_ = likelihoods::Factory::Create(likelihood_type_);
  if (!likelihood_)
    LOG_ERROR(parameters_.location(PARAM_LIKELIHOOD) << ": Likelihood " << likelihood_type_ << " does not exist. Have you defined it?");
}

/**
 * Save the comparison that was done during an observation to the list of comparisons. Each comparison contributes part to a score
 * and we will need to know what those parts are when reporting.
 *
 * @param key The name of the comparison
 * @param age The age of the population being compared
 * @param expected The value generated by the model
 * @param observed The value passed in from the configuration file
 * @param error_value The error value for this comparison
 * @param score The amount of score for this comparison
 */
void Observation::SaveComparison(string key, int age, double expected, double observed, double error_value, double score) {
  RunMode::Type run_mode = Model::Instance()->run_mode();
  if (run_mode != RunMode::kBasic && run_mode != RunMode::kProfiling && run_mode != RunMode::kSimulation)
    return;

  observations::Comparison new_comparison;
  new_comparison.key_ = key;
  new_comparison.age_ = age;
  new_comparison.expected_ = expected;
  new_comparison.observed_ = observed;
  new_comparison.error_value_ = error_value;
  new_comparison.score_ = score;
  comparisons_.push_back(new_comparison);
}

/**
 * Save the comparison that was done during an observation to the list of comparisons. Each comparison contributes part to a score
 * and we will need to know what those parts are when reporting.
 *
 * @param key The name of the comparison
 * @param age The age of the population being compared
 * @param expected The value generated by the model
 * @param observed The value passed in from the configuration file
 * @param error_value The error value for this comparison
 * @param score The amount of score for this comparison
 */
void Observation::SaveComparison(string key, double expected, double observed, double error_value, double score) {
  SaveComparison(key, -1, expected, observed, error_value, score);
}

} /* namespace isam */
