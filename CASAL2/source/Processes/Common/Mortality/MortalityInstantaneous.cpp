/**
 * @file MortalityInstantaneous.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * Consolidated implementation for both Age and Length partitioned models.
 */

// headers
#include "MortalityInstantaneous.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "AgeLengths/AgeLength.h"
#include "AgeWeights/Manager.h"
#include "Categories/Categories.h"
#include "GrowthIncrements/GrowthIncrement.h"
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
MortalityInstantaneous::MortalityInstantaneous(shared_ptr<Model> model) : age::Mortality(model), partition_(model) {
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge | PartitionType::kLength;

  catches_table_ = parameters_.BindTable(PARAM_CATCHES, "The table of removals (catch) data");
  catches_table_->set_required_columns({PARAM_YEAR}, true);
  method_table_ = parameters_.BindTable(PARAM_METHOD, "The table of method of removal data");
  // Note: Not using set_required_columns here because Age models have optional columns (age_weight_label)
  // Columns are validated manually in DoValidate

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories for instantaneous mortality")->flag_is_category();
  parameters_.Bind<Double>(PARAM_M, &m_input_, "The natural mortality rates for each category");
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_ratios_temp_, "The time step proportions for natural mortality");
  parameters_.Bind<bool>(PARAM_BIOMASS, &is_catch_biomass_, "Indicator to denote if the catches are as biomass (true) or abundance (false)")->set_default_value(true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The M-by-age/length ogives to apply to each category for natural mortality")
      ->set_alias_labels({PARAM_RELATIVE_M_BY_AGE, PARAM_RELATIVE_M_BY_LENGTH});

  RegisterAsAddressable(PARAM_M, &m_);
}

/**
 * Validate the process
 */
