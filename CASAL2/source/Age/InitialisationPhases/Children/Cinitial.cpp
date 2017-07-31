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

#include "Common/Categories/Categories.h"
#include "Common/Model/Managers.h"
#include "Common/Model/Model.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/DerivedQuantities/Manager.h"
#include "Common/InitialisationPhases/Manager.h"
#include "Age/Processes/Children/RecruitmentBevertonHolt.h"

// namesapces
namespace niwa {
namespace age {
namespace initialisationphases {

/**
 * Default constrcutor
 *
 * @param model Pointer to our core model object
 */
Cinitial::Cinitial(Model* model)
  : InitialisationPhase(model) {

  n_table_ = new parameters::Table(PARAM_N);

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The list of categories for the Cinitial initialisation", "");
  parameters_.BindTable(PARAM_N, n_table_, "Table of values for the Cinitial initialisation", "", false, false);

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
	min_age_ = model_->min_age();
	max_age_ = model_->max_age();

  if (max_age_ < min_age_)
    LOG_ERROR_P(PARAM_MIN_AGE) << "(" << min_age_ << ") cannot be less than the max age(" << max_age_ << ")";

  column_count_ = (max_age_ - min_age_) + 2;

  /**
   * Convert the string values to doubles and load them in to a table.
   */
  vector<vector<string>>& data = n_table_->data();
  unsigned row_number = 1;
  for (auto row : data) {
    if (row.size() != column_count_)
      LOG_ERROR_P(PARAM_N) << "the " << row_number << "the row has " << row.size() << " values but " << column_count_ << " values are expected";
    if (n_.find(row[0]) != n_.end())
      LOG_ERROR_P(PARAM_N) << "the category " << row[0] << " is defined more than once. You can only define a category once";


    for (unsigned i = 1; i < row.size(); ++i) {
      Double temp = Double();
      if (!utilities::To<Double>(row[i], temp))
        LOG_ERROR_P(PARAM_N) << "value (" << row[i] << ") in row " << row_number << " is not a valid numeric";
      n_[row[0]].push_back(temp);
    }
    row_number++;
  }
}

/**
 * Build runtime relationships between this object and other objects.
 * Build any data objects that need to be built.
 */
void Cinitial::DoBuild() {
  time_steps_ = model_->managers().time_step()->ordered_time_steps();

  // Set the default process labels for the time step for this phase
  for (auto time_step : time_steps_)
    time_step->SetInitialisationProcessLabels(label_, time_step->process_labels());

  // Create Category and cached category pointers
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));

  // Calculate ssb_ofset and ssb label if there is BH_recruitment process in the annual cycle
  for (auto time_step : model_->managers().time_step()->ordered_time_steps()) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kRecruitment && process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT) {
        RecruitmentBevertonHolt* recruitment = dynamic_cast<RecruitmentBevertonHolt*>(process);
        if (!recruitment)
          LOG_CODE_ERROR() << "BevertonHolt Recruitment exists but dynamic cast pointer cannot be made, if (!recruitment) ";
        if (recruitment->ssb_offset() > ssb_offset_)
          ssb_offset_ = recruitment->ssb_offset();
        derived_quanitity_.push_back(recruitment->ssb_label());
      }
    }
  }
  // Create derived quantity pointers
  unsigned i = 0;
  for (auto derived_quantities : derived_quanitity_) {
    if (derived_quantities != "") {
      derived_ptr_.push_back(model_->managers().derived_quantity()->GetDerivedQuantity(derived_quantities));
      if (!derived_ptr_[i]) {
        LOG_ERROR() << "Cannot find " << derived_quantities;
      }
    }
    ++i;
  }
}

/**
 * Execute Cinitial this code follows from the original CASAL algorithm
 */
void Cinitial::Execute() {

  map<string, vector<Double>> category_by_age_total;

  auto partition_iter = partition_->Begin();
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

  // loop through the category_labels and calculate the cinitial factor, which is the n_ / col_sums
  map<string, vector<Double>> category_by_age_factor;

  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset) {
    category_by_age_factor[category_labels_[category_offset]].assign((max_age_ - min_age_ + 1), 0.0);
    for (unsigned data_offset = 0; data_offset < (max_age_ - min_age_ + 1); ++data_offset) {

      if (category_by_age_total[category_labels_[category_offset]][data_offset] == 0.0)
        category_by_age_factor[category_labels_[category_offset]][data_offset] = 1.0;
      else {
        category_by_age_factor[category_labels_[category_offset]][data_offset] = n_[category_labels_[category_offset]][data_offset]
            / category_by_age_total[category_labels_[category_offset]][data_offset];
      }
    }
  }
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
  cached_partition_->BuildCache();

  // Execute the annual cycle for one year to calculate Cinitial
  timesteps::Manager* time_step_manager = model_->managers().time_step();
  time_step_manager->ExecuteInitialisation(label_, 1);

  // Store that SSB value ssb_offset times in the Cintiial phase GetPhaseIndex
  LOG_FINE() << "derived_ptr_.size(): " << derived_ptr_.size();
  for (auto derived_quantities : derived_ptr_) {
    vector<vector<Double>>& initialisation_values = derived_quantities->initialisation_values();
    unsigned cinit_phase_index = model_->managers().initialisation_phase()->GetPhaseIndex(label_);
    LOG_FINE() << "initialisation_values size: " << initialisation_values.size();
    LOG_FINE() << "ssb_offset: " << ssb_offset_;
    LOG_FINE() << "cinit_phase_index: " << cinit_phase_index;
    LOG_FINE() << "init_values[cinit_phase].size(): " << initialisation_values[cinit_phase_index].size();

    for(unsigned i = 0; i < ssb_offset_; ++i)
      initialisation_values[cinit_phase_index].push_back(*initialisation_values[cinit_phase_index].rbegin());
  }


  // set the partition back to Cinitial state
  auto cached_partition_iter  = cached_partition_->Begin();
  partition_iter = partition_->Begin();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      (*category_iter)->data_ = (*cached_category_iter).data_;
    }
  }
}

} /* namespace initialisationphases */
} /* namespace age */
} /* namespace niwa */
