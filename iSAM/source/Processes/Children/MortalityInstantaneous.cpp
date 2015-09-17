/**
 * @file MortalityInstantaneous.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "MortalityInstantaneous.h"

#include "Categories/Categories.h"
#include "Model/Managers.h"
#include "Penalties/Manager.h"
#include "TimeSteps/TimeStep.h"
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
MortalityInstantaneous::MortalityInstantaneous() : Process(Model::Instance()) {
  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;

  catches_table_    = TablePtr(new parameters::Table(PARAM_CATCHES));
  fisheries_table_  = TablePtr(new parameters::Table(PARAM_FISHERIES));
  // catches_table_->set_required_columns({"years"}, allow_others = true)
  // fisheries_table_->set_required_columns({"x", "x", "x,"});

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories", "");
  parameters_.BindTable(PARAM_CATCHES, catches_table_, "Table of catch data", "", true, false);
  parameters_.BindTable(PARAM_FISHERIES, fisheries_table_, "Table of fishery data", "", true, false);
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "U Max", "", 0.99)->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_M, &m_, "Mortality rates", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_TIME_STEP_RATIO, &time_step_ratios_temp_, "Time step ratios for M", "", true)->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "Selectivities on Natural Mortality", "");
  parameters_.Bind<string>(PARAM_PENALTY, &  penalty_name_, "Label of penalty to be applied", "","");

  RegisterAsEstimable(PARAM_U_MAX, &u_max_);
  RegisterAsEstimable(PARAM_M, &m_);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityInstantaneous::DoValidate() {
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
   * We've already got the catch data we'll copy on to each one so it's easily available
   * for lookup. It's not the most memory efficient but it'll be a quicker lookup.
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
    FisheryData new_fishery_data;

    new_fishery_data.name_            = row[fishery_index];
    new_fishery_data.time_step_label_   = row[time_step_index];
    new_fishery_data.category_label_    = row[category_index];
    new_fishery_data.selectivity_label_ = row[selectivity_index];
    new_fishery_data.penalty_label_     = row[penalty_index];
    if (!utilities::To<string, Double>(row[u_max_index], new_fishery_data.u_max_))
      LOG_ERROR_P(PARAM_FISHERIES) << "u_max value " << row[u_max_index] << " is not numeric";
    if (fishery_year_catch.find(new_fishery_data.name_) == fishery_year_catch.end())
      LOG_ERROR_P(PARAM_FISHERIES) << "fishery " << new_fishery_data.name_ << " does not have catch information in the catches table";
    new_fishery_data.catches_ = fishery_year_catch[new_fishery_data.name_];

    fisheries_.push_back(new_fishery_data);
  }

  /**
   * Validate the non-table parameters now. These are mostly related to the natural mortality
   * aspect of the process.
   */
  if (m_.size() == 1)
    m_.assign(category_labels_.size(), m_[0]);
  if (selectivity_names_.size() == 1)
    selectivity_names_.assign(category_labels_.size(), selectivity_names_[0]);
  if (m_.size() != category_labels_.size())
    LOG_ERROR_P(PARAM_M)
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << m_.size();
    if (selectivity_names_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << selectivity_names_.size();
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
  vector<TimeStepPtr> time_steps = model_->managers().time_step().ordered_time_steps();
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
  for (auto& fishery : fisheries_) {
    fishery.selectivity_ = model_->managers().selectivity().GetSelectivity(fishery.selectivity_label_);
    if (!fishery.selectivity_)
      LOG_ERROR_P(PARAM_FISHERIES) << "selectivity " << fishery.selectivity_label_ << " does not exist. Have you defined it?";

    if (fishery.penalty_label_ != "none") {
      fishery.penalty_ = penalties::Manager::Instance().GetProcessPenalty(penalty_name_);
      if (!fishery.penalty_)
        LOG_ERROR_P(PARAM_FISHERIES) << ": penalty " << fishery.penalty_label_ << " does not exist. Have you defined it?";
    }
  }

  /**
   * Check the natural mortality categories are valid
   */
  for (const string& label : category_labels_) {
    if (!Categories::Instance()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }

  /**
   * Assign the natural mortality selectivities
   */
  for (auto label : selectivity_names_) {
    SelectivityPtr selectivity = model_->managers().selectivity().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << "selectivity " << label << " does not exist. Have you defined it?";
    selectivities_.push_back(selectivity);
  }
}

/**
 * Execute this process
 */
void MortalityInstantaneous::DoExecute() {
  unsigned time_step_index = model_->managers().time_step().current_time_step();
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

      for (auto fishery : fisheries_) {
        if (fishery.category_label_ != categories->name_)
          continue;
        for (unsigned i = 0; i < categories->data_.size(); ++i) {
          Double vulnerable = categories->data_[i] * categories->mean_weight_per_[categories->min_age_ + i]
              * fishery.selectivity_->GetResult(categories->min_age_ + i)
              * exp(-0.5 * ratio * m_[m_offset] * selectivities_[m_offset]->GetResult(categories->min_age_ + i));

          fishery_vulnerability[fishery.name_] += vulnerable;
        }
      }
      ++m_offset;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable)
     */
    fishery_exploitation.clear();
    for (auto fishery : fisheries_) {
      if (fishery.time_step_index_ != time_step_index)
        continue;

      Double exploitation = fishery.catches_[model_->current_year()] / utilities::doublecompare::ZeroFun(fishery_vulnerability[fishery.name_]);
      fishery_exploitation[fishery.name_] = exploitation;
      LOG_FINEST() << " Vulnerable biomass for fishery : " << fishery.name_ << " = " << fishery_exploitation[fishery.name_] << " with Catch = " << fishery.catches_[model_->current_year()];
    }

    for (auto categories : partition_)  {
      for (auto fishery : fisheries_) {
        if (fishery.category_label_ != categories->name_)
          continue;

        for (unsigned i = 0; i < categories->data_.size(); ++i)
          category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] += fishery_exploitation[fishery.name_] *
            fishery.selectivity_->GetResult(categories->min_age_ + i);
      }
    }


    /**
     * Rescaling exploitation and applying penalties
     */
    bool recalculate_age_exploitation = false;
    for (auto fishery : fisheries_) {
      Double uobs_f = 0.0;
      for (auto age_exploitation : category_by_age_with_exploitation[fishery.category_label_])
        uobs_f = uobs_f > age_exploitation.second ? uobs_f : age_exploitation.second;

      LOG_FINE() << fishery.name_ << " Highest exploitation rate = " << uobs_f;

      if (uobs_f > u_max_) {
        fishery_exploitation[fishery.name_] *= u_max_ / uobs_f;
        LOG_FINE() << fishery.name_ << " Rescaled exploitation rate = " << fishery_exploitation[fishery.name_];
        recalculate_age_exploitation = true;

        if (fishery.penalty_)
          fishery.penalty_->Trigger(label_, fishery.catches_[model_->current_year()], fishery_vulnerability[fishery.name_] * u_max_);
      }
    }

    /**
     * recalculate age exploitation if we triggered penalty
     */
    if (recalculate_age_exploitation) {
      category_by_age_with_exploitation.clear();

      for (auto categories : partition_)  {
        for (auto fishery : fisheries_) {
          if (fishery.category_label_ != categories->name_)
            continue;

          for (unsigned i = 0; i < categories->data_.size(); ++i) {
            category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] += fishery_exploitation[fishery.name_] *
              fishery.selectivity_->GetResult(categories->min_age_ + i);
          }
        }
      }
    }
  } // if (model_->state() != State::kInitialise && std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {

  /**
   * Remove the stock now
   */
  m_offset = 0;
  for (auto categories : partition_) {
    for (unsigned i = 0; i < categories->data_.size(); ++i) {
      categories->data_[i] *= exp(-m_[m_offset] * ratio * selectivities_[m_offset]->GetResult(categories->min_age_ + i))
          * (1 - category_by_age_with_exploitation[categories->name_][categories->min_age_ + i]);

      if (categories->data_[i] < 0.0) {
        LOG_FINEST() << " Category : " << categories->name_ << " M = "<< m_[m_offset] << " ratio " << ratio << " selectivity : " << selectivities_[m_offset]->GetResult(categories->min_age_ + i)
            << " u_f = " << category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] << " data = " << categories->data_[i] << " Exp " << AS_DOUBLE(exp(-m_[m_offset]));
        LOG_FATAL() << " Fishing caused a negative partition : if (categories->data_[i] < 0.0)";
      }
    }

    ++m_offset;
  }
}