void MortalityInstantaneous::DoValidate() {
  if (process_profile_ == ProcessProfile::kAge) {
    parameters_.ValidateVector(PARAM_M)
        ->GreaterThanOrEqualTo(0.0)
        ->LessThanOrEqualTo(1.0)
        ->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)
        ->SameNumberOfElementsAs(PARAM_CATEGORIES);
  } else {
    parameters_.ValidateVector(PARAM_M)->GreaterThanOrEqualTo(0.0)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);
  }
  parameters_.ValidateVector(PARAM_TIME_STEP_PROPORTIONS)->GreaterThanOrEqualTo(0.0)->LessThanOrEqualTo(1.0)->SumToOne();
  parameters_.ValidateVector(PARAM_SELECTIVITIES)->ExpandToSameNumberOfElementsAs(PARAM_CATEGORIES)->SameNumberOfElementsAs(PARAM_CATEGORIES);

  m_ = utilities::OrderedMap<string, Double>::create(category_labels_, m_input_);

  /**
   * Load a temporary map of the fishery catch data
   */
  map<string, map<unsigned, Double>> fishery_year_catch;
  auto                               catch_columns = catches_table_->columns();

  if (std::find(catch_columns.begin(), catch_columns.end(), PARAM_YEAR) == catch_columns.end())
    LOG_ERROR_P(PARAM_CATCHES) << "The required column " << PARAM_YEAR << " was not found.";
  unsigned catch_year_index = std::find(catch_columns.begin(), catch_columns.end(), PARAM_YEAR) - catch_columns.begin();
  LOG_FINEST() << "The catch_year_index for catch table is: " << catch_year_index;

  auto model_years  = model()->years();
  auto catches_rows = catches_table_->data();
  for (auto row : catches_rows) {
    unsigned year = 0;
    if (!utilities::To<string, unsigned>(row[catch_year_index], year))
      LOG_ERROR_P(PARAM_CATCHES) << "year value " << row[catch_year_index] << " could not be converted to an unsigned integer.";
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_CATCHES) << "year " << year << " is not a valid year in this model";

    process_years_.push_back(year);

    for (unsigned i = 0; i < row.size(); ++i) {
      if (i == catch_year_index)
        continue;

      Double value = 0.0;
      if (!utilities::To<string, Double>(row[i], value))
        LOG_ERROR_P(PARAM_CATCHES) << "value " << row[i] << " for fishery " << catch_columns[i] << " could not be converted to a Double";
      if (value < 0)
        LOG_ERROR_P(PARAM_CATCHES) << "value " << row[i] << " for fishery " << catch_columns[i] << " must be >= 0";
      fishery_year_catch[catch_columns[i]][year] = value;
      if (process_profile_ == ProcessProfile::kAge)
        fishery_catch_[catch_columns[i]][year] = value;
    }
  }

  /**
   * Build all of our category objects
   */
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

  /**
   * Load the information from the methods table
   */
  auto method_columns = method_table_->columns();
  auto method_rows    = method_table_->data();

  LOG_FINE() << "method_table.method_columns.size(): " << method_columns.size();
  LOG_FINE() << "method_table.method_rows.size(): " << method_rows.size();
  LOG_FINE() << "columns: " << boost::join(method_columns, " ");
  for (auto row : method_rows) {
    LOG_FINE() << "row: " << boost::join(row, " ");
  }

  // Check the column headers are all specified correctly
  if (std::find(method_columns.begin(), method_columns.end(), PARAM_METHOD) == method_columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_METHOD << " was not found.";
  if (std::find(method_columns.begin(), method_columns.end(), PARAM_CATEGORY) == method_columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_CATEGORY << " was not found.";
  if (std::find(method_columns.begin(), method_columns.end(), PARAM_SELECTIVITY) == method_columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_SELECTIVITY << " was not found.";
  if (std::find(method_columns.begin(), method_columns.end(), PARAM_TIME_STEP) == method_columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_TIME_STEP << " was not found.";
  if (std::find(method_columns.begin(), method_columns.end(), PARAM_U_MAX) == method_columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_U_MAX << " was not found.";
  if (std::find(method_columns.begin(), method_columns.end(), PARAM_PENALTY) == method_columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_PENALTY << " was not found.";

  // Age-specific optional columns
  if (process_profile_ == ProcessProfile::kAge) {
    if (std::find(method_columns.begin(), method_columns.end(), PARAM_BIOMASS) == method_columns.end()) {
      use_catch_biomass_ = false;
      LOG_FINE() << "The optional column " << PARAM_BIOMASS << " was not found";
    }
    if (std::find(method_columns.begin(), method_columns.end(), PARAM_U) == method_columns.end()) {
      use_u_ = false;
      LOG_FINE() << "The optional column " PARAM_U << " was not found";
    }
    if (std::find(method_columns.begin(), method_columns.end(), PARAM_AGE_WEIGHT_LABEL) == method_columns.end()) {
      use_age_weight_ = false;
      LOG_FINE() << "The optional column " PARAM_AGE_WEIGHT_LABEL << "was not found";
    }
  } else {
    use_catch_biomass_ = false;
    use_u_             = false;
    use_age_weight_    = false;
  }

  unsigned fishery_index     = std::find(method_columns.begin(), method_columns.end(), PARAM_METHOD) - method_columns.begin();
  unsigned category_index    = std::find(method_columns.begin(), method_columns.end(), PARAM_CATEGORY) - method_columns.begin();
  unsigned selectivity_index = std::find(method_columns.begin(), method_columns.end(), PARAM_SELECTIVITY) - method_columns.begin();
  unsigned time_step_index   = std::find(method_columns.begin(), method_columns.end(), PARAM_TIME_STEP) - method_columns.begin();
  unsigned u_max_index       = std::find(method_columns.begin(), method_columns.end(), PARAM_U_MAX) - method_columns.begin();
  unsigned penalty_index     = std::find(method_columns.begin(), method_columns.end(), PARAM_PENALTY) - method_columns.begin();
  unsigned biomass_index     = 999;
  unsigned u_index           = 999;
  unsigned age_weight_index  = 999;
  if (use_catch_biomass_)
    biomass_index = std::find(method_columns.begin(), method_columns.end(), PARAM_BIOMASS) - method_columns.begin();
  if (use_u_)
    u_index = std::find(method_columns.begin(), method_columns.end(), PARAM_U) - method_columns.begin();
  if (use_age_weight_)
    age_weight_index = std::find(method_columns.begin(), method_columns.end(), PARAM_AGE_WEIGHT_LABEL) - method_columns.begin();

  LOG_FINEST() << "indexes: fishery=" << fishery_index << "; category=" << category_index << "; selectivity=" << selectivity_index << "; time_step=" << time_step_index
               << "; u_max=" << u_max_index << "; penalty " << penalty_index;

  // Business rule check: a fishery can only exist in one time-step per year
  map<string, vector<string>> fishery_time_step;
  vector<Double>              empty_year_vector(process_years_.size(), 0.0);

  unsigned row_iter = 1;
  for (auto row : method_rows) {
    FisheryData new_fishery;
    new_fishery.label_           = row[fishery_index];
    new_fishery.time_step_label_ = row[time_step_index];
    new_fishery.penalty_label_   = row[penalty_index];
    new_fishery.years_           = process_years_;

    std::pair<bool, int> this_fishery_iter = findInVector(fishery_labels_, new_fishery.label_);
    LOG_FINE() << new_fishery.label_ << " found = " << this_fishery_iter.first << " ndx = " << this_fishery_iter.second;

    if (!utilities::To<string, Double>(row[u_max_index], new_fishery.u_max_))
      LOG_ERROR_P(PARAM_METHOD) << "u_max value " << row[u_max_index] << " could not be converted to a Double";

    // Age-specific: parse biomass and U flags
    if (process_profile_ == ProcessProfile::kAge) {
      if (use_catch_biomass_) {
        if (!(utilities::To(row[biomass_index], new_fishery.catch_as_biomass_)))
          LOG_ERROR_P(PARAM_METHOD) << ": in row = " << row_iter << ", unable to convert '" << row[biomass_index] << "' to a boolean (true/false)";
        LOG_FINE() << "row[biomass_index] = " << row[biomass_index] << ", new_fishery.catch_as_biomass_ = " << new_fishery.catch_as_biomass_;
      } else {
        new_fishery.catch_as_biomass_ = true;
      }
      if (use_u_) {
        if (!(utilities::To(row[u_index], new_fishery.catch_as_u_)))
          LOG_ERROR_P(PARAM_METHOD) << ": in row = " << row_iter << ", unable to convert '" << row[u_index] << "' to a boolean (true/false) ";
      } else {
        new_fishery.catch_as_u_ = false;
      }
    }

    if (this_fishery_iter.first) {
      // We have already seen this fishery - do consistency checks
      if (fisheries_[new_fishery.label_].time_step_label_ != new_fishery.time_step_label_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had timestep label = " << new_fishery.time_step_label_ << " at row " << row_iter
                                  << " but the same fishery had time step label = " << fisheries_[new_fishery.label_].time_step_label_
                                  << " at row = " << this_fishery_iter.second + 1 << " these need to be consistent for a single method";
      new_fishery.fishery_ndx_ = this_fishery_iter.second;
      if (fisheries_[new_fishery.label_].u_max_ != new_fishery.u_max_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had u_max = " << new_fishery.u_max_ << " at row " << row_iter
                                  << ", but the same fishery had u_max = " << fisheries_[new_fishery.label_].u_max_ << " at row " << this_fishery_iter.second + 1
                                  << ". These need to be the same for a single method in the methods table";

      if (process_profile_ == ProcessProfile::kAge) {
        if (fisheries_[new_fishery.label_].catch_as_biomass_ != new_fishery.catch_as_biomass_)
          LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had biomass = " << new_fishery.catch_as_biomass_ << " at row " << row_iter
                                    << ", but the same fishery had biomass = " << fisheries_[new_fishery.label_].catch_as_biomass_ << " at row " << this_fishery_iter.second + 1
                                    << ". These need to be the same for a single method in the methods table";
        if (fisheries_[new_fishery.label_].catch_as_u_ != new_fishery.catch_as_u_)
          LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had U = " << new_fishery.catch_as_u_ << " at row " << row_iter
                                    << ", but the same fishery had U = " << fisheries_[new_fishery.label_].catch_as_u_ << " at row " << this_fishery_iter.second + 1
                                    << ". These need to be the same for a single method in the methods table";
      }
    } else {
      // Haven't seen this method so store it
      fishery_labels_.push_back(new_fishery.label_);
      new_fishery.fishery_ndx_ = fishery_labels_.size() - 1;
      fishery_time_step[new_fishery.label_].push_back(new_fishery.time_step_label_);

      if (fishery_year_catch.find(new_fishery.label_) == fishery_year_catch.end())
        LOG_ERROR_P(PARAM_METHOD) << "fishery " << new_fishery.label_ << " does not have catch information in the catches table";

      new_fishery.catches_        = fishery_year_catch[new_fishery.label_];
      new_fishery.actual_catches_ = fishery_year_catch[new_fishery.label_];

      // Age-specific: check exploitation rate values if catch_as_u
      if (process_profile_ == ProcessProfile::kAge && new_fishery.catch_as_u_) {
        for (auto iter : new_fishery.catches_) {
          if (iter.second < 0.0 || iter.second > 1.0) {
            LOG_ERROR_P(PARAM_CATCHES) << ": the column " << new_fishery.label_ << " with year " << iter.first << " has value '" << iter.second
                                       << "'. It must have a value between 0 and 1 (inclusive) as the column is defined as an exploitation rate in the " << PARAM_METHOD
                                       << " table";
          }
        }
      }

      // Store fishery and make it addressable
      fisheries_[new_fishery.label_] = new_fishery;
      RegisterAsAddressable(PARAM_METHOD + string("_") + utilities::ToLowercase(new_fishery.label_), &fisheries_[new_fishery.label_].catches_);
      LOG_FINEST() << "Creating addressable " << PARAM_METHOD << " : " << PARAM_METHOD + string("_") + utilities::ToLowercase(new_fishery.label_);
    }

    // Move onto category stuff for this method
    vector<string> categories;
    vector<string> selectivities;
    vector<string> age_weight_values;

    boost::split(categories, row[category_index], boost::is_any_of(","));
    boost::split(selectivities, row[selectivity_index], boost::is_any_of(","));

    for (auto cat : categories) {
      LOG_FINEST() << cat;
    }
    if (use_age_weight_)
      boost::split(age_weight_values, row[age_weight_index], boost::is_any_of(","));

    if (categories.size() != selectivities.size())
      LOG_FATAL_P(PARAM_METHOD) << "The number of categories (" << categories.size() << ") and selectivities (" << selectivities.size() << ") provided must be the same";

    // Create fishery category struct
    for (unsigned i = 0; i < categories.size(); ++i) {
      FisheryCategoryData new_category_data(fisheries_[new_fishery.label_], *category_data_[categories[i]]);
      if (process_profile_ == ProcessProfile::kAge)
        fishery_category_check_[new_fishery.label_].push_back((*category_data_[categories[i]]).category_label_);

      new_category_data.fishery_label_  = row[fishery_index];
      new_category_data.category_label_ = categories[i];

      // Check categories are in category_labels_
      if (std::find(category_labels_.begin(), category_labels_.end(), categories[i]) == category_labels_.end())
        LOG_ERROR_P(PARAM_METHOD) << "The category " << categories[i] << " was found in the methods table but not in the '" << PARAM_CATEGORIES << "' subcommand."
                                  << " This configuration will apply exploitation processes and not natural mortality, which is not valid."
                                  << " Make sure all categories in the methods table are in the categories subcommand.";

      new_category_data.selectivity_label_ = selectivities[i];

      if (use_age_weight_) {
        new_category_data.category_.age_weight_label_ = age_weight_values[i];
      } else if (process_profile_ == ProcessProfile::kAge) {
        new_category_data.category_.age_weight_label_ = PARAM_NONE;
        LOG_FINE() << "setting age weight label to none.";
      }

      fishery_categories_.push_back(new_category_data);
    }
    ++row_iter;
  }

  // Check the business rule that a fishery can only exist in one time-step
  for (auto fishery : fishery_time_step) {
    if (!std::equal(fishery.second.begin() + 1, fishery.second.end(), fishery.second.begin()))
      LOG_ERROR_P(PARAM_METHOD) << "The method '" << fishery.first << "' was found in more than one time step."
                                << " A method can occur once only in each time step. If a fishery occurs in multiple time steps then define each time step as a separate fishery.";
  }

  LOG_FINEST() << "Finishing DoValidate";
}

/**
 * Build any objects that will need to be utilised by this object.
 */
void MortalityInstantaneous::DoBuild() {
  LOG_TRACE();
  partition_.Init(category_labels_);

  /**
   * Build all of our category objects
   */
  unsigned bin_count = (process_profile_ == ProcessProfile::kAge) ? model()->age_spread() : model()->get_number_of_length_bins();

  for (auto& category : categories_) {
    category.category_          = &model()->partition().category(category.category_label_);
    unsigned category_bin_count = (process_profile_ == ProcessProfile::kAge) ? category.category_->age_spread() : bin_count;
    category.exploitation_.assign(category_bin_count, 0.0);
    category.exp_values_half_m_.assign(category_bin_count, 0.0);
    if (process_profile_ == ProcessProfile::kAge)
      category.m_values_.assign(category_bin_count, 0.0);
    category.selectivity_values_.assign(category_bin_count, 0.0);
  }

  for (auto& fishery_category : fishery_categories_) {
    unsigned category_bin_count = (process_profile_ == ProcessProfile::kAge) ? fishery_category.category_.category_->age_spread() : bin_count;
    fishery_category.selectivity_values_.assign(category_bin_count, 0.0);
  }

  /**
   * Organise our time step ratios
   */
  vector<TimeStep*> time_steps = model()->managers()->time_step()->ordered_time_steps();
  vector<unsigned>  active_time_steps;
  for (unsigned i = 0; i < time_steps.size(); ++i) {
    if (time_steps[i]->HasProcess(label_))
      active_time_steps.push_back(i);
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

  // Assign the selectivity, penalty and time step index to each fishery data object
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

  // Check the natural mortality categories are valid
  for (const string& label : category_labels_) {
    if (!model()->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": Category " << label << " was not found.";
  }

  // Assign the natural mortality selectivities
  for (auto& category : categories_) {
    Selectivity* selectivity = model()->managers()->selectivity()->GetSelectivity(category.selectivity_label_);
    if (!selectivity) {
      if (process_profile_ == ProcessProfile::kAge) {
        LOG_ERROR_P(PARAM_RELATIVE_M_BY_AGE) << "M-by-age ogive label " << category.selectivity_label_ << " was not found.";
      } else {
        LOG_ERROR_P(PARAM_RELATIVE_M_BY_LENGTH) << "M-by-length ogive label " << category.selectivity_label_ << " was not found.";
      }
    }
    category.selectivity_ = selectivity;
    selectivities_.push_back(selectivity);

    // Age-specific: Age Weight
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

  /**
   * Find what time_steps Instant Mortality is applied in
   */
  vector<unsigned> instant_mort_time_step;
  unsigned         i = 0;
  for (auto time_step : model()->managers()->time_step()->ordered_time_steps()) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kMortality && process->type() == PARAM_MORTALITY_INSTANTANEOUS) {
        LOG_FINEST() << "mortality_instantaneous process in time step " << i;
        instant_mort_time_step.push_back(i);
      }
    }
    ++i;
  }

  // Check if we can skip the exploitation code (no F, just M)
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

  // Reserve memory for reporting objects
  LOG_FINE() << "years " << process_years_.size();
  const vector<TimeStep*> ordered_time_steps = model()->managers()->time_step()->ordered_time_steps();
  LOG_FINE() << "time steps = " << ordered_time_steps.size();
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

/**
 * Reset the M parameter
 */
void MortalityInstantaneous::DoReset() {
  LOG_TRACE();
  unsigned m_iter = 0;
  for (auto m : m_) {
    LOG_FINEST() << "resetting M for category " << m.first << " = " << m.second;
    m_input_[m_iter] = m.second;
    ++m_iter;
  }
}

/**
 * Called from the time varying class to reset parameters
 */
void MortalityInstantaneous::RebuildCache() {
  DoReset();
}

/**
 * Execute this process
 */
void MortalityInstantaneous::DoExecute() {
  LOG_TRACE();
  unsigned             time_step_index = model()->managers()->time_step()->current_time_step();
  unsigned             year            = model()->current_year();
  double               ratio           = time_step_ratios_[time_step_index];
  std::pair<bool, int> this_year_iter  = findInVector(process_years_, year);

  if (this_year_iter.first) {
    LOG_FINE() << "Year = " << year << " found? = " << this_year_iter.first << " should = " << process_years_[this_year_iter.second];
  } else {
    LOG_FINE() << "Year = " << year << " found? = " << this_year_iter.first;
  }

  Double selectivity_value = 0.0;

  // Pre-calculate M values and exp(-0.5*M*ratio)
  for (auto& category : categories_) {
    unsigned data_size = category.category_->data_.size();
    for (unsigned i = 0; i < data_size; ++i) {
      if (process_profile_ == ProcessProfile::kAge) {
        selectivity_value              = category.selectivity_->GetAgeResult(category.category_->min_age_ + i, category.category_->age_length_);
        category.m_values_[i]          = (*category.m_) * selectivity_value;
        category.exp_values_half_m_[i] = exp(-0.5 * ratio * category.m_values_[i]);
      } else {
        selectivity_value              = category.selectivity_->GetLengthResult(i);
        category.exp_values_half_m_[i] = exp(-0.5 * ratio * (*category.m_) * selectivity_value);
      }
      category.exploitation_[i]       = 0.0;
      category.selectivity_values_[i] = selectivity_value;
      LOG_FINEST() << "category " << category.category_label_ << " index " << i << " selectivity " << selectivity_value;
    }
  }

  for (auto& fishery : fisheries_) fishery.second.vulnerability_ = 0.0;

  /**
   * This is where F gets applied
   */
  bool apply_fishing = false;
  if (model()->state() != State::kInitialise) {
    if (((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model()->final_year()))) {
      if (find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index)
          == time_steps_to_skip_applying_F_mortality_.end()) {
        apply_fishing = true;
      }
    }
  } else if (process_profile_ == ProcessProfile::kLength) {
    // Length model: different initialisation check
    if ((find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index) != time_steps_to_skip_applying_F_mortality_.end())
        && ((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model()->final_year()))) {
      apply_fishing = true;
    }
  }

  if (apply_fishing) {
    LOG_FINEST() << "time step = " << time_step_index << " not in initialisation and there is an F method in this timestep. year = " << model()->current_year();

    // Calculate selectivity values for fishery categories
    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      for (unsigned i = 0; i < fishery_category.selectivity_values_.size(); ++i) {
        if (process_profile_ == ProcessProfile::kAge) {
          fishery_category.selectivity_values_[i]
              = fishery_category.selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
        } else {
          fishery_category.selectivity_values_[i] = fishery_category.selectivity_->GetLengthResult(i);
        }
        LOG_FINEST() << "fishery category " << fishery_category.fishery_label_ << " index " << i << " selectivity " << fishery_category.selectivity_values_[i];
      }
    }

    // Calculate vulnerability
    for (auto& fishery_category : fishery_categories_) {
      LOG_FINEST() << "checking fishery = " << fishery_category.fishery_label_;

      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category   = fishery_category.category_.category_;
      Double               vulnerable = 0.0;

      if (process_profile_ == ProcessProfile::kAge) {
        if (fishery_category.category_.age_weight_) {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model()->min_age())
                          * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
          }
        } else if (fishery_category.fishery_.catch_as_biomass_) {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                          * fishery_category.category_.exp_values_half_m_[i];
          }
        } else {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
          }
        }
      } else {
        // Length
        if (is_catch_biomass_) {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * category->growth_increment_->get_mean_weight(i) * fishery_category.selectivity_values_[i]
                          * fishery_category.category_.exp_values_half_m_[i];
          }
        } else {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
          }
        }
      }

      fishery_category.fishery_.vulnerability_ += vulnerable;
      LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << vulnerable;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable) for each fishery
     */
    Double exploitation;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      exploitation  = 0.0;

      if (fishery.time_step_index_ == time_step_index) {
        if (process_profile_ == ProcessProfile::kAge && fishery.catch_as_u_) {
          exploitation = fishery.catches_[year];
        } else {
          exploitation = fishery.catches_[year] / utilities::math::ZeroFun(fishery.vulnerability_);
        }

        fishery.exploitation_ = exploitation;
        fishery.uobs_fishery_ = exploitation;

        LOG_FINEST() << "Vulnerable biomass for fishery " << fishery.label_ << " = " << fishery.vulnerability_ << " with catch = " << fishery.catches_[year]
                     << " and exploitation = " << exploitation;
      } else if (fishery.time_step_index_ > time_step_index) {
        fishery.exploitation_ = exploitation;
        fishery.uobs_fishery_ = exploitation;
      }
      LOG_FINE() << "year = " << year << " time step = " << time_step_index << " fishery = " << fishery.label_ << " exploitation = " << fishery.exploitation_;
    }

    // Calculate exploitation by category
    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category = fishery_category.category_.category_;
      for (unsigned i = 0; i < category->data_.size(); ++i) {
        fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i];
      }
    }

    /**
     * Calculate u_obs for each fishery
     */
    bool recalculate_exploitation = false;
    LOG_FINEST() << "calc uobs";

    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "fishery = " << fishery_iter.first;
      auto& fishery = fishery_iter.second;

      if (fishery.time_step_index_ != time_step_index)
        continue;

      auto& uobs = fishery.uobs_fishery_;
      uobs       = 0.0;
      for (auto& fishery_category : fishery_categories_) {
        if (fishery_category.fishery_.label_ != fishery.label_)
          continue;

        for (Double bin_exploitation : fishery_category.category_.exploitation_) {
          uobs = uobs > bin_exploitation ? uobs : bin_exploitation;
        }
      }
    }

    // Check if we need to rescale exploitation
    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "fishery = " << fishery_iter.first;
      auto& fishery = fishery_iter.second;

      if (fishery.time_step_index_ != time_step_index)
        continue;

      if (fishery.uobs_fishery_ > fishery.u_max_) {
        LOG_FINE() << fishery.label_ << " exploitation rate before rescaling = " << fishery.exploitation_ << " uobs = " << fishery.uobs_fishery_;
        fishery.exploitation_ *= (fishery.u_max_ / fishery.uobs_fishery_);
        LOG_FINE() << "fishery = " << fishery.label_ << " U_obs = " << fishery.uobs_fishery_ << " and u_max " << fishery.u_max_;
        LOG_FINE() << fishery.label_ << " rescaled exploitation rate = " << fishery.exploitation_;

        recalculate_exploitation            = true;
        fishery.actual_catches_[year]       = fishery.vulnerability_ * fishery.exploitation_;
        fishery.exploitation_by_year_[year] = fishery.exploitation_;
        if (fishery.penalty_)
          fishery.penalty_->Trigger(fishery.catches_[year], fishery.actual_catches_[year]);
      } else {
        if (process_profile_ == ProcessProfile::kAge && fishery.catch_as_u_) {
          fishery.actual_catches_[year] = fishery.vulnerability_ * fishery.exploitation_;
        } else {
          fishery.actual_catches_[year] = fishery.catches_[year];
        }
        fishery.exploitation_by_year_[year] = fishery.exploitation_;
      }
      fishery.uobs_by_year_[year] = fishery.uobs_fishery_;
    }

    /**
     * Recalculate exploitation if we triggered penalty
     */
    if (recalculate_exploitation) {
      for (auto& category : categories_) {
        LOG_FINE() << "recalculating exploitation for category " << category.category_label_ << " in time step " << time_step_index;
        for (unsigned i = 0; i < category.category_->data_.size(); ++i) category.exploitation_[i] = 0.0;
      }

      for (auto& fishery_category : fishery_categories_) {
        partition::Category* category = fishery_category.category_.category_;

        if (fishery_category.fishery_.time_step_index_ != time_step_index)
          continue;

        LOG_FINE() << "updating category exploitation with fishery " << fishery_category.fishery_.label_ << " in time step " << time_step_index
                   << ": exploitation = " << fishery_category.fishery_.exploitation_;

        for (unsigned i = 0; i < category->data_.size(); ++i) {
          fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i];
        }
      }
    }

    /**
     * Calculate the expectation for a proportions observation
     */
    LOG_FINEST() << "Calculate proportions";
    if (find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) {
      unsigned bin_count = (process_profile_ == ProcessProfile::kAge) ? model()->age_spread() : model()->get_number_of_length_bins();

      if (process_profile_ == ProcessProfile::kAge) {
        for (auto& fishery_category : fishery_categories_) {
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;
          partition::Category* category = fishery_category.category_.category_;
          LOG_FINEST() << "category = " << category->name_ << " fishery = " << fishery_category.fishery_label_;

          for (unsigned i = 0; i < bin_count; ++i) {
            unsigned age_offset = category->min_age_ - model()->min_age();
            if (i < age_offset)
              continue;
            removals_by_year_fishery_category_[this_year_iter.second][fishery_category.fishery_.fishery_ndx_][fishery_category.category_.category_ndx_][i]
                = category->data_[i - age_offset] * fishery_category.fishery_.exploitation_
                  * fishery_category.selectivity_->GetAgeResult(category->min_age_ + i, category->age_length_) * fishery_category.category_.exp_values_half_m_[i];
          }
        }
      } else {
        for (auto& categories : partition_) {
          for (auto& fishery_category : fishery_categories_) {
            if (fishery_category.category_label_ == categories->name_ && fisheries_[fishery_category.fishery_label_].time_step_index_ == time_step_index) {
              LOG_FINEST() << "category = " << categories->name_ << " fishery = " << fishery_category.fishery_label_;
              for (unsigned i = 0; i < bin_count; ++i) {
                removals_by_year_fishery_category_[this_year_iter.second][fishery_category.fishery_.fishery_ndx_][fishery_category.category_.category_ndx_][i]
                    = categories->data_[i] * fishery_category.fishery_.exploitation_ * fishery_category.selectivity_->GetLengthResult(i)
                      * fishery_category.category_.exp_values_half_m_[i];
              }
            }
          }
        }
      }
    }
  }

  /**
   * Remove the stock now using the exploitation rate
   */
  for (auto& category : categories_) {
    LOG_FINEST() << "category " << category.category_label_ << " used in time step " << time_step_index << ": " << category.used_in_current_timestep_;

    for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
      LOG_FINEST() << "category " << category.category_label_ << ": numbers = " << category.category_->data_[i] << " index " << i << " exploitation = " << category.exploitation_[i]
                   << " M = " << *category.m_ << " selectivity = " << category.selectivity_values_[i];

      category.category_->data_[i] *= category.exp_values_half_m_[i] * category.exp_values_half_m_[i] * (1.0 - category.exploitation_[i]);
      LOG_FINEST() << "category " << category.category_label_ << ": updated numbers = " << category.category_->data_[i] << " index " << i;

      if (category.category_->data_[i] < 0.0) {
        LOG_CODE_ERROR() << " Fishing caused a negative partition : category.category_->data_[i] = " << category.category_->data_[i] << " i = " << i + 1 << "; category "
                         << category.category_label_ << " exploitation = " << category.exploitation_[i] << " selectivity = " << category.selectivity_values_[i]
                         << " M = " << *category.m_ << " time step = " << time_step_index << " used in time step = " << category.used_in_current_timestep_
                         << ". The category.exp_values_half_m_[i] was " << category.exp_values_half_m_[i];
      }
    }
  }
}

