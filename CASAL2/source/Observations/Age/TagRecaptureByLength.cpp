/**
 * @file TagRecaptureByLength.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 23/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "TagRecaptureByLength.h"

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
TagRecaptureByLength::TagRecaptureByLength(Model* model) : Observation(model) {
  recaptures_table_ = new parameters::Table(PARAM_RECAPTURED);
  scanned_table_ = new parameters::Table(PARAM_SCANNED);

  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years for which there are observations", "");
  parameters_.Bind<unsigned>(PARAM_LENGTH_BINS, &length_bins_input_, "Length bins", "", true); // optional
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "Time step to execute in", "");
  parameters_.Bind<bool>(PARAM_LENGTH_PLUS, &length_plus_, "Is the last bin a plus group", "", model->length_plus()); // default to the model value
  parameters_.Bind<string>(PARAM_TAGGED_CATEGORIES, &tagged_category_labels_, "The categories of tagged individuals for the observation", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities used for untagged categories", "", true);
  parameters_.Bind<string>(PARAM_TAGGED_SELECTIVITIES, &tagged_selectivity_labels_, "The labels of the tag category selectivities", "");
  // TODO:  is tolerance missing?
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "Process error", "", true);
  parameters_.Bind<double>(PARAM_DETECTION_PARAMETER,  &detection_, "Probability of detecting a recaptured individual", "");
  parameters_.Bind<double>(PARAM_DISPERSION,  &despersion_, "Over-dispersion parameter (phi)  ", "", double(1.0));
  parameters_.BindTable(PARAM_RECAPTURED, recaptures_table_, "Table of observed recaptured individuals in each length bin", "", false);
  parameters_.BindTable(PARAM_SCANNED, scanned_table_, "Table of observed scanned individuals in each length bin", "", false);
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "Proportion through the mortality block of the time step when the observation is evaluated", "", double(0.5));

  mean_proportion_method_ = true;

  allowed_likelihood_types_.push_back(PARAM_BINOMIAL);
}

/**
 * Validate configuration file parameters
 */
void TagRecaptureByLength::DoValidate() {
  /**
   * Do some simple checks
   * e.g Validate that the length_bins are strictly increasing
   */
  if (parameters_.Get(PARAM_LENGTH_BINS)->has_been_defined()) {
    length_bins_.resize(length_bins_input_.size(), 0.0);
    for (unsigned length = 0; length < length_bins_input_.size(); ++length) {
      if (length_bins_input_[length] < 0.0)
        LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bins must be positive " << length_bins_input_[length] << " is less than 0";

      if (length > 0 && length_bins_input_[length - 1] >= length_bins_input_[length])
        LOG_ERROR_P(PARAM_LENGTH_BINS) << ": Length bins must be strictly increasing " << length_bins_input_[length - 1] << " is greater than " << length_bins_input_[length];

      // Cast to a double
      length_bins_[length] = double(length_bins_input_[length]);
    }

    // Finally Check the bins are not the same as in the @model, if user accidently set them to the be the same as the  @model
    // we can ignore there input for performance benefits.
    vector<unsigned> model_length_bins = model_->length_bins();
    if (length_bins_input_.size() == model_length_bins.size()) {
      for(unsigned i = 0; i < model_length_bins.size(); ++i) {
        if (model_length_bins[i] != length_bins_input_[i])
          use_model_length_bins_ = false;
      }
    }


  } else {
    // set to model if not defined.
    length_bins_input_ = model_->length_bins();
    length_bins_.resize(length_bins_input_.size(), 0.0);
    for (unsigned length = 0; length < length_bins_input_.size(); ++length) {
      length_bins_[length] = double(length_bins_input_[length]);
      LOG_FINE() << "length bin " << length + 1 << " " << length_bins_input_[length] << " after static " << length_bins_[length];
    }
  }



  number_bins_ = length_plus_ ? length_bins_.size() : length_bins_.size() - 1;

  LOG_FINE() << "Are we using model length bins = " << use_model_length_bins_ << " (1 = yes). n bins = " << number_bins_;

  // reserve memory that we will be using during execute
  age_length_matrix_.resize(model_->age_spread());
  cached_age_length_matrix_.resize(model_->age_spread());
  for(unsigned i = 0; i < age_length_matrix_.size(); ++i) {
    age_length_matrix_[i].resize(number_bins_, 0.0);
    cached_age_length_matrix_[i].resize(number_bins_, 0.0);
  }

  numbers_at_length_.resize(number_bins_, 0.0);
  cached_numbers_at_length_.resize(number_bins_, 0.0);
  tagged_numbers_at_length_.resize(number_bins_, 0.0);
  tagged_cached_numbers_at_length_.resize(number_bins_, 0.0);
  length_results_.resize(number_bins_, 0.0);
  tagged_length_results_.resize(number_bins_, 0.0);

  // Check if number of categories is equal to number of selectivities for category and tagged_categories
  unsigned expected_selectivity_count = 0;
  auto categories = model_->categories();
  for (const string& category_label : category_labels_)
    expected_selectivity_count += categories->GetNumberOfCategoriesDefined(category_label);

  // Expand out short hand syntax
  tagged_category_labels_ = model_->categories()->ExpandLabels(tagged_category_labels_, parameters_.Get(PARAM_TAGGED_CATEGORIES)->location());
  for (auto year : years_) {
  	if((year < model_->start_year()) || (year > model_->final_year()))
  		LOG_ERROR_P(PARAM_YEARS) << "Years can't be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << "). Please fix this.";
  }

  if (tagged_category_labels_.size() != category_labels_.size())
    LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << "we expect you to supply the same number of tagged categories 'tagged_categories' as categories. Try using the '+' syntax for the category_label subcommand.";

  /**
   * Now go through each tagged category and split it if required, then check each piece to ensure
   * it's a valid category
   */
  LOG_FINEST() << "Number of tagged categories before splitting " << tagged_category_labels_.size();
  vector<string> split_tagged_category_labels;
  for (const string& category_label : tagged_category_labels_) {
    boost::split(split_tagged_category_labels, category_label, boost::is_any_of("+"));

    for (const string& split_category_label : split_tagged_category_labels) {
      if (!categories->IsValid(split_category_label)) {
        if (split_category_label == category_label) {
          LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
        } else {
          LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
              << " It was defined in the category collection " << category_label;
        }
      }
    }
  }
  LOG_FINEST() << "Number of tagged categories after splitting " << tagged_category_labels_.size();

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << ": Number of categories(" << category_labels_.size() << ") does not match the number of "
    PARAM_SELECTIVITIES << "(" << selectivity_labels_.size() << ")";
  if (tagged_category_labels_.size() != tagged_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << ": Number of selectivities provided (" << tagged_selectivity_labels_.size()
    << ") is not valid. You can specify either the number of category collections (" << tagged_category_labels_.size() << ") or "
    << "the number of total categories (" << expected_selectivity_count << ")";

  map<unsigned, vector<double>> recaptures_by_year;
  map<unsigned, vector<double>> scanned_by_year;

  if (detection_ < 0.0 || detection_ > 1.0) {
    LOG_ERROR_P(PARAM_DETECTION_PARAMETER) << ": detection probability must be between 0 and 1";
  }
  if (delta_ < 0.0) {
    LOG_ERROR_P(PARAM_DELTA) << ": delta (" << delta_ << ") cannot be less than 0.0";
  }


  /**
   * Validate the number of recaptures provided matches age spread * category_labels * years
   * This is because we'll have 1 set of recaptures per category collection provided.
   * categories male+female male = 2 collections
   */
  unsigned obs_expected = number_bins_ * tagged_category_labels_.size() + 1;
  LOG_FINE() << "expected obs = " << obs_expected << " number of bins = " << number_bins_ << " tagged categories = " << tagged_category_labels_.size();
  vector<vector<string>>& recpatures_data = recaptures_table_->data();
  if (recpatures_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_RECAPTURED) << " has " << recpatures_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& recaptures_data_line : recpatures_data) {
    unsigned year = 0;

    if (recaptures_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " has " << recaptures_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the age_spread * categories + 1 (for year)";
      return;
    }

    if (!utilities::To<unsigned>(recaptures_data_line[0], year)) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << recaptures_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
      return;
    }

    if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << year << " is not a valid year for this observation";
      return;
    }

    for (unsigned i = 1; i < recaptures_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(recaptures_data_line[i], value))
        LOG_ERROR_P(PARAM_RECAPTURED) << " value (" << recaptures_data_line[i] << ") could not be converted to a double";
      recaptures_by_year[year].push_back(value);
    }
    if (recaptures_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_RECAPTURED) << "We received " << recaptures_by_year[year].size() << " columns but expected " << obs_expected - 1 << ", please check you have specified the table correctly";

  }


  /**
   * Build our scanned map
   */
  vector<vector<string>>& scanned_values_data = scanned_table_->data();
  if (scanned_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_SCANNED) << " has " << scanned_values_data.size() << " rows defined, but we expected " << years_.size()
        << " to match the number of years provided";
  }

  for (vector<string>& scanned_values_data_line : scanned_values_data) {
    unsigned year = 0;

    if (scanned_values_data_line.size() != 2 && scanned_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_SCANNED) << " has " << scanned_values_data_line.size() << " values defined, but we expected " << obs_expected
          << " to match the age speard * categories + 1 (for year)";
    } else if (!utilities::To<unsigned>(scanned_values_data_line[0], year)) {
      LOG_ERROR_P(PARAM_SCANNED) << " value " << scanned_values_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
    } else if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_SCANNED) << " value " << year << " is not a valid year for this observation";
    } else {
        for (unsigned i = 1; i < scanned_values_data_line.size(); ++i) {
          double value = 0.0;
        if (!utilities::To<double>(scanned_values_data_line[i], value)) {
          LOG_ERROR_P(PARAM_SCANNED) << " value (" << scanned_values_data_line[i] << ") could not be converted to a double";
        } else if (likelihood_type_ == PARAM_MULTINOMIAL && value < 0.0) {
            LOG_ERROR_P(PARAM_ERROR_VALUES) << ": error_value (" << value << ") cannot be less than 0.0";
        }

        scanned_by_year[year].push_back(value);
      }
      if (scanned_by_year[year].size() == 1) {
        auto val_s = scanned_by_year[year][0];
        scanned_by_year[year].assign(obs_expected - 1, val_s);
      }
      if (scanned_by_year[year].size() != obs_expected - 1) {
        LOG_FATAL_P(PARAM_SCANNED) << "We recieved " << scanned_by_year[year].size() << " columns but expected " << obs_expected - 1 << ", please check you have specified the table correctly";
      }
    }
  }

  /**
   * Build our Recaptured and scanned maps for use in the DoExecute() section
   */
  double value = 0.0;
  for (auto iter = recaptures_by_year.begin(); iter != recaptures_by_year.end(); ++iter) {
    double total = 0.0;

    for (unsigned i = 0; i < category_labels_.size(); ++i) {
      for (unsigned j = 0; j < number_bins_; ++j) {
        unsigned obs_index = i * number_bins_ + j;
        if (!utilities::To<double>(iter->second[obs_index], value)) {
          LOG_ERROR_P(PARAM_OBS) << ": obs_ value (" << iter->second[obs_index] << ") at index " << obs_index + 1
              << " in the definition could not be converted to a numeric double";
        }

        double error_value = scanned_by_year[iter->first][obs_index];
        scanned_[iter->first][category_labels_[i]].push_back(error_value);
        recaptures_[iter->first][category_labels_[i]].push_back(value);
        total += error_value;
      }
    }
  }

  // Split up categories if they are +
  vector<string> split_category_labels;
  category_split_labels_.resize(category_labels_.size());
  unsigned category_counter = 0;
  for(auto category : category_labels_) {
    if (model_->categories()->IsCombinedLabels(category)) {
      boost::split(split_category_labels, category, boost::is_any_of("+"));
      for (const string& split_category_label : split_category_labels) {
        if (!model_->categories()->IsValid(split_category_label)) {
          if (split_category_label == category) {
            LOG_ERROR_P(PARAM_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
          } else {
            LOG_ERROR_P(PARAM_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
                << " It was defined in the category collection " << category;
          }
        }
      }
      for (auto& split_category : split_category_labels)
        category_split_labels_[category_counter].push_back(split_category);
    } else
      category_split_labels_[category_counter].push_back(category);
    category_counter++;
  }

  category_counter = 0;
  split_category_labels.clear();
  tagged_category_split_labels_.resize(tagged_category_labels_.size());
  for(auto category : tagged_category_labels_) {
    if (model_->categories()->IsCombinedLabels(category)) {
      boost::split(split_category_labels, category, boost::is_any_of("+"));
      for (const string& split_category_label : split_category_labels) {
        if (!model_->categories()->IsValid(split_category_label)) {
          if (split_category_label == category) {
            LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << ": The category " << split_category_label << " is not a valid category.";
          } else {
            LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << ": The category " << split_category_label << " is not a valid category."
                << " It was defined in the category collection " << category;
          }
        }
      }
      for (auto& split_category : split_category_labels)
        tagged_category_split_labels_[category_counter].push_back(split_category);
    } else
      tagged_category_split_labels_[category_counter].push_back(category);

    category_counter++;
  }
  // Do a check so that every Tagged category must be in the categories as well
  for (unsigned i = 0; i < tagged_category_split_labels_.size(); ++i) {
    for (unsigned j = 0; j < tagged_category_split_labels_[i].size(); ++j) {
      if (find(category_split_labels_[i].begin(), category_split_labels_[i].end(), tagged_category_split_labels_[i][j]) == category_split_labels_[i].end()) {
        LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << "The category " << tagged_category_split_labels_[i][j] << " is not in the " << PARAM_CATEGORIES << " at element " << i + 1 << ". The code assumes that all tagged fish are also in the categories";
      }
    }
  }


}

/**
 * Build any runtime relationships we may have and ensure
 * the labels for other objects are valid.
 */
void TagRecaptureByLength::DoBuild() {
  partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  tagged_partition_ = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, tagged_category_labels_));
  tagged_cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, tagged_category_labels_));



  // Build Selectivity pointers
  for(string label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity " << label << " does not exist. Have you defined it?";
    selectivities_.push_back(selectivity);
  }

  if (selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_sel = selectivities_[0];
    selectivities_.assign(category_labels_.size(), val_sel);
  }

  for(string label : tagged_selectivity_labels_) {
    auto selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity) {
      LOG_ERROR_P(PARAM_TAGGED_SELECTIVITIES) << ": " << label << " does not exist. Have you defined it?";
    } else
        tagged_selectivities_.push_back(selectivity);
  }

  if (tagged_selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_t = tagged_selectivities_[0];
    tagged_selectivities_.assign(category_labels_.size(), val_t);
  }

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << time_step_proportion_ << ") must be between 0.0 and 1.0";
  proportion_of_time_ = time_step_proportion_;

  auto time_step = model_->managers().time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_ERROR_P(PARAM_TIME_STEP) << " " << time_step_label_ << " could not be found. Have you defined it?";
  } else {
    for (unsigned year : years_)
      time_step->SubscribeToBlock(this, year);
  }
}

