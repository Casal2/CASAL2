/**
 * @file ConstantRecruitment.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "RecruitmentConstant.h"

#include "Categories/Categories.h"
#include "Utilities/DoubleCompare.h"
#include "Logging/Logging.h"

// Namespaces
namespace niwa {
namespace processes {

using niwa::partition::accessors::CategoriesWithAge;

/**
 * Default Constructor
 */
RecruitmentConstant::RecruitmentConstant() : Process(Model::Instance()) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_names_, "Categories", "");
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "Proportions", "", true);
  parameters_.Bind<unsigned>(PARAM_AGE, &age_, "Age", "");
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0", "")
      ->set_lower_bound(0.0, false);

  RegisterAsEstimable(PARAM_R0, &r0_);
  RegisterAsEstimable(PARAM_PROPORTIONS, &proportions_categories_);

  process_type_ = ProcessType::kRecruitment;
  partition_structure_ = PartitionStructure::kAge;
}

/**
 * Validate the parameters for this process
 *
 * 1. Check for the required parameters
 * 2. Assign our label from the parameters
 * 3. Assign remaining local parameters
 */
void RecruitmentConstant::DoValidate() {
  for(const string& label : category_names_) {
    if (!Categories::Instance()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }

  /**
   * Validate age
   */
  Model* model = Model::Instance();
  if (age_ < model->min_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") is less than the model's min_age (" << model->min_age() << ")";
  if (age_ > model->max_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") is greater than the model's max_age (" << model->max_age() << ")";

  /**
   * Check our parameter proportion is the correct length
   * and sums to 1.0. If it doesn't sum to 1.0 we'll make it
   * and print a warning message
   */
  if (proportions_.size() > 0) {
    if (proportions_.size() != category_names_.size()) {
      LOG_ERROR_P(PARAM_PROPORTIONS)
          << ": Number of proportions provided is not the same as the number of categories provided. Expected: "
          << category_names_.size()<< " but got " << proportions_.size();
    }

    Double proportion_total = 0.0;

    for (Double proportion : proportions_)
      proportion_total += proportion;

    if (!utilities::doublecompare::IsOne(proportion_total)) {
      LOG_WARNING() << parameters_.location(PARAM_PROPORTIONS)
          <<": proportion does not sum to 1.0. Proportion sums to " << AS_DOUBLE(proportion_total) << ". Auto-scaling proportions to sum to 1.0";

      for (Double& proportion : proportions_)
        proportion = proportion / proportion_total;
    }

    for (unsigned i = 0; i < category_names_.size(); ++i) {
      proportions_categories_[category_names_[i]] = proportions_[i];
    }

  } else {
    // Assign equal proportions to every category
    Double proportion = category_names_.size() / 1.0;
    for (string category : category_names_)
      proportions_categories_[category] = proportion;
  }
}

/**
 * Build any runtime relationships we might
 * have to other objects in the system.
 */
void RecruitmentConstant::DoBuild() {
  partition_ = CategoriesWithAgePtr(new CategoriesWithAge(category_names_, age_));
}

/**
 * Execute our constant recruitment process
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

} /* namespace processes */
} /* namespace niwa */
