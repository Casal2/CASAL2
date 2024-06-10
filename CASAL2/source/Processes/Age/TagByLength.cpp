/**
 * @file TagByLength.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/06/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "TagByLength.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "AgeLengths/Manager.h"
#include "Categories/Categories.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default constructor
 */
TagByLength::TagByLength(shared_ptr<Model> model) : Process(model), to_partition_(model), from_partition_(model) {
  process_type_ = ProcessType::kMortality;
  // Why this was changed from type transition to mortality. CASAL includes this in the 'mortality block'
  // CASAL reference see population_section.cpp line: 1924-2006
  // There is mortality in this process, so does make sense
  partition_structure_ = PartitionType::kAge;
  numbers_table_       = new parameters::Table(PARAM_NUMBERS);
  proportions_table_   = new parameters::Table(PARAM_PROPORTIONS);
  // clang-format off
  parameters_.Bind<string>(PARAM_FROM, &from_category_labels_, "The categories that are selected for tagging (i.e., transition from)", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_labels_, "The categories that are being tagged (i.e., transition to)", "");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "The penalty label", "", "");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "The maximum exploitation rate, U_max", "", 0.99)->set_range(0.0, 1.0, true, false);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years to execute the tagging events in", "");
  parameters_.Bind<Double>(PARAM_INITIAL_MORTALITY, &initial_mortality_, "The initial mortality to apply to tags as a proportion", "", 0.0)->set_range(0.0, 1.0, true, true);
  parameters_.Bind<string>(PARAM_INITIAL_MORTALITY_SELECTIVITY, &initial_mortality_selectivity_label_, "The initial mortality selectivity label", "", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "Selectivity of the tagging process", "");
  parameters_.Bind<Double>(PARAM_N, &n_, "The number of individuals tagged", "", true);
  parameters_.BindTable(PARAM_NUMBERS, numbers_table_, "The data table of numbers to tag", "", false, true);
  parameters_.BindTable(PARAM_PROPORTIONS, proportions_table_, "The data table of proportions to tag", "", false, true);
  parameters_.Bind<double>(PARAM_TOLERANCE, &tolerance_, "Tolerance for checking the specificed proportions sum to one", "", 1e-5)->set_range(0, 1.0);
  parameters_.Bind<string>(PARAM_COMPATIBILITY, &compatibility_, "Backwards compatibility option: either casal2 (the default) or casal: effects penalty and age-length calculation", "",PARAM_CASAL2)->set_allowed_values({PARAM_CASAL, PARAM_CASAL2});
  // clang-format on
}

/**
 * Destructor
 */
TagByLength::~TagByLength() {
  delete numbers_table_;
  delete proportions_table_;
}

/**
 * Validate the parameters from the configuration file
 */
void TagByLength::DoValidate() {
  LOG_TRACE();
  min_age_    = model_->min_age();
  max_age_    = model_->max_age();
  age_spread_ = model_->age_spread();

  if (tolerance_ > 0.01) {
    LOG_WARNING_P(PARAM_TOLERANCE) << "Your tolerance is larger than " << 0.01
                                   << " this mean Casal2 may tag less fish than you think it should. It is recommended that you pick a smaller number.";
  }
  // Check length bins
  if (model_->length_bins().size() == 0)
    LOG_FATAL_P(PARAM_TYPE) << ": No length bins have been specified in @model. This process requires those to be defined, as the table dimensions depend on them.";

  // validate tables
  if (numbers_table_->row_count() != 0 && proportions_table_->row_count() != 0)
    LOG_ERROR() << location() << " cannot have both a " << PARAM_NUMBERS << " and " << PARAM_PROPORTIONS << " table defined. Please use one only.";
  if (numbers_table_->row_count() == 0) {
    if (proportions_table_->row_count() == 0) {
      LOG_ERROR() << location() << " must have either a " << PARAM_NUMBERS << " or " << PARAM_PROPORTIONS << " table defined with appropriate data";
    } else {
      if (!parameters_.Get(PARAM_N)->has_been_defined())
        LOG_ERROR() << location() << " cannot have a " << PARAM_PROPORTIONS << " table without defining " << PARAM_N;
      if (n_.size() != years_.size())
        LOG_FATAL_P(PARAM_N) << "The number of values provided (" << n_.size() << ") does not match the number of years (" << years_.size() << ")";
    }
  } else {
    if (parameters_.Get(PARAM_N)->has_been_defined())
      LOG_FATAL_P(PARAM_N) << "If a table of numbers (using subcommand table numbers) is provided, then the subcommand N should be omitted";
  }

  // Check if the user has specified combined categories, if so check the same number of categories are
  for (auto& category : to_category_labels_) {
    bool check_combined = model_->categories()->IsCombinedLabels(category);
    if (check_combined)
      LOG_FATAL_P(PARAM_TO) << "The combined category " << category << " was supplied. This subcommand can take separate categories only.";
  }
  if (from_category_labels_.size() != 1) {
    LOG_FATAL_P(PARAM_FROM) << "This process cannot specify a many-to-many tagging event. If proportions are tagged by category then create a @tag process."
                            << " 'From' category labels size " << from_category_labels_.size();
  }
  n_length_bins_ = model_->get_number_of_length_bins();

  vector<string> split_category_labels;
  unsigned       no_combined = 0;

  for (auto category : from_category_labels_) {
    if (model_->categories()->IsCombinedLabels(category)) {
      no_combined = model_->categories()->GetNumberOfCategoriesDefined(category);
      if (no_combined != to_category_labels_.size()) {
        LOG_ERROR_P(PARAM_TO) << "'" << no_combined << "' combined 'From' categories was specified, but '" << to_category_labels_.size()
                              << "' 'To' categories were supplied. The number of 'From' and 'To' categories must be the same.";
      }
      boost::split(split_category_labels, category, boost::is_any_of("+"));
      for (const string& split_category_label : split_category_labels) {
        if (!model_->categories()->IsValid(split_category_label)) {
          if (split_category_label == category) {
            LOG_ERROR_P(PARAM_FROM) << ": The category " << split_category_label << " is not a valid category.";
          } else {
            LOG_ERROR_P(PARAM_FROM) << ": The category " << split_category_label << " is not a valid category."
                                    << " It was defined in the category collection " << category;
          }
        }
      }
      for (auto& split_category : split_category_labels) split_from_category_labels_.push_back(split_category);
    } else
      split_from_category_labels_.push_back(category);
  }

  LOG_FINEST() << "from categories";
  for (auto category : split_from_category_labels_) {
    LOG_FINEST() << category;
  }

  if (split_from_category_labels_.size() != to_category_labels_.size()) {
    LOG_ERROR_P(PARAM_TO) << " the number of values supplied (" << to_category_labels_.size() << ") does not match the number of from categories provided ("
                          << split_from_category_labels_.size() << ")";
  }

  if (to_category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << "the number of selectivities must match the number of 'to_categories'. " << to_category_labels_.size()
                                     << " 'to_categories' were supplied, but " << selectivity_labels_.size() << " selectivity labels were supplied";

  // Get our first year
  first_year_ = years_[0];
  std::for_each(years_.begin(), years_.end(), [this](unsigned year) { first_year_ = year < first_year_ ? year : first_year_; });

  // Load our N data in to the map
  if (numbers_table_->row_count() != 0) {
    if (numbers_table_->row_count() != years_.size())
      LOG_FATAL_P(PARAM_NUMBERS) << "do not match the number of years. The number of rows supplied was " << numbers_table_->row_count() << " and the number of years supplied was "
                                 << years_.size() << ". These need to be the same";

    n_by_year_ = utilities::Map::create(years_, 0.0);

    // load our table data in to our map
    vector<vector<string>> data    = numbers_table_->data();
    unsigned               year    = 0;
    Double                 n_value = 0.0;

    for (auto iter : data) {
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR_P(PARAM_NUMBERS) << "value (" << iter[0] << ") could not be converted to an unsigned integer";
      if ((iter.size() - 1) != n_length_bins_) {
        LOG_FATAL_P(PARAM_NUMBERS) << "For row in year = " << year
                                   << ". The length bins for this process are defined in the @model block. A column is required for each length bin which is '" << n_length_bins_
                                   << "'. This table supplied " << iter.size() - 1 << " values.";
      }

      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], n_value))
          LOG_ERROR_P(PARAM_NUMBERS) << "value (" << iter[i] << ") could not be converted to a Double";
        if (numbers_[year].size() == 0)
          numbers_[year].resize(n_length_bins_, 0.0);
        n_by_year_[year] += n_value;
        numbers_[year][i - 1] = n_value;
      }
    }
    // Check years allign
    for (auto iter : numbers_) {
      if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
        LOG_ERROR_P(PARAM_NUMBERS) << "table contains year " << iter.first << " which is not a valid year defined in this process";
    }
  } else if (proportions_table_->row_count() != 0) {
    if (proportions_table_->row_count() != years_.size())
      LOG_FATAL_P(PARAM_PROPORTIONS) << "The number of rows supplied = '" << proportions_table_->row_count() << "'. The number of years supplied = '" << years_.size()
                                     << "'. These need to be equal";

    // build a map of n data by year
    if (n_.size() == 1) {
      auto val_n = n_[0];
      n_.assign(years_.size(), val_n);
    } else if (n_.size() != years_.size())
      LOG_ERROR_P(PARAM_N) << "The values provided (" << n_.size() << ") does not match the number of years (" << years_.size() << ")";
    n_by_year_ = utilities::Map::create(years_, n_);

    // load our table data in to our map
    vector<vector<string>> data       = proportions_table_->data();
    unsigned               year       = 0;
    Double                 proportion = 0.0;

    for (auto iter : data) {
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR_P(PARAM_PROPORTIONS) << "value (" << iter[0] << ") could not be converted to an unsigned integer";
      if ((iter.size() - 1) != n_length_bins_) {
        LOG_FATAL_P(PARAM_PROPORTIONS) << "For row in year = " << year
                                       << ". The length bins for this process are defined in the @model block. A column is required for each length bin which is '"
                                       << n_length_bins_ << "'. This table supplied '" << iter.size() - 1 << "'.";
      }

      Double total_proportion = 0.0;

      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], proportion))
          LOG_ERROR_P(PARAM_PROPORTIONS) << "value (" << iter[i] << ") could not be converted to a double.";
        if (numbers_[year].size() == 0)
          numbers_[year].resize(n_length_bins_, 0.0);
        numbers_[year][i - 1] = n_by_year_[year] * proportion;
        if (proportion < 0.0)
          LOG_ERROR_P(PARAM_PROPORTIONS) << "value (" << iter[i] << ") cannot be less than zero.";
        total_proportion += proportion;
      }

      // check the sum of proportions is equal to one
      if (fabs(1.0 - total_proportion) > tolerance_)
        LOG_ERROR_P(PARAM_PROPORTIONS) << "total (" << total_proportion << ") do not sum to 1.0 for year " << year;
    }

    // Check years allign
    for (auto iter : numbers_) {
      if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
        LOG_ERROR_P(PARAM_PROPORTIONS) << "table contains year " << iter.first << " which is not a valid year defined in this process";
    }
  }

  // Create containers for reporting data
  tagged_fish_after_init_mort_.resize(years_.size());
  actual_tagged_fish_to_.resize(years_.size());

  for (unsigned year_ndx = 0; year_ndx < years_.size(); ++year_ndx) {
    tagged_fish_after_init_mort_[year_ndx].resize(split_from_category_labels_.size());
    actual_tagged_fish_to_[year_ndx].resize(to_category_labels_.size());
    for (unsigned from_category_ndx = 0; from_category_ndx < split_from_category_labels_.size(); ++from_category_ndx)
      tagged_fish_after_init_mort_[year_ndx][from_category_ndx].resize(model_->age_spread(), 0.0);
    for (unsigned to_category_ndx = 0; to_category_ndx < to_category_labels_.size(); ++to_category_ndx)
      actual_tagged_fish_to_[year_ndx][to_category_ndx].resize(model_->age_spread(), 0.0);
  }

  if (compatibility_ == PARAM_CASAL)
    LOG_WARNING() << "in @process " << label_ << " you have specified " << PARAM_COMPATIBILITY << " = " << compatibility_
                  << " this is not recommended unless you validating for backwards compatibility reasons";
}

/**
 * Build relationships between this object and others
 */
void TagByLength::DoBuild() {
  LOG_TRACE();
  from_partition_.Init(split_from_category_labels_);
  to_partition_.Init(to_category_labels_);

  // Flag AgeLength class to build AgeLength matrix
  auto from_iter = from_partition_.begin();
  for (; from_iter != from_partition_.end(); from_iter++) {
    (*from_iter)->age_length_->BuildAgeLengthMatrixForTheseYears(years_);
  }

  LOG_FINE() << "number of 'From' categories = " << from_partition_.size();
  numbers_at_length_by_category_.resize(from_partition_.size());
  numbers_at_age_by_category_.resize(from_partition_.size());
  numbers_at_age_and_length_.resize(model_->age_spread());
  exploitation_by_age_.resize(model_->age_spread(), 0.0);
  selected_numbers_at_age_and_length_by_category_.resize(from_partition_.size());
  exploitation_by_age_category_.resize(from_partition_.size());
  tag_to_fish_by_category_age_.resize(from_partition_.size());
  for (unsigned i = 0; i < numbers_at_length_by_category_.size(); ++i) {
    tag_to_fish_by_category_age_[i].resize(model_->age_spread(), 0.0);
    numbers_at_length_by_category_[i].resize(n_length_bins_, 0.0);
    numbers_at_age_by_category_[i].resize(model_->age_spread(), 0.0);
    selected_numbers_at_age_and_length_by_category_[i].resize(model_->age_spread());
    exploitation_by_age_category_[i].resize(model_->age_spread(), 0.0);
    for (unsigned j = 0; j < model_->age_spread(); ++j) {
      selected_numbers_at_age_and_length_by_category_[i][j].resize(n_length_bins_, 0.0);
    }
  }

  for (unsigned i = 0; i < model_->age_spread(); ++i) {
    numbers_at_age_and_length_[i].resize(n_length_bins_, 0.0);
  }

  if (penalty_label_ != "")
    penalty_ = model_->managers()->penalty()->GetPenalty(penalty_label_);
  else
    LOG_WARNING() << location() << "no penalty has been specified. Attempting to tag fish above u_max of the vulnerable population will not affect the objective function";

  selectivities::Manager& selectivity_manager = *model_->managers()->selectivity();

  for (unsigned i = 0; i < selectivity_labels_.size(); ++i) {
    Selectivity* selectivity = selectivity_manager.GetSelectivity(selectivity_labels_[i]);
    if (!selectivity)
      LOG_FATAL_P(PARAM_SELECTIVITIES) << "- the selectivity with label '" << selectivity_labels_[i] << "' was not found";
    selectivities_[split_from_category_labels_[i]] = selectivity;
  }

  if (initial_mortality_selectivity_label_ != "") {
    initial_mortality_selectivity_ = selectivity_manager.GetSelectivity(initial_mortality_selectivity_label_);
    if (!initial_mortality_selectivity_)
      LOG_ERROR_P(PARAM_INITIAL_MORTALITY_SELECTIVITY) << "- the selectivity with label '" << initial_mortality_selectivity_ << "' was not found";
  }

  // population proportion_by_length_
  proportion_by_length_.resize(years_.size());
  tagged_fish_by_year_.resize(years_.size(), 0.0);

  for (unsigned year_ndx = 0; year_ndx < years_.size(); ++year_ndx) {
    proportion_by_length_[year_ndx].resize(n_length_bins_, 0.0);
    for (unsigned length_ndx = 0; length_ndx < n_length_bins_; ++length_ndx) tagged_fish_by_year_[year_ndx] += numbers_[years_[year_ndx]][length_ndx];
    for (unsigned length_ndx = 0; length_ndx < n_length_bins_; ++length_ndx)
      proportion_by_length_[year_ndx][length_ndx] = numbers_[years_[year_ndx]][length_ndx] / tagged_fish_by_year_[year_ndx];
  }

  tag_to_fish_by_age_.resize(model_->age_spread(), 0.0);
  vulnerable_fish_by_age_.resize(model_->age_spread(), 0.0);
  final_exploitation_by_age_.resize(model_->age_spread(), 0.0);
}

/**
 * Execute this process
 */
void TagByLength::DoExecute() {
  LOG_FINE() << label_;
  unsigned current_year = model_->current_year();

  if (model_->state() == State::kInitialise)
    return;
  if ((std::find(years_.begin(), years_.end(), current_year) == years_.end()))
    return;

  auto     iter        = find(years_.begin(), years_.end(), current_year);
  unsigned year_ndx    = distance(years_.begin(), iter);
  auto     from_iter   = from_partition_.begin();
  auto     to_iter     = to_partition_.begin();
  auto     length_bins = model_->length_bins();

  // Do the transition with mortality on the fish we're moving
  LOG_FINE() << "year_ndx = " << year_ndx << " year = " << current_year;
  LOG_FINEST() << "numbers_.size(): " << numbers_.size();
  LOG_FINEST() << "numbers_[current_year].size(): " << numbers_[current_year].size();
  LOG_FINE() << "number of length bins: " << n_length_bins_ << " in year " << current_year;

  // This replicates the original CASAL process
  // There is a potential calculation error, likely small -- users shouldn't use it. Hence the LOG_WARNING().
  // Implemented for comparison, backwards compatiability, and model validation/translation
  if (compatibility_ == PARAM_CASAL) {
    LOG_FINE() << "compatibility_ == PARAM_CASAL";
    LOG_WARNING_P(PARAM_COMPATIBILITY)
        << ": Using the 'casal' compatibility option. Note this is provided for backwards compatibility with CASAL - the recommended compatibility option is '" << PARAM_CASAL2
        << "'";

    fill(exploitation_by_age_.begin(), exploitation_by_age_.end(), 0.0);
    fill(tag_to_fish_by_age_.begin(), tag_to_fish_by_age_.end(), 0.0);
    fill(vulnerable_fish_by_age_.begin(), vulnerable_fish_by_age_.end(), 0.0);
    fill(final_exploitation_by_age_.begin(), final_exploitation_by_age_.end(), 0.0);
    for (unsigned i = 0; i < model_->age_spread(); ++i) {
      fill(numbers_at_age_and_length_[i].begin(), numbers_at_age_and_length_[i].end(), 0.0);
    }

    // Populate age-length matrix across all categories
    LOG_FINE() << "numbers_at_age_and_length_.size() = " << numbers_at_age_and_length_.size() << " numbers_at_age_and_length_[0].size() " << numbers_at_age_and_length_[0].size();
    unsigned from_category_iter = 0;
    for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
      LOG_FINE() << "population numbers at age and length = " << (*from_iter)->name_;
      (*from_iter)->age_length_->populate_age_length_matrix((*from_iter)->data_, numbers_at_age_and_length_, selectivities_[(*from_iter)->name_]);
    }

    // Convert numbers by length -> numbers by age
    LOG_FINE() << "Convert numbers at length to age = ";
    Double sum_age = 0.0;

    for (unsigned length_ndx = 0; length_ndx < n_length_bins_; ++length_ndx) {
      sum_age = 0.0;
      for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx) sum_age += numbers_at_age_and_length_[age_ndx][length_ndx];

      LOG_FINE() << "length = " << length_ndx << " sum_age " << sum_age;
      for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx)
        exploitation_by_age_[age_ndx] += proportion_by_length_[year_ndx][length_ndx] * (numbers_at_age_and_length_[age_ndx][length_ndx] / sum_age);
    }

    for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx) {
      tag_to_fish_by_age_[age_ndx] = exploitation_by_age_[age_ndx] * tagged_fish_by_year_[year_ndx];
      LOG_FINE() << "exploited age = " << exploitation_by_age_[age_ndx] << " age = " << age_ndx;
    }
    LOG_FINE() << "fish to tag";
    // for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx)
    //   LOG_FINE() << tag_to_fish_by_age_[age_ndx];

    // vulnerable age
    from_iter          = from_partition_.begin();
    from_category_iter = 0;

    for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
      for (unsigned age_ndx = 0; age_ndx < (*from_iter)->data_.size(); ++age_ndx) vulnerable_fish_by_age_[age_ndx] += (*from_iter)->data_[age_ndx];
    }
    LOG_FINE() << "vulnerable to tag";
    // for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx)
    //   LOG_FINE() << vulnerable_fish_by_age_[age_ndx];

    // check there is enough fish to tag by age
    for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx) {
      // comment from CASAL: we will not be able to tag as many fish as we designated - this should be rare
      if (vulnerable_fish_by_age_[age_ndx] <= tag_to_fish_by_age_[age_ndx]) {
        tag_to_fish_by_age_[age_ndx] = u_max_ * vulnerable_fish_by_age_[age_ndx];
        // flag penalty
        if (penalty_)
          penalty_->Trigger(vulnerable_fish_by_age_[age_ndx], tag_to_fish_by_age_[age_ndx]);
      }
      final_exploitation_by_age_[age_ndx] = tag_to_fish_by_age_[age_ndx] / vulnerable_fish_by_age_[age_ndx];
    }

    // Move the fish
    from_iter             = from_partition_.begin();
    unsigned category_ndx = 0;
    Double   amount       = 0.0;

    LOG_FINE() << "initial mortality = " << initial_mortality_ << " label = " << label_ << " from = " << from_category_labels_.size() << " to = " << to_category_labels_.size();

    for (; from_iter != from_partition_.end(); from_iter++, to_iter++, category_ndx++) {
      LOG_FINEST() << "from category = " << (*from_iter)->name_ << " to category = " << (*to_iter)->name_ << " category ndx = " << category_ndx;

      for (unsigned j = 0; j < (*from_iter)->data_.size(); ++j) {
        // fish to move
        amount = (*from_iter)->data_[j] * final_exploitation_by_age_[j];
        actual_tagged_fish_to_[year_ndx][category_ndx][j] += amount;

        // account for mortality
        if ((initial_mortality_selectivity_label_ != "") && (initial_mortality_ > 0.0))
          amount *= (1.0 - (initial_mortality_ * initial_mortality_selectivity_->GetAgeResult((*from_iter)->min_age_ + j, (*to_iter)->age_length_)));
        else if ((initial_mortality_selectivity_label_ == "") && (initial_mortality_ > 0.0))
          amount *= (1.0 - initial_mortality_);

        // Just do it!
        (*from_iter)->data_[j] -= amount;
        (*to_iter)->data_[j] += amount;

        tagged_fish_after_init_mort_[year_ndx][category_ndx][j] += amount;

        if ((*from_iter)->data_[j] < 0.0)
          LOG_CODE_ERROR() << "The process tag_by_length (with label " << label_ << ") caused a negative partition " << (*from_iter)->name_ << " "
                           << " age = " << j + (*from_iter)->min_age_ << " numbers at age = " << (*from_iter)->data_[j] << " tagged fish = " << amount;

        // log out for debugging
        LOG_FINEST() << "age = " << j + model_->min_age() << " = " << amount << " after process to category = " << (*to_iter)->data_[j]
                     << " from category = " << (*from_iter)->data_[j];
      }
    }
  } else if (compatibility_ == PARAM_CASAL2) {
    LOG_FINE() << "compatibility_ == PARAM_CASAL2";
    unsigned from_category_iter = 0;
    for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
      LOG_FINE() << "cat counter = " << from_category_iter;
      fill(tag_to_fish_by_category_age_[from_category_iter].begin(), tag_to_fish_by_category_age_[from_category_iter].end(), 0.0);
      fill(exploitation_by_age_category_[from_category_iter].begin(), exploitation_by_age_category_[from_category_iter].end(), 0.0);
      for (unsigned i = 0; i < model_->age_spread(); ++i) {
        fill(selected_numbers_at_age_and_length_by_category_[from_category_iter][i].begin(), selected_numbers_at_age_and_length_by_category_[from_category_iter][i].end(), 0.0);
      }
      (*from_iter)
          ->age_length_->populate_age_length_matrix((*from_iter)->data_, selected_numbers_at_age_and_length_by_category_[from_category_iter], selectivities_[(*from_iter)->name_]);
    }
    // Convert numbers by length -> numbers by age
    LOG_FINE() << "Convert numbers at length to age = ";

    for (unsigned length_ndx = 0; length_ndx < n_length_bins_; ++length_ndx) {
      from_iter           = from_partition_.begin();
      from_category_iter  = 0;
      Double temp_sum_age = 0.0;
      for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
        LOG_FINE() << "cat counter = " << from_category_iter;
        for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx)
          temp_sum_age += selected_numbers_at_age_and_length_by_category_[from_category_iter][age_ndx][length_ndx];
      }
      from_category_iter = 0;
      from_iter          = from_partition_.begin();
      for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
        // scale values
        for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx) {
          tag_to_fish_by_category_age_[from_category_iter][age_ndx]
              += proportion_by_length_[year_ndx][length_ndx] * (selected_numbers_at_age_and_length_by_category_[from_category_iter][age_ndx][length_ndx] / temp_sum_age);
        }
      }
    }

    from_iter          = from_partition_.begin();
    from_category_iter = 0;

    for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
      LOG_FINE() << "cat counter = " << from_category_iter;
      for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx) {
        tag_to_fish_by_category_age_[from_category_iter][age_ndx] *= tagged_fish_by_year_[year_ndx];
      }
    }

    from_iter          = from_partition_.begin();
    to_iter            = to_partition_.begin();
    from_category_iter = 0;

    for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++, to_iter++) {
      Double amount = 0.0;
      LOG_FINE() << "category = " << (*from_iter)->name_;
      for (unsigned age_ndx = 0; age_ndx < model_->age_spread(); ++age_ndx) {
        LOG_FINE() << "age = " << age_ndx;
        exploitation_by_age_category_[from_category_iter][age_ndx] = tag_to_fish_by_category_age_[from_category_iter][age_ndx] / (*from_iter)->data_[age_ndx];
        if (exploitation_by_age_category_[from_category_iter][age_ndx] > u_max_) {
          exploitation_by_age_category_[from_category_iter][age_ndx] = u_max_;
          // flag penalty
          if (penalty_) {
            LOG_FINEST() << " exploitation expected = " << exploitation_by_age_category_[from_category_iter][age_ndx] << " available = " << (*from_iter)->data_[age_ndx];
            penalty_->Trigger((*from_iter)->data_[age_ndx], (*from_iter)->data_[age_ndx] * exploitation_by_age_category_[from_category_iter][age_ndx]);
          }
        }
        // fish to move
        amount = (*from_iter)->data_[age_ndx] * exploitation_by_age_category_[from_category_iter][age_ndx];
        LOG_FINEST() << " exploitation = " << exploitation_by_age_category_[from_category_iter][age_ndx] << " amount - " << amount;
        actual_tagged_fish_to_[year_ndx][from_category_iter][age_ndx] += amount;
        // account for mortality
        if ((initial_mortality_selectivity_label_ != "") && (initial_mortality_ > 0.0))
          amount *= (1.0 - (initial_mortality_ * initial_mortality_selectivity_->GetAgeResult((*from_iter)->min_age_ + age_ndx, (*to_iter)->age_length_)));
        else if ((initial_mortality_selectivity_label_ == "") && (initial_mortality_ > 0.0))
          amount *= (1.0 - initial_mortality_);

        tagged_fish_after_init_mort_[year_ndx][from_category_iter][age_ndx] += amount;
        // Just do it!
        (*from_iter)->data_[age_ndx] -= amount;
        (*to_iter)->data_[age_ndx] += amount;

        if ((*from_iter)->data_[age_ndx] < 0.0)
          LOG_CODE_ERROR() << "The process tag_by_length (with label " << label_ << ") caused a negative partition " << (*from_iter)->name_ << " "
                           << " age = " << age_ndx + (*from_iter)->min_age_ << " numbers at age = " << (*from_iter)->data_[age_ndx] << " tagged fish = " << amount;
      }
    }
  }  // compatibility_ switch
}

/*
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void TagByLength::FillReportCache(ostringstream& cache) {
  LOG_FINE() << "report age distribution of tagged individuals by source and destination";
  for (unsigned category_ndx = 0; category_ndx < to_category_labels_.size(); ++category_ndx) {
    cache << "tags-after-initial_mortality-" << to_category_labels_[category_ndx] << " " << REPORT_R_DATAFRAME_ROW_LABELS << "\n";
    cache << "year ";
    for (unsigned age = min_age_; age <= max_age_; ++age) cache << age << " ";
    cache << REPORT_EOL;
    for (unsigned year_ndx = 0; year_ndx < years_.size(); ++year_ndx) {
      cache << years_[year_ndx] << " ";
      for (unsigned age_ndx = 0; age_ndx < age_spread_; ++age_ndx) cache << AS_DOUBLE(tagged_fish_after_init_mort_[year_ndx][category_ndx][age_ndx]) << " ";
      cache << REPORT_EOL;
    }
  }

  for (unsigned category_ndx = 0; category_ndx < to_category_labels_.size(); ++category_ndx) {
    cache << "tag-releases-" << to_category_labels_[category_ndx] << " " << REPORT_R_DATAFRAME_ROW_LABELS << "\n";
    cache << "year ";
    for (unsigned age = min_age_; age <= max_age_; ++age) cache << age << " ";
    cache << REPORT_EOL;
    for (unsigned year_ndx = 0; year_ndx < years_.size(); ++year_ndx) {
      cache << years_[year_ndx] << " ";
      for (unsigned age_ndx = 0; age_ndx < age_spread_; ++age_ndx) cache << AS_DOUBLE(actual_tagged_fish_to_[year_ndx][category_ndx][age_ndx]) << " ";
      cache << REPORT_EOL;
    }
  }
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
