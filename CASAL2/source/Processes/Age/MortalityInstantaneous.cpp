/**
 * @file MortalityInstantaneous.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
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
#include "Model/Managers.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {
namespace math = niwa::utilities::math;
using niwa::utilities::findInVector;

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
MortalityInstantaneous::MortalityInstantaneous(shared_ptr<Model> model) : Process(model), partition_(model) {
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;

  catches_table_ = new parameters::Table(PARAM_CATCHES);
  method_table_  = new parameters::Table(PARAM_METHOD);
  // catches_table_->set_required_columns({"years"}, allow_others = true)
  // method_table_->set_required_columns({"x", "x", "x,"});

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The categories for instantaneous mortality", "");
  parameters_.BindTable(PARAM_CATCHES, catches_table_, "The table of removals (catch) data", "", true, false);
  parameters_.BindTable(PARAM_METHOD, method_table_, "The table of method of removal data", "", true, false);
  parameters_.Bind<Double>(PARAM_M, &m_input_, "The natural mortality rates for each category", "")->set_lower_bound(0.0);
  parameters_.Bind<double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_ratios_temp_, "The time step proportions for natural mortality", "", false)->set_range(0.0, 1.0);
  parameters_.Bind<bool>(PARAM_BIOMASS, &is_catch_biomass_, "Indicator to denote if the catches are as biomass (true) or abundance (false)", "", true);
  parameters_.Bind<string>(PARAM_RELATIVE_M_BY_AGE, &selectivity_labels_, "The M-by-age ogives to apply to each category for natural mortality", "");

  RegisterAsAddressable(PARAM_M, &m_);
}

/**
 * Destructor
 */
