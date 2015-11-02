/**
 * @file MortalityHollingRate.cpp
 * @author C. Marsh
 * @github https://github.com/Zaita
 * @date 31/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "MortalityHollingRate.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Categories/Categories.h"
#include "Model/Managers.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/To.h"


// namespaces
namespace niwa {
namespace processes {
namespace dc = niwa::utilities::doublecompare;
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
MortalityHollingRate::MortalityHollingRate(Model* model)
  : Process(model),
    prey_partition_(model),
    predator_partition_(model) {
  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;

  parameters_.Bind<string>(PARAM_PREY_CATEGORIES, &prey_category_labels_, "Prey Categories labels", "");
  parameters_.Bind<string>(PARAM_PREDATOR_CATEGORIES, &predator_category_labels_, "Predator Categories labels", "");
  parameters_.Bind<Double>(PARAM_A, &a_, "parameter a", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B, &b_, "parameter b", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_X, &x_, "parameter x", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "Umax", "")->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_PREY_SELECTIVITIES, &prey_selectivity_labels_, "Selectivities for prey categories", "");
  parameters_.Bind<string>(PARAM_PREDATOR_SELECTIVITIES, &predator_selectivity_labels_, "Selectivities for predator categories", "");
  parameters_.Bind<string>(PARAM_PENALTY, &  penalty_label_, "Label of penalty to be applied", "","");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Year to execute in", "");

  RegisterAsEstimable(PARAM_A, &a_);
  RegisterAsEstimable(PARAM_B, &b_);
  RegisterAsEstimable(PARAM_X, &x_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityHollingRate::DoValidate() {
  prey_category_labels_ = model_->categories()->ExpandLabels(prey_category_labels_, parameters_.Get(PARAM_CATEGORIES));
  predator_category_labels_ = model_->categories()->ExpandLabels(predator_category_labels_, parameters_.Get(PARAM_CATEGORIES));

  if (prey_category_labels_.size() != prey_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": You provided (" << prey_selectivity_labels_.size() << ") prey selectivities but we have "
        << prey_category_labels_.size() << " prey catregories";

  if (predator_category_labels_.size() != predator_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": You provided (" << predator_selectivity_labels_.size() << ") prey selectivities but we have "
        << predator_category_labels_.size() << " prey catregories";
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 *
 * Validate any parameters that require information from other objects
 * in the system
 */
void MortalityHollingRate::DoBuild() {
  prey_partition_.Init(prey_category_labels_);
  predator_partition_.Init(predator_category_labels_);
  /**
   * Assign the selectivity, penalty and time step index to each fisher data object
   */
  unsigned category_offset = 0;
  for (string selectivity : prey_selectivity_labels_) {
    prey_selectivities_.push_back(model_->managers().selectivity()->GetSelectivity(selectivity));
    if (!prey_selectivities_[category_offset])
      LOG_ERROR_P(PARAM_PREY_SELECTIVITIES) << "selectivity " << selectivity << " does not exist. Have you defined it?";
    ++category_offset;
  }

  category_offset = 0;
  for (string selectivity : predator_selectivity_labels_) {
    prey_selectivities_.push_back(model_->managers().selectivity()->GetSelectivity(selectivity));
    if (!predator_selectivities_[category_offset])
      LOG_ERROR_P(PARAM_PREDATOR_SELECTIVITIES) << "selectivity " << selectivity << " does not exist. Have you defined it?";
    ++category_offset;
  }

  if (penalty_label_ != "none") {
    penalty_ = model_->managers().penalty()->GetProcessPenalty(penalty_label_);
    if (!penalty_)
      LOG_ERROR_P(PARAM_PENALTY) << ": penalty " << penalty_label_ << " does not exist. Have you defined it?";
  }

  /**
   * Check All the categories are valid
   */
  for (const string& label : prey_category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }
  for (const string& label : predator_category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }
}

/**
 * Execute this process
 */
void MortalityHollingRate::DoExecute() {

  // Check if we are executing this process in current year
  if (std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {

    /**
     * Loop for prey each category, calculate vulnerable abundance
     */
    Double Mortality = 0;
    Double SumMortality = 0;
    Double Vulnerable = 0;
    Double PredatorVulnerable = 0;
    unsigned prey_offset = 0;

    for (auto prey_categories : prey_partition_) {
      //prey_categories->UpdateMeanWeightData(); // is not abundance
      for (unsigned i = 0; i < prey_categories->data_.size(); ++i) {
        Vulnerable += prey_categories->data_[i] * prey_selectivities_[prey_offset]->GetResult(prey_categories->min_age_ + i);
      }
      ++prey_offset;
    }

    /**
     * Loop for predator each category, calculate vulnerable predator abundance
     */
    unsigned predator_offset = 0;
    for (auto predator_categories : predator_partition_) {
      //prey_categories->UpdateMeanWeightData(); // is not abundance
      for (unsigned i = 0; i < predator_categories->data_.size(); ++i) {
        PredatorVulnerable += predator_categories->data_[i] * predator_selectivities_[predator_offset]->GetResult(predator_categories->min_age_ + i);
      }
      ++predator_offset;
    }

    // Holling function type 2 (x=1) or 3 (x=2), or generalised (Michaelis Menten)
    Mortality = PredatorVulnerable * (a_ * pow(Vulnerable, (x_ - 1.0))) / (b_ + pow(Vulnerable, (x_ - 1.0)));

    // Work out exploitation rate to remove (catch/vulnerable Abundance)
    Double Exploitation = Mortality / dc::ZeroFun(Vulnerable, ZERO);

    if (Exploitation > u_max_) {
      Exploitation = u_max_;

      if (penalty_) // Throw Penalty
        penalty_->Trigger(penalty_label_, Mortality, (Vulnerable * u_max_));

    } else if (Exploitation < 0.0)
        Exploitation = 0.0;

    prey_offset = 0;
    // Loop Through Prey Categories & remove number based on calcuated exploitation rate
    for (auto prey_categories : prey_partition_) {
      for (unsigned i = 0; i < prey_categories->data_.size(); ++i) {
        Double Current = 0.0;
        // Get Amount to remove
        Current = prey_categories->data_[i] * prey_selectivities_[prey_offset]->GetResult(prey_categories->min_age_ + i) * Exploitation;
        // If is Zero, Cont
        if (Current <= 0.0)
          continue;

        // remove abundance
        prey_categories->data_[i] -= Current;
        SumMortality += Current;
      }
      ++prey_offset;
    }
  } //if (std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {
}


} /* namespace processes */
} /* namespace niwa */