/**
 * Fill the report cache
 */
void MortalityInstantaneous::FillReportCache(ostringstream& cache) {
  LOG_FINE();

  // Age-specific: report m_by_age
  if (process_profile_ == ProcessProfile::kAge) {
    for (auto& category : categories_) {
      cache << "m_by_age[" << category.category_label_ << "]: ";
      for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
        cache << AS_DOUBLE(category.m_values_[i]) << " ";
      }
      cache << "\n";
    }
  }

  cache << "year: ";
  for (auto year : process_years_) cache << year << " ";

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    cache << "\nexploitation_rate[" << fishery.label_ << "]: ";
    for (auto pressure : fishery.exploitation_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    cache << "\nfishing_pressure[" << fishery.label_ << "]: ";
    for (auto uobs_pressure : fishery.uobs_by_year_) cache << AS_DOUBLE(uobs_pressure.second) << " ";
    cache << "\ncatch[" << fishery.label_ << "]: ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    cache << "\nactual_catch[" << fishery.label_ << "]: ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
  }

  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 */
void MortalityInstantaneous::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto pressure : fishery.exploitation_by_year_) cache << "exploitation_rate[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto pressure : fishery.uobs_by_year_) cache << "fishing_pressure[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto catches : fishery.catches_) cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_) cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
    }

    if (process_profile_ == ProcessProfile::kAge) {
      for (auto& category : categories_) {
        for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
          cache << "m_by_age[" << category.category_label_ << "][" << i + 1 << "] ";
        }
      }
    }
    cache << REPORT_EOL;
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto pressure : fishery.exploitation_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    for (auto pressure : fishery.uobs_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
  }

  if (process_profile_ == ProcessProfile::kAge) {
    for (auto& category : categories_) {
      for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
        cache << AS_DOUBLE(category.m_values_[i]) << " ";
      }
    }
  }
  cache << REPORT_EOL;
}

