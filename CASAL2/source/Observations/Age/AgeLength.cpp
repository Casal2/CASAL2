/**
 * @file AgeLength.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// Headers
#include "AgeLength.h"

#include <algorithm>

#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "AgeingErrors/AgeingError.h"
#include "AgeingErrors/Manager.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/lognormal.hpp>
#include "TimeSteps/Manager.h"
#include "AgeLengths/AgeLength.h"

// Namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
AgeLength::AgeLength(shared_ptr<Model> model) : Observation(model) {
  parameters_.Bind<unsigned>(PARAM_YEAR, &year_, "The year this observation occurs in.", "");
  
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of the time step that the observation occurs in", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities", "", true);
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "The label of ageing error to use", "", "");
  
 
  parameters_.Bind<string>(PARAM_SAMPLING_TYPE, &sample_type_, "The sampling type used to collect this data", "", PARAM_LENGTH)->set_allowed_values({PARAM_AGE, PARAM_LENGTH, PARAM_RANDOM});
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "The proportion through the mortality block of the time step when the observation is evaluated", "", Double(0.5))->set_range(0.0, 1.0);

  parameters_.Bind<double>(PARAM_AGES, &individual_ages_, "Vector of individual ages", "", false);
  parameters_.Bind<double>(PARAM_LENGTHS, &individual_lengths_, "Vector of individual lengths. Has a one to one relationship with ages", "", false);


   /*
  parameters_.Bind<double>(PARAM_LENGTHS, &individual_lengths_, "Vector of individual lengths", "", false);

  */
  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
  allowed_likelihood_types_.push_back(PARAM_DIRICHLET);
  allowed_likelihood_types_.push_back(PARAM_LOGISTIC_NORMAL);
  
}

/**
 * Destructor
 */
AgeLength::~AgeLength() {

}
/**
 * Validate configuration file parameters
 */
 
void AgeLength::DoValidate() {
  
  LOG_TRACE();
  
  age_spread_ = model_->age_spread();
  if(individual_ages_.size() != individual_lengths_.size())
    LOG_ERROR_P(PARAM_AGES) << " needs to have the same number of elements as " << PARAM_LENGTHS << ". '" << individual_ages_.size() << "' ages supplied but '" << individual_lengths_.size() << "' lengths.";
  if(category_labels_.size() > 1) {
    LOG_ERROR_P(PARAM_CATEGORIES) << " only a single category label or a combined category label are allowed i.e., male or male+female.";
  }
  

  if(sample_type_ == PARAM_AGE) {
    actual_sample_type_ = SampleType::kAge;
  } else if (sample_type_ == PARAM_LENGTH) {
    actual_sample_type_ = SampleType::kLength;
    // first calcuate the denominator for unique fish sizes, by sex
    for(unsigned i = 0; i < individual_lengths_.size(); i++) {
      if(!utilities::math::in(unique_lengths_,individual_lengths_[i])) {
        unique_lengths_.push_back(individual_lengths_[i]);
      }
    }
    sort(unique_lengths_.begin(),unique_lengths_.end());
  } else if(sample_type_ == PARAM_RANDOM) {
    actual_sample_type_ = SampleType::kRandom;
  }
  n_fish_ = individual_ages_.size();

  // check no ages are less than min_age or greater than max_age
  for(unsigned i = 0; i < n_fish_; i++) {
    if(individual_ages_[i] < model_->min_age())
      LOG_ERROR_P(PARAM_AGES) << " age '" << individual_ages_[i] << "' at element '" << i + 1 << "' is less than the model min_age. This is not allowed.";
    if(individual_ages_[i] > model_->max_age())
      LOG_ERROR_P(PARAM_AGES) << " age '" << individual_ages_[i] << "' at element '" << i + 1 << "' is greater than the model max_age. This is not allowed.";
  }
  // TODO: check the same for lengths supplied

  // have the supplied cached categories
  if(expected_selectivity_count_ > 1) {
    combined_categories_provided_ = true;
  }
  
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void AgeLength::DoBuild() {

  LOG_TRACE();
  partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));

  // Build Selectivity pointers
  for (string label : selectivity_labels_) {
    LOG_FINEST() << "label = " << label;
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";
    selectivities_.push_back(selectivity);
  }

  if (selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_sel = selectivities_[0];
    selectivities_.assign(category_labels_.size(), val_sel);
  }

  // Create a pointer to Ageing error misclassification matrix
  if (ageing_error_label_ != "") {
    ageing_error_ = model_->managers()->ageing_error()->GetAgeingError(ageing_error_label_);
    if (!ageing_error_)
      LOG_ERROR_P(PARAM_AGEING_ERROR) << "Ageing error label (" << ageing_error_label_ << ") was not found.";
    ageing_error_matrix_ = ageing_error_->mis_matrix();
  }

  // Find out how many selectivities there are, we can have selectivities for combined categories. this is what I am trying to solve for
  // We can either have 1 selectivity
  // A selectivity for each category_label
  // or a selectivity for each combined category in each category_label (total categories) These are defined by business rules in the DoValidate.

  if (selectivity_labels_.size() > category_labels_.size()) {
    selectivity_for_combined_categories_ = true;
  }

  // set up 5 evenly spaced quantiles of normal distn
  boost::math::normal dist{};
  for (int i=1; i<=5; i++){
    quantiles_.push_back((Double(i) - 0.5) / Double(5));
    quantile_breaks_.push_back(quantile(dist, AS_DOUBLE(quantiles_[i - 1])));
  }

  // cache memory for numbers at age
  numbers_at_age_.resize(expected_selectivity_count_);
  pdf_by_age_and_length_.resize(expected_selectivity_count_);
  pre_numbers_at_age_.resize(expected_selectivity_count_);
  for(unsigned category_offset = 0; category_offset < expected_selectivity_count_; ++category_offset) {
    numbers_at_age_[category_offset].resize(age_spread_, 0.0);
    pdf_by_age_and_length_[category_offset].resize(n_fish_, 0.0);
    pre_numbers_at_age_[category_offset].resize(age_spread_, 0.0);
  }

  // Subscribe this observation to timestep to be executed
  auto time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_ERROR_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    time_step->SubscribeToBlock(this, year_);
  }
}

