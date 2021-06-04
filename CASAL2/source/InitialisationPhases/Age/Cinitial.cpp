/**
 * @file Cinitial.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 11/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Cinitial.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../../Categories/Categories.h"
#include "../../DerivedQuantities/Manager.h"
#include "../../InitialisationPhases/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../TimeSteps/Manager.h"

// namesapces
namespace niwa {
namespace initialisationphases {
namespace age {

/**
 * Default constrcutor
 *
 * @param model Pointer to our core model object
 */
Cinitial::Cinitial(shared_ptr<Model> model) : InitialisationPhase(model) {
  n_table_ = new parameters::Table(PARAM_N);

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories for the Cinitial initialisation", "");
  parameters_.BindTable(PARAM_N, n_table_, "The table of values for the Cinitial initialisation", "", false, false);

  RegisterAsAddressable(&n_);
}

/**
 * Destructor
 */
Cinitial::~Cinitial() {
  delete n_table_;
}

/**
 * Validate the parameters passed in from the configuration file
 */
void Cinitial::DoValidate() {
  LOG_TRACE();
  min_age_ = model_->min_age();
  max_age_ = model_->max_age();

  if (max_age_ < min_age_)
    LOG_ERROR_P(PARAM_MIN_AGE) << " The minimum age (" << min_age_ << ") cannot be greater than the maximum age (" << max_age_ << ")";

  column_count_ = (max_age_ - min_age_) + 2;

  /**
   * Convert the string values to doubles and load them in to a table.
   */
  vector<vector<string>>& data       = n_table_->data();
  unsigned                row_number = 1;
  for (auto row : data) {
    string row_label = row[0];
    // CHeck that it is a valid category
    bool check_combined = model_->categories()->IsCombinedLabels(row_label);
    LOG_FINEST() << "Checking row with label = " << row_label;
    if (find(category_labels_.begin(), category_labels_.end(), row_label) == category_labels_.end())
      LOG_FATAL_P(PARAM_N) << " Could not find '" << row_label << "' in the categories supplied. The categories should be the same as the row labels.";

    if (check_combined) {
      vector<string> split_category_labels;
      boost::split(split_category_labels, row_label, boost::is_any_of("+"));
      unsigned category_iter = 0;
      for (const string& split_category_label : split_category_labels) {
        if (!model_->categories()->IsValid(split_category_label)) {
          LOG_FATAL_P(PARAM_N) << ": The category " << split_category_label << " is not a valid category.";
        }
        ++category_iter;
      }
    } else {
      if (!model_->categories()->IsValid(row_label))
        LOG_FATAL_P(PARAM_N) << ": The category " << row_label << " is not a valid category.";
    }
    // convert to lower case
    row_label = utilities::ToLowercase(row_label);
    if (row.size() != column_count_)
      LOG_ERROR_P(PARAM_N) << " row " << row_number << " has " << row.size() << " values but " << column_count_ << " column values are expected";
    if (n_.find(row_label) != n_.end())
      LOG_ERROR_P(PARAM_N) << "the category " << row_label << " is defined more than once.";

    for (unsigned i = 1; i < row.size(); ++i) {
      Double temp = Double();
      if (!utilities::To<Double>(row[i], temp))
        LOG_ERROR_P(PARAM_N) << "value (" << row[i] << ") in row " << row_number << " could not be converted to a Double";
      n_[row_label].push_back(temp);
    }
    row_number++;
  }
}

/**
 * Build runtime relationships between this object and other objects.
 * Build any data objects that need to be built.
 */
void Cinitial::DoBuild() {
  LOG_TRACE();
  time_steps_ = model_->managers()->time_step()->ordered_time_steps();

  // Create Category and cached category pointers
  partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  // Create derived quantity pointers
  unsigned i = 0;
  for (auto derived_quantities : derived_quanitity_) {
    if (derived_quantities != "") {
      derived_ptr_.push_back(model_->managers()->derived_quantity()->GetDerivedQuantity(derived_quantities));
      if (!derived_ptr_[i]) {
        LOG_ERROR() << "Cannot find derived quantity " << derived_quantities;
      }
    }
    ++i;
  }
}

