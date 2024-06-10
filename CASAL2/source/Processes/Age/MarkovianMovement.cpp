/**
 * @file MarkovianMovement.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2023
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// Headers
#include "MarkovianMovement.h"

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default constructor
 */
MarkovianMovement::MarkovianMovement(shared_ptr<Model> model) : Process(model), from_partition_(model), to_partition_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_FROM, &from_category_names_, "The categories to transition from", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_names_, "The categories to transition to", "");
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportions to transition for each category", "")->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "The selectivities to apply to each proportion", "");

  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_); // reference is from-to category label

  process_type_        = ProcessType::kTransition;
  partition_structure_ = PartitionType::kAge;
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
void MarkovianMovement::DoValidate() {
  LOG_TRACE();

  if (selectivity_names_.size() == 1) {
    auto val_s = selectivity_names_[0];
    selectivity_names_.assign(from_category_names_.size(), val_s);
  }

  //  // Validate Categories
  auto categories = model_->categories();

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

  // Validate that each from and to category have the same age range.
  for (unsigned i = 0; i < from_category_names_.size(); ++i) {
    if (categories->min_age(from_category_names_[i]) != categories->min_age(to_category_names_[i])) {
      LOG_ERROR_P(PARAM_FROM) << ": 'from' category " << from_category_names_[i] << " does not"
                              << " have the same age range as the 'to' category " << to_category_names_[i];
    }

    if (categories->max_age(from_category_names_[i]) != categories->max_age(to_category_names_[i])) {
      LOG_ERROR_P(PARAM_FROM) << ": 'from' category " << from_category_names_[i] << " does not"
                              << " have the same age range as the 'to' category " << to_category_names_[i];
    }
  }

  // set up estimable parameter container which is category label referenced
  //
  for (unsigned i = 0; i < to_category_names_.size(); ++i) {
    string category_label = from_category_names_[i] + string("-") + to_category_names_[i];
    proportions_by_category_[category_label] = proportions_[i];
    category_lookup_table_.push_back(category_label);
    LOG_FINE() << "i = " << i <<  " estimable parameter = " << category_label << " from  category = " <<  from_category_names_[i] << " to = " << to_category_names_[i] << " selectivity = " << selectivity_names_[i] << " prop = " <<  proportions_[i];
    from_to_proportions_[from_category_names_[i]][to_category_names_[i]] = proportions_[i];
  }

  // Check
  for(auto from_props : from_to_proportions_) {
    Double sum_props = 0.0;
    for(auto to_props : from_props.second)
      sum_props += to_props.second;
    if(fabs(sum_props - 1.0) > 0.001)
      LOG_ERROR_P(PARAM_PROPORTIONS) << "The " << PARAM_FROM << " category labeled " << from_props.first << " has proportions across all " << PARAM_TO << " categories which sum = " << sum_props << ". This needs to sum to one or at least close to one by 0.0001";
  }
}

/**
 * Build any runtime relationships this class needs.
 *
 * - Build the partition accessors
 * - Verify the selectivities are valid
 * - Get pointers to the selectivities
 */
void MarkovianMovement::DoBuild() {
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
  for(unsigned i = 0; i < from_category_names_.size(); i++) {
    abundance_to_move_categories_[i].resize(model_->age_spread(), 0.0);
  }
}

/**
 * Execute the maturation rate process.
 */
void MarkovianMovement::DoExecute() {
  LOG_TRACE();

  auto   from_iter = from_partition_.begin();
  auto   to_iter   = to_partition_.begin();
  LOG_FINE() << "from_partition_.size(): " << from_partition_.size()
               << "; to_partition_.size(): " << to_partition_.size();

  //calculate before we take it. a cateogry can be in multiple 'froms'
  for (unsigned i = 0; from_iter != from_partition_.end() && to_iter != to_partition_.end(); ++from_iter, ++to_iter, ++i) {
    LOG_FINEST() << "category = " << (*from_iter)->name_ << " to category = " << (*to_iter)->name_ << " i = " << i << " prop = " << proportions_by_category_[category_lookup_table_[i]];
    fill(abundance_to_move_categories_[i].begin(), abundance_to_move_categories_[i].end(), 0.0);
    for (unsigned offset = 0; offset < (*from_iter)->data_.size(); ++offset) 
      abundance_to_move_categories_[i][offset] = proportions_by_category_[category_lookup_table_[i]] * selectivities_[i]->GetAgeResult(min_age_ + offset, (*from_iter)->age_length_) * (*from_iter)->data_[offset];
    
  }
  from_iter = from_partition_.begin();
  to_iter   = to_partition_.begin();
  //now we move it
  for (unsigned i = 0; from_iter != from_partition_.end() && to_iter != to_partition_.end(); ++from_iter, ++to_iter, ++i) {
    LOG_FINEST() << "category = " << (*from_iter)->name_ << " to category = " << (*to_iter)->name_ << " i = " << i << " prop = " << proportions_by_category_[category_lookup_table_[i]];
    for (unsigned offset = 0; offset < (*from_iter)->data_.size(); ++offset) {
      LOG_FINEST() << "before = " <<  (*from_iter)->data_[offset];
      (*from_iter)->data_[offset] -= abundance_to_move_categories_[i][offset];
      (*to_iter)->data_[offset] += abundance_to_move_categories_[i][offset];
      LOG_FINEST() << "age-ndx - " << offset  <<  " Moving " << abundance_to_move_categories_[i][offset] << " number of individuals, from number " << (*from_iter)->data_[offset] << " to  = " << (*to_iter)->data_[offset];
      if ((*from_iter)->data_[offset] < 0.0)
        LOG_FATAL() << "MarkovianMovement rate caused a negative partition if ((*from_iter)->data_[offset] < 0.0) ";
    }
  }
}

/**
 * Reset the maturation rates
 */
void MarkovianMovement::DoReset() {
  // reset proportions based on map
  // Estiamting proportions_by_category_ but process report reports proportions_
  for (unsigned i = 0; i < to_category_names_.size(); ++i) {
    proportions_[i] = proportions_by_category_[category_lookup_table_[i]];
  }
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