/**
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void AgeLength::PreExecute() {
  LOG_TRACE();
  cached_partition_->BuildCache();
}

/**
 * Execute
 */
void AgeLength::Execute() {
  
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;
  auto partition_iter        = partition_->Begin();  // vector<vector<partition::Category> >
  unsigned year = model_->current_year();
  unsigned time_step = model_->managers()->time_step()->current_time_step();


  LOG_FINEST() << "Number of categories " << category_labels_.size();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    Double start_value        = 0.0;
    Double end_value          = 0.0;
    auto category_iter        = partition_iter->begin();
    for (unsigned cached_counter = 0; category_iter != partition_iter->end();  ++category_iter, ++cached_counter) {
      fill(numbers_at_age_[cached_counter].begin(), numbers_at_age_[cached_counter].end(), 0.0);
      fill(pdf_by_age_and_length_[cached_counter].begin(), pdf_by_age_and_length_[cached_counter].end(), 0.0);
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        start_value = (*category_iter)->data_[data_offset];
        end_value = (*category_iter)->cached_data_[data_offset];
        if (mean_proportion_method_) {
          numbers_at_age_[cached_counter][data_offset] = start_value + ((end_value - start_value) * time_step_proportion_);
        } else {
          numbers_at_age_[cached_counter][data_offset] = (1 - proportion_of_time_) * start_value + time_step_proportion_ * end_value;
        }
      }
    }
  }
  Double numerator = 0.0;
  if(actual_sample_type_ == SampleType::kRandom) {
    LOG_FINE() << "Random sample";
    for(unsigned i = 0; i < n_fish_; ++i) {
      if(ageing_error_) {
        LOG_FINE() << "ageing error";
        // account for ageing error
        for(unsigned age_iter = 0;age_iter < age_spread_; ++age_iter) {
          if(ageing_error_matrix_[age_iter][(unsigned)individual_ages_[i] - model_->min_age()] > 1e-4) {
            // we only want access to one category not sum here
            auto category_iter        = partition_iter->begin();
            for (unsigned cached_counter = 0; category_iter != partition_iter->end();  ++category_iter, ++cached_counter) {
              if(selectivities_[cached_counter]->IsSelectivityLengthBased()) {
                numerator +=  numbers_at_age_[cached_counter][age_iter] * ageing_error_matrix_[age_iter][(unsigned)individual_ages_[i] - model_->min_age()] * (*category_iter)->age_length_->get_pdf_with_sized_based_selectivity((unsigned)individual_ages_[i], individual_lengths_[i], year, time_step,selectivities_[cached_counter]);
              } else {
                numerator +=  numbers_at_age_[cached_counter][age_iter] * ageing_error_matrix_[age_iter][(unsigned)individual_ages_[i] - model_->min_age()] * (*category_iter)->age_length_->get_pdf((unsigned)individual_ages_[i], individual_lengths_[i], year, time_step);
              }
            }
          }
        }
      } else {
        // simpler case without ageing error

      }
    }
  } else if(actual_sample_type_ == SampleType::kLength) { // random_by_size
    partition_iter        = partition_->Begin();  // vector<vector<partition::Category> >
    for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
      auto category_iter        = partition_iter->begin();
      for (unsigned cached_counter = 0; category_iter != partition_iter->end();  ++category_iter, ++cached_counter) {
        // calculate the denominator
        if(selectivities_[cached_counter]->IsSelectivityLengthBased()) {
          for(unsigned i = 0; i < n_fish_; ++i) {
            for(unsigned age_iter = 0;age_iter < age_spread_; ++age_iter) {
              unsigned age       = model_->min_age() + age_iter;
              pdf_by_age_and_length_[cached_counter][i] += numbers_at_age_[cached_counter][age_iter] * (*category_iter)->age_length_->get_pdf_with_sized_based_selectivity(age, individual_lengths_[i], year, time_step, selectivities_[cached_counter]);
            }
          }
        } else {
          for(unsigned i = 0; i < n_fish_; ++i) {
            for(unsigned age_iter = 0;age_iter < age_spread_; ++age_iter) {
              unsigned age       = model_->min_age() + age_iter;
              pdf_by_age_and_length_[cached_counter][i] += numbers_at_age_[cached_counter][age_iter] * (*category_iter)->age_length_->get_pdf(age, individual_lengths_[i], year, time_step);
            }
          }
        }
        // now loop over fish and calculate numerator, denominator and -log(L) for each
 
        for(unsigned i = 0; i < n_fish_; ++i) {
          if(ageing_error_) {
            // account for ageing error
            for(unsigned age_iter = 0;age_iter < age_spread_; ++age_iter) {
              if(ageing_error_matrix_[age_iter][(unsigned)individual_ages_[i] - model_->min_age()] > 1e-4) {
                if(selectivities_[cached_counter]->IsSelectivityLengthBased()) {
                  numerator +=  numbers_at_age_[cached_counter][age_iter] * ageing_error_matrix_[age_iter][(unsigned)individual_ages_[i] - model_->min_age()] * (*category_iter)->age_length_->get_pdf_with_sized_based_selectivity((unsigned)individual_ages_[i], individual_lengths_[i], year, time_step,selectivities_[cached_counter]);
                } else {
                  numerator +=  numbers_at_age_[cached_counter][age_iter] * ageing_error_matrix_[age_iter][(unsigned)individual_ages_[i] - model_->min_age()] * (*category_iter)->age_length_->get_pdf((unsigned)individual_ages_[i], individual_lengths_[i], year, time_step);
                }
              }
            }
          } else {
            // simpler case without ageing error
            if(selectivities_[cached_counter]->IsSelectivityLengthBased()) {
              numerator =  numbers_at_age_[cached_counter][(unsigned)individual_ages_[i] - model_->min_age()] * (*category_iter)->age_length_->get_pdf_with_sized_based_selectivity((unsigned)individual_ages_[i], individual_lengths_[i], year, time_step,selectivities_[cached_counter]);
            } else {
              numerator =  numbers_at_age_[cached_counter][(unsigned)individual_ages_[i] - model_->min_age()] * (*category_iter)->age_length_->get_pdf((unsigned)individual_ages_[i], individual_lengths_[i], year, time_step);
            }
          }
        }

      }
    }


  }
  
}
/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void AgeLength::CalculateScore() {
  
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */