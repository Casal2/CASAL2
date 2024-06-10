/**
 * @file ConstantRecruitment.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 12/18/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// Headers
#include "RecruitmentConstant.h"

#include "../../Categories/Categories.h"
#include "../../Logging/Logging.h"
#include "../../Utilities/Math.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {

/**
 * Default constructor
 */
RecruitmentConstant::RecruitmentConstant(shared_ptr<Model> model) : Process(model), partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories", "");
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportions", "", true);
  parameters_.Bind<Double>(PARAM_LENGTH_BINS, &length_bins_, "The length bin that recruits are uniformly distributed over at the time of recruitment. Needs to be consistent with @model length bin inputs", "");
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0", "")->set_lower_bound(0.0);

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_categories_);

  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kLength;
}

/**
 * Validate the parameters for this process
 *
 * 1. Check for the required parameters
 * 2. Assign our label from the parameters
 * 3. Assign remaining local parameters
 */
void RecruitmentConstant::DoValidate() {
  LOG_TRACE();
  /**
   * Validate length bins
   */
  for (auto length_bin : length_bins_) {
    if (std::find(model_->length_bins().begin(), model_->length_bins().end(), length_bin) == model_->length_bins().end())
      LOG_ERROR_P(PARAM_LENGTH_BINS) << "The length bin '" << length_bin << "' is not a length bin in the @model block.";
  }

  /**
   * Check our parameter proportion is the correct length
   * and sums to 1.0. If it doesn't sum to 1.0 we'll make it
   * and print a warning message
   */
  if (proportions_.size() > 0) {
    if (proportions_.size() != category_labels_.size()) {
      LOG_ERROR_P(PARAM_PROPORTIONS) << ": the number of proportions provided is not the same as the number of categories provided. Expected: " << category_labels_.size()
                                     << ", parsed " << proportions_.size();
    }

    Double proportion_total = 0.0;

    for (Double proportion : proportions_) proportion_total += proportion;

    if (!utilities::math::IsOne(proportion_total)) {
      LOG_WARNING() << parameters_.location(PARAM_PROPORTIONS) << ": the proportions do not sum to 1.0. The proportions sum to " << AS_DOUBLE(proportion_total)
                    << ". Auto-scaling the proportions to sum to 1.0";

      for (Double& proportion : proportions_) proportion /= proportion_total;
    }

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      proportions_categories_[category_labels_[i]] = proportions_[i];
    }

  } else {
    // Assign equal proportions to every category
    Double proportion = category_labels_.size() / 1.0;
    for (string category : category_labels_) proportions_categories_[category] = proportion;
  }
}

/**
 * Build any runtime relationships to other objects in the system.
 */
void RecruitmentConstant::DoBuild() {
  LOG_TRACE();
  partition_.Init(category_labels_);
}

/**
 * Execute the constant recruitment process
 */
void RecruitmentConstant::DoExecute() {
  LOG_TRACE();
  // Calculate new proportion totals to account for dynamic categories
  Double total_proportions = 0.0;
  for (auto category : partition_) total_proportions += proportions_categories_[category->name_];

  // Update our partition with new recruitment values
  for (auto category : partition_) {
    if (category->data_.size() != model_->length_bins().size())
      LOG_CODE_ERROR() << "This function was written when categories were forced to have the same length bins as models. This is no longer the case.";

    r0_by_length_bin_     = (r0_ * (proportions_categories_[category->name_]) / total_proportions) / length_bins_.size();
    unsigned length_index = 0;
    for (auto length_bin : length_bins_) {
      length_index = std::distance(std::find(model_->length_bins().begin(), model_->length_bins().end(), length_bin), model_->length_bins().begin());
      LOG_FINEST() << "putting " << r0_by_length_bin_ << " in category " << category->name_ << " in length bin " << model_->length_bins()[length_index];
      category->data_[length_index] += r0_by_length_bin_;
    }
  }
}

} /* namespace length */
}  // namespace processes
} /* namespace niwa */
