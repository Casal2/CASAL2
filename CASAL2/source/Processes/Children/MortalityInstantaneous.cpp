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

#include "Categories/Categories.h"
#include "Model/Managers.h"
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/TimeStep.h"
#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace processes {

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
  partition_structure_ = PartitionStructure::kAge;

  catches_table_ = new parameters::Table(PARAM_CATCHES);
  fisheries_table_ = new parameters::Table(PARAM_FISHERIES);
  // catches_table_->set_required_columns({"years"}, allow_others = true)
  // fisheries_table_->set_required_columns({"x", "x", "x,"});

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories for natural mortality", "");
  parameters_.BindTable(PARAM_CATCHES, catches_table_, "Table of catch data", "", true, false);
  parameters_.BindTable(PARAM_FISHERIES, fisheries_table_, "Table of fishery data", "", true, false);
  parameters_.Bind<Double>(PARAM_M, &m_, "Mortality rates", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_AVERAGE, &avg_, "The average mortality rate", "", Double(-999.0));
  parameters_.Bind<Double>(PARAM_DIFFERENCE, &diff_, "The difference between, two categories natural mortality around the average", "", Double(-999.0));
  parameters_.Bind<Double>(PARAM_TIME_STEP_RATIO, &time_step_ratios_temp_, "Time step ratios for M", "", true)->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_labels_, "Selectivities for Natural Mortality", "");
  parameters_.Bind<string>(PARAM_PENALTY, &  penalty_label_, "Label of penalty to be applied", "","");

  RegisterAsEstimable(PARAM_M, &m_);
  RegisterAsEstimable(PARAM_AVERAGE, &avg_);
  RegisterAsEstimable(PARAM_DIFFERENCE, &diff_);
}

/**
 * Destructor
 */
MortalityInstantaneous::~MortalityInstantaneous() {
  delete catches_table_;
  delete fisheries_table_;
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityInstantaneous::DoValidate() {
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  /**
   * Load a temporary map of the fishery catch data so we can use this
   * when we load our vector of FisheryData objects
   */
  map<string, map<unsigned, Double>> fishery_year_catch;
  auto columns = catches_table_->columns();
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
      if (i == year_index) continue;

      Double value = 0.0;
      if (!utilities::To<string, Double>(row[i], value))
        LOG_ERROR_P(PARAM_CATCHES) << "value " << row[i] << " for fishery " << columns[i] << " is not numeric";
      fishery_year_catch[columns[i]][year] = value;
    }
  }

  /**
   * Now we want to load the information from the fisheries table.
   * We create a master fishery object and then multiple category objects
   * depending on how many categories are defined.
   */
  columns = fisheries_table_->columns();
  rows = fisheries_table_->data();

  unsigned fishery_index      = std::find(columns.begin(), columns.end(), PARAM_FISHERY) - columns.begin();
  unsigned category_index     = std::find(columns.begin(), columns.end(), PARAM_CATEGORY) - columns.begin();
  unsigned selectivity_index  = std::find(columns.begin(), columns.end(), PARAM_SELECTIVITY) - columns.begin();
  unsigned time_step_index    = std::find(columns.begin(), columns.end(), PARAM_TIME_STEP) - columns.begin();
  unsigned u_max_index        = std::find(columns.begin(), columns.end(), PARAM_U_MAX) - columns.begin();
  unsigned penalty_index      = std::find(columns.begin(), columns.end(), PARAM_PENALTY) - columns.begin();
  LOG_FINEST() << "indexes: fishery=" << fishery_index << "; category=" << category_index << "; selectivity="
      << selectivity_index << "; time_step=" << time_step_index << "; u_max=" << u_max_index
      << "; penalty" << penalty_index;
  for (auto row : rows) {
    FisheryData new_fishery;
    new_fishery.label_              = row[fishery_index];
    new_fishery.time_step_label_    = row[time_step_index];
    new_fishery.penalty_label_      = row[penalty_index];
    if (!utilities::To<string, Double>(row[u_max_index], new_fishery.u_max_))
      LOG_ERROR_P(PARAM_FISHERIES) << "u_max value " << row[u_max_index] << " is not numeric";
    if (fishery_year_catch.find(new_fishery.label_) == fishery_year_catch.end())
      LOG_ERROR_P(PARAM_FISHERIES) << "fishery " << new_fishery.label_ << " does not have catch information in the catches table";
    new_fishery.catches_ = fishery_year_catch[new_fishery.label_];

    fisheries_[new_fishery.label_] = new_fishery;

    vector<string> categories;
    vector<string> selectivities;
    boost::split(categories, row[category_index], boost::is_any_of(","));
    boost::split(selectivities, row[selectivity_index], boost::is_any_of(","));
    if (categories.size() != selectivities.size())
      LOG_ERROR_P(PARAM_FISHERIES) << "The number of categories (" << categories.size()
      << ") and selectivities (" << selectivities.size() << ") provided must be identical";

    for (unsigned i = 0; i < categories.size(); ++i) {
      FisheryCategoryData new_category_data(fisheries_[new_fishery.label_]);
      new_category_data.fishery_label_     = row[fishery_index];
      new_category_data.category_label_    = categories[i];
      new_category_data.selectivity_label_ = selectivities[i];
      fishery_categories_.push_back(new_category_data);
    }
  }

  /**
   * Validate the non-table parameters now. These are mostly related to the natural mortality
   * aspect of the process.
   */
  if (m_.size() == 1)
    m_.assign(category_labels_.size(), m_[0]);
  if (selectivity_labels_.size() == 1)
    selectivity_labels_.assign(category_labels_.size(), selectivity_labels_[0]);
  if (m_.size() != category_labels_.size() && avg_ != -999.0)
    LOG_ERROR_P(PARAM_M)
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << m_.size();
  if (selectivity_labels_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << selectivity_labels_.size();
  }

  // validate if the average difference method is used for natural mortality
  if (avg_ != -999.0 && diff_ != -999.0) {
    if (category_labels_.size() % 2)
      LOG_ERROR_P(PARAM_AVERAGE) << "This method of natural mortality only works when number of categories is a multiple of 2";
    m_.assign(category_labels_.size(), 0.0);
    for (unsigned i = 0; i <= (category_labels_.size() / 2); i+=2) {
    m_[i] = avg_ + (diff_ * 0.5);
    if (m_[i] > 1.0 || m_[i] < 0.0)
      LOG_ERROR_P(PARAM_AVERAGE) << "the resulting parameters" << PARAM_AVERAGE << " " << PARAM_DIFFERENCE
      << " caused natural mortality to be less than 0.0 or greater than 1.0";
    m_[i + 1] = avg_ - (diff_ * 0.5);
    if (m_[i + 1] > 1.0 || m_[i + 1] < 0.0)
      LOG_ERROR_P(PARAM_AVERAGE) << "the resulting parameters" << PARAM_AVERAGE << " " << PARAM_DIFFERENCE
      << " caused natural mortality to be less than 0.0 or greater than 1.0";
    }
  }
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 *
 * Validate any parameters that require information from other objects
 * in the system
 */