MortalityInstantaneous::~MortalityInstantaneous() {
  delete catches_table_;
  delete method_table_;
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityInstantaneous::DoValidate() {
  // Check Natural Mortality parameter first
  for (auto M_proportion : time_step_ratios_temp_) {
    if (M_proportion < 0.0)
      LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << "Natural Mortality time step proportions cannot be less than 0.0 for a given time step";
  }

  /**
   * Load a temporary map of the fishery catch data so we can use this
   * when we load our vector of FisheryData objects
   */
  map<string, map<unsigned, Double>> fishery_year_catch;
  auto                               columns = catches_table_->columns();

  // TODO Need to catch if key column headers are missing for example year
  if (std::find(columns.begin(), columns.end(), PARAM_YEAR) == columns.end())
    LOG_ERROR_P(PARAM_CATCHES) << "The required column " << PARAM_YEAR << " was not found.";
  unsigned year_index = std::find(columns.begin(), columns.end(), PARAM_YEAR) - columns.begin();
  LOG_FINEST() << "The year_index for fisheries table is: " << year_index;

  auto model_years = model_->years();
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
      fishery_year_catch[columns[i]][year] = value;
    }
  }

  /**
   * Validate the non-table parameters now. These are mostly related to the natural mortality
   * aspect of the process.
   */
  if (selectivity_labels_.size() == 1) {
    auto val_sel = selectivity_labels_[0];
    selectivity_labels_.assign(category_labels_.size(), val_sel);
  }

  if (selectivity_labels_.size() != category_labels_.size()) {
    LOG_FATAL_P(PARAM_RELATIVE_M_BY_AGE) << ": The number of M-by-age ogives provided is not the same as the number of categories provided. Categories: " << category_labels_.size()
                                         << ", Ogives: " << selectivity_labels_.size();
  }

  if (m_input_.size() == 1) {
    auto val_m = m_input_[0];
    m_input_.assign(category_labels_.size(), val_m);
  }

  if (m_input_.size() != category_labels_.size())
    LOG_FATAL_P(PARAM_M) << ": The number of Ms provided is not the same as the number of categories provided. Categories: " << category_labels_.size()
                         << ", Ms: " << m_input_.size();
  for (unsigned i = 0; i < m_input_.size(); ++i) m_[category_labels_[i]] = m_input_[i];

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
   * Now we want to load the information from the fisheries table.
   * We create a master fishery object and then multiple category objects
   * depending on how many categories are defined.
   */
  columns = method_table_->columns();
  rows    = method_table_->data();
  LOG_FINE() << "method_table.columns.size(): " << columns.size();
  LOG_FINE() << "method_table.rows.size(): " << rows.size();
  LOG_FINE() << "columns: " << boost::join(columns, " ");
  for (auto row : rows) {
    LOG_FINE() << "row: " << boost::join(row, " ");
  }

  // Check the column headers are all specified correctly
  if (std::find(columns.begin(), columns.end(), PARAM_METHOD) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_METHOD << " was not found.";
  if (std::find(columns.begin(), columns.end(), PARAM_CATEGORY) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_CATEGORY << " was not found.";
  if (std::find(columns.begin(), columns.end(), PARAM_SELECTIVITY) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_SELECTIVITY << " was not found.";
  if (std::find(columns.begin(), columns.end(), PARAM_TIME_STEP) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_TIME_STEP << " was not found.";
  if (std::find(columns.begin(), columns.end(), PARAM_U_MAX) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_U_MAX << " was not found.";
  if (std::find(columns.begin(), columns.end(), PARAM_PENALTY) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_PENALTY << " was not found.";
  if (std::find(columns.begin(), columns.end(), PARAM_AGE_WEIGHT_LABEL) == columns.end()) {
    // Users can choose not to add this column if they wish
    use_age_weight_ = false;
    LOG_FINE() << "Age weight column not found";
  } else if (!is_catch_biomass_) {
    LOG_FATAL_P(PARAM_BIOMASS) << "The parameter " << PARAM_BIOMASS << " must be true if an Age weight relationship is also used";
  }

  unsigned fishery_index     = std::find(columns.begin(), columns.end(), PARAM_METHOD) - columns.begin();
  unsigned category_index    = std::find(columns.begin(), columns.end(), PARAM_CATEGORY) - columns.begin();
  unsigned selectivity_index = std::find(columns.begin(), columns.end(), PARAM_SELECTIVITY) - columns.begin();
  unsigned time_step_index   = std::find(columns.begin(), columns.end(), PARAM_TIME_STEP) - columns.begin();
  unsigned u_max_index       = std::find(columns.begin(), columns.end(), PARAM_U_MAX) - columns.begin();
  unsigned penalty_index     = std::find(columns.begin(), columns.end(), PARAM_PENALTY) - columns.begin();
  unsigned age_weight_index  = 999;

  if (use_age_weight_)
    age_weight_index = std::find(columns.begin(), columns.end(), PARAM_AGE_WEIGHT_LABEL) - columns.begin();

  LOG_FINEST() << "indexes: fishery=" << fishery_index << "; category=" << category_index << "; selectivity=" << selectivity_index << "; time_step=" << time_step_index
               << "; u_max=" << u_max_index << "; penalty " << penalty_index << "; age weight index " << age_weight_index;

  // This is object is going to check the business rule that a fishery can only exist in one time-step in each year
  map<string, vector<string>> fishery_time_step;
  vector<Double>              empty_year_vector(process_years_.size(), 0.0);
  // iterate over the methods column
  // not there can multiple instances for the same fishery in this table
  // so do some consistency checks that the repeat fisheries are are consistent.
  // regarding timing, and Umax
  unsigned row_iter = 1;
  for (auto row : rows) {
    FisheryData new_fishery;
    new_fishery.label_                     = row[fishery_index];
    new_fishery.time_step_label_           = row[time_step_index];
    new_fishery.penalty_label_             = row[penalty_index];
    new_fishery.years_                     = process_years_;
    std::pair<bool, int> this_fishery_iter = findInVector(fishery_labels_, new_fishery.label_);
    LOG_FINE() << new_fishery.label_ << " found = " << this_fishery_iter.first << " ndx = " << this_fishery_iter.second;
    if (this_fishery_iter.first) {
      // we have already seen this fishery in the methods table
      // Do some checks and don't cache anything
      if (fisheries_[new_fishery.label_].time_step_label_ != new_fishery.time_step_label_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had timestep label = " << new_fishery.time_step_label_ << " at row " << row_iter
                                  << " but the same fishery had time step label = " << fisheries_[new_fishery.label_].time_step_label_
                                  << " at row = " << this_fishery_iter.second + 1 << " these need to be consistent for a single method";
      new_fishery.fishery_ndx_ = this_fishery_iter.second;
      if (!utilities::To<string, Double>(row[u_max_index], new_fishery.u_max_))
        LOG_ERROR_P(PARAM_METHOD) << "u_max value " << row[u_max_index] << " could not be converted to a Double";
      if (fisheries_[new_fishery.label_].u_max_ != new_fishery.u_max_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had u_max = " << new_fishery.u_max_ << " at row " << row_iter
                                  << " but the same fishery had u_max_ = " << fisheries_[new_fishery.label_].u_max_ << " at row = " << this_fishery_iter.second + 1
                                  << " these need to be consistent for a single method";
    } else {
      // haven't seen this method so store it in the fisheries struct
      fishery_labels_.push_back(new_fishery.label_);
      new_fishery.fishery_ndx_ = fishery_labels_.size() - 1;
      fishery_time_step[new_fishery.label_].push_back(new_fishery.time_step_label_);
      if (!utilities::To<string, Double>(row[u_max_index], new_fishery.u_max_))
        LOG_ERROR_P(PARAM_METHOD) << "u_max value " << row[u_max_index] << " could not be converted to a Double";
      if (fishery_year_catch.find(new_fishery.label_) == fishery_year_catch.end())
        LOG_ERROR_P(PARAM_METHOD) << "fishery " << new_fishery.label_ << " does not have catch information in the catches table";
      new_fishery.catches_        = fishery_year_catch[new_fishery.label_];
      new_fishery.actual_catches_ = fishery_year_catch[new_fishery.label_];
      // store fishery and make it addressable
      fisheries_[new_fishery.label_] = new_fishery;
      RegisterAsAddressable(PARAM_METHOD + string("_") + utilities::ToLowercase(new_fishery.label_), &fisheries_[new_fishery.label_].catches_);
      LOG_FINEST() << "Creating addressable " << PARAM_METHOD << " : " << PARAM_METHOD + string("_") + utilities::ToLowercase(new_fishery.label_);
    }

    // move onto category stuff for this method
    vector<string> categories;
    vector<string> selectivities;
    vector<string> age_weights;

    boost::split(categories, row[category_index], boost::is_any_of(","));
    boost::split(selectivities, row[selectivity_index], boost::is_any_of(","));

    for (auto cat : categories) {
      LOG_FINEST() << cat;
    }
    if (use_age_weight_)
      boost::split(age_weights, row[age_weight_index], boost::is_any_of(","));

    if (categories.size() != selectivities.size())
      LOG_FATAL_P(PARAM_METHOD) << "The number of categories (" << categories.size() << ") and selectivities (" << selectivities.size() << ") provided must be the same";
    // Create fishery category struct
    for (unsigned i = 0; i < categories.size(); ++i) {
      FisheryCategoryData new_category_data(fisheries_[new_fishery.label_], *category_data_[categories[i]]);
      new_category_data.fishery_label_  = row[fishery_index];
      new_category_data.category_label_ = categories[i];

      // check categories are in category_labels_ as well
      if (std::find(category_labels_.begin(), category_labels_.end(), categories[i]) == category_labels_.end())
        LOG_ERROR_P(PARAM_METHOD) << "The category " << categories[i] << " was found in the methods table but not in the '" << PARAM_CATEGORIES << "' subcommand."
                                  << " This configuration will apply exploitation processes and not natural mortality, which is not valid."
                                  << " Make sure all categories in the methods table are in the categories subcommand.";

      new_category_data.selectivity_label_ = selectivities[i];

      if (use_age_weight_)
        new_category_data.category_.age_weight_label_ = age_weights[i];
      else {
        new_category_data.category_.age_weight_label_ = PARAM_NONE;
        LOG_FINE() << "setting age weight label to none.";
      }

      fishery_categories_.push_back(new_category_data);
    }
    ++row_iter;
  }

  // Check the business rule that a fishery can only exist one time-step
  for (auto fishery : fishery_time_step) {
    if (!std::equal(fishery.second.begin() + 1, fishery.second.end(), fishery.second.begin()))
      LOG_ERROR_P(PARAM_METHOD) << "The method '" << fishery.first << "' was found in more than one time step."
                                << " A method can occur once only in each time step. If a fishery occurs in multiple time steps then define each time step as a separate fishery.";
  }

  // Check to see if there are any time_steps that we don't have enter the fisheries section. i.e no fishing in certain time-steps
  LOG_FINEST() << "Finishing DoValidate";
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 *
 * Validate any parameters that require information from other objects
 */
void MortalityInstantaneous::DoBuild() {
  LOG_TRACE();
  partition_.Init(category_labels_);

  /**
   * Build all of our category objects
   */
  for (auto& category : categories_) {
    category.category_ = &model_->partition().category(category.category_label_);
    category.exploitation_.assign(category.category_->age_spread(), 0.0);
    category.exp_values_half_m_.assign(category.category_->age_spread(), 0.0);
    category.selectivity_values_.assign(category.category_->age_spread(), 0.0);
  }

  for (auto& fishery_category : fishery_categories_) {
    fishery_category.selectivity_values_.assign(fishery_category.category_.category_->age_spread(), 0.0);
  }

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of M so here we want to verify
   * we have enough
   */
  vector<TimeStep*> time_steps = model_->managers()->time_step()->ordered_time_steps();
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
    LOG_ERROR_P(PARAM_TIME_STEP_PROPORTIONS) << " need to sum to one";
  }

  for (unsigned i = 0; i < time_step_ratios_temp_.size(); ++i) time_step_ratios_[active_time_steps[i]] = time_step_ratios_temp_[i];

  /**
   * Assign the selectivity, penalty and time step index to each fisher data object
   */
  for (auto& fishery_category : fishery_categories_) {
    fishery_category.selectivity_ = model_->managers()->selectivity()->GetSelectivity(fishery_category.selectivity_label_);
    /**
     * Check the fishery categories are valid
     */
    if (!model_->categories()->IsValid(fishery_category.category_label_))
      LOG_ERROR_P(PARAM_METHOD) << ": Fishery category " << fishery_category.category_label_ << " was not found.";

    if (!fishery_category.selectivity_)
      LOG_ERROR_P(PARAM_METHOD) << ": Fishery selectivity " << fishery_category.selectivity_label_ << " was not found.";
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    if (fishery.penalty_label_ != "none") {
      fishery.penalty_ = model_->managers()->penalty()->GetProcessPenalty(fishery.penalty_label_);
      if (!fishery.penalty_)
        LOG_ERROR_P(PARAM_METHOD) << ": Penalty label " << fishery.penalty_label_ << " was not found.";
    }
    bool check_time_step = model_->managers()->time_step()->CheckTimeStep(fishery.time_step_label_);
    if (!check_time_step)
      LOG_FATAL_P(PARAM_METHOD) << "Time step label " << fishery.time_step_label_ << " was not found.";
    fishery.time_step_index_ = model_->managers()->time_step()->GetTimeStepIndex(fishery.time_step_label_);
  }

  /**
   * Check the natural mortality categories are valid
   */
  for (const string& label : category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": Category " << label << " was not found.";
  }

  /**
   * Assign the natural mortality selectivities
   */
  for (auto& category : categories_) {
    // Selectivity
    Selectivity* selectivity = model_->managers()->selectivity()->GetSelectivity(category.selectivity_label_);
    if (!selectivity)
      LOG_ERROR_P(PARAM_RELATIVE_M_BY_AGE) << "M-by-age ogive label " << category.selectivity_label_ << " was not found.";
    category.selectivity_ = selectivity;
    selectivities_.push_back(selectivity);

    // Age Weight if it is defined
    LOG_FINEST() << "age weight " << category.age_weight_label_;
    if ((category.age_weight_label_ == PARAM_NONE) || (category.age_weight_label_ == "")) {
      category.age_weight_label_ = PARAM_NONE;
      category.age_weight_       = nullptr;
      use_age_weight_            = false;
    } else {
      LOG_FINE() << "age weight found";
      AgeWeight* age_weight = model_->managers()->age_weight()->FindAgeWeight(category.age_weight_label_);
      if (!age_weight)
        LOG_ERROR_P(PARAM_METHOD) << "age weight label " << category.age_weight_label_ << " was not found.";
      category.age_weight_ = age_weight;
      use_age_weight_      = true;
    }
  }

  /**
   * Find what time_steps Instant Mortality is applied in
   */
  vector<unsigned> instant_mort_time_step;
  unsigned         i = 0;
  for (auto time_step : model_->managers()->time_step()->ordered_time_steps()) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kMortality && process->type() == PARAM_MORTALITY_INSTANTANEOUS) {
        LOG_FINEST() << "mortality_instantaneous process in time step " << i;
        instant_mort_time_step.push_back(i);
      }
    }
    ++i;
  }

  // Now out of these lets see if we can skip the exploitation code i.e no F just M
  // for some fishery
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

  // reserve memory for reporting objects
  removals_by_category_age_.resize(category_labels_.size());
  for (unsigned i = 0; i < category_labels_.size(); ++i) removals_by_category_age_[i].resize(model_->age_spread());

  LOG_FINE() << "years " << process_years_.size();
  // allocate memory for observation object
  const vector<TimeStep*> ordered_time_steps = model_->managers()->time_step()->ordered_time_steps();
  LOG_FINE() << "time steps = " << ordered_time_steps.size();
  LOG_FINE() << "partitions = " << partition_.size();
  removals_by_year_fishery_category_.resize(process_years_.size());
  for (unsigned year_ndx = 0; year_ndx < process_years_.size(); ++year_ndx) {
    removals_by_year_fishery_category_[year_ndx].resize(fisheries_.size());
    for (unsigned fishery_ndx = 0; fishery_ndx < fisheries_.size(); ++fishery_ndx) {
      removals_by_year_fishery_category_[year_ndx][fishery_ndx].resize(category_labels_.size());
      for (unsigned category_ndx = 0; category_ndx < category_labels_.size(); ++category_ndx)
        removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx].resize(model_->age_spread(), 0.0);
    }
  }
  /*
  for (auto year : process_years_) {
    for (unsigned current_time_step = 0; current_time_step < ordered_time_steps.size(); ++current_time_step) {
      for (auto& category : category_labels_) {
        for (auto& fishery_category : fishery_categories_) {
          LOG_FINE() << "Checking category " << category << " time step = " << fisheries_[fishery_category.fishery_label_].time_step_index_;
          if (fishery_category.category_label_ == category && fisheries_[fishery_category.fishery_label_].time_step_index_ == current_time_step) {
            removals_by_year_fishery_category_[year][fishery_category.fishery_label_][category].assign(model_->age_spread(), 0.0);

            LOG_FINE() << "year " << year << " fishery = " << fishery_category.fishery_label_ << " category = " << fishery_category.category_label_
                       << "  size of vector = " << removals_by_year_fishery_category_[year][fishery_category.fishery_label_][category].size();
          }
        }
      }
    }
  }
  */
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
 * Called from the time varying class (see TimeVarying.cpp line 96) used to reset parameters.
 * This avoids a full reset and clearing of printed values which is what was happening before,
 */
void MortalityInstantaneous::RebuildCache() {
  DoReset();
}

/**
 * Execute this process
 */
void MortalityInstantaneous::DoExecute() {
  LOG_TRACE();
  unsigned             time_step_index = model_->managers()->time_step()->current_time_step();
  unsigned             year            = model_->current_year();
  double               ratio           = time_step_ratios_[time_step_index];
  std::pair<bool, int> this_year_iter  = findInVector(process_years_, year);

  LOG_FINE() << "Year = " << year << " found? = " << this_year_iter.first << " should = " << process_years_[this_year_iter.second];

  Double selectivity_value = 0.0;
  // This is to do with M. so pre-allocate and only call the exp call once per category
  // per execute. will be used in initialisation and when Catch is not removed
  for (auto& category : categories_) {
    for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
      selectivity_value               = category.selectivity_->GetAgeResult(category.category_->min_age_ + i, category.category_->age_length_);
      category.exploitation_[i]       = 0.0;
      category.selectivity_values_[i] = selectivity_value;
      category.exp_values_half_m_[i]  = exp(-0.5 * ratio * (*category.m_) * selectivity_value);  // this exp call should ony
      LOG_FINEST() << "category " << category.category_label_ << " age index " << i << " selectivity " << selectivity_value;
    }
  }

  for (auto& fishery_category : fishery_categories_) {
    if (fishery_category.fishery_.time_step_index_ != time_step_index)
      continue;

    for (unsigned i = 0; i < fishery_category.selectivity_values_.size(); ++i) {
      fishery_category.selectivity_values_[i]
          = fishery_category.selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
      LOG_FINEST() << "fishery category " << fishery_category.fishery_label_ << " age index " << i << " selectivity " << selectivity_value;
    }
  }

  for (auto& fishery : fisheries_) fishery.second.vulnerability_ = 0.0;

  /**
   * This is where F gets applied, only enter if
   * - not in initialisation phase or no F in this time-step
   *  or if no F in this year or we are in projection years, as we probably want to project.
   * Loop for each category. Add the vulnerability from each
   * category in to the fisheries it belongs too
   */
  if ((model_->state() != State::kInitialise
       || (find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index)
           != time_steps_to_skip_applying_F_mortality_.end()))
      && ((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model_->final_year()))) {
    LOG_FINEST() << "time step = " << time_step_index << " not in initialisation and there is an F method in this timestep. year = " << model_->current_year();
    for (auto& fishery_category : fishery_categories_) {
      LOG_FINEST() << "checking fishery = " << fishery_category.fishery_label_;

      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category = fishery_category.category_.category_;

      Double vulnerable = 0.0;
      if (fishery_category.category_.age_weight_) {
        for (unsigned i = 0; i < category->data_.size(); ++i) {
          vulnerable += category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model_->min_age())
                        * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
        }
      } else if (is_catch_biomass_) {  // as biomass
        for (unsigned i = 0; i < category->data_.size(); ++i) {
          vulnerable += category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                        * fishery_category.category_.exp_values_half_m_[i];
        }
      } else {  // as abundance
        for (unsigned i = 0; i < category->data_.size(); ++i) {
          vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_m_[i];
        }
      }
      fishery_category.fishery_.vulnerability_ += vulnerable;

      LOG_FINEST() << "Category is fished in year " << year << " time_step " << time_step_index << ", numbers at age length = " << category->data_.size();
      LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << vulnerable;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable) for each fishery
     */
    Double exploitation;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      exploitation  = 0.0;

      // If fishery occurs in this time step calculate exploitation rate
      if (fishery.time_step_index_ == time_step_index) {
        exploitation = fishery.catches_[year] / utilities::math::ZeroFun(fishery.vulnerability_);

        fishery.exploitation_ = exploitation;
        fishery.uobs_fishery_ = exploitation;

        LOG_FINEST() << "Vulnerable biomass for fishery " << fishery.label_ << " = " << fishery.vulnerability_ << " with catch = " << fishery.catches_[year]
                     << " and exploitation = " << exploitation;
      } else if (fishery.time_step_index_ > time_step_index) {
        // reset exploitation for fisheries in subsequent time steps only
        fishery.exploitation_ = exploitation;
        fishery.uobs_fishery_ = exploitation;
      }
      // U_obs is used to account for selectivity, almost like a temporary container that we use to rescale exploitation_ at the end
      LOG_FINE() << "year = " << year << " time step = " << time_step_index << " fishery = " << fishery.label_ << " exploitation = " << fishery.exploitation_;
    }

    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category = fishery_category.category_.category_;
      for (unsigned i = 0; i < category->data_.size(); ++i) {
        // remove the deep copying.
        fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i];
      }
    }

    /*
     * Calculate u_obs for each fishery, this is defined as the maximum proportion of fish taken from any element of the partition
     * affected by fishery f in this time-step
     */
    bool recalculate_age_exploitation = false;
    LOG_FINEST() << "Size of fishery_categories_ " << fishery_categories_.size();

    LOG_FINEST() << "calc uobs";

    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "fishery = " << fishery_iter.first;
      auto& fishery = fishery_iter.second;

      // Don't enter if this fishery is not executed here.
      if (fishery.time_step_index_ != time_step_index)
        continue;

      auto& uobs = fishery.uobs_fishery_;
      uobs       = 0.0;
      for (auto& fishery_category : fishery_categories_) {
        if (fishery_category.fishery_.label_ != fishery.label_)
          continue;

        for (Double age_exploitation : fishery_category.category_.exploitation_) {
          uobs = uobs > age_exploitation ? uobs : age_exploitation;
        }
      }
    }
    LOG_FINEST() << "Size of fishery_categories_ " << fishery_categories_.size();
    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "fishery = " << fishery_iter.first;
      auto& fishery = fishery_iter.second;

      // Don't enter if this fishery is not executed here.
      if (fishery.time_step_index_ != time_step_index)
        continue;

      if (fishery.uobs_fishery_ > fishery.u_max_) {
        /**
         * Rescaling exploitation and applying penalties
         */
        LOG_FINE() << fishery.label_ << " exploitation rate before rescaling = " << fishery.exploitation_ << " uobs = " << fishery.uobs_fishery_;
        // This may seem weird to be greater than u_max but later we multiply it by the selectivity which scales it to U_max
        fishery.exploitation_ *= (fishery.u_max_ / fishery.uobs_fishery_);
        LOG_FINE() << "fishery = " << fishery.label_ << " U_obs = " << fishery.uobs_fishery_ << " and u_max " << fishery.u_max_;
        LOG_FINE() << fishery.label_ << " rescaled exploitation rate = " << fishery.exploitation_;

        recalculate_age_exploitation        = true;
        fishery.actual_catches_[year]       = fishery.vulnerability_ * fishery.exploitation_;
        fishery.exploitation_by_year_[year] = fishery.exploitation_;
        if (fishery.penalty_)
          fishery.penalty_->Trigger(label_, fishery.catches_[year], fishery.actual_catches_[year]);
      } else {
        fishery.actual_catches_[year]       = fishery.catches_[year];
        fishery.exploitation_by_year_[year] = fishery.exploitation_;
      }
      fishery.uobs_by_year_[year] = fishery.uobs_fishery_;
    }

    /**
     * recalculate age exploitation if we triggered penalty
     */
    if (recalculate_age_exploitation) {
      for (auto& category : categories_) {
        LOG_FINE() << "recalculating age exploitation for category " << category.category_label_ << " in time step " << time_step_index;
        for (unsigned i = 0; i < category.category_->age_spread(); ++i) category.exploitation_[i] = 0.0;
      }

      for (auto& fishery_category : fishery_categories_) {
        partition::Category* category = fishery_category.category_.category_;

        // Don't enter if this fishery is not executed here.
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
     * Calculate the expectation for a proportions_at_age observation
     */
    LOG_FINEST() << "Calculate proportions at age";
    if (find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) {
      // only calculate observed fits for years the process exists for
      unsigned age_spread      = model_->age_spread();
      unsigned category_offset = 0;
      for (auto& categories : partition_) {
        for (auto& fishery_category : fishery_categories_) {
          if (fishery_category.category_label_ == categories->name_ && fisheries_[fishery_category.fishery_label_].time_step_index_ == time_step_index) {
            LOG_FINEST() << "category = " << categories->name_ << " fishery = " << fishery_category.fishery_label_;
            LOG_FINEST() << " year ndx = " << this_year_iter.second << " fisheyr ndx = " << fishery_category.fishery_.fishery_ndx_
                         << " category ndx = " << fishery_category.category_.category_ndx_;
            for (unsigned i = 0; i < age_spread; ++i) {
              unsigned age_offset = categories->min_age_ - model_->min_age();

              if (i < age_offset)
                continue;
              removals_by_year_fishery_category_[this_year_iter.second][fishery_category.fishery_.fishery_ndx_][fishery_category.category_.category_ndx_][i]
                  = categories->data_[i - age_offset] * fishery_category.fishery_.exploitation_
                    * fishery_category.selectivity_->GetAgeResult(categories->min_age_ + i, categories->age_length_) * fishery_category.category_.exp_values_half_m_[i];
            }
          }
        }
        category_offset++;
      }
    }
  }  // if (model_->state() != State::kInitialise )

  /**
   * Remove the stock now using the exploitation rate
   */
  unsigned category_ndx = 0;
  for (auto& category : categories_) {
    LOG_FINEST() << "category " << category.category_label_ << " used in time step " << time_step_index << ": " << category.used_in_current_timestep_;

    for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
      // removals_by_category_age_[category_ndx][i] = category.category_->data_[i]; // initial numbers before process
      LOG_FINEST() << "category " << category.category_label_ << ": numbers at age = " << category.category_->data_[i] << " age " << i + model_->min_age()
                   << " exploitation = " << category.exploitation_[i] << " M = " << *category.m_ << " relative_m_by_age = " << category.selectivity_values_[i];

      category.category_->data_[i] *= category.exp_values_half_m_[i] * category.exp_values_half_m_[i] * (1.0 - category.exploitation_[i]);
      LOG_FINEST() << "category " << category.category_label_ << ": updated numbers at age = " << category.category_->data_[i] << " age " << i + model_->min_age();

      if (category.category_->data_[i] < 0.0) {
        LOG_CODE_ERROR() << " Fishing caused a negative partition : if (categories->data_[i] < 0.0), category.category_->data_[i] = " << category.category_->data_[i]
                         << " i = " << i + 1 << "; category " << category.category_label_ << ": numbers at age = " << category.category_->data_[i] << " age "
                         << i + model_->min_age() << " exploitation = " << category.exploitation_[i] << " relative_m_by_age = " << category.selectivity_values_[i]
                         << " M = " << *category.m_ << " time step = " << time_step_index << " used in time step = " << category.used_in_current_timestep_;
      }
      // removals_by_category_age_[category_ndx][i] -= category.category_->data_[i]; // minus what was left thus keeping the difference
    }
    ++category_ndx;
  }
  // removals_by_year_category_age_[model_->current_year()] = removals_by_category_age_;
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityInstantaneous::FillReportCache(ostringstream& cache) {
  LOG_FINE();
  // This one is niggly because we need to iterate over each year and time step to print the right information so we don't
  vector<unsigned> years = model_->years();

  cache << "year: ";
  for (auto year : years) cache << year << " ";

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

  /*
    cache << "removals " << REPORT_R_DATAFRAME << "\n";
    cache << "year category";
    unsigned age = model_->min_age();
    while(age <= model_->max_age()) {
      cache << " " << age ;
      ++age;
    }
    cache << REPORT_EOL;

    for (auto& removals : removals_by_year_category_age_) {
      LOG_FINE() << "printing year = " << removals.first;
      for (unsigned category_ndx = 0; category_ndx < removals.second.size(); ++category_ndx) {
        cache << removals.first << " " << category_labels_[category_ndx];
        for (unsigned age_ndx = 0; age_ndx < removals.second[category_ndx].size(); ++age_ndx) {
          cache << " " << removals.second[category_ndx][age_ndx];
        }
        cache << REPORT_EOL;
      }
    }
  */
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 */
void MortalityInstantaneous::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    // print header
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto pressure : fishery.exploitation_by_year_) cache << "exploitation_rate[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto pressure : fishery.uobs_by_year_) cache << "fishing_pressure[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto catches : fishery.catches_) cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_) cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
    }
    cache << REPORT_EOL;
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto pressure : fishery.exploitation_by_year_) cache << AS_DOUBLE(pressure.second) << " ";
    for (auto uobs_pressure : fishery.uobs_by_year_) cache << AS_DOUBLE(uobs_pressure.second) << " ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
  }
  cache << REPORT_EOL;
}

