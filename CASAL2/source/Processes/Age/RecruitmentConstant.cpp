/**
 * @file ConstantRecruitment.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "RecruitmentConstant.h"

#include "../../Utilities/Math.h"
#include "Categories/Categories.h"
#include "Logging/Logging.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

using niwa::partition::accessors::CategoriesWithAge;

/**
 * Default Constructor
 */
RecruitmentConstant::RecruitmentConstant(shared_ptr<Model> model) : Process(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories", "");
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportion for each category", "", true);
  parameters_.Bind<unsigned>(PARAM_AGE, &age_, "The age at recruitment", "");
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0, the recruitment used for annual recruits and initialise the model", "")->set_lower_bound(0.0);

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_categories_);

  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate the parameters for this process
 *
 * 1. Check for the required parameters
 * 2. Assign the label from the parameters
 * 3. Assign remaining local parameters
 */
void RecruitmentConstant::DoValidate() {
  /**
   * Validate age
   */
  if (age_ < model_->min_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") is less than the model's min_age (" << model_->min_age() << ")";
  if (age_ > model_->max_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") is greater than the model's max_age (" << model_->max_age() << ")";

  /**
   * Check our parameter proportion is the correct length
   * and sums to 1.0. If it doesn't sum to 1.0 we'll make it
   * and print a warning message
   */
  if (proportions_.size() > 0) {
    if (proportions_.size() != category_labels_.size()) {
      LOG_ERROR_P(PARAM_PROPORTIONS) << ": The number of proportions provided is not the same as the number of categories provided. Categories: " << category_labels_.size()
                                     << ", proportions size " << proportions_.size();
    }

    Double proportion_total = 0.0;

    for (Double proportion : proportions_) proportion_total += proportion;

    if (!utilities::math::IsOne(proportion_total)) {
      LOG_WARNING() << parameters_.location(PARAM_PROPORTIONS) << ": proportion does not sum to 1.0. Proportion sums to " << AS_DOUBLE(proportion_total)
                    << ". Auto-scaling proportions to sum to 1.0";

      for (Double& proportion : proportions_) proportion /= proportion_total;
    }

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      proportions_categories_[category_labels_[i]] = proportions_[i];
    }

  } else {
    // Assign equal proportions to every category
    Double proportion = category_labels_.size() / 1.0;
    for (string category : category_labels_) {
      proportions_categories_[category] = proportion;
      LOG_FINE() << "category " << category << " prop = " << proportion;
    }
  }
}

/**
 * Build any runtime relationships to other objects in the system.
 */
void RecruitmentConstant::DoBuild() {
  partition_ = CategoriesWithAgePtr(new CategoriesWithAge(model_, category_labels_, age_));
}

/**
 * Execute the constant recruitment process
 */
void RecruitmentConstant::DoExecute() {
  /**
   * Calculate new proportion totals to account for dynamic categories
   */
  Double total_proportions = 0.0;
  for (auto iterator = partition_->begin(); iterator != partition_->end(); ++iterator) {
    total_proportions += proportions_categories_[iterator->first];
  }

  /**
   * Update our partition with new recruitment values
   */
  for (auto iterator = partition_->begin(); iterator != partition_->end(); ++iterator) {
    *iterator->second += (proportions_categories_[iterator->first] / total_proportions) * r0_;
  }
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void RecruitmentConstant::FillReportCache(ostringstream& cache) {}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
 */
void RecruitmentConstant::FillTabularReportCache(ostringstream& cache, bool first_run) {}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
