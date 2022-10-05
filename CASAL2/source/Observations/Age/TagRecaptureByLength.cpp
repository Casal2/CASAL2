/**
 * @file TagRecaptureByLength.cpp
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 23/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "TagRecaptureByLength.h"

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 * Default constructor
 */
TagRecaptureByLength::TagRecaptureByLength(shared_ptr<Model> model) : Observation(model) {
  recaptures_table_ = new parameters::Table(PARAM_RECAPTURED);
  scanned_table_    = new parameters::Table(PARAM_SCANNED);

  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for which there are observations", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_label_, "The time step to execute in", "");
  parameters_.Bind<double>(PARAM_LENGTH_BINS, &length_bins_, "The length bins", "", true);  // optional
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &length_plus_, "Is the last length bin a plus group? (defaults to @model value)", "",
                         model->length_plus());  // default to the model value
  parameters_.Bind<string>(PARAM_TAGGED_CATEGORIES, &tagged_category_labels_, "The categories of tagged individuals for the observation", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The labels of the selectivities used for untagged categories", "", true);
  parameters_.Bind<string>(PARAM_TAGGED_SELECTIVITIES, &tagged_selectivity_labels_, "The labels of the tag category selectivities", "");
  // TODO:  is tolerance missing?
  parameters_.Bind<Double>(PARAM_PROCESS_ERRORS, &process_error_values_, "The process error", "", true);
  parameters_.Bind<Double>(PARAM_DETECTION_PARAMETER, &detection_, "The probability of detecting a recaptured individual", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_DISPERSION, &despersion_, "The overdispersion parameter (phi)  ", "", Double(1.0))->set_lower_bound(0.0);
  parameters_.BindTable(PARAM_RECAPTURED, recaptures_table_, "The table of observed recaptured individuals in each length bin", "", false);
  parameters_.BindTable(PARAM_SCANNED, scanned_table_, "The table of observed scanned individuals in each length bin", "", false);
  parameters_
      .Bind<Double>(PARAM_TIME_STEP_PROPORTION, &time_step_proportion_, "The proportion through the mortality block of the time step when the observation is evaluated", "",
                    Double(0.5))
      ->set_range(0.0, 1.0);
  // Don't ever make detection_ addressable or estimable. At line 427 it is multiplied to an observation which needs to remain a constant
  // if you make this estimatble we will break the auto-diff stack.
  mean_proportion_method_ = true;
  
  RegisterAsAddressable(PARAM_DETECTION_PARAMETER, &detection_);

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
  // Check value for initial mortality
  if (model_->length_bins().size() == 0)
    LOG_ERROR_P(PARAM_LABEL) << ": No length bins have been specified in @model. This observation requires those to be defined";

  if(length_plus_ & !model_->length_plus())
    LOG_ERROR_P(PARAM_LENGTH_PLUS) << "you have specified a plus group on this observation, but the global length bins don't have a plus group. This is an inconsistency that must be fixed. Try changing the model plus group to false or this plus group to true";

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
      LOG_FINE() << "using have actually just respecified model bins so we are ignoring bespoke length bin code";
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
        LOG_FINE() << "i = " << i << " " << map_local_length_bins_to_global_length_bins_[i];
      }
    }
  }
  // more checks on the model length bins.
  /*
  * TODO: this should be moved to the model to check rather than replicating in every child
  */
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


  // Check if number of categories is equal to number of selectivities for category and tagged_categories
  unsigned expected_selectivity_count = 0;
  auto     categories                 = model_->categories();
  for (const string& category_label : category_labels_) expected_selectivity_count += categories->GetNumberOfCategoriesDefined(category_label);

  for (auto year : years_) {
    if ((year < model_->start_year()) || (year > model_->final_year()))
      LOG_ERROR_P(PARAM_YEARS) << "Years cannot be less than start_year (" << model_->start_year() << "), or greater than final_year (" << model_->final_year() << ").";
  }

  if (tagged_category_labels_.size() != category_labels_.size())
    LOG_ERROR_P(PARAM_TAGGED_CATEGORIES)
        << "the number of tagged categories 'tagged_categories' should match the number of categories. Try using the '+' syntax for the category_label subcommand.";

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
  LOG_FINEST() << "Number of tagged categories after splitting " << split_tagged_category_labels.size();

  if (category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << ": Number of categories(" << category_labels_.size() << ") does not match the number of " PARAM_SELECTIVITIES << "("
                                  << selectivity_labels_.size() << ")";
  if (tagged_category_labels_.size() != tagged_selectivity_labels_.size())
    LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << ": Number of selectivities provided (" << tagged_selectivity_labels_.size()
                                         << ") is not valid. Specify either the number of category collections (" << tagged_category_labels_.size() << ") or "
                                         << "the number of total categories (" << expected_selectivity_count << ")";

  map<unsigned, vector<double>> recaptures_by_year;
  map<unsigned, vector<double>> scanned_by_year;

  if (detection_ < 0.0 || detection_ > 1.0) {
    LOG_ERROR_P(PARAM_DETECTION_PARAMETER) << ": detection probability must be between 0.0 and 1.0 inclusive";
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
    LOG_ERROR_P(PARAM_RECAPTURED) << " has " << recpatures_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& recaptures_data_line : recpatures_data) {
    unsigned year = 0;

    if (recaptures_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " has " << recaptures_data_line.size() << " values defined, but " << obs_expected
                                    << " should match the age spread * categories + 1 (for year)";
      return;
    }

    if (!utilities::To<unsigned>(recaptures_data_line[0], year)) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << recaptures_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
      return;
    }

    if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_RECAPTURED) << " value " << year << " is not a valid year for this observation";
      return;
    }

    for (unsigned i = 1; i < recaptures_data_line.size(); ++i) {
      double value = 0.0;
      if (!utilities::To<double>(recaptures_data_line[i], value))
        LOG_ERROR_P(PARAM_RECAPTURED) << " value (" << recaptures_data_line[i] << ") could not be converted to a Double";
      recaptures_by_year[year].push_back(value);
    }
    if (recaptures_by_year[year].size() != obs_expected - 1)
      LOG_FATAL_P(PARAM_RECAPTURED) << " " << recaptures_by_year[year].size() << " columns but " << obs_expected - 1 << " required. Check that the table is specified correctly";
  }

  /**
   * Build our scanned map
   */
  vector<vector<string>>& scanned_values_data = scanned_table_->data();
  if (scanned_values_data.size() != years_.size()) {
    LOG_ERROR_P(PARAM_SCANNED) << " has " << scanned_values_data.size() << " rows defined, but " << years_.size() << " should match the number of years provided";
  }

  for (vector<string>& scanned_values_data_line : scanned_values_data) {
    unsigned year = 0;

    if (scanned_values_data_line.size() != 2 && scanned_values_data_line.size() != obs_expected) {
      LOG_ERROR_P(PARAM_SCANNED) << " has " << scanned_values_data_line.size() << " values defined, but " << obs_expected
                                 << " should match the age spread * categories + 1 (for year)";
    } else if (!utilities::To<unsigned>(scanned_values_data_line[0], year)) {
      LOG_ERROR_P(PARAM_SCANNED) << " value " << scanned_values_data_line[0] << " could not be converted to an unsigned integer. It should be the year for this line";
    } else if (std::find(years_.begin(), years_.end(), year) == years_.end()) {
      LOG_ERROR_P(PARAM_SCANNED) << " value " << year << " is not a valid year for this observation";
    } else {
      for (unsigned i = 1; i < scanned_values_data_line.size(); ++i) {
        double value = 0.0;
        if (!utilities::To<double>(scanned_values_data_line[i], value)) {
          LOG_ERROR_P(PARAM_SCANNED) << " value (" << scanned_values_data_line[i] << ") could not be converted to a Double";
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
        LOG_FATAL_P(PARAM_SCANNED) << " " << scanned_by_year[year].size() << " columns but " << obs_expected - 1 << " required. Check that the table is specified correctly";
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
          LOG_ERROR_P(PARAM_OBS) << ": obs_ value (" << iter->second[obs_index] << ") at index " << obs_index + 1 << " in the definition could not be converted to a Double";
        }

        auto s_f = scanned_by_year.find(iter->first);
        if (s_f != scanned_by_year.end()) {
          auto error_value = s_f->second[obs_index];
          scanned_[iter->first][category_labels_[i]].push_back(error_value);
          recaptures_[iter->first][category_labels_[i]].push_back(value);
          total += error_value;
        }
      }
    }
  }

  // Split up categories if they are +
  vector<string> split_category_labels;
  category_split_labels_.resize(category_labels_.size());
  unsigned category_counter = 0;
  for (auto category : category_labels_) {
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
  for (auto category : tagged_category_labels_) {
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
      for (auto& split_category : split_category_labels) tagged_category_split_labels_[category_counter].push_back(split_category);
    } else
      tagged_category_split_labels_[category_counter].push_back(category);

    category_counter++;
  }
  // Do a check so that every Tagged category must be in the categories as well
  for (unsigned i = 0; i < tagged_category_split_labels_.size(); ++i) {
    for (unsigned j = 0; j < tagged_category_split_labels_[i].size(); ++j) {
      if (find(category_split_labels_[i].begin(), category_split_labels_[i].end(), tagged_category_split_labels_[i][j]) == category_split_labels_[i].end()) {
        LOG_ERROR_P(PARAM_TAGGED_CATEGORIES) << "The category " << tagged_category_split_labels_[i][j] << " is not in the " << PARAM_CATEGORIES << " at element " << i + 1
                                             << ". All tagged individuals should be in the categories";
      }
    }
  }
}

