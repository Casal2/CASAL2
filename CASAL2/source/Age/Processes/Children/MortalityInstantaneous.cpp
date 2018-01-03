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

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Common/Categories/Categories.h"
#include "Common/Model/Managers.h"
#include "Common/Penalties/Manager.h"
#include "Common/Selectivities/Manager.h"
#include "Common/TimeSteps/TimeStep.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/Utilities/To.h"
#include "Common/Utilities/Math.h"
#include "Age/AgeWeights/Manager.h"

// namespaces
namespace niwa {
namespace age {
namespace processes {
namespace math = niwa::utilities::math;

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
MortalityInstantaneous::MortalityInstantaneous(Model* model)
  : Process(model),
    partition_(model) {
  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;

  catches_table_ = new parameters::Table(PARAM_CATCHES);
  method_table_ = new parameters::Table(PARAM_METHOD);
  // catches_table_->set_required_columns({"years"}, allow_others = true)
  // method_table_->set_required_columns({"x", "x", "x,"});

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories for instantaneous mortality", "");
  parameters_.BindTable(PARAM_CATCHES, catches_table_, "Table of removals (catch) data", "", true, false);
  parameters_.BindTable(PARAM_METHOD, method_table_, "Table of Method of removal data", "", true, false);
  parameters_.Bind<Double>(PARAM_M, &m_input_, "Natural mortality rates for each category", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_TIME_STEP_RATIO, &time_step_ratios_temp_, "Time step ratios for natural mortality", "", true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "The selectivities to apply on the categories for natural mortality", "");

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
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityInstantaneous::DoValidate() {

  // Check Natural Mortality parameter first
  for (auto M_proportion : time_step_ratios_temp_) {
    if ((M_proportion < 0.0) | (M_proportion > 1.0))
      LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << "Natural Mortality time step ratio cannot be greater than 1 or less than 0 for a given time step";
  }

  /**
   * Load a temporary map of the fishery catch data so we can use this
   * when we load our vector of FisheryData objects
   */
  map<string, map<unsigned, Double>> fishery_year_catch;
  auto columns = catches_table_->columns();
  // TODO Need to catch if key column headers are missing for example year
  if (std::find(columns.begin(), columns.end(), PARAM_YEAR) == columns.end())
    LOG_ERROR_P(PARAM_CATCHES) << "Cannot find the column " << PARAM_YEAR << ", this column is needed, for casal2 to run this process. Please add it =)";
  unsigned year_index = std::find(columns.begin(), columns.end(), PARAM_YEAR) - columns.begin();
  LOG_FINEST() << "year_index for fisheries table is: " << year_index;

  auto model_years = model_->years();
  auto rows = catches_table_->data();
  for (auto row : rows) {
    unsigned year = 0;
    if (!utilities::To<string, unsigned>(row[year_index], year))
      LOG_ERROR_P(PARAM_CATCHES) << "year value " << row[year_index] << " is not numeric.";
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_CATCHES) << "year " << year << " is not a valid year in this model";
    for (unsigned i = 0; i < row.size(); ++i) {

      if (i == year_index)
        continue;

      Double value = 0.0;
      if (!utilities::To<string, Double>(row[i], value))
        LOG_ERROR_P(PARAM_CATCHES) << "value " << row[i] << " for fishery " << columns[i] << " is not numeric";
      fishery_year_catch[columns[i]][year] = value;
    }
  }

  /**
   * Validate the non-table parameters now. These are mostly related to the natural mortality
   * aspect of the process.
   */
  if (selectivity_labels_.size() == 1)
      selectivity_labels_.assign(category_labels_.size(), selectivity_labels_[0]);
  if (selectivity_labels_.size() != category_labels_.size()) {
    LOG_FATAL_P(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << selectivity_labels_.size();
  }

  if (m_input_.size() == 1)
    m_input_.assign(category_labels_.size(), m_input_[0]);
  if (m_input_.size() != category_labels_.size())
    LOG_FATAL_P(PARAM_M)
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << m_input_.size();
  for (unsigned i = 0; i < m_input_.size(); ++i)
    m_[category_labels_[i]] = m_input_[i];

  /**
   * Build all of our category objects
   */
  categories_.resize(category_labels_.size());
  for (unsigned i = 0; i < category_labels_.size(); ++i) {
  string label = category_labels_[i];
    CategoryData category;
    category.category_label_ = label;
    category.m_ = &m_[label];
    category.selectivity_label_ = selectivity_labels_[i];

    categories_[i]= category;
    category_data_[label] = &categories_[i];
  }

  /**
   * Now we want to load the information from the fisheries table.
   * We create a master fishery object and then multiple category objects
   * depending on how many categories are defined.
   */
  columns = method_table_->columns();
  rows = method_table_->data();
  LOG_FINE() << "method_table.columns.size(): " << columns.size();
  LOG_FINE() << "method_table.rows.size(): " << rows.size();
  LOG_FINE() << "columns: " << boost::join(columns, " ");
  for (auto row : rows) {
    LOG_FINE() << "row: " << boost::join(row, " ");
  }

  // Check the column headers are all specified corectly
  if (std::find(columns.begin(), columns.end(), PARAM_METHOD) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "Cannot find the column " << PARAM_METHOD << ", this column is needed, for casal2 to run this process. Please add it =)";
  if (std::find(columns.begin(), columns.end(), PARAM_CATEGORY) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "Cannot find the column " << PARAM_CATEGORY << ", this column is needed, for casal2 to run this process. Please add it =)";
  if (std::find(columns.begin(), columns.end(), PARAM_SELECTIVITY) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "Cannot find the column " << PARAM_SELECTIVITY << ", this column is needed, for casal2 to run this process. Please add it =)";
  if (std::find(columns.begin(), columns.end(), PARAM_TIME_STEP) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "Cannot find the column " << PARAM_TIME_STEP << ", this column is needed, for casal2 to run this process. Please add it =)";
  if (std::find(columns.begin(), columns.end(), PARAM_U_MAX) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "Cannot find the column " << PARAM_U_MAX << ", this column is needed, for casal2 to run this process. Please add it =)";
  if (std::find(columns.begin(), columns.end(), PARAM_PENALTY) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "Cannot find the column " << PARAM_PENALTY << ", this column is needed, for casal2 to run this process. Please add it =)";
  if (std::find(columns.begin(), columns.end(), PARAM_AGE_WEIGHT) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "Cannot find the column " << PARAM_AGE_WEIGHT << ", this column is needed, for casal2 to run this process. Please add it =)";

  unsigned fishery_index      = std::find(columns.begin(), columns.end(), PARAM_METHOD) - columns.begin();
  unsigned category_index     = std::find(columns.begin(), columns.end(), PARAM_CATEGORY) - columns.begin();
  unsigned selectivity_index  = std::find(columns.begin(), columns.end(), PARAM_SELECTIVITY) - columns.begin();
  unsigned time_step_index    = std::find(columns.begin(), columns.end(), PARAM_TIME_STEP) - columns.begin();
  unsigned u_max_index        = std::find(columns.begin(), columns.end(), PARAM_U_MAX) - columns.begin();
  unsigned penalty_index      = std::find(columns.begin(), columns.end(), PARAM_PENALTY) - columns.begin();
  unsigned age_weight_index   = std::find(columns.begin(), columns.end(), PARAM_AGE_WEIGHT) - columns.begin();

  LOG_FINEST() << "indexes: fishery=" << fishery_index << "; category=" << category_index << "; selectivity="
      << selectivity_index << "; time_step=" << time_step_index << "; u_max=" << u_max_index
      << "; penalty" << penalty_index << " age weight index " << age_weight_index;
  // This is object is going to check the business rule that a fishery can only exist in one time-step in each year
  map<string,vector<string>> fishery_time_step;
  for (auto row : rows) {
    FisheryData new_fishery;
    new_fishery.label_              = row[fishery_index];
    new_fishery.time_step_label_    = row[time_step_index];
    new_fishery.penalty_label_      = row[penalty_index];
    fishery_time_step[new_fishery.label_].push_back(new_fishery.time_step_label_);
    if (!utilities::To<string, Double>(row[u_max_index], new_fishery.u_max_))
      LOG_ERROR_P(PARAM_METHOD) << "u_max value " << row[u_max_index] << " is not numeric";
    if (fishery_year_catch.find(new_fishery.label_) == fishery_year_catch.end())
      LOG_ERROR_P(PARAM_METHOD) << "fishery " << new_fishery.label_ << " does not have catch information in the catches table";
    new_fishery.catches_ = fishery_year_catch[new_fishery.label_];
    new_fishery.actual_catches_ = fishery_year_catch[new_fishery.label_];

    fisheries_[new_fishery.label_] = new_fishery;
    
    RegisterAsAddressable(PARAM_METHOD + string("_") + utilities::ToLowercase(new_fishery.label_), &fisheries_[new_fishery.label_].catches_);

    LOG_FINEST() << "Creating addressable: " << PARAM_FISHERY + string("_") + utilities::ToLowercase(new_fishery.label_), &fisheries_[new_fishery.label_].catches_;
    // remove after build
    vector<string> categories;
    vector<string> selectivities;
    vector<string> age_weights;

    boost::split(categories, row[category_index], boost::is_any_of(","));
    boost::split(selectivities, row[selectivity_index], boost::is_any_of(","));
    boost::split(age_weights, row[age_weight_index], boost::is_any_of(","));

    if (categories.size() != selectivities.size())
      LOG_FATAL_P(PARAM_METHOD) << "The number of categories (" << categories.size()
      << ") and selectivities (" << selectivities.size() << ") provided must be identical";

    for (unsigned i = 0; i < categories.size(); ++i) {
      FisheryCategoryData new_category_data(fisheries_[new_fishery.label_], *category_data_[categories[i]]);
      new_category_data.fishery_label_     = row[fishery_index];
      new_category_data.category_label_    = categories[i];
      // check categories are in category_labels_ as well
		  if (std::find(category_labels_.begin(), category_labels_.end(), categories[i]) == category_labels_.end())
		  	LOG_ERROR_P(PARAM_METHOD) << "Found the category " << categories[i] << " in table but not in the '" << PARAM_CATEGORIES << "' subcommand, this means you are applying exploitation processes and not natural mortality, which is not currently allowed. Make sure all categories in the methods table are in the categories subcommand.";
      new_category_data.selectivity_label_ = selectivities[i];

      new_category_data.category_.age_weight_label_ = age_weights[i];

      fishery_categories_.push_back(new_category_data);
    }
  }
  // Check the business rule that a fishery can only exist one time-step
  for(auto fishery : fishery_time_step) {
    if (!std::equal(fishery.second.begin() + 1, fishery.second.end(), fishery.second.begin()))
      LOG_ERROR_P(PARAM_METHOD) << "Found method '" << fishery.first << "' in more than one time step. You can only have a method occur in each time step. If a fishery occcurs in multiple time steps then define each time step as a seperate fishery.";
  }
  // Check to see if there are any time_steps that we don't have enter the fisheries section. i.e no fishing in certain time-steps
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 *
 * Validate any parameters that require information from other objects
 * in the system
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
    category.exp_values_.assign(category.category_->age_spread(), 0.0);
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
  vector<TimeStep*> time_steps = model_->managers().time_step()->ordered_time_steps();
  vector<unsigned> active_time_steps;
  for (unsigned i = 0; i < time_steps.size(); ++i) {
    if (time_steps[i]->HasProcess(label_))
      active_time_steps.push_back(i);
  }

  if (time_step_ratios_temp_.size() == 0) {
    for (unsigned i : active_time_steps)
      time_step_ratios_[i] = 1.0;
  } else {
    if (time_step_ratios_temp_.size() != active_time_steps.size())
      LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << " length (" << time_step_ratios_temp_.size()
          << ") does not match the number of time steps this process has been assigned to (" << active_time_steps.size() << ")";

    for (Double value : time_step_ratios_temp_) {
      if (value < 0.0 || value > 1.0)
        LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << " value (" << value << ") must be between 0.0 (exclusive) and 1.0 (inclusive)";
    }

    for (unsigned i = 0; i < time_step_ratios_temp_.size(); ++i)
      time_step_ratios_[active_time_steps[i]] = time_step_ratios_temp_[i];
  }

  /**
   * Assign the selectivity, penalty and time step index to each fisher data object
   */
  for (auto& fishery_category : fishery_categories_) {
    fishery_category.selectivity_ = model_->managers().selectivity()->GetSelectivity(fishery_category.selectivity_label_);
    /**
     * Check the fishery categories are valid
     */
    if (!model_->categories()->IsValid(fishery_category.category_label_))
        LOG_ERROR_P(PARAM_METHOD) << ": category " << fishery_category.category_label_ << " does not exist. Have you defined it?";

    if (!fishery_category.selectivity_)
      LOG_ERROR_P(PARAM_METHOD) << "selectivity " << fishery_category.selectivity_label_ << " does not exist. Have you defined it?";
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    if (fishery.penalty_label_ != "none") {
      fishery.penalty_ = model_->managers().penalty()->GetProcessPenalty(fishery.penalty_label_);
      if (!fishery.penalty_)
        LOG_ERROR_P(PARAM_METHOD) << ": penalty " << fishery.penalty_label_ << " does not exist. Have you defined it?";
    }
    bool check_time_step = model_->managers().time_step()->CheckTimeStep(fishery.time_step_label_);
    if (!check_time_step)
      LOG_FATAL_P(PARAM_METHOD) << "The time step " << fishery.time_step_label_ << " could not be found have you defined it in the annual cycle";
    fishery.time_step_index_ = model_->managers().time_step()->GetTimeStepIndex(fishery.time_step_label_);
  }

  /**
   * Check the natural mortality categories are valid
   */
  for (const string& label : category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }

  /**
   * Assign the natural mortality selectivities
   */
  for (auto& category : categories_) {
    // Selectivity
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(category.selectivity_label_);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << "selectivity " << category.selectivity_label_ << " does not exist. Have you defined it?";
    category.selectivity_ = selectivity;
    selectivities_.push_back(selectivity);

    // Age Weight if it is defined
    LOG_FINEST() << "age weight " << category.age_weight_label_;
    if (category.age_weight_label_ != PARAM_NONE) {
      LOG_FINE() << "age weight found";
      AgeWeight* age_weight = model_->managers().age_weight()->FindAgeWeight(category.age_weight_label_);
      if (!age_weight)
        LOG_ERROR_P(PARAM_METHOD) << "age weight " << category.age_weight_label_ << " does not exist. Have you defined it?";
      category.age_weight_ = age_weight;
    } else {
      category.age_weight_label_ = PARAM_NONE;
      category.age_weight_ = nullptr;

    }
  }

  /**
   * Find what time_steps Instant Mortality is applied in
   */
  vector<unsigned> instant_mort_time_step;
  unsigned i = 0;
  for (auto time_step : model_->managers().time_step()->ordered_time_steps()) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kMortality && process->type() == PARAM_MORTALITY_INSTANTANEOUS) {
        LOG_FINEST() << "instant_mortality process in time step " << i;
        instant_mort_time_step.push_back(i);
      }
    }
    ++i;
  }

  // Now out of these lets see if we can skip the exploitation code i.e no F just M
  for (auto time_step : instant_mort_time_step) {
    bool fishery_in_timestep = false;
    for (auto& fishery_iter : fisheries_) {
      LOG_FINEST() << "checking fishery " << fishery_iter.first << " in time step index = " << fishery_iter.second.time_step_index_;
      if (fishery_iter.second.time_step_index_ == time_step) {
        LOG_FINEST() << "fishery = " << fishery_iter.first << " is in time step " << time_step;
        fishery_in_timestep = true;
      }
    }
    if (!fishery_in_timestep) {
      time_steps_to_skip_applying_F_mortaltiy_.push_back(time_step);
      LOG_FINEST() << "time step " << time_step << " doesn't have a method associated so we will skip the exploitation calculation during DoExecute";
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
 * Called from the time varying class (see TimeVarying.cpp line 96) used to reset parameters.
 * this avoids a full reset and clearing of printed values which is what was happening before
 */
void MortalityInstantaneous::RebuildCache() {
	LOG_TRACE();
  unsigned m_iter = 0;
  for (auto m : m_) {
  	LOG_FINEST() << "resetting M for category " << m.first << " = " << m.second;
    m_input_[m_iter] = m.second;
    ++m_iter;
  }
}

/**
 * Execute this process
 */

void MortalityInstantaneous::DoExecute() {
  LOG_TRACE();

  unsigned time_step_index = model_->managers().time_step()->current_time_step();
  unsigned year =  model_->current_year();
  Double ratio = time_step_ratios_[time_step_index];
  Double selectivity_value = 0.0;

  for (auto& category : categories_) {
    // Is this category used?
    bool used = false;
    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.category_label_ == category.category_label_ && fishery_category.fishery_.time_step_index_ == time_step_index)
        used = true;

      category.used_in_current_timestep_ = used;

      for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
        selectivity_value = category.selectivity_->GetAgeResult(category.category_->min_age_ + i, category.category_->age_length_);
        category.exploitation_[i] = 0.0;
        category.selectivity_values_[i] = selectivity_value;
        if (used)
          category.exp_values_[i] = exp(-0.5 * ratio * (*category.m_) * selectivity_value);
      }
    }
  }

  for (auto& fishery_category : fishery_categories_) {
    if (fishery_category.fishery_.time_step_index_ != time_step_index)
      continue;

    for (unsigned i = 0; i < fishery_category.selectivity_values_.size(); ++i)
      fishery_category.selectivity_values_[i] = fishery_category.selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
  }

  for (auto& fishery : fisheries_)
    fishery.second.vulnerability_ = 0.0;

  /**
   * Loop for each category. Add the vulnerability from each
   * category in to the fisheries it belongs too
   */
  if (model_->state() != State::kInitialise || (find(time_steps_to_skip_applying_F_mortaltiy_.begin(),time_steps_to_skip_applying_F_mortaltiy_.end(), time_step_index) != time_steps_to_skip_applying_F_mortaltiy_.end())) {
    LOG_FINEST() << "time step = " << time_step_index << " not in initialisation and there is an F method in this timestep.";
      for (auto& fishery_category : fishery_categories_) {
        LOG_FINEST() << "checking fishery = " << fishery_category.fishery_label_;
        if (fishery_category.fishery_.time_step_index_ != time_step_index)
          continue;

        partition::Category* category = fishery_category.category_.category_;
        if (fishery_category.category_.age_weight_label_ != PARAM_NONE) {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            Double vulnerable = category->data_[i]
                * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model_->min_age())
                * fishery_category.selectivity_values_[i]
                * fishery_category.category_.exp_values_[i];

            fishery_category.fishery_.vulnerability_ += vulnerable;
          }
        } else {
          for (unsigned i = 0; i < category->data_.size(); ++i) {
            Double vulnerable = category->data_[i]
                * category->mean_weight_by_time_step_age_[time_step_index][category->min_age_ + i]
                * fishery_category.selectivity_values_[i]
                * fishery_category.category_.exp_values_[i];

            fishery_category.fishery_.vulnerability_ += vulnerable;
          }
        }
        LOG_FINEST() << "Category is fished in this time_step " << time_step_index << " numbers at age = " << category->data_.size();
        LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << fishery_category.fishery_.vulnerability_;
      }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable) for each fishery
     */
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      Double exploitation = 0.0;
      // If fishery occurs in this time step calculate exploitation rate
      if (fishery.time_step_index_ == time_step_index) {
        exploitation = fishery.catches_[year] / utilities::doublecompare::ZeroFun(fishery.vulnerability_);
        LOG_FINEST() << " Vulnerable biomass for fishery : " << fishery.label_ << " = " << fishery.vulnerability_ << " with Catch = " << fishery.catches_[model_->current_year()] << " = exploitation = " << exploitation;
      }

      fishery.exploitation_ = exploitation;
    }

    for (auto& fishery_category : fishery_categories_) {
      if (fishery_category.fishery_.time_step_index_ != time_step_index)
        continue;

      partition::Category* category = fishery_category.category_.category_;
      for (unsigned i = 0; i < category->data_.size(); ++i) {
        fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i];
      }
    }

  /*
   * Calculate u_obs for each fishery, this is defined as the maximum proportion of fish taken from any element of the partition
   * affected by fishery f in this time-step
  */
    bool recalculate_age_exploitation = false;
    LOG_FINEST() << "Size of fishery_categories_ " << fishery_categories_.size();
    vector<string> fisheries_checked;

    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      // Don't enter if this fishery is not executed here.
      if (fishery.time_step_index_ != time_step_index)
        continue;
      auto& uobs = fishery.uobs_fishery_;
      uobs = 0.0;
      for (auto& fishery_category : fishery_categories_) {
        if (fishery_category.fishery_.label_ != fishery.label_)
          continue;
        for (Double age_exploitation : fishery_category.category_.exploitation_) {
          uobs = uobs > age_exploitation ? uobs : age_exploitation;
        }
      }
    }

    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      if (fishery.uobs_fishery_ > u_max_) {
        /**
         * Rescaling exploitation and applying penalties
         */
        LOG_FINE() << fishery.label_ << " exploitation rate before rescaling = " << fishery.exploitation_ << " uobs = " << fishery.uobs_fishery_;
        fishery.exploitation_ *= (u_max_ / fishery.uobs_fishery_); // This may seem weird to be greater than u_max but later we multiply it by the selectivity which scales it to U_max
        LOG_FINE() << "fishery = " << fishery.label_ << " U_obs = " << fishery.uobs_fishery_ << " and u_max " << u_max_;
        LOG_FINE() << fishery.label_ << " Rescaled exploitation rate = " << fishery.exploitation_;
        recalculate_age_exploitation = true;
        fishery.actual_catches_[year] = fishery.vulnerability_ * fishery.exploitation_;
        fishery.exploitation_by_year_[year] = fishery.exploitation_;
        if (fishery.penalty_)
          fishery.penalty_->Trigger(label_, fishery.catches_[year], fishery.actual_catches_[year]);
      } else {
        fishery.actual_catches_[year] = fishery.catches_[year];
        fishery.exploitation_by_year_[year] = fishery.uobs_fishery_;
      }
    }

