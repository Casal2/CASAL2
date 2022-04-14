/**
 * @file Tagging.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// Headers
#include "Tagging.h"

#include "Categories/Categories.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "../../Utilities/Math.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {

/**
 * Default constructor
 */
Tagging::Tagging(shared_ptr<Model> model) : Process(model), to_partition_(model), from_partition_(model) {
  process_type_        = ProcessType::kMortality;  
  // Why this was changed from type transition to mortality. CASAL includes this in the 'mortality block'
  // CASAL reference see population_section.cpp line: 1924-2006
  // There is mortality in this process, so does make sense
  partition_structure_ = PartitionType::kLength;

  proportions_table_ = new parameters::Table(PARAM_PROPORTIONS);
  parameters_.Bind<string>(PARAM_FROM, &from_category_labels_, "The categories to transition from", "");
  parameters_.Bind<string>(PARAM_TO, &to_category_labels_, "The categories to transition to", "");
  parameters_.Bind<string>(PARAM_PENALTY, &penalty_label_, "The penalty label", "", "");
  parameters_.Bind<double>(PARAM_U_MAX, &u_max_, "The maximum exploitation rate, U_max", "", 0.99)->set_range(0.0, 1.0, true, false);
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years to execute the transition in", "");
  parameters_.Bind<Double>(PARAM_INITIAL_MORTALITY, &initial_mortality_, "The initial mortality proportion", "", 0.0)->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_INITIAL_MORTALITY_SELECTIVITY, &initial_mortality_selectivity_label_, "The initial mortality selectivity label", "", "");
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The selectivity labels", "");
  parameters_.Bind<Double>(PARAM_N, &n_, "Number of tags (N)", "");
  parameters_.BindTable(PARAM_PROPORTIONS, proportions_table_, "The table of proportions to move", "", true, true);
  parameters_.Bind<double>(PARAM_TOLERANCE, &tolerance_, "Tolerance for checking the specificed proportions sum to one", "", 1e-5)->set_range(0, 1.0);

}

/**
 * Destructor
 */
Tagging::~Tagging() {
  delete proportions_table_;
}

/**
 * Validate the parameters for this process
 *
 * 1. Check for the required parameters
 * 2. Assign our label from the parameters
 * 3. Assign remaining local parameters
 */
void Tagging::DoValidate() {
  LOG_TRACE();
  if (from_category_labels_.size() != to_category_labels_.size()) {
    LOG_ERROR_P(PARAM_TO) << " number of values supplied (" << to_category_labels_.size() << ") does not match the number of from categories provided ("
                          << from_category_labels_.size() << ")";
  }

  // Load data from proportions table using n parameter
  vector<string> columns = proportions_table_->columns();
  if (columns.size() != (model_->get_number_of_length_bins() + 1))
    LOG_ERROR_P(PARAM_PROPORTIONS) << "The number of columns provided (" << columns.size() << ") does not match the model's length bins + 1 (" << (model_->get_number_of_length_bins() + 1) << ")";
  if (columns[0] != PARAM_YEAR)
    LOG_ERROR_P(PARAM_PROPORTIONS) << "The first column label (" << columns[0] << ") provided must be 'year'";

  // build a map of n data by year
  if (n_.size() == 1) {
    auto val_n = n_[0];
    n_.assign(years_.size(), val_n);
  } else if (n_.size() != years_.size())
    LOG_ERROR_P(PARAM_N) << "The number of values provided (" << n_.size() << ") does not match the number of years (" << years_.size() << ")";

  map<unsigned, Double> n_by_year = utilities::Map::create(years_, n_);
  // load our table data in to our map
  vector<vector<string>> data       = proportions_table_->data();
  unsigned               year       = 0;
  Double                 proportion = 0.0;
  for (auto iter : data) {
    if (!utilities::To<unsigned>(iter[0], year))
      LOG_ERROR_P(PARAM_PROPORTIONS) << " value (" << iter[0] << ") could not be converted to an unsigned integer";
    Double total_proportion = 0.0;
    for (unsigned i = 1; i < iter.size(); ++i) {
      if (!utilities::To<Double>(iter[i], proportion))
        LOG_ERROR_P(PARAM_PROPORTIONS) << " value (" << iter[i] << ") could not be converted to a Double";
      if (numbers_[year].size() == 0)
        numbers_[year].resize(model_->get_number_of_length_bins(), 0.0);
      numbers_[year][i - 1] = n_by_year[year] * proportion;
      total_proportion += proportion;
    }
    if(!utilities::math::IsOne(total_proportion))
      LOG_ERROR_P(PARAM_PROPORTIONS) << " total (" << total_proportion << ") do not sum to 1.0 for year " << year;
  }

  for (auto iter : numbers_) {
    if (std::find(years_.begin(), years_.end(), iter.first) == years_.end())
      LOG_ERROR_P(PARAM_PROPORTIONS) << " table contains year " << iter.first << " which is not a valid year defined in this process";
  }
}