/**
 * Build any runtime relationships and ensure that the labels for other objects are valid.
 */
void TagRecaptureByLength::DoBuild() {
  partition_               = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, category_labels_));
  cached_partition_        = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, category_labels_));
  tagged_partition_        = CombinedCategoriesPtr(new niwa::partition::accessors::CombinedCategories(model_, tagged_category_labels_));
  tagged_cached_partition_ = CachedCombinedCategoriesPtr(new niwa::partition::accessors::cached::CombinedCategories(model_, tagged_category_labels_));

  auto partition_iter = partition_->Begin();  
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter) {
    auto category_iter        = partition_iter->begin();
    for (; category_iter != partition_iter->end();  ++category_iter)
      (*category_iter)->age_length_->BuildAgeLengthMatrixForTheseYears(years_);
  }

  partition_iter = tagged_partition_->Begin();  
  for (unsigned category_offset = 0; category_offset < tagged_category_labels_.size(); ++category_offset, ++partition_iter) {
    auto category_iter        = partition_iter->begin();
    for (; category_iter != partition_iter->end();  ++category_iter)
      (*category_iter)->age_length_->BuildAgeLengthMatrixForTheseYears(years_);
  }


  // Build Selectivity pointers
  for (string label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": Selectivity " << label << " does not exist.";
    selectivities_.push_back(selectivity);
  }

  if (selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_sel = selectivities_[0];
    selectivities_.assign(category_labels_.size(), val_sel);
  }

  for (string label : tagged_selectivity_labels_) {
    auto selectivity = model_->managers()->selectivity()->GetSelectivity(label);
    if (!selectivity) {
      LOG_ERROR_P(PARAM_TAGGED_SELECTIVITIES) << ": Selectivity " << label << " does not exist.";
    } else
      tagged_selectivities_.push_back(selectivity);
  }

  if (tagged_selectivities_.size() == 1 && category_labels_.size() != 1) {
    auto val_t = tagged_selectivities_[0];
    tagged_selectivities_.assign(category_labels_.size(), val_t);
  }

  if (time_step_proportion_ < 0.0 || time_step_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTION) << ": time_step_proportion (" << time_step_proportion_ << ") must be between 0.0 and 1.0 inclusive";
  proportion_of_time_ = time_step_proportion_;

  auto time_step = model_->managers()->time_step()->GetTimeStep(time_step_label_);
  if (!time_step) {
    LOG_ERROR_P(PARAM_TIME_STEP) << "Time step label " << time_step_label_ << " was not found.";
  } else {
    for (unsigned year : years_) time_step->SubscribeToBlock(this, year);
  }

  // reserve memory
  numbers_at_length_.resize(category_labels_.size());
  cached_numbers_at_length_.resize(category_labels_.size());
  tagged_numbers_at_length_.resize(tagged_category_labels_.size());
  tagged_cached_numbers_at_length_.resize(tagged_category_labels_.size());

  for(unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset) {
    numbers_at_length_[category_offset].resize(number_bins_, 0.0);
    cached_numbers_at_length_[category_offset].resize(number_bins_, 0.0);
    tagged_numbers_at_length_[category_offset].resize(number_bins_, 0.0);
    tagged_cached_numbers_at_length_[category_offset].resize(number_bins_, 0.0);
  }

  length_results_.resize(number_bins_, 0.0);
  tagged_length_results_.resize(number_bins_, 0.0);

}