    /**
     * recalculate age exploitation if we triggered penalty
     */
    if (recalculate_age_exploitation) {
      for (auto& category : categories_) {
        if (!category.used_in_current_timestep_)
          continue;

        for (unsigned i = 0; i < category.category_->age_spread(); ++i)
          category.exploitation_[i] = 0.0;
      }

      for (auto& fishery_category : fishery_categories_) {
        partition::Category* category = fishery_category.category_.category_;
        for (unsigned i = 0; i < category->data_.size(); ++i) {
          fishery_category.category_.exploitation_[i] += fishery_category.fishery_.exploitation_ * fishery_category.selectivity_values_[i];
        }
      }
    }
    /**
     * Calculate the expectation for a proportions_at_age observation
     *
     */
    unsigned age_spread = model_->age_spread();
    unsigned category_offset = 0;
    for (auto& categories : partition_) {
      for (auto& fishery_category : fishery_categories_) {
        if (fishery_category.category_label_ == categories->name_
            && fisheries_[fishery_category.fishery_label_].time_step_index_ == time_step_index) {
          removals_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_].assign(age_spread, 0.0);
          for (unsigned i = 0; i < age_spread; ++i) {
            unsigned age_offset = categories->min_age_ - model_->min_age();
            if (i < age_offset)
              continue;
            removals_by_year_fishery_category_[year][fishery_category.fishery_label_][categories->name_][i] += categories->data_[i - age_offset]
//                * fishery_exploitation[fishery_category.fishery_label_]
                * fishery_category.fishery_.exploitation_
                * fishery_category.selectivity_->GetAgeResult(categories->min_age_ + i, categories->age_length_)
                * exp(-0.5 * ratio * m_[categories->name_] * selectivities_[category_offset]->GetAgeResult(categories->min_age_ + i, categories->age_length_));
          }
        }
      }
      category_offset++;
    }

  } // if (model_->state() != State::kInitialise )