/**
 * Check the categories in methods for removal obs
 */
bool MortalityInstantaneous::check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels) {
  LOG_TRACE();

  if (process_profile_ == ProcessProfile::kAge) {
    // Use the pre-built fishery_category_check_ map
    for (auto& method : methods) {
      if (fishery_category_check_.find(method) == fishery_category_check_.end())
        return false;
      for (auto& category : category_labels) {
        if (find(fishery_category_check_[method].begin(), fishery_category_check_[method].end(), category) == fishery_category_check_[method].end())
          return false;
      }
    }
  } else {
    // Length: iterate through fisheries directly
    for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
      unsigned categories_counter = 0;
      for (auto& fishery_iter : fisheries_) {
        auto& fishery = fishery_iter.second;
        if (fishery.label_ != methods[fishery_index])
          continue;
        for (unsigned category_index = 0; category_index < category_labels.size(); ++category_index) {
          for (auto& fishery_category : fishery_categories_) {
            if ((fishery_category.fishery_.label_ == fishery.label_) && (fishery_category.category_label_ == category_labels[category_index]))
              ++categories_counter;
          }
        }
        if (categories_counter != category_labels.size()) {
          LOG_FINEST() << "category counter = " << categories_counter << " categories supplies = " << category_labels.size();
          return false;
        }
      }
    }
  }

  return true;
}

/**
 * Check the years in methods for removal obs
 */
bool MortalityInstantaneous::check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods) {
  LOG_TRACE();

  if (process_profile_ == ProcessProfile::kAge) {
    // Use the pre-built fishery_catch_ map
    for (auto& method : methods) {
      if (fishery_catch_.find(method) == fishery_catch_.end())
        return false;
      for (auto& year : years) {
        if (fishery_catch_[method].find(year) == fishery_catch_[method].end())
          return false;
        if (fishery_catch_[method][year] <= 0)
          return false;
      }
    }
  } else {
    // Length: iterate through fisheries directly
    for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
      for (auto& fishery_iter : fisheries_) {
        auto& fishery = fishery_iter.second;
        if (fishery.label_ != methods[fishery_index])
          continue;
        unsigned year_counter = 0;
        for (auto& catches : fishery.catches_) {
          if (find(years.begin(), years.end(), catches.first) != years.end()) {
            LOG_FINE() << "found year = " << catches.first << " with catch = " << catches.second;
            if (catches.second <= 0)
              return false;
            ++year_counter;
          }
        }
        if (year_counter != years.size())
          return false;
      }
    }
  }
  return true;
}

/**
 * Check the methods for removal obs
 */
bool MortalityInstantaneous::check_methods_for_removal_obs(vector<string> methods) {
  LOG_TRACE();
  unsigned method_counter = 0;
  for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      if (fishery.label_ == methods[fishery_index]) {
        ++method_counter;
        break;
      }
    }
  }
  return method_counter == methods.size();
}

/**
 * Get fishery index for catch at
 */
vector<unsigned> MortalityInstantaneous::get_fishery_ndx_for_catch_at(vector<string> fishery_labels) {
  LOG_TRACE();
  vector<unsigned> result;
  for (auto& label : fishery_labels) {
    if (fisheries_.find(label) != fisheries_.end())
      result.push_back(fisheries_[label].fishery_ndx_);
  }
  return result;
}

/**
 * Get category index for catch at
 */
vector<unsigned> MortalityInstantaneous::get_category_ndx_for_catch_at(vector<string> category_labels) {
  LOG_TRACE();
  vector<unsigned> result;
  for (auto& label : category_labels) {
    if (category_data_.find(label) != category_data_.end())
      result.push_back(category_data_[label]->category_ndx_);
  }
  return result;
}

/**
 * Get year index for catch at
 */
vector<unsigned> MortalityInstantaneous::get_year_ndx_for_catch_at(vector<unsigned> years) {
  LOG_TRACE();
  vector<unsigned> result;
  for (auto& year : years) {
    std::pair<bool, int> year_iter = findInVector(process_years_, year);
    if (year_iter.first)
      result.push_back(year_iter.second);
  }
  return result;
}

}  // namespace niwa::processes::common
