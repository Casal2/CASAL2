/**
 * @file TagByAge.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 26/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "TagByAge.h"

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Penalties/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace processes {

/**
 * Default constructor
 */
TagByAge::TagByAge() {
  process_type_ = ProcessType::kTransition;

  numbers_table_ = TablePtr(new parameters::Table(PARAM_NUMBERS));
  proportions_table_ = TablePtr(new parameters::Table(PARAM_PROPORTIONS));

  parameters_.Bind<string>(PARAM_FROM, &from_category_labels_, "Categories to transition from", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_labels_, "Categories to transition to", "");
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age to transition", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age to transition", "");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "Penalty label", "", "");
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "U Max", "", 0.99);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute the transition in", "");
  parameters_.Bind<Double>(PARAM_INITIAL_MORTALITY, &initial_mortality_, "", "", Double(0));
  parameters_.Bind<string>(PARAM_INITIAL_MORTALITY_SELECTIVITY, &initial_mortality_selectivity_label_, "", "", "");
  parameters_.Bind<Double>(PARAM_LOSS_RATE, &loss_rate_, "", "");
  parameters_.Bind<string>(PARAM_LOSS_RATE_SELECTIVITIES, &loss_rate_selectivity_labels_, "", "", true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "", "");
  parameters_.Bind<Double>(PARAM_N, &n_, "", "", true);
  parameters_.BindTable(PARAM_NUMBERS, numbers_table_, "Table of N data", "", true, true);
  parameters_.BindTable(PARAM_PROPORTIONS, proportions_table_, "Table of proportions to move", "" , true, true);

  model_ = Model::Instance();
}

/**
 * Validate the parameters from the configuration file
 */