/**
 * Check the categories in methods for removal obs
 * @description method checks if there is a category in each method, to make sure the observation class is compatable with the process
 * @param methods a vector of methods
 * @param category_labels a vector of categories to check.
 */
bool MortalityInstantaneous::check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels) {
  LOG_TRACE();

  unsigned fishery_index = 0;

  for (; fishery_index < methods.size(); ++fishery_index) {
    unsigned categories_counter = 0;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      // Check that the fishery occurs in this time step.
      if (fishery.label_ != methods[fishery_index])
        continue;
      // Check all categories are in this method
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
  

  return true;
}

/**
 * Check the years in methods for removal obs
 * @description method checks if there is a category in each method for each year, to make sure the observation class is compatable with the process
 * @param years a vector of years
 * @param methods a vector of methods
 */
bool MortalityInstantaneous::check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods) {
  LOG_TRACE();
  for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      // Check that the fishery occurs in this time step.
      if (fishery.label_ != methods[fishery_index])
        continue;
      unsigned year_counter = 0;
      for (auto& catches : fishery.catches_) {
        // Check year is in the vector
        if (find(years.begin(), years.end(), catches.first) != years.end()) {
          if (catches.second <= 0)
            return false;
          ++year_counter;
        }
      }
      if (year_counter != years.size())
        return false;
    }
  }
  return true;
}