/**
 * Build any runtime relationships to other objects in the system.
 */
void Tagging::DoBuild() {
  LOG_TRACE();

  from_partition_.Init(from_category_labels_);
  to_partition_.Init(to_category_labels_);

  if (penalty_label_ != "")
    penalty_ = model_->managers()->penalty()->GetPenalty(penalty_label_);
  else
    LOG_WARNING() << location() << "No penalty has been specified. Exploitation above u_max will not affect the objective function";

  selectivities::Manager& selectivity_manager = *model_->managers()->selectivity();
  for (unsigned i = 0; i < selectivity_labels_.size(); ++i) {
    Selectivity* selectivity = selectivity_manager.GetSelectivity(selectivity_labels_[i]);
    if (!selectivity)
      LOG_ERROR() << "Selectivity label " << selectivity_labels_[i] << " was not found";
    selectivities_[from_category_labels_[i]] = selectivity;
  }
  if (initial_mortality_selectivity_label_ != "")
    initial_mortality_selectivity_ = selectivity_manager.GetSelectivity(initial_mortality_selectivity_label_);

}

/**
 * Execute the constant recruitment process
 */
void Tagging::DoExecute() {
  LOG_TRACE();
  if (model_->current_year() < first_year_)
    return;

  auto from_iter = from_partition_.begin();
  auto to_iter   = to_partition_.begin();

  // Do the transition with mortality on the fish we're moving
  unsigned current_year = model_->current_year();
  if (std::find(years_.begin(), years_.end(), current_year) == years_.end())
    return;

  LOG_FINEST() << "numbers_.size(): " << numbers_.size();
  LOG_FINEST() << "numbers_[current_year].size(): " << numbers_[current_year].size();
  
  for (unsigned i = 0; i < numbers_[current_year].size(); ++i)
    LOG_FINEST() << "numbers_[current_year][" << i << "]: " << numbers_[current_year][i];

  Double total_stock_with_selectivities = 0.0;
  LOG_FINE() << "age_spread: " << model_->get_number_of_length_bins() << " in year " << current_year;

  for (unsigned i = 0; i < model_->get_number_of_length_bins(); ++i) {
    // Calculate the Exploitation rate
    from_iter = from_partition_.begin();
    to_iter   = to_partition_.begin();
    LOG_FINEST() << "selectivity.size(): " << selectivities_.size();
    for (auto iter : selectivities_)
      LOG_FINE() << "selectivity: " << iter.first;

    total_stock_with_selectivities = 0.0;
    for (; from_iter != from_partition_.end(); from_iter++, to_iter++) {
      Double stock_amount = (*from_iter)->data_[i] * selectivities_[(*from_iter)->name_]->GetLengthResult(i);
      total_stock_with_selectivities += stock_amount;

      LOG_FINEST() << "name: " << (*from_iter)->name_ << " at length index " << i;
      LOG_FINEST() << "selectivity value: " << selectivities_[(*from_iter)->name_]->GetLengthResult(i);
      LOG_FINEST() << "population: " << (*from_iter)->data_[i];
      LOG_FINEST() << "amount added to total_stock_with_selectivities: " << stock_amount;
      LOG_FINEST() << "**";
    }
    LOG_FINEST() << "total_stock_with_selectivities: " << total_stock_with_selectivities << " at length ndx " << i;

    // Migrate the exploited amount
    from_iter = from_partition_.begin();
    to_iter   = to_partition_.begin();
    for (; from_iter != from_partition_.end(); from_iter++, to_iter++) {
      LOG_FINE() << "--";
      LOG_FINE() << "Working with categories: from " << (*from_iter)->name_ << "; to " << (*to_iter)->name_;
      string   category_label = (*from_iter)->name_;

      if (numbers_[current_year][i] == 0)
        continue;

      Double current
          = numbers_[current_year][i]
            * ((*from_iter)->data_[i] * selectivities_[category_label]->GetLengthResult(i) / total_stock_with_selectivities);

      Double exploitation
          = current / utilities::math::ZeroFun((*from_iter)->data_[i] * selectivities_[category_label]->GetLengthResult(i));
      if (exploitation > u_max_) {
        LOG_FINE() << "Exploitation(" << exploitation << ") triggered u_max(" << u_max_ << ") with current(" << current << ")";

        current = (*from_iter)->data_[i] * selectivities_[category_label]->GetLengthResult(i) * u_max_;
        LOG_FINE() << "tagging amount overridden with " << current << " = " << (*from_iter)->data_[i] << " * "
                   << selectivities_[category_label]->GetLengthResult(i) << " * " << u_max_;

        if (penalty_)
          penalty_->Trigger(label_, numbers_[current_year][i], current);
      }

      LOG_FINE() << "total_stock_with_selectivities: " << total_stock_with_selectivities;
      LOG_FINE() << "numbers/n: " << numbers_[current_year][i];
      LOG_FINE() << (*from_iter)->name_ << " population at length ndx " << i << ": " << (*from_iter)->data_[i];
      LOG_FINE() << "selectivity: " << selectivities_[category_label]->GetLengthResult(i);
      if (exploitation > u_max_) {
        LOG_FINE() << "exploitation: " << u_max_ << " (u_max)";
        LOG_FINE() << "tagging amount: " << current << " = " << (*from_iter)->data_[i] << " * "
                   << selectivities_[category_label]->GetLengthResult(i) << " * " << u_max_;
      } else {
        LOG_FINE() << "exploitation: " << exploitation << "; calculated as " << current << " / (" << (*from_iter)->data_[i] << " * "
                   << selectivities_[category_label]->GetLengthResult(i) << ")";
        LOG_FINE() << "tagging amount: " << current << " = " << numbers_[current_year][i] << " * " << (*from_iter)->data_[i] << " * "
                   << selectivities_[category_label]->GetLengthResult(i) << " / " << total_stock_with_selectivities;
      }

      if (current <= 0.0)
        continue;

      Double current_with_mortality = current - (current * initial_mortality_);
      LOG_FINEST() << "tagging_amount_with_mortality: " << current_with_mortality << "; initial mortality: " << initial_mortality_;
      if (initial_mortality_selectivity_) {
        current_with_mortality *= initial_mortality_selectivity_->GetLengthResult(i);
        LOG_FINEST() << "tagging_amount_with_mortality: " << current_with_mortality
                     << "; initial_mortality_selectivity : " << initial_mortality_selectivity_->GetLengthResult(i);
      }
      LOG_FINEST() << "Removing " << current << " from " << (*from_iter)->name_;
      LOG_FINEST() << "Adding " << current_with_mortality << " to " << (*to_iter)->name_;
      (*from_iter)->data_[i] -= current;
      (*to_iter)->data_[i] += current_with_mortality;
    }
  }

  for (unsigned year : years_) {
    if (numbers_.find(year) == numbers_.end())
      LOG_ERROR_P(PARAM_YEARS) << " value (" << year << ") does not have a corresponding entry in the numbers or proportions table";
  }
}

} /* namespace length */
}  // namespace processes
} /* namespace niwa */