void TagByAge::DoValidate() {
  if (from_category_labels_.size() != to_category_labels_.size()) {
    LOG_ERROR(parameters_.location(PARAM_TO) << " number of values supplied (" << to_category_labels_.size()
        << ") does not match the number of from categories provided (" << from_category_labels_.size() << ")");
  }
  if (u_max_ <= 0.0 || u_max_ > 1.0)
    LOG_ERROR(parameters_.location(PARAM_U_MAX) << " (" << u_max_ << ") must be greater than 0.0 and less than 1.0");
  if (min_age_ < model_->min_age())
    LOG_ERROR(parameters_.location(PARAM_MIN_AGE) << " (" << min_age_ << ") is less than the model's minimum age (" << model_->min_age() << ")");
  if (max_age_ > model_->max_age())
    LOG_ERROR(parameters_.location(PARAM_MAX_AGE) << " (" << max_age_ << ") is greater than the model's maximum age (" << model_->max_age() << ")");

  unsigned age_spread = (max_age_ - min_age_) + 1;

  niwa::CategoriesPtr categories = niwa::Categories::Instance();
  from_category_labels_ = categories->ExpandLabels(from_category_labels_, parameters_.Get(PARAM_FROM));
  to_category_labels_   = categories->ExpandLabels(to_category_labels_, parameters_.Get(PARAM_TO));

  /**
   * Get our first year
   */
  first_year_ = years_[0];
  std::for_each(years_.begin(), years_.end(), [this](unsigned year){ first_year_ = year < first_year_ ? year : first_year_; });

  /**
   * Build our loss rate map
   */
  if (loss_rate_.size() == 1) {
    loss_rate_.assign(from_category_labels_.size(), loss_rate_[0]);
    loss_rate_by_category_ = utilities::MapCreate(from_category_labels_, loss_rate_);

  } else if (loss_rate_.size() == from_category_labels_.size()) {
    loss_rate_by_category_ = utilities::MapCreate(from_category_labels_, loss_rate_);

  } else {
    LOG_ERROR(parameters_.location(PARAM_LOSS_RATE) << " number provided (" << loss_rate_.size() << " does not match the number of " << PARAM_FROM << " categories ("
        << from_category_labels_.size() << ")");
  }

  /**
   * Build our tables
   */
  if (numbers_table_->row_count() == 0 && proportions_table_->row_count() == 0)
    LOG_ERROR(location() << " must have either a " << PARAM_NUMBERS << " or " << PARAM_PROPORTIONS << " table defined with appropriate data");
  if (numbers_table_->row_count() != 0 && proportions_table_->row_count() != 0)
    LOG_ERROR(location() << " cannot have both a " << PARAM_NUMBERS << " and " << PARAM_PROPORTIONS << " table defined. Please only use 1.");
  if (proportions_table_->row_count() != 0 && !parameters_.Get(PARAM_N)->has_been_defined())
    LOG_ERROR(location() << " cannot have a " << PARAM_PROPORTIONS << " table without defining " << PARAM_N);

  /**
   * Load our N data in to the map
   */
  if (numbers_table_->row_count() != 0) {
    vector<string> columns = numbers_table_->columns();
    if (columns.size() != age_spread + 1)
      LOG_ERROR(parameters_.location(PARAM_NUMBERS) << " number of columns provided (" << columns.size() << ") does not match the model's age spread + 1 ("
          << (age_spread + 1) << ")");
    if (columns[0] != PARAM_YEAR)
      LOG_ERROR(parameters_.location(PARAM_NUMBERS) << " first column label (" << columns[0] << ") provided must be 'year'");

    map<unsigned, unsigned> age_index;
    for (unsigned i = 1; i < columns.size(); ++i) {
      unsigned age = 0;
      if (!utilities::To<unsigned>(columns[i], age))
        LOG_ERROR("");
      age_index[age] = i;
    }

    // load our table data in to our map
    vector<vector<string>> data = numbers_table_->data();
    unsigned year = 0;
    Double n_value = 0.0;
    for (auto iter : data) {
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR(parameters_.location(PARAM_NUMBERS) << " value (" << iter[0] << ") is not a valid unsigned value that could be converted to a model year");
      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], n_value))
          LOG_ERROR(parameters_.location(PARAM_NUMBERS) << " value (" << iter[i] << ") could not be converted to a double. Please ensure it's a numeric value");
        if (numbers_[year].size() == 0)
          numbers_[year].resize(age_spread, 0.0);
        numbers_[year][i - 1] = n_value;
      }
    }

    for (auto iter : numbers_) {
      if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
        LOG_ERROR(parameters_.location(PARAM_NUMBERS) << " table contains year " << iter.first << " that is not a valid year defined in this process");
    }

  } else if (proportions_table_->row_count() != 0) {
    /**
     * Load data from proportions table using n parameter
     */
    vector<string> columns = proportions_table_->columns();
    if (columns.size() != age_spread + 1)
      LOG_ERROR(parameters_.location(PARAM_PROPORTIONS) << " number of columns provided (" << columns.size() << ") does not match the model's age spread + 1 ("
          << (age_spread + 1) << ")");
    if (columns[0] != PARAM_YEAR)
      LOG_ERROR(parameters_.location(PARAM_PROPORTIONS) << " first column label (" << columns[0] << ") provided must be 'year'");

    map<unsigned, unsigned> age_index;
    for (unsigned i = 1; i < columns.size(); ++i) {
      unsigned age = 0;
      if (!utilities::To<unsigned>(columns[i], age))
        LOG_ERROR("");
      age_index[age] = i;
    }

    // build a map of n data by year
    if (n_.size() == 1)
      n_.assign(years_.size(), n_[0]);
    else if (n_.size() != years_.size())
      LOG_ERROR(parameters_.location(PARAM_N) << " values provied (" << n_.size() << ") does not match the number of years (" << years_.size() << ")");
    map<unsigned, Double> n_by_year = utilities::MapCreate(years_, n_);

    // load our table data in to our map
    vector<vector<string>> data = proportions_table_->data();
    unsigned year = 0;
    Double proportion = 0.0;
    for (auto iter : data) {
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR(parameters_.location(PARAM_NUMBERS) << " value (" << iter[0] << ") is not a valid unsigned value that could be converted to a model year");
      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], proportion))
          LOG_ERROR(parameters_.location(PARAM_NUMBERS) << " value (" << iter[i] << ") could not be converted to a double. Please ensure it's a numeric value");
        if (numbers_[year].size() == 0)
          numbers_[year].resize(age_spread, 0.0);
        numbers_[year][i - 1] = n_by_year[year] * proportion;
      }
    }

    for (auto iter : numbers_) {
      if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
        LOG_ERROR(parameters_.location(PARAM_PROPORTIONS) << " table contains year " << iter.first << " that is not a valid year defined in this process");
    }
  }
}

/**
 * Build relationships between this object and others
 */
void TagByAge::DoBuild() {
  from_partition_.Init(from_category_labels_);
  to_partition_.Init(to_category_labels_);

  if (penalty_label_ != "")
    penalty_ = penalties::Manager::Instance().GetPenalty(penalty_label_);

  selectivities::Manager& selectivity_manager = selectivities::Manager::Instance();
  for (unsigned i = 0; i < selectivity_labels_.size(); ++i) {
    SelectivityPtr selectivity = selectivity_manager.GetSelectivity(selectivity_labels_[i]);
    if (!selectivity)
      LOG_ERROR("Selectivity: " << selectivity_labels_[i] << " not found");
    selectivities_[from_category_labels_[i]] = selectivity;
  }
  for (unsigned i = 0 ; i < loss_rate_selectivity_labels_.size(); ++i)
    loss_rate_selectivity_by_category_[from_category_labels_[i]] = selectivity_manager.GetSelectivity(loss_rate_selectivity_labels_[i]);
  if (initial_mortality_selectivity_label_ != "")
    initial_mortality_selectivity_ = selectivity_manager.GetSelectivity(initial_mortality_selectivity_label_);
}

/**
 * Execute this process
 */