/**
 * Check the categories in methods for removal obs
 * @description method checks if each method exists, to make sure the observation class is compatable with the process
 * @param methods a vector of methods
 */
bool MortalityInstantaneous::check_methods_for_removal_obs(vector<string> methods) {
  LOG_TRACE();
  unsigned method_counter = 0;
  for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
    for (auto& fishery_iter : fisheries_) {
      if (fishery_iter.second.label_ == methods[fishery_index])
        ++method_counter;
    }
  }
  if (method_counter != methods.size())
    return false;

  return true;
}
/**
 * for a set of given years return the ndx for the catch at object.
 * This should only be called in the build, and then during execute
 * observations will query this class with these indicies using the
 * accessor get_catch_at_by_year_fishery_category()
 * this function assumes all fishery labs have been passed.
 */
vector<unsigned> MortalityInstantaneous::get_fishery_ndx_for_catch_at(vector<string> fishery_labels) {
  vector<unsigned> fishery_ndxs;
  for (unsigned fishery_ndx = 0; fishery_ndx < fishery_labels.size(); ++fishery_ndx) {
    std::pair<bool, int> this_fishery_iter = findInVector(fishery_labels_, fishery_labels[fishery_ndx]);
    if (!this_fishery_iter.first) {
      LOG_CODE_ERROR() << "couldn't find fishery lab = " << fishery_labels[fishery_ndx] << " in fishery_labels_. this should be validated before this function is used.";
    }
    fishery_ndxs.push_back(this_fishery_iter.second);
  }
  return fishery_ndxs;
}

/**
 * for a set of given years return the ndx for the catch at object.
 * This should only be called in the build, and then during execute
 * observations will query this class with these indicies using the
 * accessor get_catch_at_by_year_fishery_category()
 * this function assumes all fishery labs have been passed.
 */
vector<unsigned> MortalityInstantaneous::get_year_ndx_for_catch_at(vector<unsigned> years) {
  vector<unsigned> year_ndxs;
  for (unsigned year_ndx = 0; year_ndx < years.size(); ++year_ndx) {
    std::pair<bool, int> this_year_iter = findInVector(process_years_, years[year_ndx]);
    if (!this_year_iter.first) {
      LOG_CODE_ERROR() << "couldn't find year = " << years[year_ndx] << " in process_years_. this should be validated before this function is used.";
    }
    year_ndxs.push_back(this_year_iter.second);
  }
  return year_ndxs;
}
/**
 * for a set of given years return the ndx for the catch at object.
 * This should only be called in the build, and then during execute
 * observations will query this class with these indicies using the
 * accessor get_catch_at_by_year_fishery_category()
 * this function assumes all fishery labs have been passed.
 */
vector<unsigned> MortalityInstantaneous::get_category_ndx_for_catch_at(vector<string> category_labels) {
  vector<unsigned> category_ndxs;
  for (unsigned category_ndx = 0; category_ndx < category_labels.size(); ++category_ndx) {
    std::pair<bool, int> this_category_iter = findInVector(category_labels_, category_labels[category_ndx]);
    if (!this_category_iter.first) {
      LOG_CODE_ERROR() << "couldn't find category = " << category_labels[category_ndx] << " in category_labels_. this should be validated before this function is used.";
    }
    category_ndxs.push_back(this_category_iter.second);
  }
  return category_ndxs;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
