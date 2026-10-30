/**
 * @file MortalityFisheryBase.cpp
 * @github https://github.com/Zaita
 * @date 30/10/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "MortalityFisheryBase.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "AgeLengths/AgeLength.h"
#include "AgeWeights/Manager.h"
#include "Categories/Categories.h"
#include "Model/Managers.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "Utilities/Map.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// namespaces
namespace niwa::processes::common {
namespace math = niwa::utilities::math;
using niwa::utilities::findInVector;

/**
 * Default constructor
 */
MortalityFisheryBase::MortalityFisheryBase(shared_ptr<Model> model) : age::Mortality(model), partition_(model) {}

/**
 * Reset the M parameter
 */
void MortalityFisheryBase::DoReset() {
  LOG_TRACE();
  unsigned m_iter = 0;
  for (auto m : m_) {
    LOG_FINEST() << "resetting M for category " << m.first << " = " << m.second;
    m_input_[m_iter] = m.second;
    ++m_iter;
  }
}

/**
 * Called from the time varying class to reset parameters.
 * This avoids a full reset and clearing of printed values
 */
void MortalityFisheryBase::RebuildCache() {
  DoReset();
}

/**
 * The cell of this row in the named column. Only used for columns known to exist.
 */
const string& MortalityFisheryBase::MethodRow::operator[](const string& column) const {
  return cells_[std::find(columns_.begin(), columns_.end(), column) - columns_.begin()];
}

/**
 * Load the M map and both tables into the category and fishery structures
 *
 * @param spec What differs between the method tables of each child
 */
void MortalityFisheryBase::BuildFisheryTables(const FisheryTableSpec& spec) {
  m_ = utilities::OrderedMap<string, Double>::create(category_labels_, m_input_);

  map<string, map<unsigned, Double>> fishery_year_catch = ParseCatchesTable();
  BuildCategoryData();
  ParseMethodTable(spec, fishery_year_catch);

  LOG_FINEST() << "Finishing DoValidate";
}

/**
 * Load a temporary map of the fishery catch data so we can use this
 * when we load our FisheryData objects
 *
 * @return fishery x year = catch
 */
map<string, map<unsigned, Double>> MortalityFisheryBase::ParseCatchesTable() {
  map<string, map<unsigned, Double>> fishery_year_catch;
  auto                               columns = catches_table_->columns();

  if (std::find(columns.begin(), columns.end(), PARAM_YEAR) == columns.end())
    LOG_FATAL_P(PARAM_CATCHES) << "The required column " << PARAM_YEAR << " was not found.";
  unsigned year_index = std::find(columns.begin(), columns.end(), PARAM_YEAR) - columns.begin();
  LOG_FINEST() << "The year_index for catch table is: " << year_index;

  auto model_years = model()->years();
  auto rows        = catches_table_->data();
  for (auto row : rows) {
    unsigned year = 0;
    if (!utilities::To<string, unsigned>(row[year_index], year))
      LOG_ERROR_P(PARAM_CATCHES) << "year value " << row[year_index] << " could not be converted to an unsigned integer.";
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_CATCHES) << "year " << year << " is not a valid year in this model";

    process_years_.push_back(year);

    for (unsigned i = 0; i < row.size(); ++i) {
      if (i == year_index)
        continue;

      Double value = 0.0;
      if (!utilities::To<string, Double>(row[i], value))
        LOG_ERROR_P(PARAM_CATCHES) << "value " << row[i] << " for fishery " << columns[i] << " could not be converted to a Double";
      // Normalised: this check was only in mortality_instantaneous
      if (value < 0)
        LOG_ERROR_P(PARAM_CATCHES) << "value " << row[i] << " for fishery " << columns[i] << " must be >= 0";
      fishery_year_catch[columns[i]][year] = value;
      fishery_catch_[columns[i]][year]     = value;  // needed for Mortality.h checks
    }
  }

  return fishery_year_catch;
}

/**
 * Build all of our category objects
 */