void TagByAge::DoExecute() {
  if (model_->current_year() < first_year_)
    return;

  /**
   * Apply the loss rate
   */
  auto from_iter = from_partition_.begin();
  auto to_iter   = to_partition_.begin();
  for (; from_iter != from_partition_.end(); from_iter++, to_iter++) {
    string category_label = (*from_iter)->name_;
    Double loss_rate = loss_rate_by_category_[category_label];
    LOG_INFO("Applying loss rate: " << loss_rate << " for category " << category_label);

    for (unsigned i = 0; i < (*to_iter)->data_.size(); ++i) {
      Double amount = (*to_iter)->data_[i] * loss_rate;
      if (loss_rate_selectivity_by_category_.find(category_label) != loss_rate_selectivity_by_category_.end())
        amount *= loss_rate_selectivity_by_category_[category_label]->GetResult((*from_iter)->min_age_ + i);
      (*to_iter)->data_[i] -= amount;
      (*from_iter)->data_[i] += amount;
    }
  }

  /**
   * Do the transition with mortality on the fish we're moving
   */
  unsigned current_year = Model::Instance()->current_year();
  if (std::find(years_.begin(), years_.end(), current_year) == years_.end())
    return;

  LOG_INFO("numbers__.size(): " << numbers_.size());
  LOG_INFO("numbers__[current_year].size(): " << numbers_[current_year].size());
  for (unsigned i = 0; i < numbers_[current_year].size(); ++i)
    LOG_INFO("numbers__[current_year][" << i << "]: " << numbers_[current_year][i]);

  Double total_stock = 0.0;
  Double total_stock_with_selectivities = 0.0;
  unsigned age_spread = (max_age_ - min_age_) + 1;
  LOG_INFO("age_spread: " << age_spread << " in year " << current_year);

  for (unsigned i = 0; i < age_spread; ++i) {
    /**
     * Calculate the Exploitation rate
     */
    from_iter = from_partition_.begin();
    to_iter   = to_partition_.begin();

    LOG_INFO("selectivity.size(): " << selectivities_.size());
    for (auto iter : selectivities_)
      LOG_INFO("selectivity: " << iter.first);

    total_stock_with_selectivities = 0.0;
    for (; from_iter != from_partition_.end(); from_iter++, to_iter++) {
      unsigned offset = (min_age_ - (*from_iter)->min_age_) + i;
      LOG_INFO("total_stock_offset: " << offset << " (" << (*from_iter)->min_age_ << " - " << min_age_ << ") + " << i);

      LOG_INFO("name: " << (*from_iter)->name_);
      LOG_INFO("selectivity value: " << selectivities_[(*from_iter)->name_]->GetResult(min_age_ + offset));
      total_stock_with_selectivities += (*from_iter)->data_[offset] * selectivities_[(*from_iter)->name_]->GetResult(min_age_ + offset);
    }
    LOG_INFO("total_stock_with_selectivities: " << total_stock_with_selectivities << " at age " << min_age_ + i << " (" << min_age_ << " + " << i << ")");

    /**
     * Migrate the exploited amount
     */
    from_iter = from_partition_.begin();
    to_iter   = to_partition_.begin();
    for (; from_iter != from_partition_.end(); from_iter++, to_iter++) {
      LOG_INFO("Working with categories: from " << (*from_iter)->name_ << "; to " << (*to_iter)->name_);
      unsigned offset = (min_age_ - (*from_iter)->min_age_) + i;
      string category_label = (*from_iter)->name_;

      Double current = numbers_[current_year][i] *
          ((*from_iter)->data_[offset] * selectivities_[category_label]->GetResult(min_age_ + offset) / total_stock_with_selectivities);

      Double exploitation = current / utilities::doublecompare::ZeroFun((*from_iter)->data_[offset]);
      if ( exploitation > u_max_ ) {
        current = total_stock * u_max_;
        if (penalty_)
          penalty_->Trigger(label_, numbers_[current_year][i], (*from_iter)->data_[offset] * u_max_);
      }

      LOG_INFO("numbers: " << numbers_[current_year][i]);
      LOG_INFO("population: " << (*from_iter)->data_[offset]);
      LOG_INFO("selectivity: " << selectivities_[category_label]->GetResult(min_age_ + offset));
      LOG_INFO("current: " << current << "; exploitation: " << exploitation);
      LOG_INFO("current calculated as current = " << numbers_[current_year][i] << " * "
          << (*from_iter)->data_[offset] << " * " << selectivities_[category_label]->GetResult(min_age_ + offset)
          << " / " << total_stock_with_selectivities
      );

      if (current <= 0.0)
        continue;

      Double current_with_mortality = current - (current * initial_mortality_);
      LOG_INFO("current_with_mortality: " << current_with_mortality);
      if (initial_mortality_selectivity_)
        current_with_mortality *= initial_mortality_selectivity_->GetResult(min_age_ + i);
      (*from_iter)->data_[offset] -= current;
      (*to_iter)->data_[offset] += current_with_mortality;
    }
  }

  for (unsigned year : years_) {
    if (numbers_.find(year) == numbers_.end())
      LOG_ERROR(parameters_.location(PARAM_YEARS) << " value (" << year << ") does not have a corresponding entry in the numbers or proportions table");
  }
}
} /* namespace processes */
} /* namespace niwa */
