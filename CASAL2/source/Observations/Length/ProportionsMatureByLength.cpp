/**
 * @file ProportionsMatureByLength.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// Headers
#include "ProportionsMatureByLength.h"

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
// Namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 * Default constructor
 */
ProportionsMatureByLength::ProportionsMatureByLength(shared_ptr<Model> model) : Observation(model) {
  obs_table_          = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);
  parameters_.Bind<double>(PARAM_LENGTH_BINS, &length_bins_, "The length bins", "", true);
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &length_plus_, "Is the last length bin a plus group? (defaults to @model value)", "", true);  // default to the model value
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The label of time-step that the observation occurs in", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.BindTable(PARAM_OBS, obs_table_, "The table of proportions at length mature ", "", false);
  // TODO:  is tolerance missing?
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "The table of error values of the observed values (note the units depend on the likelihood)", "", false);
  parameters_.Bind<string>(PARAM_TOTAL_CATEGORIES, &total_category_labels_,
                           "All category labels that were vulnerable to sampling at the time of this observation (not including the categories already given)", "", true);
  parameters_
      .Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "The proportion through the mortality block of the time step when the observation is evaluated", "",
                    Double(0.5))
      ->set_range(0.0, 1.0);

  mean_proportion_method_ = false;

  allowed_likelihood_types_.push_back(PARAM_BINOMIAL);
}

/**
 * Destructor
 */
ProportionsMatureByLength::~ProportionsMatureByLength() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate total_categories command
 */
void ProportionsMatureByLength::DoValidate() {
  // Check value for initial mortality
  if (model_->length_bins().size() == 0)
    LOG_ERROR_P(PARAM_LABEL) << ": No length bins have been specified in @model. This observation requires those to be defined";

  if(length_plus_ & !model_->length_plus())
    LOG_ERROR_P(PARAM_LENGTH_PLUS) << "you have specified a plus group on this observation, but the global length bins don't have a plus group. This is an inconsistency that must be fixed. Try changing the model plus group to false or this plus group to true";


  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ").";
  }

  map<unsigned, vector<double>> error_values_by_year;
  map<unsigned, vector<double>> obs_by_year;

  /**
   * Do some simple checks
   * e.g Validate that the length_bins are strictly increasing
   */  
  vector<double> model_length_bins = model_->length_bins();
  if (length_bins_.size() == 0) {
    LOG_FINE() << "using model length bins";
    length_bins_ = model_length_bins;
    using_model_length_bins = true;
    //length_plus_     = model_->length_plus();
  } else {
    LOG_FINE() << "using bespoke length bins";
    // allow for the use of observation-defined length bins, as long as all values are in the set of model length bin values
    using_model_length_bins = false;
    // check users haven't just respecified the moedl length bins
    bool length_bins_match = false;
    LOG_FINE() << length_bins_.size()  << "  " << model_length_bins.size();
    if(length_bins_.size() == model_length_bins.size()) {
      length_bins_match = true;
      for(unsigned len_ndx = 0; len_ndx < length_bins_.size(); len_ndx++) {
        if(length_bins_[len_ndx] != model_length_bins[len_ndx])
          length_bins_match = false;
      }
    }
    if(length_bins_match) {
      LOG_FINE() << "using have actually just respecified model bins so we are ignoring it";
      using_model_length_bins = true;
    } else {
      // Need to validate length bins are subclass of mdoel length bins.
      if(!model_->are_length_bin_compatible_with_model_length_bins(length_bins_)) {
        LOG_ERROR_P(PARAM_LENGTH_BINS) << "Length bins need to be a subset of the model length bins. See manual for more information";
      }
      LOG_FINE() << "length bins = " << length_bins_.size();
      map_local_length_bins_to_global_length_bins_ = model_->get_map_for_bespoke_length_bins_to_global_length_bins(length_bins_, length_plus_);

      LOG_FINE() << "check index";
      for(unsigned i = 0; i < map_local_length_bins_to_global_length_bins_.size(); ++i) {
        LOG_FINE() << "map_local_length_bins_to_global_length_bins_[" << i << "] = " << map_local_length_bins_to_global_length_bins_[i];
      }
    }
  }
  // more checks on the model length bins.
  for (unsigned length = 0; length < length_bins_.size(); ++length) {
    if (length_bins_[length] < 0.0)
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Observation length bin values must be positive. '" << length_bins_[length] << "' is less than 0";

    if (length > 0 && length_bins_[length - 1] >= length_bins_[length])
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Observation length bins must be strictly increasing. " << length_bins_[length - 1] << " is greater than or equal to "
                                      << length_bins_[length];

    if (std::find(model_length_bins.begin(), model_length_bins.end(), length_bins_[length]) == model_length_bins.end())
      LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Observation length bin values must be in the set of model length bins. Length '" << length_bins_[length]
                                      << "' is not in the set of model length bins.";
  }
  number_bins_                         = length_plus_ ? length_bins_.size() : length_bins_.size() - 1;

  /**
   * Now go through each category and split it if required, then check each piece to ensure
   * it's a valid category
   */
  Categories* categories = model_->categories();
  LOG_FINEST() << "Number of different total categories " << total_category_labels_.size();
  vector<string> split_category_labels;

  for (const string& category_label : total_category_labels_) {
    boost::split(split_category_labels, category_label, boost::is_any_of("+"));
    for (const string& split_category_label : split_category_labels) {
      if (!categories->IsValid(split_category_label)) {
        if (split_category_label == category_label) {
          LOG_ERROR_P(PARAM_TOTAL_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
        } else {
          LOG_ERROR_P(PARAM_TOTAL_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
                                              << " It was defined in the category collection " << category_label;
        }
      }
    }
  }
  for (auto tot_category : total_category_labels_) {
    if (std::find(category_labels_.begin(), category_labels_.end(), tot_category) != category_labels_.end())
      LOG_ERROR_P(PARAM_TOTAL_CATEGORIES) << "category '" << tot_category << "' was found in the parameter " << PARAM_CATEGORIES << ". Please remove it from the "
                                          << PARAM_TOTAL_CATEGORIES << " parameter";
  }
  LOG_FINEST() << "Number of different total categories after splitting " << total_category_labels_.size();

  /**
   * Do some simple checks
   */
  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
  }

  if (process_error_values_.size() != 0) {
    if (process_error_values_.size() != years_.size()) {
      LOG_FATAL_P(PARAM_PROCESS_ERRORS) << "Supply a process error for each year. Values for " << process_error_values_.size() << " years were provided, but " << years_.size()
                                        << " years are required";
    }
    process_errors_by_year_ = utilities::Map::create(years_, process_error_values_);
  } else {
    Double process_val      = 0.0;
    process_errors_by_year_ = utilities::Map::create(years_, process_val);
  }

  /**
   * Validate the number of obs provided matches length bins * category_labels * years
   * This is because we'll have 1 set of obs per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned                obs_expected = number_bins_ * category_labels_.size() + 1;
  vector<vector<string>>& obs_data     = obs_table_->data();
  if (obs_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_OBS) << " has " << obs_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& obs_data_line : obs_data) {
    if (obs_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but " << obs_expected << " expected. Should match the length bins * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(obs_data_line[0], year))
      LOG_ERROR_P(PARAM_OBS) << " value " << obs_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_OBS) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < obs_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(obs_data_line[i], value))
        LOG_ERROR_P(PARAM_OBS) << " value (" << obs_data_line[i] << ") could not be converted to a Double";
      obs_by_year[year].push_back(value);
    }
    if (obs_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "obs_by_year_[year].size() (" << obs_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected - 1 << ")";
  }

  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    LOG_FINEST() << "cols = " << error_values_data_line.size();
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data_line.size() << " values defined, but " << obs_expected
                                      << " should match the number of length bins * categories + 1 (for year)";
    }
    unsigned year = 0;
    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " value " << year << " is not a valid year for this observation";

    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(error_values_data_line[i], value))
        LOG_ERROR_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a Double";
      if (likelihood_type_ == PARAM_LOGNORMAL && value <= 0.0) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be equal to or less than 0.0";
      } else if ((likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) || (likelihood_type_ == PARAM_DIRICHLET && value < 0.0)) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be less than 0.0";
      }

      error_values_by_year[year].push_back(value);
    }

    if (error_values_by_year[year].size() == 1) {
      auto val_e = error_values_by_year[year][0];
      error_values_by_year[year].assign(obs_expected - 1, val_e);
    }

    if (error_values_by_year[year].size() != obs_expected - 1)
      LOG_CODE_ERROR() << "error_values_by_year_[year].size() (" << error_values_by_year[year].size() << ") != obs_expected - 1 (" << obs_expected - 1 << ")";
  }

  /**
   * Build our proportions and error values for use in the observation
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  double value = 0.0;
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {
    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < number_bins_; ++j) {
        unsigned obs_index = i * number_bins_ + j;
        if (!utilities::To<double>(iter->second[obs_index], value))
          LOG_ERROR_P(PARAM_OBS) << ": obs_ value (" << iter->second[obs_index] << ") at index " << obs_index + 1 << " in the definition could not be converted to a Double";

        auto e_f = error_values_by_year.find(iter->first);
        if (e_f != error_values_by_year.end()) {
          error_values_[iter->first][category_labels_[i]].push_back(e_f->second[obs_index]);
          proportions_[iter->first][category_labels_[i]].push_back(value);
        }
      }
    }
  }
}

/**
 * Build any runtime relationships and ensure that
 * the labels for other objects are valid.
 */
void ProportionsMatureByLength::DoBuild() {
  LOG_TRACE();
  // Get all categories in the system.
  total_partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, total_category_labels_));
  total_cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, total_category_labels_));

  // all categories
  partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  
  vector<Double>              total_numbers_at_length_;
  vector<Double>              cached_total_numbers_at_length_;
  vector<Double>              numbers_at_length_;
  vector<Double>              cached_numbers_at_length_;

  total_numbers_at_length_.resize(model_->get_number_of_length_bins(), 0.0);
  cached_total_numbers_at_length_.resize(model_->get_number_of_length_bins(), 0.0);
  numbers_at_length_.resize(model_->get_number_of_length_bins(), 0.0);
  cached_numbers_at_length_.resize(model_->get_number_of_length_bins(), 0.0);
  final_numbers_at_length_.resize(model_->get_number_of_length_bins(), 0.0);
  final_total_numbers_at_length_.resize(number_bins_, 0.0);
  expected_values_.resize(number_bins_, 0.0);
  TimeStep* time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    for (unsigned year : years_) 
      time_step->SubscribeToBlock(this, year);
  }
}

/**
 * This method is called at the start of the time step for this observation.
 *
 * Build the cache for the partition structure to use with any interpolation
 */
void ProportionsMatureByLength::PreExecute() {
  LOG_TRACE();
  cached_partition_->BuildCache();
  total_cached_partition_->BuildCache();
  LOG_FINEST() << "PreExecute: observation " << label_;
}

/**
 * Execute
 */
void ProportionsMatureByLength::Execute() {
  LOG_TRACE();

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto partition_iter              = partition_->Begin();
  auto total_partition_iter        = total_partition_->Begin();
  Double start_value = 0.0;
  Double end_value   = 0.0;
  Double final_value  = 0.0;
  Double total_start_value = 0.0;
  Double total_end_value   = 0.0;
  Double total_final_value  = 0.0;
  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  LOG_FINEST() << "Number of categories " << category_labels_.size();
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {

    // clear these temporay vectors
    std::fill(expected_values_.begin(), expected_values_.end(), 0.0);
    std::fill(cached_total_numbers_at_length_.begin(), cached_total_numbers_at_length_.end(), 0.0);
    std::fill(total_numbers_at_length_.begin(), total_numbers_at_length_.end(), 0.0);
    std::fill(cached_numbers_at_length_.begin(), cached_numbers_at_length_.end(), 0.0);
    std::fill(numbers_at_length_.begin(), numbers_at_length_.end(), 0.0);

    /**
     * Loop through the total categories building up numbers at age.
     */
    auto total_category_iter        = total_partition_iter->begin();
    for (; total_category_iter != total_partition_iter->end(); ++total_category_iter) {
      if(using_model_length_bins) {
        LOG_FINE() << "using model length bins";
        for (unsigned model_length_offset = 0; model_length_offset < model_->get_number_of_length_bins(); ++model_length_offset) {
          // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
          cached_total_numbers_at_length_[model_length_offset] += (*total_category_iter)->data_[model_length_offset] ;
          total_numbers_at_length_[model_length_offset]   += (*total_category_iter)->cached_data_[model_length_offset] ;
        }
      } else {
        LOG_FINE() << "using bespoke length bins";
        for (unsigned model_length_offset = 0; model_length_offset < model_->get_number_of_length_bins(); ++model_length_offset) {
          if(map_local_length_bins_to_global_length_bins_[model_length_offset] >= 0) {
            // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
            cached_total_numbers_at_length_[map_local_length_bins_to_global_length_bins_[model_length_offset]] += (*total_category_iter)->data_[model_length_offset];
            total_numbers_at_length_[map_local_length_bins_to_global_length_bins_[model_length_offset]]   += (*total_category_iter)->cached_data_[model_length_offset];
          }
        }
      }
    }
    /**
     * Loop through the categories building up numbers at age for the mature, but also adding them onto the total .
     */
    auto category_iter        = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      if(using_model_length_bins) {
        LOG_FINE() << "using model length bins";
        for (unsigned model_length_offset = 0; model_length_offset < model_->get_number_of_length_bins(); ++model_length_offset) {
          // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
          cached_numbers_at_length_[model_length_offset] += (*category_iter)->data_[model_length_offset] ;
          numbers_at_length_[model_length_offset]   += (*category_iter)->cached_data_[model_length_offset] ;
          cached_total_numbers_at_length_[model_length_offset] += (*category_iter)->data_[model_length_offset] ;
          total_numbers_at_length_[model_length_offset]   += (*category_iter)->cached_data_[model_length_offset] ;

        }
      } else {
        LOG_FINE() << "using bespoke length bins";
        for (unsigned model_length_offset = 0; model_length_offset < model_->get_number_of_length_bins(); ++model_length_offset) {
          if(map_local_length_bins_to_global_length_bins_[model_length_offset] >= 0) {
            // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
            cached_numbers_at_length_[map_local_length_bins_to_global_length_bins_[model_length_offset]] += (*category_iter)->data_[model_length_offset];
            numbers_at_length_[map_local_length_bins_to_global_length_bins_[model_length_offset]]   += (*category_iter)->cached_data_[model_length_offset];
            cached_total_numbers_at_length_[map_local_length_bins_to_global_length_bins_[model_length_offset]] += (*category_iter)->data_[model_length_offset];
            total_numbers_at_length_[map_local_length_bins_to_global_length_bins_[model_length_offset]]   += (*category_iter)->cached_data_[model_length_offset];
          }
        }
      }
    }
    for (unsigned length_offset = 0; length_offset < number_bins_; ++length_offset) {
      start_value = cached_numbers_at_length_[length_offset];
      end_value = numbers_at_length_[length_offset];
      total_start_value = cached_total_numbers_at_length_[length_offset];
      total_end_value = total_numbers_at_length_[length_offset];
      if (mean_proportion_method_)  {
        final_value = start_value + ((end_value - start_value) * proportion_of_time_);
        total_final_value = total_start_value + ((total_end_value - total_start_value) * proportion_of_time_);
      } else {
        final_value = (1 - proportion_of_time_) * start_value + proportion_of_time_ * end_value;
        total_final_value = (1 - proportion_of_time_) * total_start_value + proportion_of_time_ * total_end_value;
      }
      expected_values_[length_offset] += final_value / total_final_value;;
      LOG_FINE() << "----------";
      LOG_FINE() << "Category: " << (*total_category_iter)->name_ << " at length " << length_bins_[length_offset];
      LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
      LOG_FINE() << "total_start_value: " << total_start_value << "; total_end_value: " << total_end_value << "; final_value: " << total_final_value;
      LOG_FINE() << "expected_value becomes: " << expected_values_[length_offset];
    }
    if (expected_values_.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values.size(" << expected_values_.size() << ") != proportions_[category_offset].size("
                       << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */
    for (unsigned i = 0; i < expected_values_.size(); ++i) {
      LOG_FINEST() << "proportions mature at age " <<  i << " = " << expected_values_[i];
      SaveComparison(category_labels_[category_offset], 0.0, length_bins_[i], expected_values_[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
                     process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProportionsMatureByLength::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {
    /**
     * The comparisons are already proportions so the can be sent straight to the likelihood
     */
    likelihood_->GetScores(comparisons_);

    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "] + neglogLikelihood: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