  /**
   * Remove the stock now using the exploitation rate
   */
  for (auto& category : categories_) {
    for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
      category.category_->data_[i] *= exp(-(*category.m_) * ratio * category.selectivity_values_[i]) * (1 - category.exploitation_[i]);
      if (category.category_->data_[i] < 0.0) {
        LOG_CODE_ERROR() << " Fishing caused a negative partition : if (categories->data_[i] < 0.0), category.category_->data_[i] = " << category.category_->data_[i] << " i = " << i + 1;
      }
    }
  }
}

/*
 * @fun FillReportCache
 * @description A method for reporting process information
 * @param cache a cache object to print to
*/
void MortalityInstantaneous::FillReportCache(ostringstream& cache) {
  // This one is niggly because we need to iterate over each year and time step to print the right information so we don't
  vector<unsigned> years = model_->years();
  cache << "year: ";
  for (auto year : years)
    cache << year << " ";
  cache << "\n";
  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    cache << "\nfishing_pressure[" << fishery.label_ << "]: ";
    for (auto pressure : fishery.exploitation_by_year_)
      cache << pressure.second << " ";
    cache << "\ncatch[" << fishery.label_ << "]: ";
    for (auto catches : fishery.catches_)
      cache << catches.second << " ";
    cache << "\nactual_catch[" << fishery.label_ << "]: ";
    for (auto actual_catches : fishery.actual_catches_)
      cache << actual_catches.second << " ";
  }
}

