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

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

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
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of observation", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time step to execute in", "");
  parameters_.Bind<string>(PARAM_LIKELIHOOD, &likelihood_type_, "Type of likelihood to use", "");
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Category labels to use", "", true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "Selectivity labels to use", "", true);
  parameters_.Bind<string>(PARAM_SIMULATION_LIKELIHOOD, &simulation_likelihood_label_, "Simulation likelihood to use", "", "");

  mean_proportion_method_ = true;
}

/**
 * Validate the parameters passed in from the
 * configuration file
 */
void Observation::Validate() {
  parameters_.Populate();

  if (Model::Instance()->run_mode() == RunMode::kSimulation) {
    if (likelihood_type_ == PARAM_PSEUDO) {
      likelihood_type_ = simulation_likelihood_label_;
    } else {
      simulation_likelihood_label_ = likelihood_type_;
    }
  }

  /**
   * Because this observation supports categories that are provided in groups
   * (using the + operator) we need to verify the number of selectivities
   * matches the true number of categories
   *
   * The number of selectivities can be either the number of true categories
   * or the number of defined collections
   */
  unsigned expected_selectivity_count = 0;
  CategoriesPtr categories = Categories::Instance();
  for (const string& category_label : category_labels_)
    expected_selectivity_count += categories->GetNumberOfCategoriesDefined(category_label);

  if (category_labels_.size() != selectivity_labels_.size() && expected_selectivity_count != selectivity_labels_.size())
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": Number of selectivities provided (" << selectivity_labels_.size()
        << ") is not valid. You can specify either the number of category collections (" << category_labels_.size() << ") or "
        << "the number of total categories (" << expected_selectivity_count << ")");

  /**
   * Now go through each category and split it if required, then check each piece to ensure
   * it's a valid category
   */
  vector<string> split_category_labels;
  for (const string& category_label : category_labels_) {
    boost::split(split_category_labels, category_label, boost::is_any_of("+"));

    for (const string& split_category_label : split_category_labels) {
      if (!categories->IsValid(split_category_label)) {
        if (split_category_label == category_label) {
          LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.");
        } else {
          LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
              << " It was defined in the category collection " << category_label);
        }
      }
    }
  }

  DoValidate();
}

/**
 * Build all of the runtime relationships required for
 * this observation
 */
void Observation::Build() {
  LOG_TRACE();

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

  DoBuild();
}

/**
 * Reset our observation so it can be called again
 */
void Observation::Reset() {
  comparisons_.clear();
  scores_.clear();

  DoReset();
}

/**
 * Save the comparison that was done during an observation to the list of comparisons. Each comparison contributes part to a score
 * and we will need to know what those parts are when reporting.
 *
 * @param category The name of the comparison
 * @param age The age of the population being compared
 * @param expected The value generated by the model
 * @param observed The value passed in from the configuration file
 * @param error_value The error value for this comparison
 * @param score The amount of score for this comparison
 */
void Observation::SaveComparison(string category, unsigned age, Double expected, Double observed,
    Double process_error, Double error_value, Double delta, Double score) {
  observations::Comparison new_comparison;
  new_comparison.category_ = category;
  new_comparison.age_ = age;
  new_comparison.expected_ = expected;
  new_comparison.observed_ = observed;
  new_comparison.process_error_ = process_error;
  new_comparison.error_value_ = error_value;
  new_comparison.delta_ = delta;
  new_comparison.score_ = score;
  comparisons_[Model::Instance()->current_year()].push_back(new_comparison);
}

/**
 * Save the comparison that was done during an observation to the list of comparisons. Each comparison contributes part to a score
 * and we will need to know what those parts are when reporting.
 *
 * @param category The name of the comparison
 * @param age The age of the population being compared
 * @param expected The value generated by the model
 * @param observed The value passed in from the configuration file
 * @param error_value The error value for this comparison
 * @param score The amount of score for this comparison
 */
void Observation::SaveComparison(string category, Double expected, Double observed,
    Double process_error, Double error_value, Double delta, Double score) {
  SaveComparison(category, 0, expected, observed, process_error, error_value, delta, score);
}

} /* namespace isam */