/**
 * COMMENT THIS
 */
Double MortalityInstantaneous::m(const string& label, unsigned age) {
//  unsigned i = 0;
//  for (auto categories : partition_) {
//    if (categories->name_ == label)
//      return m_[i] * selectivities_[i]->GetResult(age);
//    ++i;
//  }

  return 0.0;
}

/**
 * COMMENT THIS
 */
Double MortalityInstantaneous::time_step_ratio() {
  unsigned time_step = model_->managers().time_step().current_time_step();
  return time_step_ratios_[time_step];
}

/**
 * COMMENT THIS
 */
Double MortalityInstantaneous::fishery_exploitation_fraction(const string& fishery_label, const string& category_label, unsigned age) {
//  Double running_total = 0;
//
//  for (auto fishery_iter : fishery_by_category_with_selectivity_) {
//    LOG_MEDIUM() << fishery_iter.second.begin()->first;
//    if (fishery_iter.second.find(category_label) != fishery_iter.second.end())
//      running_total += fishery_exploitation[fishery_iter.first] * fishery_by_category_with_selectivity_[fishery_iter.first][category_label]->GetResult(age);
//  }

  return 0.0; // (fishery_exploitation[fishery_label] * fishery_by_category_with_selectivity_[fishery_label][category_label]->GetResult(age) / running_total);
}

/**
 * COMMENT THIS
 */
bool MortalityInstantaneous::validate_fishery_category(const string& fishery_label, const string& category_label) {
//  for (auto fishery_iter : fishery_by_category_with_selectivity_) {
//    if (fishery_iter.second.find(category_label) != fishery_iter.second.end())
//      return true;
//  }
  return false;
}

} /* namespace processes */
} /* namespace niwa */