/**
 * This method is called at the start of the tagged
 * time step for this observation.
 *
 * Build the cache for the partition
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
 * This method is called at the start of the tagged
 * time step for this observation.
 */
void TagRecaptureByLength::Execute() {
  LOG_TRACE();
  LOG_FINEST() << "Entering observation " << label_;

  /**
   * Verify our cached partition and partition sizes are correct
   */
  auto partition_iter               = partition_->Begin();  // vector<vector<partition::Category> >
  auto tagged_partition_iter        = tagged_partition_->Begin();  // vector<vector<partition::Category> >
  // Reset some comtainers

  /**
   * Loop through the provided categories. Each provided category (combination) will have a list of observations
   * with it. 
   * This is equal between the category_labels_ == tagged_category_labels_
   */
  for (unsigned category_offset = 0; category_offset < category_labels_.size(); ++category_offset, ++partition_iter,  ++tagged_partition_iter) {
    LOG_FINEST() << "Observing first collection of categories " << category_labels_[category_offset];
    Double start_value = 0.0;
    Double end_value   = 0.0;
    Double final_value = 0.0;
    // reset some category specific containers
    std::fill(numbers_at_length_[category_offset].begin(), numbers_at_length_[category_offset].end(), 0.0);
    std::fill(cached_numbers_at_length_[category_offset].begin(), cached_numbers_at_length_[category_offset].end(), 0.0);
    std::fill(tagged_cached_numbers_at_length_[category_offset].begin(), tagged_cached_numbers_at_length_[category_offset].end(), 0.0);
    std::fill(tagged_numbers_at_length_[category_offset].begin(), tagged_numbers_at_length_[category_offset].end(), 0.0);
    std::fill(length_results_.begin(), length_results_.end(), 0.0);
    std::fill(tagged_length_results_.begin(), tagged_length_results_.end(), 0.0);
   /**
     * Loop through the  combined categories if they are supplied, building up the
     * numbers at length
     */
    auto category_iter        = partition_iter->begin();
    for (; category_iter != partition_iter->end(); ++category_iter) {
      // get numbers at length
      if(using_model_length_bins) {
        (*category_iter)->age_length_->populate_numbers_at_length((*category_iter)->data_, numbers_at_length_[category_offset], selectivities_[category_offset]);
        (*category_iter)->age_length_->populate_numbers_at_length((*category_iter)->cached_data_, cached_numbers_at_length_[category_offset], selectivities_[category_offset]);
      } else {
        (*category_iter)->age_length_->populate_numbers_at_length((*category_iter)->data_, numbers_at_length_[category_offset], selectivities_[category_offset], map_local_length_bins_to_global_length_bins_);
        (*category_iter)->age_length_->populate_numbers_at_length((*category_iter)->cached_data_, cached_numbers_at_length_[category_offset], selectivities_[category_offset], map_local_length_bins_to_global_length_bins_);
      }
    }
    /**
     * Loop through the  combined categories if they are supplied, building up the
     * numbers at length
     */
    auto tagged_category_iter        = tagged_partition_iter->begin();
    for (; tagged_category_iter != tagged_partition_iter->end(); ++tagged_category_iter) {
      // get numbers at length
      if(using_model_length_bins) {
        (*tagged_category_iter)->age_length_->populate_numbers_at_length((*tagged_category_iter)->data_, tagged_numbers_at_length_[category_offset], tagged_selectivities_[category_offset]);
        (*tagged_category_iter)->age_length_->populate_numbers_at_length((*tagged_category_iter)->cached_data_, tagged_cached_numbers_at_length_[category_offset], tagged_selectivities_[category_offset]);
      } else {
        (*tagged_category_iter)->age_length_->populate_numbers_at_length((*tagged_category_iter)->data_, tagged_numbers_at_length_[category_offset], tagged_selectivities_[category_offset], map_local_length_bins_to_global_length_bins_);
        (*tagged_category_iter)->age_length_->populate_numbers_at_length((*tagged_category_iter)->cached_data_, tagged_cached_numbers_at_length_[category_offset], tagged_selectivities_[category_offset], map_local_length_bins_to_global_length_bins_);
      }
    }
    // Interpolate between the cached and current values for bothe tagged and untagged
    for (unsigned length_offset = 0; length_offset < number_bins_; ++length_offset) {
      start_value = tagged_cached_numbers_at_length_[category_offset][length_offset];
      end_value   = tagged_numbers_at_length_[category_offset][length_offset];
      final_value = 0.0;

      if (mean_proportion_method_)
        final_value = start_value + ((end_value - start_value) * proportion_of_time_);
      else
        final_value = (1 - proportion_of_time_) * start_value + proportion_of_time_ * end_value;

      tagged_length_results_[length_offset] += final_value;

      // Tagged
      start_value = cached_numbers_at_length_[category_offset][length_offset];
      end_value   = numbers_at_length_[category_offset][length_offset];
      final_value = 0.0;

      if (mean_proportion_method_)
        final_value = start_value + ((end_value - start_value) * proportion_of_time_);
      else
        final_value = (1 - proportion_of_time_) * start_value + proportion_of_time_ * end_value;
      length_results_[length_offset] += final_value;
    }

    if (length_results_.size() != scanned_[model_->current_year()][category_labels_[category_offset]].size()) {
      LOG_CODE_ERROR() << "expected_values.size(" << length_results_.size() << ") != proportions_[category_offset].size("
                       << scanned_[model_->current_year()][category_labels_[category_offset]].size() << ")";
    }

    // save our comparisons so we can use them to generate the score from the likelihoods later
    for (unsigned i = 0; i < length_results_.size(); ++i) {
      Double expected = 0.0;
      double observed = 0.0;
      if (length_results_[i] != 0.0) {
        //expected = detection_ * tagged_length_results_[i] / length_results_[i];
        expected = detection_ * tagged_length_results_[i] / length_results_[i];
        LOG_FINEST() << " total numbers at length " << length_bins_[i] << " = " << tagged_length_results_[i] << ", denominator = " << length_results_[i];
      }

      if (scanned_[model_->current_year()][category_labels_[category_offset]][i] == 0.0)
        observed = 0.0;
      else
        observed = (recaptures_[model_->current_year()][category_labels_[category_offset]][i]) / scanned_[model_->current_year()][category_labels_[category_offset]][i];

      SaveComparison(tagged_category_labels_[category_offset], 0, length_bins_[i], expected, observed, process_errors_by_year_[model_->current_year()],
                     scanned_[model_->current_year()][category_labels_[category_offset]][i], 0.0, delta_, 0.0);
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
  LOG_FINEST() << "Calculating neglogLikelihood for observation = " << label_;

  if (model_->run_mode() == RunMode::kSimulation) {
    likelihood_->SimulateObserved(comparisons_);
  } else {
    likelihood_->GetScores(comparisons_);
    for (unsigned year : years_) {
      scores_[year] = likelihood_->GetInitialScore(comparisons_, year);
      LOG_FINEST() << "-- Observation neglogLikelihood calculation " << label_;
      LOG_FINEST() << "[" << year << "] Initial neglogLikelihood:" << scores_[year];
      for (obs::Comparison comparison : comparisons_[year]) {
        LOG_FINEST() << "[" << year << "] + neglogLikelihood: " << comparison.score_;
        scores_[year] += comparison.score_;
      }

      // Add the dispersion factor to the likelihood score
      scores_[year] /= despersion_;
    }

    LOG_FINEST() << "Finished calculating neglogLikelihood for = " << label_;
  }
}

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