/**
 * Execute Cinitial - this code follows from the original CASAL algorithm
 */
void Cinitial::Execute() {
  LOG_TRACE();
  map<string, vector<Double>> category_by_age_total;
  auto                        partition_iter = partition_->Begin();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    category_by_age_total[category_labels_[category_offset]].assign((max_age_ - min_age_ + 1), 0.0);
    /**
     * Loop through the  combined categories building up the total abundance for each category label
     */
    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      for (unsigned data_offset = 0; data_offset < (max_age_ - min_age_ + 1); ++data_offset) {
        unsigned age_offset = (*category_iter)->min_age_ - min_age_;
        // if this category min_age occurs after model min age ignore current age
        if (data_offset < age_offset)
          continue;
        category_by_age_total[category_labels_[category_offset]][data_offset] += (*category_iter)->data_[data_offset - age_offset];
      }
    }
  }
  LOG_TRACE();
  // loop through the category_labels and calculate the cinitial factor, which is the n_ / col_sums
  map<string, vector<Double>> category_by_age_factor;

  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset) {
    category_by_age_factor[category_labels_[category_offset]].assign((max_age_ - min_age_ + 1), 0.0);
    for (unsigned data_offset = 0; data_offset < (max_age_ - min_age_ + 1); ++data_offset) {
      if (category_by_age_total[category_labels_[category_offset]][data_offset] == 0.0)
        category_by_age_factor[category_labels_[category_offset]][data_offset] = 1.0;
      else {
        category_by_age_factor[category_labels_[category_offset]][data_offset]
            = n_[utilities::ToLowercase(category_labels_[category_offset])][data_offset] / category_by_age_total[category_labels_[category_offset]][data_offset];
      }
    }
  }
  LOG_TRACE();
  // Now loop through the combined categories multiplying each category by the factory
  // from the combined group it belongs to
  partition_iter = partition_->Begin();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    /**
     * Loop through the  combined categories building up the total abundance for each category label
     */
    auto category_iter = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      for (unsigned data_offset = 0; data_offset < (max_age_ - min_age_ + 1); ++data_offset) {
        unsigned age_offset = (*category_iter)->min_age_ - min_age_;
        // if this category min_age occurs after model min age ignore this age
        if (data_offset < age_offset)
          continue;
        (*category_iter)->data_[data_offset - age_offset] *= category_by_age_factor[category_labels_[category_offset]][data_offset];
      }
    }
  }
  // Build cache
  LOG_FINEST() << "finished calculating Cinitial";
  cached_partition_->BuildCache();
  // Execute the annual cycle for one year to calculate Cinitial
  timesteps::Manager* time_step_manager = model_->managers()->time_step();
  time_step_manager->ExecuteInitialisation(label_, 1);

  // Store that SSB value ssb_offset times in the Cintiial phase GetPhaseIndex
  LOG_FINE() << "derived_ptr_.size(): " << derived_ptr_.size();
  for (auto derived_quantities : derived_ptr_) {
    vector<vector<Double>>& initialisation_values = derived_quantities->initialisation_values();
    unsigned                cinit_phase_index     = model_->managers()->initialisation_phase()->GetPhaseIndex(label_);
    LOG_FINE() << "initialisation_values size: " << initialisation_values.size();
    LOG_FINE() << "ssb_offset: " << ssb_offset_;
    LOG_FINE() << "cinit_phase_index: " << cinit_phase_index;
    LOG_FINE() << "init_values[cinit_phase].size(): " << initialisation_values[cinit_phase_index].size();

    for (unsigned i = 0; i < ssb_offset_; ++i) initialisation_values[cinit_phase_index].push_back(*initialisation_values[cinit_phase_index].rbegin());
  }

  // set the partition back to Cinitial state
  auto cached_partition_iter = cached_partition_->Begin();
  partition_iter             = partition_->Begin();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    auto category_iter        = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      (*category_iter)->data_ = (*cached_category_iter).data_;
    }
  }
}

} /* namespace age */
} /* namespace initialisationphases */
} /* namespace niwa */
