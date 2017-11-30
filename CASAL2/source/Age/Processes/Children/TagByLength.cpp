/**
 * @file TagByLength.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "TagByLength.h"

#include "Common/Categories/Categories.h"
#include "Common/Selectivities/Manager.h"
#include "Common/Penalties/Manager.h"
#include "Common/Utilities/DoubleCompare.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

// namespaces
namespace niwa {
namespace age {
namespace processes {

/**
 *
 */
TagByLength::TagByLength(Model* model)
  : Process(model),
    to_partition_(model),
    from_partition_(model) {
  process_type_ = ProcessType::kTransition;
  partition_structure_ = PartitionType::kAge;

  numbers_table_ = new parameters::Table(PARAM_NUMBERS);
  proportions_table_ = new parameters::Table(PARAM_PROPORTIONS);

  parameters_.Bind<string>(PARAM_FROM, &from_category_labels_, "Categories to transition from", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_labels_, "ategories to transition to", "");
  parameters_.Bind<bool>(PARAM_PLUS_GROUP, &plus_group_, "Use plus group for last length bin", "", false);
  parameters_.Bind<Double>(PARAM_MAX_LENGTH, &max_length_, "The upper length when there is no plus group", "",Double(0));
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "Penalty label", "", "");
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "U Max", "", 0.99);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to execute the transition in", "");
  parameters_.Bind<Double>(PARAM_INITIAL_MORTALITY, &initial_mortality_, "", "", Double(0));
  parameters_.Bind<string>(PARAM_INITIAL_MORTALITY_SELECTIVITY, &initial_mortality_selectivity_label_, "", "", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "", "");
  parameters_.Bind<Double>(PARAM_N, &n_, "", "", true);
  parameters_.BindTable(PARAM_NUMBERS, numbers_table_, "Table of N data", "", true, true);
  parameters_.BindTable(PARAM_PROPORTIONS, proportions_table_, "Table of proportions to move", "" , true, true);

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
  // TODO add a year specific initial mortality and N input for the user.
  LOG_TRACE();
  // Check if the user has specified combined categories, if so check the same number of categories are
  for(auto& category : to_category_labels_) {
    bool check_combined = model_->categories()->IsCombinedLabels(category);
    if(check_combined)
      LOG_FATAL_P(PARAM_TO) << "You supplied the combined category " << category << " this sub command can only take separate categories.";
  }

  if (to_category_labels_.size() != 1) {
    LOG_FATAL_P(PARAM_FROM) << "This process cannot specify a many to many tagging event. If you have proportions tagged by category then I suggest you create @tag process for each seperate category, see the manual for more details.";
  }

  vector<string> split_category_labels;
  unsigned no_combined = 0;
  for(auto category : from_category_labels_) {
    if (model_->categories()->IsCombinedLabels(category)) {
      no_combined= model_->categories()->GetNumberOfCategoriesDefined(category);
      if (no_combined != to_category_labels_.size()) {
        LOG_ERROR_P(PARAM_TO) << "you specified '" << no_combined << "' combined from_categories where as you suppled '" << to_category_labels_.size() << "' to categories. you must have an equal number of categories from to";
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
      for (auto& split_category : split_category_labels)
        split_from_category_labels_.push_back(split_category);
    } else
      split_from_category_labels_.push_back(category);
  }

  LOG_FINEST() << "from categories";
  for(auto category : split_from_category_labels_) {
    LOG_FINEST() << category;
  }

  if (split_from_category_labels_.size() != to_category_labels_.size()) {
    LOG_ERROR_P(PARAM_TO) << " number of values supplied (" << to_category_labels_.size()
        << ") does not match the number of from categories provided (" << split_from_category_labels_.size() << ")";
  }

  if (to_category_labels_.size() != selectivity_labels_.size())
    LOG_ERROR_P(PARAM_SELECTIVITIES) << "there must be the same number of selectivities as 'to_categories'. You supplied " << to_category_labels_.size() << " 'to_categories' but " << selectivity_labels_.size() << " selectivity labels";
  if (u_max_ <= 0.0 || u_max_ > 1.0)
    LOG_ERROR_P(PARAM_U_MAX) << " (" << u_max_ << ") must be greater than 0.0 and less than 1.0";
  if (!plus_group_ && max_length_ == 0.0)
    LOG_ERROR_P(PARAM_MAX_LENGTH) << "Must supply parameter " << PARAM_MAX_LENGTH << " when plus group is false";
  if (plus_group_ && max_length_ != 0.0)
    LOG_WARNING() << "You have specified parameter " << PARAM_MAX_LENGTH << " and set plus group = true. " << PARAM_MAX_LENGTH
          << " will be ignored";

  /**
   * Get our first year
   */
  first_year_ = years_[0];
  std::for_each(years_.begin(), years_.end(), [this](unsigned year){ first_year_ = year < first_year_ ? year : first_year_; });

  /**
   * Build our tables
   */
  if (numbers_table_->row_count() == 0 && proportions_table_->row_count() == 0)
    LOG_ERROR() << location() << " must have either a " << PARAM_NUMBERS << " or " << PARAM_PROPORTIONS << " table defined with appropriate data";
  if (numbers_table_->row_count() != 0 && proportions_table_->row_count() != 0)
    LOG_ERROR() << location() << " cannot have both a " << PARAM_NUMBERS << " and " << PARAM_PROPORTIONS << " table defined. Please only use 1.";
  if (proportions_table_->row_count() != 0 && !parameters_.Get(PARAM_N)->has_been_defined())
    LOG_ERROR() << location() << " cannot have a " << PARAM_PROPORTIONS << " table without defining " << PARAM_N;

  /**
   * Load our N data in to the map
   */
  if (numbers_table_->row_count() != 0) {
    vector<string> columns = numbers_table_->columns();
    if (columns[0] != PARAM_YEAR)
      LOG_ERROR_P(PARAM_NUMBERS) << " first column label (" << columns[0] << ") provided must be 'year'";

    unsigned number_bins = columns.size();

    for (unsigned i = 1; i < number_bins; ++i) {
      Double length = 0;
      if (!utilities::To<Double>(columns[i], length))
        LOG_ERROR() << "Could not convert value " << columns[i] << " to Double";
      length_bins_.push_back(length);
    }
    if (!plus_group_)
      length_bins_.push_back(max_length_);


    // load our table data in to our map
    vector<vector<string>> data = numbers_table_->data();
    unsigned year = 0;
    Double n_value = 0.0;
    for (auto iter : data) {
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR_P(PARAM_NUMBERS) << " value (" << iter[0] << ") is not a valid unsigned value that could be converted to a model year";

      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], n_value))
          LOG_ERROR_P(PARAM_NUMBERS) << " value (" << iter[i] << ") could not be converted to a double. Please ensure it's a numeric value";
        if (numbers_[year].size() == 0)
          numbers_[year].resize(number_bins, 0.0);
        numbers_[year][i - 1] = n_value;
      }
    }
    // Check years allign
    for (auto iter : numbers_) {
      if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
        LOG_ERROR_P(PARAM_NUMBERS) << " table contains year " << iter.first << " that is not a valid year defined in this process";
    }

  } else if (proportions_table_->row_count() != 0) {
    /**
     * Load data from proportions table using n parameter
     */
    vector<string> columns = proportions_table_->columns();
    if (columns[0] != PARAM_YEAR)
      LOG_ERROR_P(PARAM_PROPORTIONS) << " first column label (" << columns[0] << ") provided must be 'year'";
    unsigned number_bins = columns.size();

    for (unsigned i = 1; i < number_bins; ++i) {
      Double length = 0;
      if (!utilities::To<Double>(columns[i], length))
        LOG_ERROR() << "Could not convert value " << columns[i] << " to Double";
      length_bins_.push_back(length);
    }
    if (!plus_group_)
      length_bins_.push_back(max_length_);

    // build a map of n data by year
    if (n_.size() == 1)
      n_.assign(years_.size(), n_[0]);
    else if (n_.size() != years_.size())
      LOG_ERROR_P(PARAM_N) << " values provied (" << n_.size() << ") does not match the number of years (" << years_.size() << ")";
    map<unsigned, Double> n_by_year = utilities::Map::create(years_, n_);

    // load our table data in to our map
    vector<vector<string>> data = proportions_table_->data();
    unsigned year = 0;
    Double proportion = 0.0;
    for (auto iter : data) {
      if (!utilities::To<unsigned>(iter[0], year))
        LOG_ERROR_P(PARAM_PROPORTIONS) << " value (" << iter[0] << ") is not a valid unsigned value that could be converted to a model year";
      Double total_proportion = 0.0;
      for (unsigned i = 1; i < iter.size(); ++i) {
        if (!utilities::To<Double>(iter[i], proportion))
          LOG_ERROR_P(PARAM_PROPORTIONS) << " value (" << iter[i] << ") could not be converted to a double. Please ensure it's a numeric value";
        if (numbers_[year].size() == 0)
          numbers_[year].resize(number_bins, 0.0);
        numbers_[year][i - 1] = n_by_year[year] * proportion;
        total_proportion += proportion;
      }
      if (fabs(1.0 - total_proportion) > 0.001)
        LOG_ERROR_P(PARAM_PROPORTIONS) << " total (" << total_proportion << ") is not 1.0 for year " << year;
    }
    // Check years allign
    for (auto iter : numbers_) {
      if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
        LOG_ERROR_P(PARAM_PROPORTIONS) << " table contains year " << iter.first << " that is not a valid year defined in this process";
    }
  }

  // Check value for initial mortality
  if ((initial_mortality_ < 0) | (initial_mortality_ > 1.0))
    LOG_ERROR_P(PARAM_INITIAL_MORTALITY) << ": must be between 0.0 (inclusive) amd less than 1.0 (inclusive)";

}

/**
 * Build relationships between this object and others
 */
void TagByLength::DoBuild() {
  LOG_TRACE();
  LOG_FINEST() << "Initialising from categories";
  from_partition_.Init(split_from_category_labels_);
  LOG_FINEST() << "Initialising to categories";
  to_partition_.Init(to_category_labels_);

  if (penalty_label_ != "")
    penalty_ = model_->managers().penalty()->GetPenalty(penalty_label_);
  else
    LOG_WARNING() << location() << " no penalty has been specified. Exploitation above u_max will not affect the objective function";

  selectivities::Manager& selectivity_manager = *model_->managers().selectivity();
  for (unsigned i = 0; i < selectivity_labels_.size(); ++i) {
    Selectivity* selectivity = selectivity_manager.GetSelectivity(selectivity_labels_[i]);
    if (!selectivity)
      LOG_FATAL_P(PARAM_SELECTIVITIES) << "Selectivity: " << selectivity_labels_[i] << " not found";
    selectivities_[split_from_category_labels_[i]] = selectivity;
  }
  if (initial_mortality_selectivity_label_ != "")
    initial_mortality_selectivity_ = selectivity_manager.GetSelectivity(initial_mortality_selectivity_label_);
}

/**
 * Execute this process
 */
void TagByLength::DoExecute() {
  LOG_TRACE();
  unsigned current_year = model_->current_year();
  if (std::find(years_.begin(), years_.end(), current_year) == years_.end())
    return;

  auto from_iter = from_partition_.begin();
  auto to_iter   = to_partition_.begin();
  /**
   * Do the transition with mortality on the fish we're moving
   */
  LOG_FINEST() << "numbers_.size(): " << numbers_.size();
  LOG_FINEST() << "numbers_[current_year].size(): " << numbers_[current_year].size();
  for (unsigned i = 0; i < numbers_[current_year].size(); ++i)
    LOG_FINEST() << "numbers_[current_year][" << i << "]: " << numbers_[current_year][i];

  Double total_stock_with_selectivities = 0.0;
  unsigned number_bins = length_bins_.size();
  if (!plus_group_)
    number_bins -= 1;

  LOG_FINE() << "number of length bins: " << number_bins << " in year " << current_year;

  map<string, vector<Double>> numbers_at_age_by_category;

  // iterate over from_categories to update length data and age length matrix instead of doing in a length loop
  for (; from_iter != from_partition_.end(); from_iter++) {
    //(*from_iter)->UpdateMeanLengthData();
    //  build numbers at age and length
    (*from_iter)->UpdateAgeLengthData(length_bins_, plus_group_, selectivities_[(*from_iter)->name_]);
    //  total numbers at length
    (*from_iter)->CollapseAgeLengthDataToLength();
    numbers_at_age_by_category[(*from_iter)->name_].resize((*from_iter)->data_.size(),0.0);
  }



  // Calculate the exploitation rate by length bin
  for (unsigned i = 0; i <  length_bins_.size(); ++i) {
   // Only continue if we have fish to tag in this length bin.
   if (numbers_[current_year][i] == 0.0)
      continue;

    /**
     * Calculate the vulnerable abundance to the tagging event in this length bin
     */
    from_iter = from_partition_.begin();
    total_stock_with_selectivities = 0.0;
    for (; from_iter != from_partition_.end(); from_iter++) {
      total_stock_with_selectivities += (*from_iter)->length_data_[i];
    }
    LOG_FINEST() << "total_stock_with_selectivities: " << total_stock_with_selectivities << " at length " << length_bins_[i];

    /**
     * Migrate the exploited amount using a method analagous to exploitation rate.
     */
    from_iter = from_partition_.begin();
    for (; from_iter != from_partition_.end(); from_iter++) {
      LOG_FINE() << "--";
      LOG_FINE() << "Working with categories: from " << (*from_iter)->name_;
      string category_label = (*from_iter)->name_;



      Double proportion_in_this_category_by_length = (*from_iter)->length_data_[i] / total_stock_with_selectivities;
      //Double current = numbers_[current_year][i] * ((*from_iter)->length_data_[i] / total_stock_with_selectivities);
      Double exploitation_by_length =  numbers_[current_year][i] / total_stock_with_selectivities;
      Double exploitation_by_length_and_category = exploitation_by_length * proportion_in_this_category_by_length;
      Double tagged_fish_for_this_category = exploitation_by_length_and_category * total_stock_with_selectivities;

      //Double exploitation = current / utilities::doublecompare::ZeroFun((*from_iter)->length_data_[i]);
      if (exploitation_by_length_and_category > u_max_) {
        LOG_FINE() << "Exploitation for length " << length_bins_[i] << " = (" << exploitation_by_length_and_category << ") triggered u_max(" << u_max_;

        exploitation_by_length_and_category = u_max_;
        Double current = (*from_iter)->length_data_[i] *  u_max_;
        LOG_FINE() << "tried to tag " << tagged_fish_for_this_category << " tagging amount overridden with " << current << " = " << (*from_iter)->length_data_[i] << " * " << u_max_;

        if (penalty_)
          penalty_->Trigger(label_, tagged_fish_for_this_category, current);
      }
      LOG_FINE() << "proportion for length " << length_bins_[i] << " = " << proportion_in_this_category_by_length << " tagged fish = " << tagged_fish_for_this_category << " exploitation = " << exploitation_by_length_and_category;

      LOG_FINE() << "numbers: " << numbers_[current_year][i] << " total = " << total_stock_with_selectivities;
      LOG_FINE() << (*from_iter)->name_ << " population at length " << length_bins_[i] << ": " << (*from_iter)->length_data_[i];

      //vector<Double> numbers_at_age((*from_iter)->data_.size(), 0.0);
      for (unsigned j = 0; j < (*from_iter)->data_.size(); ++j) {
        numbers_at_age_by_category[(*from_iter)->name_][j] += (*from_iter)->age_length_matrix_[j][i] * exploitation_by_length_and_category;
      }

    }
  }
  from_iter = from_partition_.begin();
  LOG_FINE() << "initial mortality = " << initial_mortality_ << " label = " << label_;
  for (; from_iter != from_partition_.end(); from_iter++, to_iter++) {
    LOG_FINEST() << "from category = " << (*from_iter)->name_ << " to category = "<< (*to_iter)->name_;
    for (unsigned j = 0; j < (*from_iter)->data_.size(); ++j) {
      (*from_iter)->data_[j] -= numbers_at_age_by_category[(*from_iter)->name_][j];
      (*to_iter)->data_[j] += numbers_at_age_by_category[(*from_iter)->name_][j];
      // Apply the Initial mortality and tag loss after the tagging event
      if((initial_mortality_selectivity_label_ != "") & (initial_mortality_ > 0.0))
        (*to_iter)->data_[j] -=  numbers_at_age_by_category[(*from_iter)->name_][j] * initial_mortality_ * initial_mortality_selectivity_->GetAgeResult((*to_iter)->min_age_ + j, (*to_iter)->age_length_);
      else if((initial_mortality_selectivity_label_ == "") & (initial_mortality_ > 0.0))
        (*to_iter)->data_[j] -= numbers_at_age_by_category[(*from_iter)->name_][j] * initial_mortality_;

      LOG_FINEST() << "age = " << j + model_->min_age() << " = " << numbers_at_age_by_category[(*from_iter)->name_][j] << " after init mort = " << (*to_iter)->data_[j];

    }
  }
}

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */
