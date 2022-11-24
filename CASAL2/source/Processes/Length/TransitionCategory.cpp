/**
 * @file TransitionCategory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "TransitionCategory.h"

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {

/**
 * Default constructor
 */
TransitionCategory::TransitionCategory(shared_ptr<Model> model) : Process(model), from_partition_(model), to_partition_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_FROM, &from_category_names_, "The categories to transition from", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_names_, "The categories to transition to", "");
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportions to transition for each category", "")->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The selectivities to apply to each proportion", "");

  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_);

  process_type_        = ProcessType::kTransition;
  partition_structure_ = PartitionType::kLength;
}

/**
 * Validate the Transition process
 *
 * - Check for the required parameters
 * - Assign variables from the parameters
 * - Verify the categories are real
 * - If proportions or selectivities only has one element specified
 *   then add more elements until they match number of categories
 * - Verify vector lengths are all the same
 * - Verify categories From->To have matching age ranges
 * - Check all proportions are between 0.0 and 1.0
 */
void TransitionCategory::DoValidate() {
  LOG_TRACE();

  if (selectivity_names_.size() == 1) {
    auto val_s = selectivity_names_[0];
    selectivity_names_.assign(from_category_names_.size(), val_s);
  }

  //  // Validate Categories
  //  auto categories = model_->categories();

  // Validate the from and to vectors are the same size
  if (from_category_names_.size() != to_category_names_.size()) {
    LOG_ERROR_P(PARAM_TO) << ": the number of 'to' categories provided does not match the number of 'from' categories provided. " << from_category_names_.size()
                          << " categories were supplied, but " << to_category_names_.size() << " categories are required";
  }

  // Allow a one to many relationship between proportions and number of categories.
  if (proportions_.size() == 1)
    proportions_.resize(to_category_names_.size(), proportions_[0]);

  // Validate the to category and proportions vectors are the same size
  if (to_category_names_.size() != proportions_.size()) {
    LOG_ERROR_P(PARAM_PROPORTIONS) << ": the number of proportions provided does not match the number of 'to' categories provided. " << to_category_names_.size()
                                   << " categories were supplied, but proportions size is " << proportions_.size();
  }

  // Validate the number of selectivities matches the number of proportions
  if (proportions_.size() != selectivity_names_.size() && proportions_.size() != 1) {
    LOG_ERROR_P(PARAM_SELECTIVITIES) << ": the number of selectivities provided does not match the number of proportions provided. "
                                     << " proportions size is " << proportions_.size() << " but number of selectivities is " << selectivity_names_.size();
  }

  for (unsigned i = 0; i < to_category_names_.size(); ++i) {
    proportions_by_category_[to_category_names_[i]] = proportions_[i];
    LOG_FINE() << "i = " << i << " from  category = " << from_category_names_[i] << " to = " << to_category_names_[i] << " selectivity = " << selectivity_names_[i]
               << " prop = " << proportions_[i];
  }
}

/**
 * Build any runtime relationships this class needs.
 *
 * - Build the partition accessors
 * - Verify the selectivities are valid
 * - Get pointers to the selectivities
 */
void TransitionCategory::DoBuild() {
  LOG_TRACE();

  from_partition_.Init(from_category_names_);
  to_partition_.Init(to_category_names_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";
    selectivities_.push_back(selectivity);
  }

  if (from_partition_.size() != to_partition_.size()) {
    LOG_FATAL() << "The list of categories for the transition category process are not of equal size in year " << model_->current_year() << ". Number of 'From' "
                << from_partition_.size() << " and 'To' " << to_partition_.size() << " categories to transition between";
  }
  abundance_to_move_categories_.resize(from_category_names_.size());
  for (unsigned i = 0; i < from_category_names_.size(); i++) {
    abundance_to_move_categories_[i].resize(model_->get_number_of_length_bins(), 0.0);
  }
}
/**
 * Execute the maturation rate process.
 */
void TransitionCategory::DoExecute() {
  LOG_TRACE();

  auto from_iter = from_partition_.begin();
  auto to_iter   = to_partition_.begin();
  LOG_FINE() << "from_partition_.size(): " << from_partition_.size() << "; to_partition_.size(): " << to_partition_.size();

  // calculate before we take it. a category can be in multiple 'froms'
  for (unsigned i = 0; from_iter != from_partition_.end() && to_iter != to_partition_.end(); ++from_iter, ++to_iter, ++i) {
    LOG_FINEST() << "category = " << (*from_iter)->name_ << " to category = " << (*to_iter)->name_ << " i = " << i << " prop = " << proportions_by_category_[(*to_iter)->name_];
    fill(abundance_to_move_categories_[i].begin(), abundance_to_move_categories_[i].end(), 0.0);
    for (unsigned offset = 0; offset < (*from_iter)->data_.size(); ++offset)
      abundance_to_move_categories_[i][offset] = proportions_by_category_[(*to_iter)->name_] * selectivities_[i]->GetLengthResult(i) * (*from_iter)->data_[offset];
  }
  from_iter = from_partition_.begin();
  to_iter   = to_partition_.begin();
  // now we move it
  for (unsigned i = 0; from_iter != from_partition_.end() && to_iter != to_partition_.end(); ++from_iter, ++to_iter, ++i) {
    LOG_FINEST() << "category = " << (*from_iter)->name_ << " to category = " << (*to_iter)->name_ << " i = " << i << " prop = " << proportions_by_category_[(*to_iter)->name_];
    for (unsigned offset = 0; offset < (*from_iter)->data_.size(); ++offset) {
      LOG_FINEST() << "before = " << (*from_iter)->data_[offset];
      (*from_iter)->data_[offset] -= abundance_to_move_categories_[i][offset];
      (*to_iter)->data_[offset] += abundance_to_move_categories_[i][offset];
      LOG_FINEST() << "length-ndx: " << offset << " Moving " << abundance_to_move_categories_[i][offset] << " out of " << (*from_iter)->data_[offset] << " to "
                   << (*to_iter)->data_[offset];
      if ((*from_iter)->data_[offset] < 0.0)
        LOG_FATAL() << "TransitionCategory rate caused a negative partition if ((*from_iter)->data_[offset] < 0.0) ";
    }
  }
}

/**
 * Reset the maturation rates
 */
void TransitionCategory::DoReset() {}

}  // namespace length
} /* namespace processes */
} /* namespace niwa */