void MortalityInstantaneous::DoBuild() {
  partition_.Init(category_labels_);

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
      if (value <= 0.0 || value > 1.0)
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
    if (!fishery_category.selectivity_)
      LOG_ERROR_P(PARAM_FISHERIES) << "selectivity " << fishery_category.selectivity_label_ << " does not exist. Have you defined it?";
  }

  for (auto& fishery_iter : fisheries_) {
    auto& fishery = fishery_iter.second;
    if (fishery.penalty_label_ != "none") {
      fishery.penalty_ = model_->managers().penalty()->GetProcessPenalty(fishery.penalty_label_);
      if (!fishery.penalty_)
        LOG_ERROR_P(PARAM_FISHERIES) << ": penalty " << fishery.penalty_label_ << " does not exist. Have you defined it?";
    }
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
  for (auto label : selectivity_labels_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << "selectivity " << label << " does not exist. Have you defined it?";
    selectivities_.push_back(selectivity);
  }
}

/**
 * Execute this process
 */
void MortalityInstantaneous::DoExecute() {
  unsigned time_step_index = model_->managers().time_step()->current_time_step();
//  unsigned year =  model_->current_year();

  Double ratio = time_step_ratios_[time_step_index];

  /**
   * Loop for each category. Add the vulnerability from each
   * category in to the fisheries it belongs too
   */
  map<string, map<unsigned, Double>> category_by_age_with_exploitation;
  unsigned m_offset = 0;

  if (model_->state() != State::kInitialise) {
    map<string, Double> fishery_vulnerability;
    m_offset = 0;
    for (auto categories : partition_) {
      categories->UpdateMeanWeightData();

      for (auto fishery_category : fishery_categories_) {
        if (fishery_category.category_label_ != categories->name_ || fisheries_[fishery_category.fishery_label_].time_step_index_ != time_step_index)
          continue;

        for (unsigned i = 0; i < categories->data_.size(); ++i) {
          Double vulnerable = categories->data_[i] * categories->mean_weight_per_[categories->min_age_ + i]
              * fishery_category.selectivity_->GetResult(categories->min_age_ + i, categories->age_length_)
              * exp(-0.5 * ratio * m_[m_offset] * selectivities_[m_offset]->GetResult(categories->min_age_ + i, categories->age_length_));
          fishery_vulnerability[fishery_category.fishery_label_] += vulnerable;
        }
        LOG_FINEST() << ": Vulnerable biomass from category " << categories->name_ << " contributing to fishery " << fishery_category.fishery_label_ << " = " << fishery_vulnerability[fishery_category.fishery_label_];
      }
      ++m_offset;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable) for each fishery
     */
    fishery_exploitation.clear();

    for (auto fishery_iter : fisheries_) {
      auto fishery = fishery_iter.second;
      Double exploitation = 0.0;
      // If fishery occurs in this time step calculate exploitation rate
      if (fishery.time_step_index_ == time_step_index) {
        exploitation = fishery.catches_[model_->current_year()] / utilities::doublecompare::ZeroFun(fishery_vulnerability[fishery.label_]);
        LOG_FINEST() << " Vulnerable biomass for fishery : " << fishery.label_ << " = " << fishery_vulnerability[fishery.label_] << " with Catch = " << fishery.catches_[model_->current_year()];
      }

      fishery_exploitation[fishery.label_] = exploitation;
    }

    for (auto categories : partition_) {
      for (auto fishery_category : fishery_categories_) {
        if (fishery_category.category_label_ != categories->name_ || fisheries_[fishery_category.fishery_label_].time_step_index_ != time_step_index)
          continue;

        for (unsigned i = 0; i < categories->data_.size(); ++i)
          category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] += fishery_exploitation[fishery_category.fishery_label_]
              * fishery_category.selectivity_->GetResult(categories->min_age_ + i, categories->age_length_);
      }
    }


    /**
     * Rescaling exploitation and applying penalties
     */
    bool recalculate_age_exploitation = false;
    map<string, Double> uobs_fishery;
    for (auto fishery_category : fishery_categories_) {
      for (auto age_exploitation : category_by_age_with_exploitation[fishery_category.category_label_]) {
        if (fisheries_[fishery_category.fishery_label_].time_step_index_ != time_step_index)
          continue;

        uobs_fishery[fishery_category.fishery_label_] = uobs_fishery[fishery_category.fishery_label_] > age_exploitation.second
          ? uobs_fishery[fishery_category.fishery_label_] : age_exploitation.second;
      }
    }

    for (auto uobs : uobs_fishery) {
      LOG_FINE() << uobs.first << " Highest exploitation rate = " << uobs.second;
      Double u_max_ = fisheries_[uobs.first].u_max_;
      if (uobs.second > u_max_) {
        fishery_exploitation[uobs.first] *= (u_max_ / uobs.second);
        LOG_FINE() << " Uobs.first = " << uobs.first << " Uobs second = " << uobs.second << " and u_max " << u_max_;
        LOG_FINE() << uobs.first << " Rescaled exploitation rate = " << fishery_exploitation[uobs.first];
        recalculate_age_exploitation = true;

        if (fisheries_[uobs.first].penalty_)
          fisheries_[uobs.first].penalty_->Trigger(label_, fisheries_[uobs.first].catches_[model_->current_year()],
              fishery_vulnerability[fisheries_[uobs.first].label_] * u_max_);
      }
    }

    /**
     * recalculate age exploitation if we triggered penalty
     */
    if (recalculate_age_exploitation) {
      category_by_age_with_exploitation.clear();
      for (auto categories : partition_)  {
        for (auto fishery_category : fishery_categories_) {
          if (fishery_category.category_label_ != categories->name_)
            continue;

          for (unsigned i = 0; i < categories->data_.size(); ++i) {
            category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] += fishery_exploitation[fishery_category.fishery_label_]
                * fishery_category.selectivity_->GetResult(categories->min_age_ + i, categories->age_length_);
          }
        }
      }
    }
    /**
     * Calculate the expectation for a proportions_at_age observation
     */
    /*
    unsigned age_spread = model_->max_age() - model_->min_age();
    m_offset = 0;
    for (auto categories : partition_)  {
      for (auto fishery_category : fishery_categories_) {
        if (fishery_category.category_label_ != categories->name_)
          continue;
        removals_by_year_timestep_fishery_[year][time_step_index][fishery_category.fishery_label_].assign(0.0, age_spread);
        for (unsigned i = 0; i < age_spread; ++i) {
          unsigned age_offset = categories->min_age_ - model_->min_age();
          if (i < age_offset)
            continue;
          removals_by_year_timestep_fishery_[year][time_step_index][fishery_category.fishery_label_][i] += categories->data_[i - age_offset] * fishery_exploitation[fishery_category.fishery_label_]
              * fishery_category.selectivity_->GetResult(categories->min_age_ + i, categories->age_length_) * exp(-0.5 * ratio * m_[m_offset]
                 * selectivities_[m_offset]->GetResult(categories->min_age_ + i, categories->age_length_));
        }
      }
      m_offset++;
    }
    */
  } // if (model_->state() != State::kInitialise && std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {

  /**
   * Remove the stock now
   */
  m_offset = 0;
  for (auto categories : partition_) {
    for (unsigned i = 0; i < categories->data_.size(); ++i) {
      categories->data_[i] *= exp(-m_[m_offset] * ratio * selectivities_[m_offset]->GetResult(categories->min_age_ + i, categories->age_length_))
          * (1 - category_by_age_with_exploitation[categories->name_][categories->min_age_ + i]);
      if (categories->data_[i] < 0.0) {
        LOG_FINEST() << " Category : " << categories->name_ << " M = "<< m_[m_offset] << " ratio " << ratio << " selectivity : " << selectivities_[m_offset]->GetResult(categories->min_age_ + i, categories->age_length_)
            << " u_f = " << category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] << " data = " << categories->data_[i] << " Exp " << AS_DOUBLE(exp(-m_[m_offset]));
        LOG_FATAL() << " Fishing caused a negative partition : if (categories->data_[i] < 0.0)";
      }
    }

    ++m_offset;
  }
}

/**
 * This method will return the mortality rate (m) multiplied by the selectivity
 * for a specific age.
 *
 * @param category The category label we want M and selectivity for
 * @param age The age we want to get from the selectivity
 * @return M * selectivity
 */
Double MortalityInstantaneous::GetMBySelectivity(const string& category_label, unsigned age) {
  unsigned index = std::find(category_labels_.begin(), category_labels_.end(), category_label) - category_labels_.begin();
  if (index == 0 && category_labels_[0] != category_label)
    LOG_ERROR() << "Category: " << category_label << " is not a valid category for the mortality instantaneous process " << label_;
  AgeLength* age_length = model_->categories()->age_length(category_label);
  //LOG_MEDIUM() << ": selectivity value = " << selectivities_[index]->GetResult(age, age_length);
  return m_[index] * selectivities_[index]->GetResult(age, age_length);
}

/**
 * Return the time step ratio for the current time step
 *
 * @return Current time step ratio
 */
Double MortalityInstantaneous::time_step_ratio() {
  unsigned time_step = model_->managers().time_step()->current_time_step();
  return time_step_ratios_[time_step];
}

/**
 * GetFisheryExploitationFraction() this function returns an exploitation rate for a fishery, age and category, from all other fisheries
 *    this is used in Observations for_fishery
 * @param fishery_label the name of the fishery in the observation
 * @praam category_label the name of the category of interest
 * @param age
 *
 * @return Exploitation_fraction
 */
Double MortalityInstantaneous::GetFisheryExploitationFraction(const string& fishery_label, const string& category_label, unsigned age) {
  unsigned time_step = model_->managers().time_step()->current_time_step();
  Double selectivity_value = 0.0;

  Double running_total = 0;
  Double fishery_exploitation_rate = 0.0;
  for (auto fishery_category : fishery_categories_) {
    if (fishery_category.category_label_ == category_label && fishery_category.fishery_.time_step_index_ == time_step) {
      if (fishery_category.fishery_label_ == fishery_label) {
        AgeLength* age_length = model_->categories()->age_length(category_label);
        selectivity_value = fishery_category.selectivity_->GetResult(age, age_length);
        fishery_exploitation_rate = fishery_exploitation[fishery_category.fishery_label_] * selectivity_value;
      }
      AgeLength* age_length = model_->categories()->age_length(fishery_category.category_label_);
      running_total += fishery_exploitation[fishery_category.fishery_label_] * fishery_category.selectivity_->GetResult(age, age_length);
    }
  }


  return fishery_exploitation_rate / running_total;
}

/**
 * This method checks to see if the parameter fishery_label is a valid
 * fishery that has been loaded from the table
 *
 * @param fishery_label The fishery to look for
 * @return true if fishery exists, false otherwise
 */
bool MortalityInstantaneous::IsFisheryValid(const string& fishery_label) {
  return fisheries_.find(fishery_label) != fisheries_.end();
}

} /* namespace processes */
} /* namespace niwa */
