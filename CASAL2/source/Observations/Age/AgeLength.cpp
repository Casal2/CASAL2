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
#include "Utilities/Vector.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/lognormal.hpp>
#include "TimeSteps/Manager.h"
#include "AgeLengths/AgeLength.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
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
  
  parameters_.Bind<string>(PARAM_NUMERATOR_CATEGORIES, &numerator_categories_, "The categories sampled (used in the numerator for the observation)", "", true);

  parameters_.Bind<string>(PARAM_SAMPLING_TYPE, &sample_type_, "The sampling type used to collect this data", "", PARAM_LENGTH)->set_allowed_values({PARAM_AGE, PARAM_LENGTH, PARAM_RANDOM});
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "The proportion through the mortality block of the time step when the observation is evaluated", "", Double(0.5))->set_range(0.0, 1.0);

  parameters_.Bind<unsigned>(PARAM_AGES, &individual_ages_, "Vector of individual ages", "", false);
  parameters_.Bind<double>(PARAM_LENGTHS, &individual_lengths_, "Vector of individual lengths. Has a one to one relationship with ages", "", false);

  allowed_likelihood_types_.push_back(PARAM_PSEUDO);
  allowed_likelihood_types_.push_back(PARAM_BERNOULLI);
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
  // check ages and parameters are sensible
  age_spread_ = model_->age_spread();
  if(individual_ages_.size() != individual_lengths_.size())
    LOG_ERROR_P(PARAM_AGES) << " needs to have the same number of elements as " << PARAM_LENGTHS << ". '" << individual_ages_.size() << "' ages supplied but '" << individual_lengths_.size() << "' lengths.";
  if(category_labels_.size() > 1) {
    LOG_ERROR_P(PARAM_CATEGORIES) << " only a single category label or a combined category label are allowed i.e., male or male+female.";
  }
  
  // if users don't supply this paramater we set it equal to category_labels_
  if(!parameters_.Get(PARAM_NUMERATOR_CATEGORIES)->has_been_defined()) {
    LOG_FINEST() << "users hasn't specified " << PARAM_NUMERATOR_CATEGORIES << " so we are setting it to category_labels which contains";
    for(unsigned i = 0; i < category_labels_.size(); ++i) 
      LOG_FINEST() << category_labels_[i];
    numerator_categories_ = category_labels_;
  }

  // set sum internal flags
  // enums have a cheaper lookup cost than string
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
    for(unsigned i = 0; i < unique_lengths_.size(); i++) {
      LOG_FINEST() << unique_lengths_[i];
    }
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
  // split out all combined categories that were supplied
  // do some simple sanity checks on these categories
  vector<string> split_category_labels;
  for(unsigned i = 0; i < category_labels_.size(); ++i) {
    boost::split(split_category_labels, category_labels_[i], boost::is_any_of("+"));
    for (string category_label : split_category_labels)
      split_category_labels_.push_back(category_label);
  }
  // Check correct number of selectivities provided, one for each combined category
  if(selectivity_labels_.size() != split_category_labels_.size()) {
    LOG_FATAL_P(PARAM_SELECTIVITIES) << " you need to supply a selectivity for each combined category '" << split_category_labels_.size() << "', but only supplied '" <<  selectivity_labels_.size() << "'.";
  }
  vector<string> split_category_labels_with_age_length;
  for(unsigned i = 0; i < numerator_categories_.size(); ++i) {
    boost::split(split_category_labels, numerator_categories_[i], boost::is_any_of("+"));
    for (string category_label : split_category_labels)
      split_numerator_categories_.push_back(category_label);
  }
  // check split_numerator_categories_ are all in split_category_labels_
  for(const string  cat : split_numerator_categories_) {
    std::pair<bool, int >  this_category_iter =niwa::utilities::findInVector(split_category_labels_, cat);
    if(!this_category_iter.first)
      LOG_FATAL_P(PARAM_NUMERATOR_CATEGORIES) << " the category " << cat << " could not be found in the categories supplied in " << PARAM_CATEGORIES << ". All categories supplied in " << PARAM_NUMERATOR_CATEGORIES << " must also be in " << PARAM_CATEGORIES << ".";
    if(selectivity_label_for_numerator_ != "") {
      if(selectivity_label_for_numerator_ != selectivity_labels_[this_category_iter.second]) {
        LOG_FATAL_P(PARAM_SELECTIVITIES) << "The selectivities supplied for numerator categories are not the same. All numerator categories must have the same selectivity. Category " << cat << " has selectivity " << selectivity_labels_[this_category_iter.second] << ", but an earlier numerator category had the following selectivity label " << selectivity_label_for_numerator_;
      }
    }
    selectivity_label_for_numerator_ = selectivity_labels_[this_category_iter.second];
  }
  if(split_category_labels_.size() != expected_selectivity_count_) {
    LOG_ERROR_P(PARAM_SELECTIVITIES) << " You need to specify a selectivity for all categories provided (even one for each combined category)";
  }
  LOG_FINEST() << "selectivity for numerator = " << selectivity_label_for_numerator_;

}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void AgeLength::DoBuild() {
  LOG_TRACE();
  partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  // create an index so that we can get a pointer to an individual category (we will need to access their age-length pointer)
  // from the partition

  // of the cached categories which ones are in the numerator
  // in CASAL sex was automatically accumulated, whereas in Casal2 this is more of a manual process with respect to 
  // the code blah....
  auto partition_iter        = partition_->Begin(); 
  string current_age_length_label = "";
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    auto category_iter        = partition_iter->begin();
    for (unsigned cached_counter = 0; category_iter != partition_iter->end();  ++category_iter, ++cached_counter) {
      if(find(split_numerator_categories_.begin(), split_numerator_categories_.end(),  (*category_iter)->name_) == split_numerator_categories_.end()) {
        vector_of_cached_categories_in_numerator_.push_back(false);
      } else {
        // found a numerator category in a denominator catgory
        // check age-length classes are the same in numerator categories
        vector_of_cached_categories_in_numerator_.push_back(true);
        age_length_ptr_ = (*category_iter)->age_length_; // slightly inefficient as we will overide this when there are multiple numerator categories.
        if(!age_length_ptr_)
          LOG_CODE_ERROR() << "age_length_ptr_ = (*category_iter)->age_length_";
        LOG_FINEST() << "get pointer to age length " << age_length_ptr_->label();
        if(current_age_length_label == "") {
          current_age_length_label = (*category_iter)->age_length_->label();
        } else {
          if((*category_iter)->age_length_->label() != current_age_length_label) {
            if (!parameters_.Get(PARAM_NUMERATOR_CATEGORIES)->has_been_defined()) {
              LOG_ERROR_P(PARAM_CATEGORIES) << " The categories supplied have different age-length objects. We recommend specifying " << PARAM_NUMERATOR_CATEGORIES << " with categories that have a common age-length class";
            } else {
              LOG_ERROR_P(PARAM_NUMERATOR_CATEGORIES) << " The categories supplied do not have the same age-length objects. This is an error.";
            }
          }

        }
      }
    }
  }
  // Build Selectivity pointers
  for (string label : selectivity_labels_) {
    LOG_FINEST() << "label = " << label;
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity label " << label << " was not found.";
    selectivities_.push_back(selectivity);
    if(selectivity->IsSelectivityLengthBased()) 
      LOG_WARNING() << "you have used age-length observations with a length-based selectivity - this will be quite computationally expensive";
    if(label == selectivity_label_for_numerator_) {
      numerator_selectivity_ = selectivity;
      LOG_FINEST() << "numerator label = " << selectivity->label();

    }
  }
  if(!numerator_selectivity_)
    LOG_CODE_ERROR() << "!numerator_selectivity_";

  // Create a pointer to Ageing error misclassification matrix
  if (ageing_error_label_ != "") {
    ageing_error_ = model_->managers()->ageing_error()->GetAgeingError(ageing_error_label_);
    if (!ageing_error_)
      LOG_ERROR_P(PARAM_AGEING_ERROR) << "Ageing error label (" << ageing_error_label_ << ") was not found.";
    ageing_error_matrix_ = ageing_error_->mis_matrix();
    apply_ageing_error_ = true;
  }
  LOG_FINE() <<"apply ageing error = " << apply_ageing_error_;

  // set up 5 evenly spaced quantiles of normal distn
  boost::math::normal dist{};
  for (int i=1; i<=5; i++){
    quantiles_.push_back((Double(i) - 0.5) / Double(5));
    quantile_breaks_.push_back(quantile(dist, AS_DOUBLE(quantiles_[i - 1])));
  }

  // cache memory for numbers at age
  numbers_at_age_.resize(expected_selectivity_count_);
  numbers_at_age_numerator_.resize(age_spread_, 0.0);
  numbers_by_unique_size_.resize(unique_lengths_.size(), 0.0);
  for(unsigned category_offset = 0; category_offset < expected_selectivity_count_; ++category_offset) {
    numbers_at_age_[category_offset].resize(age_spread_, 0.0);
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
  fill(numbers_by_unique_size_.begin(), numbers_by_unique_size_.end(), 0.0);
  fill(numbers_at_age_numerator_.begin(), numbers_at_age_numerator_.end(), 0.0);
  LOG_FINEST() << "Number of categories " << category_labels_.size();
  unsigned all_category_iter = 0;
  // this should only contain one element. i.e., the category_labels has to be a single combined category label male+female+ ....
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    Double start_value        = 0.0;
    Double select_value       = 0.0;
    Double end_value          = 0.0;
    auto category_iter        = partition_iter->begin();
    // this will loop over individual combined categories and store the neccessary information.
    for (unsigned cached_counter = 0; category_iter != partition_iter->end();  ++category_iter, ++cached_counter) {
      fill(numbers_at_age_[cached_counter].begin(), numbers_at_age_[cached_counter].end(), 0.0);
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        select_value = selectivities_[all_category_iter]->GetAgeResult(data_offset + model_->min_age(), age_length_ptr_);
        start_value = (*category_iter)->data_[data_offset] * select_value;
        end_value = (*category_iter)->cached_data_[data_offset] * select_value;
        LOG_FINEST() << "start : " << start_value << " end " << end_value << " selectivity = " << select_value;
        if (mean_proportion_method_) {
          numbers_at_age_[cached_counter][data_offset] = start_value + ((end_value - start_value) * time_step_proportion_);
        } else {
          numbers_at_age_[cached_counter][data_offset] = (1 - proportion_of_time_) * start_value + time_step_proportion_ * end_value;
        }
        if(vector_of_cached_categories_in_numerator_[all_category_iter])
          numbers_at_age_numerator_[data_offset] += numbers_at_age_[cached_counter][data_offset]; // this should only be accumulating over the sexes 
      }
      all_category_iter++;
    }
  }
  Double numerator = 0.0;
  Double denominator = 0.0;
  if(actual_sample_type_ == SampleType::kRandom || actual_sample_type_ == SampleType::kAge) {
    LOG_FINE() << "Random sample";
    unsigned individual_age_iter = 0;
    for(unsigned i = 0; i < n_fish_; ++i) {
      numerator = 0.0;
      denominator = 0.0;
      individual_age_iter = individual_ages_[i] - model_->min_age();

      LOG_FINEST() << "age = " << individual_ages_[i] << " individual_age_iter = " << individual_age_iter;

      if(!apply_ageing_error_) {
        LOG_FINE() << "No ageing error";
        // simpler case without ageing error
        if(numerator_selectivity_->IsSelectivityLengthBased()) {
          LOG_FINEST() << "N = " << numbers_at_age_numerator_[individual_age_iter] << " pdf = " << age_length_ptr_->get_pdf_with_sized_based_selectivity(individual_ages_[i], individual_lengths_[i], year, time_step, numerator_selectivity_);
          numerator =  numbers_at_age_numerator_[individual_age_iter] * age_length_ptr_->get_pdf_with_sized_based_selectivity(individual_ages_[i], individual_lengths_[i], year, time_step, numerator_selectivity_);
        } else {
          numerator =  numbers_at_age_numerator_[individual_age_iter] * age_length_ptr_->get_pdf(individual_ages_[i], individual_lengths_[i], year, time_step);
        }
      } else {
        LOG_FINE() << "ageing error";
        // account for ageing error
        for(unsigned age_iter = 0; age_iter < age_spread_; ++age_iter) {
          if(ageing_error_matrix_[age_iter][individual_age_iter] > 1e-4) {
            if(numerator_selectivity_->IsSelectivityLengthBased()) {
              LOG_FINEST() << "N " <<  numbers_at_age_numerator_[age_iter] << " mis " << ageing_error_matrix_[age_iter][individual_age_iter] << " pdf " << age_length_ptr_->get_pdf_with_sized_based_selectivity(age_iter + model_->min_age(), individual_lengths_[i], year, time_step, numerator_selectivity_);
              numerator +=  numbers_at_age_numerator_[age_iter] * ageing_error_matrix_[age_iter][individual_age_iter] * age_length_ptr_->get_pdf_with_sized_based_selectivity(age_iter + model_->min_age(), individual_lengths_[i], year, time_step, numerator_selectivity_);
            } else {
              LOG_FINEST() << "N " <<  numbers_at_age_numerator_[age_iter] << " mis " << ageing_error_matrix_[age_iter][individual_age_iter];// << " pdf " << age_length_ptr_->get_pdf_with_sized_based_selectivity(age_iter + model_->min_age(), individual_lengths_[i], year, time_step, numerator_selectivity_);
              LOG_FINEST() << "age-length label = " << age_length_ptr_->label();
              LOG_FINEST() << "pdf = " << age_length_ptr_->get_pdf(age_iter + model_->min_age(), individual_lengths_[i], year, time_step);
              numerator +=  numbers_at_age_numerator_[age_iter] * ageing_error_matrix_[age_iter][individual_age_iter] * age_length_ptr_->get_pdf(age_iter + model_->min_age(), individual_lengths_[i], year, time_step);
            }
          }
        }
      }
      LOG_FINEST() << "i = " << i << " numerator = " << numerator;
      // denominator 
      if(actual_sample_type_ == SampleType::kRandom) {
        LOG_FINE() << " c's = " << numbers_at_age_.size();
        for(unsigned c = 0; c < numbers_at_age_.size(); ++c) {
          for(unsigned age_iter = 0; age_iter < numbers_at_age_[c].size(); ++age_iter) {
            LOG_FINEST() << "Na = " << numbers_at_age_[c][age_iter];
            denominator += numbers_at_age_[c][age_iter];
          }
        }
      } else if(actual_sample_type_ == SampleType::kAge) {
        if(!apply_ageing_error_) {
          LOG_FINE() << "No ageing error";
          for(unsigned c = 0; c < numbers_at_age_.size(); ++c) {
            denominator += numbers_at_age_[c][individual_age_iter];
          }
        } else {
          LOG_FINE() << "ageing error";
          for(unsigned c = 0; c < numbers_at_age_.size(); ++c) {
            for(unsigned age_iter = 0; age_iter < age_spread_; ++age_iter) {
              denominator += numbers_at_age_[c][age_iter] * ageing_error_matrix_[age_iter][individual_age_iter];
            }
          }
        }
      }
      LOG_FINEST() << "i = " << i << " denominator = " << denominator;
      SaveComparison(numerator_categories_[0], individual_ages_[i], individual_lengths_[i], utilities::math::ZeroFun(numerator, delta_)  / utilities::math::ZeroFun(denominator, delta_), 1.0, 0.0, 1.0, 0.0, delta_, 0.0);
    }
  } else if(actual_sample_type_ == SampleType::kLength) { // random_by_size
    LOG_FINEST() << "random by length";
    // this is calculates the denominator
    for(unsigned l = 0; l < unique_lengths_.size(); ++l) {
      partition_iter        = partition_->Begin();  
      for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
        auto category_iter        = partition_iter->begin();
        for (unsigned cached_counter = 0; category_iter != partition_iter->end();  ++category_iter, ++cached_counter) {
          for(unsigned age_iter = 0; age_iter < age_spread_; ++age_iter) {
            if(selectivities_[cached_counter]->IsSelectivityLengthBased()) {
              //LOG_FINEST() << "age = " << age_iter << " N = " << numbers_at_age_[cached_counter][age_iter] << " pdf = " << age_length_ptr_->age_length_->get_pdf_with_sized_based_selectivity(age_iter + model_->min_age(), unique_lengths_[l], year, time_step, selectivities_[cached_counter]);
              numbers_by_unique_size_[l] += numbers_at_age_[cached_counter][age_iter] *  age_length_ptr_->get_pdf_with_sized_based_selectivity(age_iter + model_->min_age(), unique_lengths_[l], year, time_step, selectivities_[cached_counter]);
              LOG_FINEST() << numbers_by_unique_size_[l];
            } else {
              //LOG_FINEST() << "age = " << age_iter << " N = " << numbers_at_age_[cached_counter][age_iter] << " pdf = " << age_length_ptr_->age_length_->get_pdf(age_iter + model_->min_age(), unique_lengths_[l], year, time_step);
              numbers_by_unique_size_[l] += numbers_at_age_[cached_counter][age_iter] *  age_length_ptr_->get_pdf(age_iter + model_->min_age(), unique_lengths_[l], year, time_step);
            }
          }
        }
      }
      LOG_FINEST() << "numbers_by_unique_size_ = " << numbers_by_unique_size_[l];
    }
    LOG_FINEST() << "calculate numerator";
    unsigned individual_age_iter = 0;
    for(unsigned i = 0; i < n_fish_; ++i) {
      numerator = 0.0;
      denominator = 0.0;
      individual_age_iter = individual_ages_[i] - model_->min_age(); 
      if(!apply_ageing_error_) {
        LOG_FINE() << "No ageing error";
        if(numerator_selectivity_->IsSelectivityLengthBased()) {
          numerator = numbers_at_age_numerator_[individual_age_iter] * age_length_ptr_->get_pdf_with_sized_based_selectivity(individual_ages_[i], individual_lengths_[i], year, time_step, numerator_selectivity_);
        } else {
          LOG_FINE() << numbers_at_age_numerator_[individual_age_iter] << " get pdf = " << age_length_ptr_->get_pdf(individual_ages_[i], individual_lengths_[i], year, time_step);
          numerator =  numbers_at_age_numerator_[individual_age_iter] * age_length_ptr_->get_pdf(individual_ages_[i], individual_lengths_[i], year, time_step);
        }
      } else {
        LOG_FINE() << "ageing error";
        for(unsigned age_iter = 0; age_iter < age_spread_; ++age_iter) {
          if(ageing_error_matrix_[age_iter][individual_age_iter] > 1e-4) {
            if(numerator_selectivity_->IsSelectivityLengthBased()) {
              numerator +=  numbers_at_age_numerator_[age_iter] * ageing_error_matrix_[age_iter][individual_age_iter] * age_length_ptr_->get_pdf_with_sized_based_selectivity(age_iter + model_->min_age(), individual_lengths_[i], year, time_step, numerator_selectivity_);
            } else {
              LOG_FINE() << numbers_at_age_numerator_[age_iter] << " mis " <<  ageing_error_matrix_[age_iter][individual_age_iter] << " get pdf = " <<  age_length_ptr_->get_pdf(age_iter + model_->min_age(), individual_lengths_[i], year, time_step);
              numerator +=  numbers_at_age_numerator_[age_iter] * ageing_error_matrix_[age_iter][individual_age_iter] * age_length_ptr_->get_pdf(age_iter + model_->min_age(), individual_lengths_[i], year, time_step);
            }
          }
        }
      }
      // denominator
      for(unsigned l = 0; l < unique_lengths_.size(); ++l) {
        LOG_FINEST() << "checking = " << unique_lengths_[l] << " current value = " << individual_lengths_[i] << " N for this length = " << numbers_by_unique_size_[l];
        if(unique_lengths_[l] == individual_lengths_[i]) {
          denominator = numbers_by_unique_size_[l];
          break;
        }
      }
      LOG_FINEST() << "age " << individual_ages_[i] << " length " <<  individual_lengths_[i] << " numerator = " << numerator << " denominator = " << denominator;
      SaveComparison(numerator_categories_[0], individual_ages_[i], individual_lengths_[i], utilities::math::ZeroFun(numerator, delta_)  / utilities::math::ZeroFun(denominator, delta_), 1.0, 0.0, 1.0, 0.0, delta_, 0.0);
    }
  }
}
/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void AgeLength::CalculateScore() {
  likelihood_->GetScores(comparisons_);
  scores_[year_] = likelihood_->GetInitialScore(comparisons_, year_);
  LOG_FINEST() << "-- Observation neglogLikelihood calculation " << label_;
  LOG_FINEST() << "[" << year_ << "] Initial neglogLikelihood:" << scores_[year_];
  for (obs::Comparison comparison : comparisons_[year_]) {
    LOG_FINEST() << "[" << year_ << "] + neglogLikelihood: " << comparison.score_;
    scores_[year_] += comparison.score_;
  }
  
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */