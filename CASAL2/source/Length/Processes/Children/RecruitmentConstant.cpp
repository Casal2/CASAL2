/**
 * @file ConstantRecruitment.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 12/18/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// Headers
#include "RecruitmentConstant.h"

#include "Common/Categories/Categories.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/Logging/Logging.h"

// Namespaces
namespace niwa {
namespace length {
namespace processes {

/**
 * Default Constructor
 */
RecruitmentConstant::RecruitmentConstant(Model* model)
  : Process(model),
    partition_(model) {
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories", "");
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "Proportions", "", true);
  parameters_.Bind<unsigned>(PARAM_LENGTH_BINS, &length_bins_, "The length bins recruits are uniformly distributed over, when recruitment occurs", "");
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0", "")
      ->set_lower_bound(0.0, false);

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_categories_);

  process_type_ = ProcessType::kRecruitment;
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
  // Validate categories
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  for(const string& label : category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }

  /**
   * Validate length bins
   */
  for (auto length_bin : length_bins_) {
    if (std::find(model_->length_bins().begin(),model_->length_bins().end(), length_bin) == model_->length_bins().end())
      LOG_ERROR_P(PARAM_LENGTH_BINS) << "The length bin " << length_bin << ", wasn't a length bin in the @model block, please check you have specified the correct lenght bins.";
  }

  /**
   * Check our parameter proportion is the correct length
   * and sums to 1.0. If it doesn't sum to 1.0 we'll make it
   * and print a warning message
   */
  if (proportions_.size() > 0) {
    if (proportions_.size() != category_labels_.size()) {
      LOG_ERROR_P(PARAM_PROPORTIONS)
          << ": Number of proportions provided is not the same as the number of categories provided. Expected: "
          << category_labels_.size()<< " but got " << proportions_.size();
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

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      proportions_categories_[category_labels_[i]] = proportions_[i];
    }

  } else {
    // Assign equal proportions to every category
    Double proportion = category_labels_.size() / 1.0;
    for (string category : category_labels_)
      proportions_categories_[category] = proportion;
  }
}

/**
 * Build any runtime relationships we might
 * have to other objects in the system.
 */
void RecruitmentConstant::DoBuild() {
  LOG_TRACE();
  partition_.Init(category_labels_);
}

/**
 * Execute our constant recruitment process
 */
void RecruitmentConstant::DoExecute() {
  LOG_TRACE();
  //Calculate new proportion totals to account for dynamic categories
  Double total_proportions = 0.0;
  for (auto category : partition_)
   total_proportions += proportions_categories_[category->name_];


  //Update our partition with new recruitment values
  for (auto category : partition_) {
   if (category->length_data_.size() != model_->length_bins().size())
     LOG_CODE_ERROR() << "This function was written when categories were forced to have the same length bins as models, this is not the case now please review code.";
   r0_by_length_bin_ = (r0_ * (proportions_categories_[category->name_]) / total_proportions) / length_bins_.size();
   unsigned length_index = 0;
   for (auto length_bin : length_bins_) {
     length_index = std::distance(std::find(model_->length_bins().begin(), model_->length_bins().end(), length_bin), model_->length_bins().begin());
     LOG_FINEST() << "putting " << r0_by_length_bin_ << " in category " << category->name_ << " in length bin " << model_->length_bins()[length_index];
     category->length_data_[length_index] += r0_by_length_bin_;
   }
  }
}

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */
