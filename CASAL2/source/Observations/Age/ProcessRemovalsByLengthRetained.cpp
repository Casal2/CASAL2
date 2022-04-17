/**
 * @file ProcessRemovalsByLengthRetained.cpp
 * @author  S. Datta
 * @version 1.0
 * @date 06/05/2019
 * @section LICENSE
 *
 * Copyright NIWA Science 2016 - www.niwa.co.nz
 */

// Headers
#include "ProcessRemovalsByLengthRetained.h"

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "AgeLengths/AgeLength.h"
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
namespace age {

/**
 * Default constructor
 */
ProcessRemovalsByLengthRetained::ProcessRemovalsByLengthRetained(shared_ptr<Model> model) : Observation(model) {
  obs_table_          = new parameters::Table(PARAM_OBS);
  error_values_table_ = new parameters::Table(PARAM_ERROR_VALUES);

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The time step to execute in", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.Bind<string>(PARAM_METHOD_OF_REMOVAL, &method_, "The label of observed method of removals", "", "");
  parameters_.Bind<double>(PARAM_LENGTH_BINS, &length_bins_, "The length bins", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &length_plus_, "Is the last length bin a plus group? (defaults to @model value)", "",
                         model->length_plus());  // default to the model value
  parameters_.BindTable(PARAM_OBS, obs_table_, "The table of observed values", "", false);
  parameters_.BindTable(PARAM_ERROR_VALUES, error_values_table_, "The table of error values of the observed values (note that the units depend on the likelihood)", "", false);
  parameters_.Bind<string>(PARAM_MORTALITY_INSTANTANEOUS_PROCESS, &process_label_, "The label of the mortality instantaneous process for the observation", "");
  parameters_.Bind<bool>(PARAM_SIMULATED_DATA_SUM_TO_ONE, &simulated_data_sum_to_one_, "Whether simulated data is discrete or scaled by totals to be proportions for each year", "", true);
  parameters_.Bind<bool>(PARAM_SUM_TO_ONE, &sum_to_one_, "Scale year (row) observed values by the total, so they sum = 1", "", false);

  mean_proportion_method_ = false;

  RegisterAsAddressable(PARAM_PROCESS_ERRORS, &process_error_values_);

  allowed_likelihood_types_.push_back(PARAM_LOGNORMAL);
  allowed_likelihood_types_.push_back(PARAM_MULTINOMIAL);
}

/**
 * Destructor
 */
ProcessRemovalsByLengthRetained::~ProcessRemovalsByLengthRetained() {
  delete obs_table_;
  delete error_values_table_;
}

/**
 * Validate configuration file parameters
 */
void ProcessRemovalsByLengthRetained::DoValidate() {
  // Check value for initial mortality
  if (model_->length_bins().size() == 0)
    LOG_ERROR_P(PARAM_LABEL) << ": No length bins have been specified in @model. This observation requires those to be defined";

  if(length_plus_ & !model_->length_plus())
    LOG_ERROR_P(PARAM_LENGTH_PLUS) << "you have specified a plus group on this observation, but the global length bins don't have a plus group. This is an inconsistency that must be fixed. Try changing the model plus group to false or this plus group to true";

  // How many elements are expected in our observed table;
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
   /**
   * Do some simple checks
   * e.g Validate that the length_bins are strictly increasing
   */  
  vector<double> model_length_bins = model_->length_bins();
  if (length_bins_.size() == 0) {
    LOG_FINE() << "using model length bins";
    length_bins_ = model_length_bins;
    using_model_length_bins = true;
    // length_plus_     = model_->length_plus();
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
        LOG_FINE() << "i = " << map_local_length_bins_to_global_length_bins_[i];
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

  if (process_error_values_.size() != 0 && process_error_values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_PROCESS_ERRORS) << " number of values provided (" << process_error_values_.size() << ") does not match the number of years provided (" << years_.size()
                                      << ")";
  }
  for (Double process_error : process_error_values_) {
    if (process_error < 0.0)
      LOG_ERROR_P(PARAM_PROCESS_ERRORS) << ": process_error (" << AS_DOUBLE(process_error) << ") cannot be less than 0.0";
  }
  if (process_error_values_.size() != 0)
    process_errors_by_year_ = utilities::Map::create(years_, process_error_values_);
  if (delta_ < 0.0)
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0";

  /**
   * Validate the number of obs provided matches age spread * category_labels * years
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
      LOG_ERROR_P(PARAM_OBS) << " has " << obs_data_line.size() << " values defined, but " << obs_expected << " should match the number bins * categories + 1 (for year)";
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
      LOG_FATAL_P(PARAM_OBS) << " " << obs_by_year[year].size() << " lengths were provided, but " << obs_expected - 1 << " lengths are required";
  }

  /**
   * Build our error value map
   */
  vector<vector<string>>& error_values_data = error_values_table_->data();
  if (error_values_data.size() != years_.size()) {
    LOG_FATAL_P(PARAM_ERROR_VALUES) << " has " << error_values_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& error_values_data_line : error_values_data) {
    if (error_values_data_line.size() != 2 && error_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_ERROR_VALUES) << " has " << error_values_data_line.size() << " values defined, but " << obs_expected
                                      << " should match the number bins * categories + 1 (for year)";
    }

    unsigned year = 0;
    if (!utilities::To<unsigned>(error_values_data_line[0], year))
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " value " << error_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    if (std::find(years_.begin(), years_.end(), year) == years_.end())
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " value " << year << " is not a valid year for this observation";
    for (unsigned i = 1; i < error_values_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(error_values_data_line[i], value))
        LOG_FATAL_P(PARAM_ERROR_VALUES) << " value (" << error_values_data_line[i] << ") could not be converted to a Double";
      if (likelihood_type_ == PARAM_LOGNORMAL && value <= 0.0) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be equal to or less than 0.0";
      } else if (likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) {
        LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be less than 0.0";
      }

      error_values_by_year[year].push_back(value);
    }

    if (error_values_by_year[year].size() == 1) {
      auto val_e = error_values_by_year[year][0];
      error_values_by_year[year].assign(obs_expected - 1, val_e);
    }

    if (error_values_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_ERROR_VALUES) << " " << error_values_by_year[year].size() << " error values by year were provided, but " << obs_expected - 1
                                      << " values are required based on the obs table";
  }

  /**
   * Build our proportions and error values for use in the observation
   * If the proportions for a given observation do not sum to 1.0
   * and is off by more than the tolerance rescale them.
   */
  double value = 0.0;
  for (auto iter = obs_by_year.begin(); iter != obs_by_year.end(); ++iter) {
    double total = 0.0;

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < number_bins_; ++j) {
        auto e_f = error_values_by_year.find(iter->first);
        if (e_f != error_values_by_year.end()) {
          unsigned obs_index = i * number_bins_ + j;
          value              = iter->second[obs_index];
          error_values_[iter->first][category_labels_[i]].push_back(e_f->second[obs_index]);
          // if not rescaling add the data
          if(!sum_to_one_)
            proportions_[iter->first][category_labels_[i]].push_back(value);
          total += value;
        }
      }
    }
    // rescale the year obs so sum = 1
    if(sum_to_one_) {
      for (unsigned i = 0; i < category_labels_.size(); ++i) {
        for (unsigned j = 0; j < number_bins_; ++j) {
          unsigned obs_index = i * number_bins_ + j;
          value = iter->second[obs_index];
          proportions_[iter->first][category_labels_[i]].push_back(value / total);
        }
      }
    } else {
      if (!utilities::math::IsOne(total)) {
        LOG_WARNING()  << "obs sum total (" << total << ") for year (" << iter->first << ") doesn't sum to 1.0";
      }
    }
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void ProcessRemovalsByLengthRetained::DoBuild() {
  partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  // flag age-length to Build age-length matrix
  auto partition_iter = partition_->Begin();  
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    auto category_iter        = partition_iter->begin();
    for (; category_iter != partition_iter->end();  ++category_iter)
      (*category_iter)->age_length_->BuildAgeLengthMatrixForTheseYears(years_);
  }
  //  if (ageing_error_label_ != "")
  //   LOG_CODE_ERROR() << "ageing error has not been implemented for the proportions at age observation";

  length_results_.resize(number_bins_ * category_labels_.size(), 0.0);

  auto time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_FATAL_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    auto process                      = time_step->SubscribeToProcess(this, years_, process_label_);
    mortality_instantaneous_retained_ = dynamic_cast<MortalityInstantaneousRetained*>(process);
  }

  if (mortality_instantaneous_retained_ == nullptr)
    LOG_FATAL() << "Observation " << label_ << " can be used with Process type " << PARAM_MORTALITY_INSTANTANEOUS_RETAINED << " only. Process " << process_label_
                << " was not found or does not have retained catch characteristics specified.";

  // Need to split the categories if any are combined for checking
  vector<string> temp_split_category_labels, split_category_labels;

  for (const string& category_label : category_labels_) {
    boost::split(temp_split_category_labels, category_label, boost::is_any_of("+"));
    for (const string& split_category_label : temp_split_category_labels) {
      split_category_labels.push_back(split_category_label);
    }
  }

  // Need to make this a vector so its compatible with the function couldn't be bothered templating sorry
  vector<string> methods;
  methods.push_back(method_);

  // Do some checks so that the observation and process are compatible
  if (!mortality_instantaneous_retained_->check_methods_for_removal_obs(methods))
    LOG_ERROR_P(PARAM_METHOD_OF_REMOVAL) << "could not find all these methods in the instantaneous_mortality process labeled " << process_label_
                                         << ". Check that the methods are compatible with this process";
  if (!mortality_instantaneous_retained_->check_categories_in_methods_for_removal_obs(methods, split_category_labels))
    LOG_ERROR_P(PARAM_CATEGORIES) << "could not find all these categories in the instantaneous_mortality process labeled " << process_label_
                                  << ". Check that the categories are compatible with this process";
  if (!mortality_instantaneous_retained_->check_years_in_methods_for_removal_obs(years_, methods))
    LOG_ERROR_P(PARAM_YEARS) << "could not find catches in all years in the instantaneous_mortality process labeled " << process_label_
                             << ". Check that the years are compatible with this process";

  numbers_at_age_.resize(model_->age_spread(), 0.0);
  numbers_at_length_.resize(number_bins_, 0.0);
  expected_values_.resize(number_bins_, 0.0);
}

/**
 * This method is called at the start of the targeted
 * time step for this observation.
 *
 * Build the cache for the partition
 * structure to use with any interpolation
 */
void ProcessRemovalsByLengthRetained::PreExecute() {
  LOG_FINEST() << "Entering PREExecute " << label_;

  if (partition_->Size() != proportions_[model_->current_year()].size())
    LOG_CODE_ERROR() << "partition_->Size() != proportions_[model->current_year()].size()";
}

/**
 * Execute the ProcessRemovalsByLengthRetained expected values calculations
 */
void ProcessRemovalsByLengthRetained::Execute() {
  LOG_TRACE();
  /**
   * Verify our cached partition and partition sizes are correct
   */
  //  auto categories = model_->categories();
  unsigned year       = model_->current_year();
  auto                                                     partition_iter        = partition_->Begin();  // vector<vector<partition::Category> >
  map<unsigned, map<string, map<string, vector<Double>>>>& Removals_at_age       = mortality_instantaneous_retained_->retained_data();
  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each length using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    LOG_FINEST() << "category: " << category_labels_[category_offset];
    //    LOG_WARNING() << "This is bad code because it allocates memory in the middle of an execute";
    //    vector<Double> expected_values(number_bins_, 0.0);
    //    vector<Double> numbers_at_length;
    std::fill(expected_values_.begin(), expected_values_.end(), 0.0);

    /**
     * Loop through the 2 combined categories building up the
     * expected proportions values.
     */
    auto category_iter        = partition_iter->begin();
    for (; category_iter != partition_iter->end();  ++category_iter) {
      // clear these temporay vectors
      std::fill(numbers_at_age_.begin(), numbers_at_age_.end(), 0.0);
      std::fill(numbers_at_length_.begin(), numbers_at_length_.end(), 0.0);
      // get numbers at age for this category 
      for (unsigned data_offset = 0; data_offset < (*category_iter)->data_.size(); ++data_offset) {
        numbers_at_age_[data_offset] += Removals_at_age[year][method_][(*category_iter)->name_][data_offset];
      }
      // Now convert numbers at age to numbers at length using the categories age-length transition matrix
      if(using_model_length_bins) {
        (*category_iter)->age_length_->populate_numbers_at_length(numbers_at_age_, numbers_at_length_);
      } else {
        (*category_iter)->age_length_->populate_numbers_at_length(numbers_at_age_, numbers_at_length_, map_local_length_bins_to_global_length_bins_);
      }
      
      // Add this to the expected values
      LOG_FINE() << "----------";
      LOG_FINE() << "Category: " << (*category_iter)->name_;;

      for (unsigned j = 0; j < number_bins_; ++j) {
        expected_values_[j] += numbers_at_length_[j];
        LOG_FINE() << "expected_value for length = " << length_bins_[j] << " = " << expected_values_[j];
      }
    }


    if (expected_values_.size() != proportions_[model_->current_year()][category_labels_[category_offset]].size())
      LOG_CODE_ERROR() << "expected_values_.size(" << expected_values_.size() << ") != proportions_[category_offset].size("
                       << proportions_[model_->current_year()][category_labels_[category_offset]].size() << ")";

    /**
     * save our comparisons so we can use them to generate the score from the likelihoods later
     */
    for (unsigned i = 0; i < expected_values_.size(); ++i) {
      SaveComparison(category_labels_[category_offset], 0, length_bins_[i], expected_values_[i], proportions_[model_->current_year()][category_labels_[category_offset]][i],
                     process_errors_by_year_[model_->current_year()], error_values_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void ProcessRemovalsByLengthRetained::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
    for (auto& iter : comparisons_) {
      double total = 0.0;
      for (auto& comparison : iter.second) total += comparison.observed_;
      if (simulated_data_sum_to_one_) {
        for (auto& comparison : iter.second) 
          comparison.observed_ /= total;
      }    
    }
  } else {
    /**
     * Convert the expected_values in to a proportion
     */
    for (unsigned year : years_) {
      Double running_total = 0.0;
      for (obs::Comparison comparison : comparisons_[year]) {
        running_total += comparison.expected_;
      }
      for (obs::Comparison& comparison : comparisons_[year]) {
        if (running_total != 0.0)
          comparison.expected_ = comparison.expected_ / running_total;
        else
          comparison.expected_ = 0.0;
      }
    }
    likelihood_->GetScores(comparisons_);
    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      LOG_FINEST() << "-- Observation neglogLikelihood calculation";
      LOG_FINEST() << "[" << year << "] Initial neglogLikelihood:" << scores_[year];
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "] + neglogLikelihood: " << comparison.score_;
        scores_[year] += comparison.score_;
      }
    }

    LOG_FINEST() << "Finished calculating neglogLikelihood for = " << label_;
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