/**
 * This method is called at the start of the taggedted
 * time step for this observation.
 *
 * At this point we need to build our cache for the partition
 * structure to use with any interpolation
 */
void TagRecaptureByLength::PreExecute() {
  cached_partition_->BuildCache();
  tagged_cached_partition_->BuildCache();

  if (cached_partition_->Size() != scanned_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "cached_partition_->Size() != scanned_[model->current_year()].size()";
  }
  if (partition_->Size() != scanned_[model_->current_year()].size()) {
    LOG_CODE_ERROR() << "partition_->Size() != scanned_[model->current_year()].size()";
  }
}

/**
 *
 */
void TagRecaptureByLength::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto cached_partition_iter  = cached_partition_->Begin();
  auto partition_iter         = partition_->Begin(); // vector<vector<partition::Category> >
  auto tagged_cached_partition_iter  = tagged_cached_partition_->Begin();
  auto tagged_partition_iter         = tagged_partition_->Begin(); // vector<vector<partition::Category> >
  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. We need to build a vector of proportions for each age using that combination and then
   * compare it to the observations.
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter, ++cached_partition_iter) {
    LOG_FINEST() << "Observing first collection of categories";
    Double      start_value        = 0.0;
    Double      end_value          = 0.0;
    Double      final_value        = 0.0;
    // Reset some comtainers
    std::fill(length_results_.begin(), length_results_.end(), 0.0);
    std::fill(tagged_length_results_.begin(), tagged_length_results_.end(), 0.0);
    /**
     * Loop through the tagged combined categories building up the
     * tagged age results
     */
    auto tagged_category_iter = tagged_partition_iter->begin();
    auto tagged_cached_category_iter = tagged_cached_partition_iter->begin();
    for (; tagged_category_iter != tagged_partition_iter->end(); ++tagged_cached_category_iter, ++tagged_category_iter) {
      //if (find((*tagged_category_iter)->name_, )
      LOG_FINEST() << "Selectivity for " << tagged_category_labels_[category_offset] << " " << tagged_selectivities_[category_offset]->label();
      if (use_model_length_bins_) {
        tagged_cached_category_iter->PopulateAgeLengthMatrix(selectivities_[category_offset]);
        (*tagged_category_iter)->PopulateAgeLengthMatrix(selectivities_[category_offset]);

        (*tagged_cached_category_iter).CollapseAgeLengthDataToLength();
        (*tagged_category_iter)->CollapseAgeLengthDataToLength();

  //      if ((*tagged_category_iter)->length_data_.size() != tagged_length_results_.size())
  //              LOG_CODE_ERROR() << "(*category_iter)->length_data_.size() !- length_results_.size()";

        for (unsigned length_offset = 0; length_offset < tagged_length_results_.size(); ++length_offset) {
         // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
          start_value = (*tagged_cached_category_iter).length_data_[length_offset];
          end_value = (*tagged_category_iter)->length_data_[length_offset];
          final_value   = 0.0;

          if (mean_proportion_method_)
            final_value = start_value + ((end_value - start_value) * proportion_of_time_);
          else
            final_value = (1-proportion_of_time_) * start_value + proportion_of_time_ * end_value;

          tagged_length_results_[length_offset] += final_value;

          LOG_FINE() << "----------";
          LOG_FINE() << "Total categories: " << (*tagged_category_iter)->name_ << " at length " << length_bins_[length_offset];
          LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
          LOG_FINE() << "expected_value becomes: " << tagged_length_results_[length_offset];
        }
      } else {
        // Reset these containers
        std::fill(numbers_at_length_.begin(), numbers_at_length_.end(), 0.0);
        std::fill(cached_numbers_at_length_.begin(), cached_numbers_at_length_.end(), 0.0);

        tagged_cached_category_iter->CalculateNumbersAtLength(selectivities_[category_offset], length_bins_, age_length_matrix_, numbers_at_length_, length_plus_);
        (*tagged_category_iter)->CalculateNumbersAtLength(selectivities_[category_offset], length_bins_, cached_age_length_matrix_, cached_numbers_at_length_, length_plus_);
        for (unsigned length_offset = 0; length_offset < numbers_at_length_.size(); ++length_offset) {
         // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
          start_value = cached_numbers_at_length_[length_offset];
          end_value = numbers_at_length_[length_offset];
          final_value   = 0.0;

          if (mean_proportion_method_)
            final_value = start_value + ((end_value - start_value) * proportion_of_time_);
          else
            final_value = (1 - proportion_of_time_) * start_value + proportion_of_time_ * end_value;

          tagged_length_results_[length_offset] += final_value;

          LOG_FINE() << "----------";
          LOG_FINE() << "Category: " << (*tagged_category_iter)->name_ << " at length " << length_bins_[length_offset];
          LOG_FINE() << "Selectivity: " << selectivities_[category_offset]->label();
          LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
          LOG_FINE() << "expected_value becomes: " << tagged_length_results_[length_offset];
        }
      }
    }
    /**
     * Loop through the  combined categories if they are supplied, building up the
     * numbers at length
     */
    auto category_iter = partition_iter->begin();
    auto cached_category_iter = cached_partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++cached_category_iter, ++category_iter) {
      // Update/Populate Numbers At Length container for each category
      LOG_FINEST() << "Populating age length matrix for: " << cached_category_iter->name_;
      if (find(tagged_category_split_labels_[category_offset].begin(), tagged_category_split_labels_[category_offset].end(), (*category_iter)->name_) != tagged_category_split_labels_[category_offset].end()) {
        LOG_FINE() << "we have already done this calculation in the loop above so skip this, category = " << (*category_iter)->name_;
        continue;
      }
      if (use_model_length_bins_) {
        cached_category_iter->PopulateAgeLengthMatrix(selectivities_[category_offset]);
        (*category_iter)->PopulateAgeLengthMatrix(selectivities_[category_offset]);
        (*cached_category_iter).CollapseAgeLengthDataToLength();
        (*category_iter)->CollapseAgeLengthDataToLength();
         for (unsigned length_offset = 0; length_offset < length_results_.size(); ++length_offset) {
          // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
           start_value = (*cached_category_iter).length_data_[length_offset];
           end_value = (*category_iter)->length_data_[length_offset];
           final_value   = 0.0;

           if (mean_proportion_method_)
             final_value = start_value + ((end_value - start_value) * proportion_of_time_);
           else
             final_value = (1-proportion_of_time_) * start_value + proportion_of_time_ * end_value;

           length_results_[length_offset] += final_value;

           LOG_FINE() << "----------";
           LOG_FINE() << "Category: " << (*category_iter)->name_ << " at length " << length_bins_[length_offset];
           LOG_FINE() << "Selectivity: " << selectivities_[category_offset]->label();
           LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
           LOG_FINE() << "expected_value becomes: " << length_results_[length_offset];
         }
      } else {
        cached_category_iter->CalculateNumbersAtLength(selectivities_[category_offset], length_bins_, age_length_matrix_, numbers_at_length_, length_plus_);
        (*category_iter)->CalculateNumbersAtLength(selectivities_[category_offset], length_bins_, cached_age_length_matrix_, cached_numbers_at_length_, length_plus_);
        for (unsigned length_offset = 0; length_offset < numbers_at_length_.size(); ++length_offset) {
         // now for each column (length bin) in age_length_matrix sum up all the rows (ages) for both cached and current matricies
          start_value = cached_numbers_at_length_[length_offset];
          end_value = numbers_at_length_[length_offset];
          final_value   = 0.0;

          if (mean_proportion_method_)
            final_value = start_value + ((end_value - start_value) * proportion_of_time_);
          else
            final_value = (1 - proportion_of_time_) * start_value + proportion_of_time_ * end_value;

          length_results_[length_offset] += final_value;

          LOG_FINE() << "----------";
          LOG_FINE() << "Category: " << (*category_iter)->name_ << " at length " << length_bins_[length_offset];
          LOG_FINE() << "Selectivity: " << selectivities_[category_offset]->label();
          LOG_FINE() << "start_value: " << start_value << "; end_value: " << end_value << "; final_value: " << final_value;
          LOG_FINE() << "expected_value becomes: " << length_results_[length_offset];
        }
      }
    }

    if (length_results_.size() != scanned_[model_->current_year()][category_labels_[category_offset]].size()) {
      LOG_CODE_ERROR() << "expected_values.size(" << length_results_.size() << ") != proportions_[category_offset].size("
        << scanned_[model_->current_year()][category_labels_[category_offset]].size() << ")";
    }
    //save our comparisons so we can use them to generate the score from the likelihoods later
    for (unsigned i = 0; i < length_results_.size(); ++i) {
      Double expected = 0.0;
      double observed = 0.0;
      if (length_results_[i] != 0.0) {
        expected = detection_ * tagged_length_results_[i]  / (length_results_[i]+  tagged_length_results_[i]);
        LOG_FINEST() << " total numbers at length " << length_bins_[i] << " = " << tagged_length_results_[i] << ", denominator = " << length_results_[i] << " + " << tagged_length_results_[i] ;
      }
      if (scanned_[model_->current_year()][category_labels_[category_offset]][i] == 0.0)
        observed = 0.0;
      else
        observed = (recaptures_[model_->current_year()][category_labels_[category_offset]][i]) / scanned_[model_->current_year()][category_labels_[category_offset]][i];                                                                                                                                                                    ;
      SaveComparison(tagged_category_labels_[category_offset], 0, length_bins_[i], expected, observed,
          process_errors_by_year_[model_->current_year()], scanned_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
    }
  }
}

/**
 * This method is called at the end of a model iteration
 * to calculate the score for the observation.
 */
void TagRecaptureByLength::CalculateScore() {
  /**
   * Simulate or generate results
   * During simulation mode we'll simulate results for this observation
   */
	LOG_FINEST() << "Calculating score for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {

    likelihood_->GetScores(comparisons_);
    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      for (obs::Comparison comparison : comparisons_[year]) {
        scores_[year] += comparison.score_;
      }
      // Add the dispersion factor to the likelihood score
      scores_[year] /= despersion_;
    }
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
