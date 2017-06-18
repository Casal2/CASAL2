/**
 * @file MortalityPreySuitability.cpp
 * @author C. Marsh
 * @github https://github.com/Zaita
 * @date 1/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science 2015 - www.niwa.co.nz
 */

// headers
#include "MortalityPreySuitability.h"

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
MortalityPreySuitability::MortalityPreySuitability(Model* model)
  : Process(model) {
  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;

  parameters_.Bind<string>(PARAM_PREY_CATEGORIES, &prey_category_labels_, "Prey Categories labels", "");
  parameters_.Bind<string>(PARAM_PREDATOR_CATEGORIES, &predator_category_labels_, "Predator Categories labels", "");
  parameters_.Bind<Double>(PARAM_CONSUMPTION_RATE, &consumption_rate_, "Predator consumption rate", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_ELECTIVITIES, &electivities_, "Prey Electivities", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "Umax", "")->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_PREY_SELECTIVITIES, &prey_selectivity_labels_, "Selectivities for prey categories", "");
  parameters_.Bind<string>(PARAM_PREDATOR_SELECTIVITIES, &predator_selectivity_labels_, "Selectivities for predator categories", "");
  parameters_.Bind<string>(PARAM_PENALTY, &  penalty_label_, "Label of penalty to be applied", "","");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Year that process occurs", "");

  RegisterAsAddressable(PARAM_CONSUMPTION_RATE, &consumption_rate_);
  RegisterAsAddressable(PARAM_ELECTIVITIES, &electivities_);

}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityPreySuitability::DoValidate() {

  /**
   * Now go through each category and split it if required, then check each piece to ensure
   * it's a valid category
   */
  Categories* categories = model_->categories();

  vector<string> split_prey_category_labels;
  for (const string& category_label : prey_category_labels_) {
    boost::split(split_prey_category_labels, category_label, boost::is_any_of("+"));

    for (const string& split_category_label : split_prey_category_labels) {
      if (!categories->IsValid(split_category_label)) {
        if (split_category_label == category_label) {
          LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
        } else {
          LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
              << " It was defined in the category collection " << category_label;
        }
      }
    }
  }

  vector<string> split_predator_category_labels;
  for (const string& category_label : predator_category_labels_) {
    boost::split(split_predator_category_labels, category_label, boost::is_any_of("+"));

    for (const string& split_category_label : split_predator_category_labels) {
      if (!categories->IsValid(split_category_label)) {
        if (split_category_label == category_label) {
          LOG_ERROR_P(PARAM_PREDATOR_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
        } else {
          LOG_ERROR_P(PARAM_PREDATOR_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
              << " It was defined in the category collection " << category_label;
        }
      }
    }
  }

  // Check length of categories are the same as selectivities
  if (prey_category_labels_.size() != prey_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": You provided (" << prey_selectivity_labels_.size() << ") prey selectivities but we have "
        << prey_category_labels_.size() << " prey catregories";

  if (predator_category_labels_.size() != predator_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": You provided (" << predator_selectivity_labels_.size() << ") predator selectivities but we have "
        << predator_category_labels_.size() << " predator categories";

  if (prey_category_labels_.size() != electivities_.size())
    LOG_ERROR_P(PARAM_ELECTIVITIES) << ": You provided (" << prey_category_labels_.size() << ") prey categories but we have "
            << electivities_.size() << " prey electivities, these must be equal";

}

/**
 * Build any objects that will need to be utilised by this object.
 * Validate any parameters that require information from other objects
 * in the system
 */
void MortalityPreySuitability::DoBuild() {
  prey_partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, prey_category_labels_));
  predator_partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, predator_category_labels_));

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
    predator_selectivities_.push_back(model_->managers().selectivity()->GetSelectivity(selectivity));
    if (!predator_selectivities_[category_offset])
      LOG_ERROR_P(PARAM_PREDATOR_SELECTIVITIES) << "selectivity " << selectivity << " does not exist. Have you defined it?";
    ++category_offset;
  }

  if (penalty_label_ != "none") {
    penalty_ = model_->managers().penalty()->GetProcessPenalty(penalty_label_);
    if (!penalty_)
      LOG_ERROR_P(PARAM_PENALTY) << ": penalty " << penalty_label_ << " does not exist. Have you defined it?";
  }

/*  *
   * Check All the categories are valid

  for (const string& label : prey_category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_PREY_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }
  for (const string& label : predator_category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_PREDATOR_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }*/
}

/**
 * Execute this process
 */
