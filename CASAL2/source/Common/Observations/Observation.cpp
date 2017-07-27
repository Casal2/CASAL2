/**
 * @file Observation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Observation.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Common/Utilities/DoubleCompare.h"
#include "Common/Categories/Categories.h"
#include "Common/Likelihoods/Manager.h"
#include "Common/Model/Managers.h"
#include "Common/Model/Model.h"

// Namespaces
namespace niwa {

/**
 * Default Constructor
 */

Observation::Observation(Model* model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of observation", "");
  parameters_.Bind<string>(PARAM_LIKELIHOOD, &likelihood_type_, "Type of likelihood to use", "");
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Category labels to use", "", true);
  parameters_.Bind<Double>(PARAM_DELTA, &delta_, "Robustification value (delta) for the likelihood", "", DELTA);
  parameters_.Bind<string>(PARAM_SIMULATION_LIKELIHOOD, &simulation_likelihood_label_, "Simulation likelihood to use", "", "");
  parameters_.Bind<Double>(PARAM_LIKELIHOOD_MULTIPLIER, &likelihood_multiplier_, "Likelihood score multiplier", "", Double(1.0));
  parameters_.Bind<Double>(PARAM_ERROR_VALUE_MULTIPLIER, &error_value_multiplier_, "Error value multiplier for likelihood", "", Double(1.0));
  mean_proportion_method_ = true;
}

/**
 * Validate the parameters passed in from the
 * configuration file
 */
void Observation::Validate() {
  LOG_TRACE();
  parameters_.Populate();
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));
  if (model_->run_mode() == RunMode::kSimulation) {
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
  expected_selectivity_count_ = 0;
  Categories* categories = model_->categories();
  for (const string& category_label : category_labels_)
    expected_selectivity_count_ += categories->GetNumberOfCategoriesDefined(category_label);

  LOG_FINEST() << "Expected Selectivity cout = " << expected_selectivity_count_;
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
          LOG_ERROR_P(PARAM_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
        } else {
          LOG_ERROR_P(PARAM_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
              << " It was defined in the category collection " << category_label;
        }
      }
    }
  }

  LOG_TRACE();
  DoValidate();
}

/**
 * Build all of the runtime relationships required for
 * this observation
 */
void Observation::Build() {
  LOG_TRACE();

  likelihood_ = model_->managers().likelihood()->GetOrCreateLikelihood(model_, label_, likelihood_type_);
  if (!likelihood_) {
    LOG_FATAL_P(PARAM_LIKELIHOOD) << "(" << likelihood_type_ << ") could not be found or constructed.";
    return;
  }
  likelihood_->set_multiplier(likelihood_multiplier_);
  likelihood_->set_error_value_multiplier(error_value_multiplier_);
  if (std::find(allowed_likelihood_types_.begin(), allowed_likelihood_types_.end(), likelihood_->type()) == allowed_likelihood_types_.end()) {
    string allowed = boost::algorithm::join(allowed_likelihood_types_, ", ");
    LOG_FATAL_P(PARAM_LIKELIHOOD) << ": likelihood " << likelihood_->type() << " is not supported by the " << type_ << " observation."
        << " Allowed types are: " << allowed;
  }

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
void Observation::SaveComparison(string category, unsigned age, Double length, Double expected, Double observed,
    Double process_error, Double error_value, Double adjusted_error, Double delta, Double score) {
  observations::Comparison new_comparison;
  new_comparison.category_ = category;
  new_comparison.age_ = age;
  new_comparison.length_ = length;
  new_comparison.expected_ = expected;
  new_comparison.observed_ = observed;
  new_comparison.process_error_ = process_error;
  new_comparison.error_value_ = error_value;
  new_comparison.adjusted_error_ = adjusted_error;
  new_comparison.delta_ = delta;
  new_comparison.score_ = score;
  comparisons_[model_->current_year()].push_back(new_comparison);
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
    Double process_error, Double error_value, Double adjusted_error, Double delta, Double score) {
  SaveComparison(category, 0, 0, expected, observed, process_error, error_value,adjusted_error, delta, score);
}

} /* namespace niwa */
