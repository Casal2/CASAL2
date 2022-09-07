/**
 * @file MortalityHybrid.cpp
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * This mortality process is taken from Stock Synthesis see the hybrid approach 
 * found at
 * https://github.com/nmfs-stock-synthesis/stock-synthesis/blob/main/SS_popdyn.tpl
 *
 */

// headers
#include "MortalityHybrid.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "AgeLengths/AgeLength.h"
#include "AgeWeights/Manager.h"
#include "Categories/Categories.h"
#include "Model/Managers.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"
#include "Penalties/Manager.h"

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
MortalityHybrid::MortalityHybrid(shared_ptr<Model> model) : Mortality(model), partition_(model) {
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
  parameters_.Bind<Double>(PARAM_MAX_F, &max_F_, "The maximum F applied in a time-step", "", 4.0)->set_lower_bound(0.0);
  parameters_.Bind<unsigned>(PARAM_F_ITERATIONS, &F_iterations_, "The number of iterations to tune the F coeffecients", "", 4)->set_lower_bound(0);

  
  RegisterAsAddressable(PARAM_M, &m_);
}

/**
 * Destructor
 */
MortalityHybrid::~MortalityHybrid() {
  delete catches_table_;
  delete method_table_;
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityHybrid::DoValidate() {
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
      fishery_catch_[columns[i]][year]  = value; // needed for Mortality.h checks

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
  if (std::find(columns.begin(), columns.end(), PARAM_PENALTY) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_PENALTY << " was not found.";
  if (std::find(columns.begin(), columns.end(), PARAM_ANNUAL_DURATION) == columns.end())
    LOG_FATAL_P(PARAM_METHOD) << "The required column " << PARAM_ANNUAL_DURATION << " was not found.";
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
  unsigned penalty_index     = std::find(columns.begin(), columns.end(), PARAM_PENALTY) - columns.begin();
  unsigned annual_duration_index = std::find(columns.begin(), columns.end(), PARAM_ANNUAL_DURATION) - columns.begin();
  unsigned age_weight_index  = 999;

  if (use_age_weight_)
    age_weight_index = std::find(columns.begin(), columns.end(), PARAM_AGE_WEIGHT_LABEL) - columns.begin();

  LOG_FINEST() << "indexes: fishery=" << fishery_index << "; category=" << category_index << "; selectivity=" << selectivity_index << "; time_step=" << time_step_index
               <<"; time-step-prop " << annual_duration_index << "; age weight index " << age_weight_index;

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
    if (!utilities::To<string, Double>(row[annual_duration_index], new_fishery.annual_duration_))
      LOG_ERROR_P(PARAM_METHOD) << PARAM_ANNUAL_DURATION << " value " << row[annual_duration_index] << " could not be converted to a Double";
    
    if ((new_fishery.annual_duration_ < 0) | (new_fishery.annual_duration_ > 1)) {
      LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had " << PARAM_ANNUAL_DURATION << " = " << new_fishery.annual_duration_  << " at row " << row_iter
                                << " This must be a value between 0 and 1.";
    }
    if(new_fishery.annual_duration_ != 1) {
      LOG_WARNING() << "The " << PARAM_ANNUAL_DURATION << " for fishery " << new_fishery.label_ << " was equal to one. This an uncommon assumption this warning is to check that you understand this assumptions. Disregard this warning if you are comfortable with making this assumption.";
    }

    std::pair<bool, int> this_fishery_iter = findInVector(fishery_labels_, new_fishery.label_);
    LOG_FINE() << new_fishery.label_ << " found = " << this_fishery_iter.first << " ndx = " << this_fishery_iter.second;
    if (this_fishery_iter.first) {
      // we have already seen this fishery in the methods table
      // Do some checks and don't cache anything
      if (fisheries_[new_fishery.label_].time_step_label_ != new_fishery.time_step_label_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had timestep label = " << new_fishery.time_step_label_ << " at row " << row_iter
                                  << " but the same fishery had time step label = " << fisheries_[new_fishery.label_].time_step_label_
                                  << " at row = " << this_fishery_iter.second + 1 << " these need to be consistent for a single method";
      
      if (fisheries_[new_fishery.label_].annual_duration_ != new_fishery.annual_duration_)
        LOG_ERROR_P(PARAM_METHOD) << "Fishery labelled " << new_fishery.label_ << " had annual duration = " << new_fishery.annual_duration_ << " at row " << row_iter
                                  << " but the same fishery had  annual duration = " << fisheries_[new_fishery.label_].annual_duration_
                                  << " at row = " << this_fishery_iter.second + 1 << " these need to be consistent for a single fishery";
      
                               
    } else {
      // haven't seen this method so store it in the fisheries struct
      fishery_labels_.push_back(new_fishery.label_);
      new_fishery.fishery_ndx_ = fishery_labels_.size() - 1;
      fishery_time_step[new_fishery.label_].push_back(new_fishery.time_step_label_);
      if (fishery_year_catch.find(new_fishery.label_) == fishery_year_catch.end())
        LOG_ERROR_P(PARAM_METHOD) << "fishery " << new_fishery.label_ << " does not have catch information in the catches table";
      new_fishery.catches_        = fishery_year_catch[new_fishery.label_];
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
      fishery_category_check_[new_fishery.label_].push_back((*category_data_[categories[i]]).category_label_);

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
void MortalityHybrid::DoBuild() {
  LOG_TRACE();
  partition_.Init(category_labels_);

  /**
   * Build all of our category objects
   */
  for (auto& category : categories_) {
    category.category_ = &model_->partition().category(category.category_label_);
    category.survivor_with_z_by_age_.assign(category.category_->age_spread(), 0.0);
    category.z_values_by_age_.assign(category.category_->age_spread(), 0.0);
    category.exp_values_half_duration_m_.assign(category.category_->age_spread(), 0.0);
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
   * Assign the selectivity, and time step index to each fisher data object
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
      if (process->process_type() == ProcessType::kMortality && process->type() == PARAM_MORTALITY_HYBRID) {
        LOG_FINEST() << PARAM_MORTALITY_HYBRID << " process in time step " << i;
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
  LOG_FINE() << "years " << process_years_.size();
  // allocate memory for observation object
  const vector<TimeStep*> ordered_time_steps = model_->managers()->time_step()->ordered_time_steps();
  LOG_FINE() << "time steps = " << ordered_time_steps.size();
  LOG_FINE() << "partitions = " << partition_.size();
  removals_by_year_fishery_category_.resize(process_years_.size());
  total_catch_by_year_timestep_.resize(process_years_.size());
  for (unsigned year_ndx = 0; year_ndx < process_years_.size(); ++year_ndx) {
    total_catch_by_year_timestep_[year_ndx].resize(ordered_time_steps.size(), 0.0);
    removals_by_year_fishery_category_[year_ndx].resize(fisheries_.size());
    for (unsigned fishery_ndx = 0; fishery_ndx < fisheries_.size(); ++fishery_ndx) {
      removals_by_year_fishery_category_[year_ndx][fishery_ndx].resize(category_labels_.size());
      for (unsigned category_ndx = 0; category_ndx < category_labels_.size(); ++category_ndx)
        removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx].resize(model_->age_spread(), 0.0);
    }
  }
  // populate total_catch_by_year_timestep_


  unsigned year_iter = 0;
  for(auto year : process_years_) {
    for(unsigned time_step_iter = 0; time_step_iter < ordered_time_steps.size(); ++time_step_iter) {
      for (auto& fishery_iter : fisheries_) {
        auto& fishery = fishery_iter.second;
        if (fishery.time_step_index_ != time_step_iter)
          continue;
        total_catch_by_year_timestep_[year_iter][time_step_iter] += fishery.catches_[year];
      }
    }
    ++year_iter;
  }
  // check fisheries in the same time-step have the same annual duration
  Double current_annual_value = 0.0;
  string last_fishery = "";
  bool first_fishery = true;
  annual_duration_by_timestep_.resize(ordered_time_steps.size());
  for(unsigned time_step_iter = 0; time_step_iter < ordered_time_steps.size(); ++time_step_iter) {
    current_annual_value = 0.0;
    first_fishery = true;
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      if (fishery.time_step_index_ != time_step_iter)
        continue;
      if(first_fishery) {
        current_annual_value = fishery.annual_duration_;
        last_fishery = fishery.label_;
        first_fishery = false;
      } else {
        if(current_annual_value != fishery.annual_duration_)
          LOG_ERROR_P(PARAM_METHOD) << "Fishery " << fishery.label_ << " had an annual duration = " << fishery.annual_duration_ << ". Whereas fishery " << last_fishery << " had an annual duration value = " << current_annual_value << ". Fisheries in the same time-step have to have the same annual duration value.";
      }
    }
    annual_duration_by_timestep_[time_step_iter] = current_annual_value;
  }

}

/**
 * Reset the M parameter
 */
void MortalityHybrid::DoReset() {
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
void MortalityHybrid::RebuildCache() {
  DoReset();
}

/**
 * Execute this process
 */
void MortalityHybrid::DoExecute() {
  LOG_TRACE();
  unsigned             time_step_index = model_->managers()->time_step()->current_time_step();
  unsigned             year            = model_->current_year();
  double               ratio           = time_step_ratios_[time_step_index];
  std::pair<bool, int> this_year_iter  = findInVector(process_years_, year);
  LOG_FINE() << "state = " << model_->state();
  LOG_FINE() << "Year = " << year << " time-step = " << time_step_index << " found? = " << this_year_iter.first << " should = " << process_years_[this_year_iter.second];
  LOG_FINE() << "season duration = " << annual_duration_by_timestep_[time_step_index];
  Double selectivity_value = 0.0;
  // This is to do with M. so pre-allocate and only call the exp call once per category
  // per execute. will be used in initialisation and when Catch is not removed
  for (auto& category : categories_) {
    for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
      selectivity_value               = category.selectivity_->GetAgeResult(category.category_->min_age_ + i, category.category_->age_length_);
      category.z_values_by_age_[i]       = annual_duration_by_timestep_[time_step_index] * ratio * (*category.m_) * selectivity_value; // if no F this Z = M
      category.survivor_with_z_by_age_[i] = 0.0;
      category.selectivity_values_[i] = selectivity_value;
      category.exp_values_half_duration_m_[i]  = exp(-0.5 * annual_duration_by_timestep_[time_step_index] * ratio * (*category.m_) * selectivity_value);  // this exp call should ony
      //LOG_FINEST() << "category " << category.category_label_ << " age index " << i << " selectivity " << selectivity_value;
    }
  }

  /**
   * This is where F gets applied, only enter if
   * - not in initialisation phase or no F in this time-step
   *  or if no F in this year or we are in projection years, as we probably want to project.
   * Loop for each category. Add the vulnerability from each
   * category in to the fisheries it belongs too
   */
  if (model_->state() != State::kInitialise) {
    // Not in initialisation phase
    LOG_FINE() << "Should we calcualte F based on years on and time-step";
    if(((find(process_years_.begin(), process_years_.end(), year) != process_years_.end()) || (year > model_->final_year()))) {
      // There is F in this year are we in the right time-step
      LOG_FINE() << "check time-step";
      if(find(time_steps_to_skip_applying_F_mortality_.begin(), time_steps_to_skip_applying_F_mortality_.end(), time_step_index) == time_steps_to_skip_applying_F_mortality_.end()) {
        // Good to calculate F
        LOG_FINEST() << "time step = " << time_step_index << " not in initialisation and there is an F method in this timestep. year = " << model_->current_year();
        for (auto& fishery_category : fishery_categories_) {
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;

          for (unsigned i = 0; i < fishery_category.selectivity_values_.size(); ++i) {
            fishery_category.selectivity_values_[i]
                = fishery_category.selectivity_->GetAgeResult(fishery_category.category_.category_->min_age_ + i, fishery_category.category_.category_->age_length_);
          }
        }
        // clear some variables
        for (auto& fishery : fisheries_) {
          fishery.second.vulnerability_ = 0.0;
        }
          
        Double total_vulnerable = 0.0;
        for (auto& fishery_category : fishery_categories_) {
          LOG_FINEST() << "checking fishery = " << fishery_category.fishery_label_;

          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;

          partition::Category* category = fishery_category.category_.category_;

          Double vulnerable = 0.0;
          if (fishery_category.category_.age_weight_) {
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              vulnerable += category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model_->min_age())
                            * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_duration_m_[i];
            }
          } else if (is_catch_biomass_) {  // as biomass
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              vulnerable += category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                            * fishery_category.category_.exp_values_half_duration_m_[i];
                            Double sel_bio = category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i];
                            Double Nmid = category->data_[i] * fishery_category.category_.exp_values_half_duration_m_[i];
              LOG_FINEST() << "i = " << i << " Nmid = " << Nmid << " sel bio = " << sel_bio;
            }
          } else {  // as abundance
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.exp_values_half_duration_m_[i];
            }
          }
          fishery_category.fishery_.vulnerability_ += vulnerable;
          total_vulnerable += vulnerable;
          LOG_FINEST() << "Category is fished in year " << year << " time_step " << time_step_index << ", numbers at age length = " << category->data_.size();
          LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << vulnerable;
        }
        LOG_FINE() << " total vulnerable = " << total_vulnerable;
        // Start by doing a Pope's approximation
        for (auto& fishery_iter : fisheries_) {
          auto& fishery = fishery_iter.second;
          fishery.init_popes_rate_ = fishery.catches_[year]  / (fishery.vulnerability_ + 0.1 * fishery.catches_[year]); //  Pope's rate  robust A.1.22 of SS appendix
          fishery.steep_jointer_ = 1.0 / (1.0 + exp(30.0 * (fishery.init_popes_rate_ - 0.95))); // steep logistic joiner at harvest rate of 0.95 //steep logistic joiner at harvest rate of 0.95
          fishery.popes_rate_ = fishery.steep_jointer_ * fishery.init_popes_rate_ + (1.0 - fishery.steep_jointer_) * 0.95;
          fishery.init_F_ = -log(1.0 - fishery.popes_rate_) / annual_duration_by_timestep_[time_step_index];
          LOG_FINEST() << "init_popes_rate_ = " << fishery.init_popes_rate_ << " vbio = " << fishery.vulnerability_ << " popes rate " <<  fishery.popes_rate_  << " init F " << fishery.init_F_;
          // reset vulnerable 
          fishery.vulnerability_ = 0.0;
        }
        //  Do a specified number of loops to tune up these F values to more closely match the observed catch
        Double interim_tot_catch; // interim catch over all fisheries in this time-step
        for(unsigned f = 0; f < F_iterations_; ++f) {
          LOG_FINE() << "F-iter = " << f + 1;
          interim_tot_catch = 0.0;
          // set start z value to M for each category and iteration
          for (auto& category : categories_) {
            for (unsigned i = 0; i < category.category_->age_spread(); ++i)
              category.z_values_by_age_[i] = (*category.m_) * ratio * category.selectivity_values_[i];
          }
          // Use this F to calculate a Z for categories
          for (auto& fishery_category : fishery_categories_) {
            partition::Category* category = fishery_category.category_.category_;
            // Don't enter if this fishery is not executed in this time-step.
            if (fishery_category.fishery_.time_step_index_ != time_step_index)
              continue;
            // M + sum(F_f_a * S_f_a)
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.category_.z_values_by_age_[i] += fishery_category.fishery_.init_F_ * fishery_category.selectivity_values_[i];
            }
          }
          // The survivorship is calculated as:
          for (auto& category : categories_) {
            for (unsigned i = 0; i < category.category_->age_spread(); ++i){
              category.survivor_with_z_by_age_[i] = (1.0 - exp(-annual_duration_by_timestep_[time_step_index]* category.z_values_by_age_[i])) / category.z_values_by_age_[i];
              //LOG_FINEST() << " age = " << model_->min_age() << " Z " << category.z_values_by_age_[i] << " survivorship = " << category.survivor_with_z_by_age_[i];
            }
          }
          // Calculate the expected total catch that would occur with the current Hrates and Z
          for (auto& fishery_category : fishery_categories_) {
            partition::Category* category = fishery_category.category_.category_;
            // Don't enter if this fishery is not executed in this time-step.
            if (fishery_category.fishery_.time_step_index_ != time_step_index)
              continue;
            // M + sum(F_f_a * S_f_a)
            if (fishery_category.category_.age_weight_) {
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                interim_tot_catch += fishery_category.fishery_.init_F_ * category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model_->min_age())
                              * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else if (is_catch_biomass_) {  // as biomass
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                interim_tot_catch += fishery_category.fishery_.init_F_ * category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else {  // as abundance
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                interim_tot_catch += fishery_category.fishery_.init_F_ * category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            }
          }
          LOG_FINEST() << "interim tot catch = " << interim_tot_catch << " observed total catch = " << total_catch_by_year_timestep_[this_year_iter.second][time_step_index];
          // make Z adjustments
          Double z_adjustment = total_catch_by_year_timestep_[this_year_iter.second][time_step_index] / (interim_tot_catch + 0.0001);
          LOG_FINEST() << "Z adjustment = " << z_adjustment;
          // Update rates Z's with adjustment
          for (auto& category : categories_) {
            for (unsigned i = 0; i < category.category_->age_spread(); ++i) {
             // Double old_F = category.z_values_by_age_[i] - ((*category.m_) * ratio * category.selectivity_values_[i]);
              //Double new_F = z_adjustment * (category.z_values_by_age_[i] - ((*category.m_) * ratio * category.selectivity_values_[i]));
              //LOG_FINEST() << "age = " << model_->min_age() + i << " old F " << old_F << " new F " << new_F;
              category.z_values_by_age_[i] = (*category.m_) * ratio * category.selectivity_values_[i] + z_adjustment * (category.z_values_by_age_[i] - ((*category.m_) * ratio * category.selectivity_values_[i]));
              category.survivor_with_z_by_age_[i] = (1.0 - exp(-annual_duration_by_timestep_[time_step_index] * category.z_values_by_age_[i])) / category.z_values_by_age_[i];
              //LOG_FINEST() << " age = " << model_->min_age() << " Z " << category.z_values_by_age_[i] << " survivorship " << category.survivor_with_z_by_age_[i];
            }
          }
          // Now re-calculate a new pope rate using a vulnerable biomass based
          // on the newly adjusted F
          for (auto& fishery_category : fishery_categories_) {
            if (fishery_category.fishery_.time_step_index_ != time_step_index)
              continue;
            partition::Category* category = fishery_category.category_.category_;
            Double vulnerable = 0.0;
            if (fishery_category.category_.age_weight_) {
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                vulnerable += category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model_->min_age())
                              * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else if (is_catch_biomass_) {  // as biomass
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                vulnerable += category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i]
                              * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            } else {  // as abundance
              for (unsigned i = 0; i < category->data_.size(); ++i) {
                vulnerable += category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
              }
            }
            fishery_category.fishery_.vulnerability_ += vulnerable;
            //LOG_FINEST() << "Vulnerable biomass from category " << category->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << vulnerable;
          }
          //Update Pope's approximation
          for (auto& fishery_iter : fisheries_) {
            auto& fishery = fishery_iter.second;
            fishery.popes_rate_ = fishery.catches_[year]  / (fishery.vulnerability_ + 0.0001); //  Pope's rate  robust A.1.27 of SS appendix
            fishery.steep_jointer_ = 1.0 / (1.0 + exp(30.0 * (fishery.popes_rate_ - 0.95 * max_F_))); // steep logistic joiner at harvest rate of 0.95 including F max
            fishery.init_F_ = fishery.steep_jointer_ * fishery.popes_rate_ + (1.0 - fishery.steep_jointer_) * max_F_;
            fishery.final_F_ = fishery.init_F_;
            LOG_FINEST() << "fishery = " << fishery.label_ << " observed catch = " << fishery.catches_[year] << " vulnerable " << fishery.vulnerability_ << " popes_rate_ = " << fishery.popes_rate_ << " join = " << fishery.steep_jointer_ << " new F " << fishery.init_F_;
            // reset vulnerable 
            fishery.vulnerability_ = 0.0;
          }
        } // F-tune
        // Update last Zrates and calculate catch at age and actual Catch
        for (auto& category : categories_) {
          for (unsigned i = 0; i < category.category_->age_spread(); ++i)
            category.z_values_by_age_[i] = (*category.m_) * ratio * category.selectivity_values_[i];
        }
        // Use this F to calculate a Z for categories
        for (auto& fishery_category : fishery_categories_) {
          partition::Category* category = fishery_category.category_.category_;
          // Don't enter if this fishery is not executed in this time-step.
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;
          // M + sum(F_f_a * S_f_a)
          for (unsigned i = 0; i < category->data_.size(); ++i)
            fishery_category.category_.z_values_by_age_[i] += fishery_category.fishery_.final_F_ * fishery_category.selectivity_values_[i];
        }
        // The survivorship is calculated as:
        for (auto& category : categories_) {
          for (unsigned i = 0; i < category.category_->age_spread(); ++i)
            category.survivor_with_z_by_age_[i] = (1.0 - exp(-annual_duration_by_timestep_[time_step_index] * category.z_values_by_age_[i])) / category.z_values_by_age_[i];
        }
        // Calculate Numbers at age and actual catch
        for (auto& fishery_category : fishery_categories_) {
          partition::Category* category = fishery_category.category_.category_;
          // Don't enter if this fishery is not executed in this time-step.
          if (fishery_category.fishery_.time_step_index_ != time_step_index)
            continue;
          // M + sum(F_f_a * S_f_a)
          if (fishery_category.category_.age_weight_) {
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.fishery_.actual_catches_[year] += fishery_category.fishery_.final_F_ * category->data_[i] * fishery_category.category_.age_weight_->mean_weight_at_age_by_year(year, i + model_->min_age())
                            * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
            }
          } else if (is_catch_biomass_) {  // as biomass
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.fishery_.actual_catches_[year] += fishery_category.fishery_.final_F_ * category->data_[i] * category->age_length_->mean_weight(time_step_index, category->min_age_ + i) * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
            }
          } else {  // as abundance
            for (unsigned i = 0; i < category->data_.size(); ++i) {
              fishery_category.fishery_.actual_catches_[year] += fishery_category.fishery_.final_F_ * category->data_[i] * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
            }
          }
          // Catch at age
          LOG_FINEST() << "Catch at age for year ndx = " << this_year_iter.second << " fishery ndx = " << fishery_category.fishery_.fishery_ndx_ <<  " category ndx " << fishery_category.category_.category_ndx_;
          for (unsigned i = 0; i < model_->age_spread(); ++i) {
            unsigned age_offset = category->min_age_ - model_->min_age();
            if (i < age_offset)
              continue;
            removals_by_year_fishery_category_[this_year_iter.second][fishery_category.fishery_.fishery_ndx_][fishery_category.category_.category_ndx_][i]
                = fishery_category.fishery_.final_F_ * category->data_[i - age_offset] * fishery_category.selectivity_values_[i] * fishery_category.category_.survivor_with_z_by_age_[i];
          }
        }
        // Update Final F by year for each fishery for reporting
        for (auto& fishery_iter : fisheries_) {
          fishery_iter.second.F_by_year_[year] = fishery_iter.second.final_F_;
          LOG_FINE() << "save final F = " << fishery_iter.second.final_F_ << " fishery = " << fishery_iter.second.label_ << " reported value = " <<fishery_iter.second.F_by_year_[year];
          LOG_FINE() << "actual catches = " << fishery_iter.second.actual_catches_[year];
          // Flag a penalty if it exists
          if (fishery_iter.second.penalty_)
            fishery_iter.second.penalty_->Trigger(fishery_iter.second.catches_[year], fishery_iter.second.actual_catches_[year]);
        }
      }
    }
  }//!=Kinitialise
  // now kill the partition if No F then Z = M else Z = F + M
  for (auto& category : categories_) {
    for (unsigned i = 0; i < category.category_->data_.size(); ++i) {
      category.category_->data_[i] *= exp(-category.z_values_by_age_[i]);
      LOG_FINEST() << "category " << category.category_label_ << ": updated numbers at age = " << category.category_->data_[i] << " age " << i + model_->min_age() << " Z = " << category.z_values_by_age_[i];
    }
  }
} //DoExecute

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void MortalityHybrid::FillReportCache(ostringstream& cache) {
  LOG_FINE();
  // This one is niggly because we need to iterate over each year and time step to print the right information so we don't
  // these years are for M and F
  cache << "year: ";
  for (auto year : process_years_) 
    cache << year << " ";
  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    cache << "\ncatch[" << fishery.label_ << "]: ";
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    cache << "\nactual_catch[" << fishery.label_ << "]: ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
    cache << "\nf_by_year[" << fishery.label_ << "]: ";
    for (auto f_year : fishery.F_by_year_) cache << AS_DOUBLE(f_year.second) << " ";
  }
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 */
void MortalityHybrid::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    // print header
    for (auto& fishery_iter : fisheries_) {
      auto& fishery = fishery_iter.second;
      for (auto catches : fishery.catches_) cache << "catch[" << fishery.label_ << "][" << catches.first << "] ";
      for (auto actual_catches : fishery.actual_catches_) cache << "actual_catches[" << fishery.label_ << "][" << actual_catches.first << "] ";
    }
    cache << REPORT_EOL;
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    for (auto catches : fishery.catches_) cache << AS_DOUBLE(catches.second) << " ";
    for (auto actual_catches : fishery.actual_catches_) cache << AS_DOUBLE(actual_catches.second) << " ";
  }
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