void MortalityFisheryBase::BuildCategoryData() {
  categories_.resize(category_labels_.size());
  for (unsigned i = 0; i < category_labels_.size(); ++i) {
    string       label = category_labels_[i];
    CategoryData category;
    category.category_label_    = label;
    category.m_                 = &m_[label];
    category.selectivity_label_ = selectivity_labels_[i];
    category.category_ndx_      = i;
    categories_[i]              = category;
    category_data_[label]       = &categories_[i];
  }
}

/**
 * Load the information from the method table. We create one FisheryData object for each fishery
 * and one FisheryCategoryData object for each fishery x category. A fishery can be on many rows:
 * Table::Populate() also writes one row per category when the category column is a shorthand such
 * as * or format=male.*
 *
 * @param spec What differs between the method tables of each child
 * @param fishery_year_catch The catches table, as fishery x year = catch
 */
void MortalityFisheryBase::ParseMethodTable(const FisheryTableSpec& spec, map<string, map<unsigned, Double>>& fishery_year_catch) {
  auto columns = method_table_->columns();
  auto rows    = method_table_->data();

  LOG_FINE() << "method_table.columns.size(): " << columns.size();
  LOG_FINE() << "method_table.rows.size(): " << rows.size();
  LOG_FINE() << "columns: " << boost::join(columns, " ");
  for (auto row : rows) {
    LOG_FINE() << "row: " << boost::join(row, " ");
  }

  // Check the column headers are all specified correctly
  vector<string> required_columns = {PARAM_METHOD, PARAM_CATEGORY, PARAM_SELECTIVITY, PARAM_TIME_STEP, PARAM_PENALTY};
  required_columns.insert(required_columns.end(), spec.extra_required_method_columns.begin(), spec.extra_required_method_columns.end());
  for (const string& column : required_columns) {
    if (std::find(columns.begin(), columns.end(), column) == columns.end())
      LOG_FATAL_P(PARAM_METHOD) << "The required column " << column << " was not found.";
  }

  // Users can choose not to add this column if they wish
  if (process_profile_ != ProcessProfile::kAge || std::find(columns.begin(), columns.end(), PARAM_AGE_WEIGHT_LABEL) == columns.end()) {
    use_age_weight_ = false;
    LOG_FINE() << "Age weight column not found";
  }

  unsigned row_number = 1;
  for (const auto& cells : rows) {
    MethodRow row{columns, cells, row_number};

    FisheryData new_fishery;
    new_fishery.label_           = row[PARAM_METHOD];
    new_fishery.time_step_label_ = row[PARAM_TIME_STEP];
    new_fishery.penalty_label_   = row[PARAM_PENALTY];
    ParseFisheryColumns(row, new_fishery);

    std::pair<bool, int> this_fishery_iter = findInVector(fishery_labels_, new_fishery.label_);
    LOG_FINE() << new_fishery.label_ << " found = " << this_fishery_iter.first << " ndx = " << this_fishery_iter.second;

    if (this_fishery_iter.first) {
      // We have already seen this fishery - do consistency checks
      // Normalised: mortality_instantaneous_retained used to re-store the fishery here, so its last row won
      const FisheryData& stored    = fisheries_[new_fishery.label_];
      unsigned           first_row = this_fishery_iter.second + 1;
      if (stored.time_step_label_ != new_fishery.time_step_label_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had timestep label = " << new_fishery.time_step_label_ << " at row " << row_number
                                  << " but the same fishery had time step label = " << stored.time_step_label_ << " at row = " << first_row
                                  << " these need to be consistent for a single method";
      // Normalised: rows disagreeing on the penalty used to silently take the first (last for mortality_instantaneous_retained)
      if (stored.penalty_label_ != new_fishery.penalty_label_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had penalty = " << new_fishery.penalty_label_ << " at row " << row_number
                                  << ", but the same fishery had penalty = " << stored.penalty_label_ << " at row " << first_row
                                  << ". These need to be the same for a single method in the methods table";
      CheckRepeatedFishery(row, stored, new_fishery, first_row);
    } else {
      // Haven't seen this method so store it
      fishery_labels_.push_back(new_fishery.label_);
      new_fishery.fishery_ndx_ = fishery_labels_.size() - 1;

      if (fishery_year_catch.find(new_fishery.label_) == fishery_year_catch.end())
        LOG_ERROR_P(PARAM_METHOD) << "fishery " << new_fishery.label_ << " does not have catch information in the catches table";

      new_fishery.catches_ = fishery_year_catch[new_fishery.label_];
      InitialiseFishery(new_fishery);

      // Store fishery and make it addressable
      FisheryData& stored = fisheries_[new_fishery.label_];
      stored              = new_fishery;
      RegisterAsAddressable(PARAM_METHOD + string("_") + utilities::ToLowercase(new_fishery.label_),
                            spec.retained_catches_addressable ? &stored.retained_catches_ : &stored.catches_);
      LOG_FINEST() << "Creating addressable " << PARAM_METHOD << " : " << PARAM_METHOD + string("_") + utilities::ToLowercase(new_fishery.label_);
    }

    // Move onto category stuff for this method
    vector<string> categories;
    vector<string> selectivities;
    vector<string> age_weights;

    boost::split(categories, row[PARAM_CATEGORY], boost::is_any_of(","));
    boost::split(selectivities, row[PARAM_SELECTIVITY], boost::is_any_of(","));

    for (auto cat : categories) {
      LOG_FINEST() << cat;
    }

    if (categories.size() != selectivities.size())
      LOG_FATAL_P(PARAM_METHOD) << "The number of categories (" << categories.size() << ") and selectivities (" << selectivities.size() << ") provided must be the same";
    if (use_age_weight_) {
      boost::split(age_weights, row[PARAM_AGE_WEIGHT_LABEL], boost::is_any_of(","));
      if (categories.size() != age_weights.size())
        LOG_FATAL_P(PARAM_METHOD) << "The number of categories (" << categories.size() << ") and age weights (" << age_weights.size() << ") provided must be the same";
    }

    // Create fishery category struct
    unsigned first_fishery_category = fishery_categories_.size();
    for (unsigned i = 0; i < categories.size(); ++i) {
      // The method table category can be valid for the model without being one of this process's
      // categories. This has to be checked before category_data_ is used.
      auto category_iter = category_data_.find(categories[i]);
      if (category_iter == category_data_.end()) {
        LOG_ERROR_P(PARAM_METHOD) << "The category " << categories[i] << " was found in the methods table but not in the '" << PARAM_CATEGORIES << "' subcommand."
                                  << " This configuration will apply exploitation processes and not natural mortality, which is not valid."
                                  << " Make sure all categories in the methods table are in the categories subcommand.";
        continue;
      }

      FisheryCategoryData new_category_data(fisheries_[new_fishery.label_], *category_iter->second);
      fishery_category_check_[new_fishery.label_].push_back(categories[i]);

      new_category_data.fishery_label_     = new_fishery.label_;
      new_category_data.category_label_    = categories[i];
      new_category_data.selectivity_label_ = selectivities[i];

      if (use_age_weight_) {
        new_category_data.category_.age_weight_label_ = age_weights[i];
      } else if (process_profile_ == ProcessProfile::kAge) {
        new_category_data.category_.age_weight_label_ = PARAM_NONE;
        LOG_FINE() << "setting age weight label to none.";
      }

      fishery_categories_.push_back(new_category_data);
    }

    ParseFisheryCategoryColumns(row, categories.size(), first_fishery_category);
    ++row_number;
  }
}

/**
 * Parse the fishery's u_max from a method table row
 *
 * @param row The method table row
 * @param fishery The fishery to populate
 */
void MortalityFisheryBase::ParseUMax(const MethodRow& row, FisheryData& fishery) {
  if (!utilities::To<string, Double>(row[PARAM_U_MAX], fishery.u_max_))
    LOG_ERROR_P(PARAM_METHOD) << "u_max value " << row[PARAM_U_MAX] << " could not be converted to a Double";
}

/**
 * Check a repeated fishery has the same u_max
 *
 * @param row The method table row
 * @param stored The fishery from the row it was first seen on
 * @param repeat The fishery from this row
 * @param first_row The row the fishery was first seen on
 */
void MortalityFisheryBase::CheckRepeatedUMax(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) {
  if (stored.u_max_ != repeat.u_max_)
    LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << repeat.label_ << " had u_max = " << repeat.u_max_ << " at row " << row.number_
                              << ", but the same fishery had u_max = " << stored.u_max_ << " at row " << first_row
                              << ". These need to be the same for a single method in the methods table";
}

/**
 * @param category The category
 * @return The number of partition bins in the category
 */
unsigned MortalityFisheryBase::CategoryBinCount(const CategoryData& category) {
  return (process_profile_ == ProcessProfile::kAge) ? category.category_->age_spread() : model()->get_number_of_length_bins();
}

/**
 * Bind our categories to the partition and size the selectivity containers
 */
void MortalityFisheryBase::BindCategories() {
  partition_.Init(category_labels_);

  for (auto& category : categories_) {
    category.category_ = &model()->partition().category(category.category_label_);
    category.selectivity_values_.assign(CategoryBinCount(category), 0.0);
  }

  for (auto& fishery_category : fishery_categories_) {
    fishery_category.selectivity_values_.assign(CategoryBinCount(fishery_category.category_), 0.0);
  }
}

/**
 * Organise our time step ratios. Each time step can apply a different ratio of M so here we want
 * to verify we have enough
 *
 * @param proportions_are_optional When there are none, apply all of M in each time step
 */
void MortalityFisheryBase::BuildTimeStepRatios(bool proportions_are_optional) {
  vector<TimeStep*> time_steps = model()->managers()->time_step()->ordered_time_steps();
  vector<unsigned>  active_time_steps;
  for (unsigned i = 0; i < time_steps.size(); ++i) {
    if (time_steps[i]->HasProcess(label_))
      active_time_steps.push_back(i);
  }

  if (proportions_are_optional && time_step_ratios_temp_.size() == 0) {
    for (unsigned i : active_time_steps) time_step_ratios_[i] = 1.0;
    return;
  }

  if (time_step_ratios_temp_.size() != active_time_steps.size())
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << " The time step proportions length (" << time_step_ratios_temp_.size()
                                             << ") does not match the number of time steps this process has been assigned to (" << active_time_steps.size() << ")";
  Double total_val = 0.0;
  for (Double value : time_step_ratios_temp_) {
    total_val += value;
  }
  if (!math::IsOne(total_val)) {
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << " needs to sum to one";
  }

  for (unsigned i = 0; i < time_step_ratios_temp_.size(); ++i) time_step_ratios_[active_time_steps[i]] = time_step_ratios_temp_[i];
}

/**
 * Assign the selectivity, penalty and time step index to each fishery data object
 */
void MortalityFisheryBase::ResolveFisheries() {
  for (auto& fishery_category : fishery_categories_) {
    fishery_category.selectivity_ = model()->managers()->selectivity()->GetSelectivity(fishery_category.selectivity_label_);
    if (!model()->categories()->IsValid(fishery_category.category_label_))
      LOG_ERROR_P(PARAM_METHOD) << ": Fishery category " << fishery_category.category_label_ << " was not found.";

    if (!fishery_category.selectivity_)
      LOG_ERROR_P(PARAM_METHOD) << ": Fishery selectivity " << fishery_category.selectivity_label_ << " was not found.";
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    if (fishery.penalty_label_ != "none") {
      fishery.penalty_ = model()->managers()->penalty()->GetProcessPenalty(fishery.penalty_label_);
      if (!fishery.penalty_)
        LOG_ERROR_P(PARAM_METHOD) << ": Penalty label " << fishery.penalty_label_ << " was not found.";
    }
    bool check_time_step = model()->managers()->time_step()->CheckTimeStep(fishery.time_step_label_);
    if (!check_time_step)
      LOG_FATAL_P(PARAM_METHOD) << "Time step label " << fishery.time_step_label_ << " was not found.";
    fishery.time_step_index_ = model()->managers()->time_step()->GetTimeStepIndex(fishery.time_step_label_);
  }
}

/**
 * Check the natural mortality categories are valid, and assign their selectivities and age weights
 */
void MortalityFisheryBase::ResolveCategories() {
  for (const string& label : category_labels_) {
    if (!model()->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": Category " << label << " was not found.";
  }

  for (auto& category : categories_) {
    Selectivity* selectivity = model()->managers()->selectivity()->GetSelectivity(category.selectivity_label_);
    // PARAM_SELECTIVITIES is the bound label: location() does not resolve the relative_m_by_* aliases
    if (!selectivity) {
      if (process_profile_ == ProcessProfile::kAge) {
        LOG_ERROR_P(PARAM_SELECTIVITIES) << "M-by-age ogive label " << category.selectivity_label_ << " was not found.";
      } else {
        LOG_ERROR_P(PARAM_SELECTIVITIES) << "M-by-length ogive label " << category.selectivity_label_ << " was not found.";
      }
    }
    category.selectivity_ = selectivity;
    selectivities_.push_back(selectivity);

    // Age Weight if it is defined
    if (process_profile_ == ProcessProfile::kAge) {
      LOG_FINEST() << "age weight " << category.age_weight_label_;
      if ((category.age_weight_label_ == PARAM_NONE) || (category.age_weight_label_ == "")) {
        category.age_weight_label_ = PARAM_NONE;
        category.age_weight_       = nullptr;
        use_age_weight_            = false;
      } else {
        LOG_FINE() << "age weight found";
        AgeWeight* age_weight = model()->managers()->age_weight()->FindAgeWeight(category.age_weight_label_);
        if (!age_weight)
          LOG_ERROR_P(PARAM_METHOD) << "age weight label " << category.age_weight_label_ << " was not found.";
        category.age_weight_ = age_weight;
        use_age_weight_      = true;
      }
    }
  }
}

/**
 * Find the time steps with a process of this type but none of our fisheries, as we can skip the
 * exploitation code there i.e no F just M
 *
 * @param process_type The PARAM_ type string of the child
 */
void MortalityFisheryBase::BuildSkippedFTimeSteps(const string& process_type) {
  vector<unsigned> instant_mort_time_step;
  unsigned         i = 0;
  for (auto time_step : model()->managers()->time_step()->ordered_time_steps()) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kMortality && process->type() == process_type) {
        LOG_FINEST() << process_type << " process in time step " << i;
        instant_mort_time_step.push_back(i);
      }
    }
    ++i;
  }

  for (auto time_step : instant_mort_time_step) {
    bool fishery_in_timestep = false;
    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "checking fishery " << fishery_iter.first << " in time step index " << fishery_iter.second.time_step_index_;
      if (fishery_iter.second.time_step_index_ == time_step) {
        LOG_FINEST() << "fishery " << fishery_iter.first << " is in time step " << time_step;
        fishery_in_timestep = true;
      }
    }
    if (!fishery_in_timestep) {
      time_steps_to_skip_applying_F_mortality_.push_back(time_step);
      LOG_FINEST() << "The time step " << time_step << " does not have a method associated so the exploitation calculation will be skipped during DoExecute";
    }
  }
}

/**
 * Reserve memory for removals_by_year_fishery_category_
 *
 * @param bin_count The number of ages or length bins
 */
void MortalityFisheryBase::AllocateRemovals(unsigned bin_count) {
  LOG_FINE() << "years " << process_years_.size();
  LOG_FINE() << "time steps = " << model()->managers()->time_step()->ordered_time_steps().size();
  LOG_FINE() << "partitions = " << partition_.size();

  removals_by_year_fishery_category_.resize(process_years_.size());
  for (unsigned year_ndx = 0; year_ndx < process_years_.size(); ++year_ndx) {
    removals_by_year_fishery_category_[year_ndx].resize(fisheries_.size());
    for (unsigned fishery_ndx = 0; fishery_ndx < fisheries_.size(); ++fishery_ndx) {
      removals_by_year_fishery_category_[year_ndx][fishery_ndx].resize(category_labels_.size());
      for (unsigned category_ndx = 0; category_ndx < category_labels_.size(); ++category_ndx)
        removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx].resize(bin_count, 0.0);
    }
  }
}

}  // namespace niwa::processes::common