/*
 * @fun FillTabularReportCache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
*/
void MortalityInstantaneous::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    // print header
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto pressure : fishery.exploitation_by_year_)
        cache << "fishing_pressure[" << fishery.label_ << "][" << pressure.first << "] ";
      for (auto catches : fishery.catches_)
        cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_)
        cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
    }
    cache << "\n";
  }
  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto pressure : fishery.exploitation_by_year_)
      cache << pressure.second << " ";
    for (auto catches : fishery.catches_)
      cache << catches.second << " ";
    for (auto actual_catches : fishery.actual_catches_)
      cache <<  actual_catches.second << " ";
  }
}

/*
 * @fun check_categories_in_methods_for_removal_obs
 * @description method checks if there is a category in each method, to make sure the observation class is compatable with the process
 * @param methods a vector of methods
 * @param category_labels a vector of categories to check.
 *
*/
bool MortalityInstantaneous::check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels) {
  LOG_TRACE();

  unsigned fishery_index = 0;

  for (; fishery_index < methods.size(); ++fishery_index) {
    unsigned categories_counter = 0;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      // Check that the fishery occurs in this time step.
      if (fishery.label_!= methods[fishery_index])
        continue;
      // Check all categories are in this method
      for (unsigned category_index = 0; category_index < category_labels.size(); ++category_index) {
        for (auto& fishery_category : fishery_categories_) {
          if ((fishery_category.fishery_.label_ == fishery.label_) & (fishery_category.category_label_ == category_labels[category_index]))
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

/*
 * @fun check_years_in_methods_for_removal_obs
 * @description method checks if there is a category in each method for each year, to make sure the observation class is compatable with the process
 * @param years a vector of years
 * @param methods a vector of methods
 *
*/
bool MortalityInstantaneous::check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods) {
  LOG_TRACE();
  for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      // Check that the fishery occurs in this time step.
      if (fishery.label_!= methods[fishery_index])
        continue;
      unsigned year_counter = 0;
      for (auto& catches : fishery.catches_) {
        // Check year is in the vector
        if (find(years.begin(),years.end(),catches.first) != years.end()) {
          if(catches.second <= 0)
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

/*
 * @fun check_categories_in_methods_for_removal_obs
 * @description method checks if each method exists, to make sure the observation class is compatable with the process
 * @param methods a vector of methods
 *
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

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */

