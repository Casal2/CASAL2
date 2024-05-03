/**
 * @file TagByAge.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 26/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// Requires checking for consistency with TagByAge

// headers
#include "TagByAge.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../../Utilities/Math.h"
#include "Categories/Categories.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default constructor
 */
TagByAge::TagByAge(shared_ptr<Model> model) : Process(model), to_partition_(model), from_partition_(model) {
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
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "The minimum age to transition", "", model->min_age());
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "The maximum age to transition", "", model->max_age());
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
  // clang-format on
}

/**
 * Destructor
 */
TagByAge::~TagByAge() {
  delete numbers_table_;
  delete proportions_table_;
}

/**
 * Validate the parameters from the configuration file
 */
void TagByAge::DoValidate() {
  LOG_TRACE();

  if (min_age_ < model_->min_age())
    LOG_FATAL_P(PARAM_MIN_AGE) << " (" << min_age_ << ") is less than the model's minimum age (" << model_->min_age() << ")";
  if (max_age_ > model_->max_age())
    LOG_FATAL_P(PARAM_MAX_AGE) << " (" << max_age_ << ") is greater than the model's maximum age (" << model_->max_age() << ")";
  if (min_age_ > max_age_)
    LOG_FATAL_P(PARAM_MIN_AGE) << " (" << min_age_ << ") cannot be greater than max_age (" << max_age_ << ")";

  age_spread_        = (max_age_ - min_age_) + 1;
  number_categories_ = from_category_labels_.size();
  min_age_offset_ = min_age_ - model_->min_age();

  if (tolerance_ > 0.01) {
    LOG_WARNING_P(PARAM_TOLERANCE) << "Your tolerance is larger than " << 0.01
                                   << " this mean Casal2 may tag less fish than you think it should. It is recommended that you pick a smaller number.";
  }
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

  vector<string> split_category_labels;
  unsigned       no_combined = 0;

  for (auto category : from_category_labels_) {
    if (model_->categories()->IsCombinedLabels(category)) {
      // TODO: Combined categories are mostly coded below, but theres an error that will need to be resolved when time permits.
      // TODO: Once fixed, then the following LOG_FATAL_P() can be removed
      // LOG_FATAL_P(PARAM_FROM) << "combined categories are not yet implemented";
      no_combined = model_->categories()->GetNumberOfCategoriesDefined(category);
      if (no_combined != to_category_labels_.size()) {
        LOG_ERROR_P(PARAM_TO) << "'" << no_combined << "' combined 'From' categories was specified, but '" << to_category_labels_.size()
                              << "' 'To' categories were supplied. The number of 'From' and 'To' categories must be the same.";
      }
      boost::split(split_category_labels, category, boost::is_any_of("+"));

      for (const string& split_category_label : split_category_labels) {
        LOG_FINE() << "split category " << split_category_label;
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
    LOG_ERROR_P(PARAM_TO) << " the number of values supplied (" << to_category_labels_.size() << ") does not match the number of 'From' categories provided ("
                          << split_from_category_labels_.size() << ")";
  }

  if (to_category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << "the number of selectivities must match the number of 'to_categories'. " << to_category_labels_.size()
                                     << " 'to_categories' were supplied, but " << selectivity_labels_.size() << " selectivity labels were supplied";

  // Get our first year
  first_year_ = years_[0];
  std::for_each(years_.begin(), years_.end(), [this](unsigned year) { first_year_ = year < first_year_ ? year : first_year_; });

  // Load our table data
  if (numbers_table_->row_count() != 0) {
    if (numbers_table_->row_count() != years_.size())
      LOG_FATAL_P(PARAM_NUMBERS) << "do not match the number of years. The number of rows supplied was " << numbers_table_->row_count() << " and the number of years supplied was "
                                 << years_.size() << ". These need to be the same";

    // Load data from numbers table and calculate n
    map<unsigned, Double> n_by_year_ = utilities::Map::create(years_, 0.0);

    // load our table data in to our map
    vector<vector<string>> data    = numbers_table_->data();
    unsigned               year    = 0;
    Double                 n_value = 0.0;

    for (auto iter : data) {
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR_P(PARAM_NUMBERS) << " value (" << iter[0] << ") could not be converted to an unsigned integer";

      if ((iter.size() - 1) != age_spread_) {
        LOG_FATAL_P(PARAM_NUMBERS) << "for row in year = " << year << ". The ages for this process are defined as " << min_age_ << " to " << max_age_
                                   << ". A column is required for each, which is " << age_spread_ << " columns. This table supplied " << iter.size() - 1 << " values.";
      }

      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], n_value))
          LOG_ERROR_P(PARAM_NUMBERS) << "value (" << iter[i] << ") could not be converted to a Double";
        if (numbers_[year].size() == 0)
          numbers_[year].resize(age_spread_, 0.0);
        n_by_year_[year] += n_value;
        numbers_[year][i - 1] = n_value;
      }
      // Check years allign
      for (auto iter : numbers_) {
        if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
          LOG_ERROR_P(PARAM_NUMBERS) << "table contains year " << iter.first << " which is not a valid year defined in this process";
      }
    }
  } else if (proportions_table_->row_count() != 0) {
    if (proportions_table_->row_count() != years_.size()) {
      LOG_FATAL_P(PARAM_NUMBERS) << "do not match the number of years. The number of rows supplied was " << numbers_table_->row_count() << " and the number of years supplied was "
                                 << years_.size() << ". These need to be the same";
    }

    // build a map of n data by year
    if (n_.size() == 1) {
      auto val_n = n_[0];
      n_.assign(years_.size(), val_n);
    } else if (n_.size() != years_.size())
      LOG_ERROR_P(PARAM_N) << "The number of values provided (" << n_.size() << ") does not match the number of years (" << years_.size() << ")";
    n_by_year_ = utilities::Map::create(years_, n_);

    // load our table data in to our map
    vector<vector<string>> data       = proportions_table_->data();
    unsigned               year       = 0;
    Double                 proportion = 0.0;

    for (auto iter : data) {
      if ((iter.size() - 1) != age_spread_) {
        LOG_FATAL_P(PARAM_PROPORTIONS) << "For row in year = " << year << ". The number of age values for this process should be " << age_spread_ << "'. This table supplied '"
                                       << iter.size() - 1 << "'.";
      }

      LOG_ERROR_P(PARAM_PROPORTIONS) << " value (" << iter[0] << ") could not be converted to an unsigned integer";

      Double total_proportion = 0.0;
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR_P(PARAM_PROPORTIONS) << "value (" << iter[0] << ") could not be converted to an unsigned integer";

      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], proportion))
          LOG_ERROR_P(PARAM_PROPORTIONS) << "value (" << iter[i] << ") could not be converted to a double.";
        if (numbers_[year].size() == 0)
          numbers_[year].resize(age_spread_, 0.0);
        numbers_[year][i - 1] = n_by_year_[year] * proportion;
        if (proportion < 0.0)
          LOG_ERROR_P(PARAM_PROPORTIONS) << "value (" << iter[i] << ") cannot be less than zero.";
        total_proportion += proportion;
      }

      // check the sum of proportions is equal to one
      if (fabs(1.0 - total_proportion) > tolerance_)
        LOG_ERROR_P(PARAM_PROPORTIONS) << "total (" << total_proportion << ") do not sum to 1.0 for year " << year;
    }
    for (auto iter : numbers_) {
      if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
        LOG_ERROR_P(PARAM_NUMBERS) << "table contains year " << iter.first << " which is not a valid year defined in this process";
    }
  }
  // Create containers for reporting data
  tagged_fish_after_init_mort_.resize(years_.size());
  actual_tagged_fish_to_.resize(years_.size());

  for (unsigned year_ndx = 0; year_ndx < years_.size(); ++year_ndx) {
    tagged_fish_after_init_mort_[year_ndx].resize(split_from_category_labels_.size());
    actual_tagged_fish_to_[year_ndx].resize(to_category_labels_.size());
    for (unsigned from_category_ndx = 0; from_category_ndx < split_from_category_labels_.size(); ++from_category_ndx)
      tagged_fish_after_init_mort_[year_ndx][from_category_ndx].resize(age_spread_, 0.0);
    for (unsigned to_category_ndx = 0; to_category_ndx < to_category_labels_.size(); ++to_category_ndx)
      actual_tagged_fish_to_[year_ndx][to_category_ndx].resize(age_spread_, 0.0);
  }
}

/**
 * Build relationships between this object and others
 */
void TagByAge::DoBuild() {
  LOG_TRACE();
  from_partition_.Init(split_from_category_labels_);
  to_partition_.Init(to_category_labels_);

  LOG_FINE() << "number of 'From' categories = " << from_partition_.size();
  numbers_at_age_by_category_.resize(from_partition_.size());
  selected_numbers_at_age_by_category_.resize(from_partition_.size());
  exploitation_by_age_category_.resize(from_partition_.size());
  tag_to_fish_by_category_age_.resize(from_partition_.size());
  for (unsigned i = 0; i < numbers_at_age_by_category_.size(); ++i) {
    numbers_at_age_by_category_[i].resize(age_spread_, 0.0);
    exploitation_by_age_category_[i].resize(age_spread_, 0.0);
    selected_numbers_at_age_by_category_[i].resize(age_spread_, 0.0);
    tag_to_fish_by_category_age_[i].resize(age_spread_, 0.0);
  }

  if (penalty_label_ != "")
    penalty_ = model_->managers()->penalty()->GetPenalty(penalty_label_);
  else
    LOG_WARNING() << location() << "No penalty has been specified. Attempting to tag fish above u_max of the vulnerable population will not affect the objective function";

  selectivities::Manager& selectivity_manager = *model_->managers()->selectivity();

  for (unsigned i = 0; i < selectivity_labels_.size(); ++i) {
    Selectivity* selectivity = selectivity_manager.GetSelectivity(selectivity_labels_[i]);
    LOG_FINEST() << "selectivity : " << selectivity_labels_[i];
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << "- the selectivity with label '" << selectivity_labels_[i] << "' was not found";
    selectivities_[split_from_category_labels_[i]] = selectivity;
  }

  if (initial_mortality_selectivity_label_ != "") {
    initial_mortality_selectivity_ = selectivity_manager.GetSelectivity(initial_mortality_selectivity_label_);
    LOG_FINEST() << "initial_mortality_selectivity_ : " << initial_mortality_selectivity_;
    if (!initial_mortality_selectivity_)
      LOG_ERROR_P(PARAM_INITIAL_MORTALITY_SELECTIVITY) << "- the selectivity with label '" << initial_mortality_selectivity_ << "' was not found";
  }

  tagged_fish_by_year_.resize(years_.size(), 0.0);
  for (unsigned year_ndx = 0; year_ndx < years_.size(); ++year_ndx) {
    for (unsigned age_ndx = 0; age_ndx < age_spread_; ++age_ndx) 
      tagged_fish_by_year_[year_ndx] += numbers_[years_[year_ndx]][age_ndx];
  }

  tag_to_fish_by_age_.resize(age_spread_, 0.0);
  vulnerable_fish_by_age_.resize(age_spread_, 0.0);
  final_exploitation_by_age_.resize(age_spread_, 0.0);
}

/**
 * Execute this process
 */
void TagByAge::DoExecute() {
  LOG_FINE() << label_;
  unsigned current_year = model_->current_year();

  if (model_->state() == State::kInitialise)
    return;
  if ((std::find(years_.begin(), years_.end(), current_year) == years_.end()))
    return;

  auto     iter      = find(years_.begin(), years_.end(), current_year);
  unsigned year_ndx  = distance(years_.begin(), iter);
  auto     from_iter = from_partition_.begin();
  auto     to_iter   = to_partition_.begin();

  // Do the transition with mortality on the fish we're moving
  LOG_FINE() << "year_ndx = " << year_ndx << " year = " << current_year;
  LOG_FINEST() << "numbers_.size(): " << numbers_.size();
  LOG_FINEST() << "numbers_[current_year].size(): " << numbers_[current_year].size();
  LOG_FINE() << "number of ages: " << age_spread_ << " in year " << current_year;

  unsigned from_category_iter = 0;
  for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
    LOG_FINE() << "category counter = " << from_category_iter;
    fill(selected_numbers_at_age_by_category_[from_category_iter].begin(), selected_numbers_at_age_by_category_[from_category_iter].end(), 0.0);
    fill(exploitation_by_age_category_[from_category_iter].begin(), exploitation_by_age_category_[from_category_iter].end(), 0.0);
    fill(tag_to_fish_by_category_age_[from_category_iter].begin(), tag_to_fish_by_category_age_[from_category_iter].end(), 0.0);
  }

  from_iter          = from_partition_.begin();
  from_category_iter = 0;

  for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++) {
    LOG_FINE() << "category counter = " << from_category_iter;
    for (unsigned age_ndx = 0; age_ndx < age_spread_; ++age_ndx) {
      tag_to_fish_by_category_age_[from_category_iter][age_ndx]
          = numbers_[years_[year_ndx]][age_ndx] * selectivities_[(*from_iter)->name_]->GetAgeResult(min_age_ + age_ndx, (*from_iter)->age_length_);
    }
  }

  from_iter          = from_partition_.begin();
  from_category_iter = 0;
  
  for (; from_iter != from_partition_.end(); from_iter++, from_category_iter++, to_iter++) {
    Double amount = 0.0;
    LOG_FINE() << "category = " << (*from_iter)->name_;
    for (unsigned age_ndx = 0; age_ndx < age_spread_; ++age_ndx) {
      LOG_FINE() << "age = " << age_ndx;
      exploitation_by_age_category_[from_category_iter][age_ndx] = tag_to_fish_by_category_age_[from_category_iter][age_ndx] / (*from_iter)->data_[min_age_offset_ + age_ndx];
      if (exploitation_by_age_category_[from_category_iter][age_ndx] > u_max_) {
        exploitation_by_age_category_[from_category_iter][age_ndx] = u_max_;
        // flag penalty
        if (penalty_) {
          LOG_FINEST() << " exploitation expected = " << exploitation_by_age_category_[from_category_iter][age_ndx] << " available = " << (*from_iter)->data_[min_age_offset_ + age_ndx];
          penalty_->Trigger((*from_iter)->data_[min_age_offset_ + age_ndx], (*from_iter)->data_[min_age_offset_ + age_ndx] * exploitation_by_age_category_[from_category_iter][age_ndx]);
        }
      }
      // fish to move
      amount = (*from_iter)->data_[min_age_offset_ + age_ndx] * exploitation_by_age_category_[from_category_iter][age_ndx];
      LOG_FINEST() << " exploitation = " << exploitation_by_age_category_[from_category_iter][age_ndx] << " amount - " << amount;
      actual_tagged_fish_to_[year_ndx][from_category_iter][age_ndx] += amount;
      // account for mortality
      if ((initial_mortality_selectivity_label_ != "") && (initial_mortality_ > 0.0))
        amount *= (1.0 - (initial_mortality_ * initial_mortality_selectivity_->GetAgeResult((*from_iter)->min_age_ + age_ndx, (*to_iter)->age_length_)));
      else if ((initial_mortality_selectivity_label_ == "") && (initial_mortality_ > 0.0))
        amount *= (1.0 - initial_mortality_);

      tagged_fish_after_init_mort_[year_ndx][from_category_iter][age_ndx] += amount;
      // Just do it!
      (*from_iter)->data_[min_age_offset_ + age_ndx] -= amount;
      (*to_iter)->data_[min_age_offset_ + age_ndx] += amount;

      if ((*from_iter)->data_[min_age_offset_ + age_ndx] < 0.0)
        LOG_CODE_ERROR() << "The process tag_by_age (with label " << label_ << ") caused a negative partition " << (*from_iter)->name_ << " "
                         << " age = " << age_ndx + (*from_iter)->min_age_ << " numbers at age = " << (*from_iter)->data_[min_age_offset_ + age_ndx] << " tagged fish = " << amount;
    }
  }
}

void TagByAge::FillReportCache(ostringstream& cache) {
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