void MortalityPreySuitability::DoExecute() {

  // Check if we are executing this process in current year
  if (std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {

    Double TotalPreyVulnerable = 0;
    Double TotalPreyAvailability = 0;
    Double TotalPredatorVulnerable = 0;
    Double SumMortality = 0.0;

    map<string, Double> Vulnerable_by_Prey;
    map<string, Double> Exploitation_by_Prey;

    auto partition_iter = prey_partition_->Begin(); // vector<vector<partition::Category> >

    for (unsigned category_offset = 0; category_offset < prey_category_labels_.size(); ++category_offset, ++partition_iter) {
      /**
        * Loop through the  combined categories building up the prey abundance for each prey category label
        */
       auto category_iter = partition_iter->begin();
       for (; category_iter != partition_iter->end(); ++category_iter) {
         for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
           Double vulnerable = (*category_iter)->data_[data_offset] * prey_selectivities_[category_offset]->GetResult((*category_iter)->min_age_ + data_offset, (*category_iter)->age_length_);
           if (vulnerable <= 0.0)
             vulnerable = 0.0;
           Vulnerable_by_Prey[prey_category_labels_[category_offset]] += vulnerable;
           TotalPreyVulnerable += vulnerable * electivities_[category_offset];
           TotalPreyAvailability += vulnerable;
         }
       }
       LOG_FINEST() << ": Vulnerable abundance for prey category " << prey_category_labels_[category_offset] << " = " << Vulnerable_by_Prey[prey_category_labels_[category_offset]];
    }

    TotalPreyAvailability = dc::ZeroFun(TotalPreyAvailability, ZERO);
    TotalPreyVulnerable = dc::ZeroFun(TotalPreyVulnerable / TotalPreyAvailability, ZERO);

    /*
     * Loop through the predators calculating vulnerable predator abyundance
     */
    auto predator_partition_iter = predator_partition_->Begin();
    for (unsigned category_offset = 0; category_offset < predator_category_labels_.size(); ++category_offset, ++predator_partition_iter) {

      /*
       * Loop through the  combined categories building up the predator abundance for each prey category label
       */
      auto category_iter = predator_partition_iter->begin();
      for (; category_iter != predator_partition_iter->end(); ++category_iter) {
        for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {

          Double predator_vulnerable = (*category_iter)->data_[data_offset] * predator_selectivities_[category_offset]->GetResult((*category_iter)->min_age_ + data_offset, (*category_iter)->age_length_);
          if (predator_vulnerable <= 0.0)
            predator_vulnerable = 0.0;

          TotalPredatorVulnerable += predator_vulnerable;
        }
      }
    }
    LOG_FINEST() << ": Total predator abundance = " << TotalPredatorVulnerable;

    /*
     * Work out exploitation rate for each prey category
     */
    for (unsigned category_offset = 0; category_offset < prey_category_labels_.size(); ++category_offset) {
      Double Exploitation = TotalPredatorVulnerable * consumption_rate_ * ((Vulnerable_by_Prey[prey_category_labels_[category_offset]] / TotalPreyAvailability) * electivities_[category_offset])
          / TotalPreyVulnerable;

      if (Exploitation > u_max_) {
        Exploitation = u_max_;

        if (penalty_) // Throw Penalty
          penalty_->Trigger(penalty_label_, Exploitation, (Vulnerable_by_Prey[prey_category_labels_[category_offset]] * u_max_));

      } else if (Exploitation < 0.0)
        Exploitation = 0.0;

      Exploitation_by_Prey[prey_category_labels_[category_offset]] = Exploitation;
      LOG_FINEST() << ": Exploitation rate for prey category " << prey_category_labels_[category_offset] << " = " << Exploitation_by_Prey[prey_category_labels_[category_offset]];

    }

    // removal of prey components using the exploitation rate.
    partition_iter = prey_partition_->Begin();

    for (unsigned category_offset = 0; category_offset < prey_category_labels_.size(); ++category_offset, ++partition_iter) {

       auto category_iter = partition_iter->begin();
       for (; category_iter != partition_iter->end(); ++category_iter) {
         for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {

           Double Current = (*category_iter)->data_.size() * prey_selectivities_[category_offset]->GetResult((*category_iter)->min_age_ + data_offset, (*category_iter)->age_length_)
               * Exploitation_by_Prey[prey_category_labels_[category_offset]];
           if (Current <= 0.0) {
             LOG_WARNING() << ": Negative partition create";
             continue;
           }

           // remove abundance
           (*category_iter)->data_[data_offset] -= Current;
           SumMortality += Current;
        }
      }
    }
  } // if (std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {
}

} /* namespace processes */
} /* namespace niwa */
