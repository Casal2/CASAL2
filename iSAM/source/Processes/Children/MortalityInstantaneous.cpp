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

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories", "");
  parameters_.BindTable(PARAM_CATCHES, catches_table_, "Table of catch data", "", true, false);
  parameters_.BindTable(PARAM_FISHERIES, fisheries_table_, "Table of fishery data", "", true, false);
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "U Max", "", 0.99)->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_M, &m_, "Mortality rates", "")->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_TIME_STEP_RATIO, &time_step_ratios_temp_, "Time step ratios for M", "", true)->set_range(0.0, 1.0);

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
   * Pull the years from our catches table so we can populate our data
   */
  vector<string> columns = catches_table_->columns();
  unsigned year = 0;
  for (string column : columns) {
    if (column == PARAM_FISHERY || column == PARAM_TIME_STEP)
      continue;
    if (utilities::To<string, unsigned>(column, year))
      years_.push_back(year);
    else
      LOG_ERROR_P(PARAM_CATCHES) << "column header " << column << " is not a valid numeric that could be converted to a year in the model";
  }
  vector<unsigned> model_years = model_->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_CATCHES) << "year " << year << " was defined in the table, but is not a valid year in the model";
  }

  if (!catches_table_->Populate3D<string, string, unsigned, Double>(PARAM_FISHERY, PARAM_TIME_STEP, years_, catch_table_data_))
    return;

  vector<string> table_columns { PARAM_CATEGORY, PARAM_SELECTIVITY };
  if (!fisheries_table_->Populate2D<string, string, string>(PARAM_FISHERY, table_columns, fishery_table_data_))
    return;

  if (m_.size() == 1)
    m_.assign(category_labels_.size(), m_[0]);
  if (m_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_M)
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << m_.size();
  }

  // Check categories are real
  for (const string& label : category_labels_) {
    if (!Categories::Instance()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of M so here we want to verify
   * we have enough and re-scale them to 1.0
   */
  vector<TimeStepPtr> time_steps = model_->managers().time_step().ordered_time_steps();
  LOG_FINEST() << "time_steps.size(): " << time_steps.size();
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
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void MortalityInstantaneous::DoBuild() {
  partition_.Init(category_labels_);

  /**
   * Put the fishery data with timesteps etc in to the map:
   * From: catch_table_data_; // fishery - timestep - year - catch
   * Into: fishery_by_time_step_by_year_with_catch_
   */
  for (auto fishery_iter : catch_table_data_) {
    for (auto time_step_iter : fishery_iter.second) {
      unsigned time_step_index = model_->managers().time_step().GetTimeStepIndex(time_step_iter.first);
      fishery_by_time_step_by_year_with_catch_[fishery_iter.first][time_step_index] = time_step_iter.second;
    }
  }

  /**
   * Put the selectivities in to the map
   */
  for (auto fishery_iter : fishery_table_data_) {
    SelectivityPtr selectivity = model_->managers().selectivity().GetSelectivity(fishery_iter.second[PARAM_SELECTIVITY]);
    if (!selectivity)
      LOG_ERROR() << "selectivity " << fishery_iter.second[PARAM_SELECTIVITY] << " could not be found ";

      fishery_by_category_with_selectivity_[fishery_iter.first][fishery_iter.second[PARAM_CATEGORY]] = selectivity;
  }

  if (penalty_name_ != "") {
    penalty_ = penalties::Manager::Instance().GetProcessPenalty(penalty_name_);
    if (!penalty_) {
      LOG_ERROR_P(PARAM_PENALTY) << ": penalty " << penalty_name_ << " does not exist. Have you defined it?";
    }
  }
}

/**
 * Execute this process
 */
void MortalityInstantaneous::DoExecute() {
  unsigned time_step = model_->managers().time_step().current_time_step();
  Double ratio = time_step_ratios_[time_step];

  /**
   * Loop for each category. Add the vulnerability from each
   * category in to the fisheries it belongs too
   */
  map<string, map<unsigned, Double>> category_by_age_with_exploitation;
  unsigned m_offset = 0;

  if (model_->state() != State::kInitialise && std::find(years_.begin(), years_.end(), model_->current_year()) != years_.end()) {
    map<string, Double> fishery_vulnerability;
    m_offset = 0;
    for (auto categories : partition_) {
      categories->UpdateMeanWeightData();

      for (auto fishery_iter : fishery_by_category_with_selectivity_) {
        if (fishery_iter.second.find(categories->name_) != fishery_iter.second.end()) {
          for (unsigned i = 0; i < categories->data_.size(); ++i) {
            Double vulnerable = categories->data_[i] * categories->mean_weight_per_[categories->min_age_ + i]
                * fishery_by_category_with_selectivity_[fishery_iter.first][categories->name_]->GetResult(categories->min_age_ + i)
                * exp(-0.5 * ratio * m_[m_offset]);

            fishery_vulnerability[fishery_iter.first] += vulnerable;
          }
        }
      }
      ++m_offset;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable)
     */
    fishery_exploitation.clear();
    for (auto fishery_iter : fishery_by_time_step_by_year_with_catch_) {
      if (fishery_iter.second.find(time_step) != fishery_iter.second.end()) {
        Double exploitation = fishery_iter.second[time_step][model_->current_year()] / utilities::doublecompare::ZeroFun(fishery_vulnerability[fishery_iter.first]);
        fishery_exploitation[fishery_iter.first] = exploitation;
      }
    }

    for (auto categories : partition_)  {
      for (auto fishery_iter : fishery_by_category_with_selectivity_) {
        if (fishery_iter.second.find(categories->name_) == fishery_iter.second.end())
          continue;

        for (unsigned i = 0; i < categories->data_.size(); ++i) {
          category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] += fishery_exploitation[fishery_iter.first] *
            fishery_by_category_with_selectivity_[fishery_iter.first][categories->name_]->GetResult(categories->min_age_ + i);
        }
      }
    }

    /**
     * Rescaling exploitation and applying penalties
     */
    bool recalculate_age_exploitation = false;
    for (auto fishery_iter : fishery_by_category_with_selectivity_) {
      Double uobs_f = 0.0;
      for (auto category_iter : fishery_iter.second) {
        for (auto age_exploitation : category_by_age_with_exploitation[category_iter.first]) {
          uobs_f = uobs_f > age_exploitation.second ? uobs_f : age_exploitation.second;

        }
      }

      LOG_FINE() << fishery_iter.first << " Highest exploitation rate = " << uobs_f;

      if (uobs_f > u_max_) {
        fishery_exploitation[fishery_iter.first] *= u_max_ / uobs_f;
        LOG_FINE() << fishery_iter.first  << " Rescaled exploitation rate = " << fishery_exploitation[fishery_iter.first];
        recalculate_age_exploitation = true;

        if (penalty_)
          penalty_->Trigger(label_, fishery_by_time_step_by_year_with_catch_[fishery_iter.first][time_step][model_->current_year()], fishery_vulnerability[fishery_iter.first] * u_max_);
      }
    }

    /**
     * recalculate age exploitation if we triggered penalty
     */
    if (recalculate_age_exploitation) {
      category_by_age_with_exploitation.clear();

      for (auto categories : partition_)  {
        for (auto fishery_iter : fishery_by_category_with_selectivity_) {
          if (fishery_iter.second.find(categories->name_) == fishery_iter.second.end())
            continue;
          for (unsigned i = 0; i < categories->data_.size(); ++i) {
            category_by_age_with_exploitation[categories->name_][categories->min_age_ + i] += fishery_exploitation[fishery_iter.first] *
              fishery_by_category_with_selectivity_[fishery_iter.first][categories->name_]->GetResult(categories->min_age_ + i);
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
      categories->data_[i] *= exp(-m_[m_offset] * ratio) * (1 - category_by_age_with_exploitation[categories->name_][categories->min_age_ + i]);
      if (categories->data_[i] < 0.0)
        LOG_FATAL() << " Fishing caused a negative partition : if (categories->data_[i] < 0.0)";
    }

    ++m_offset;
  }
}

/**
 *
 */
Double MortalityInstantaneous::m(const string& label) {
  unsigned i = 0;
  for (auto categories : partition_) {
    if (categories->name_ == label)
      return m_[i];
    ++i;
  }

  return 0.0;
}

Double MortalityInstantaneous::time_step_ratio() {
    unsigned time_step = model_->managers().time_step().current_time_step();
    return time_step_ratios_[time_step];
  }

Double MortalityInstantaneous::fishery_exploitation_fraction(const string& fishery_label, const string& category_label, unsigned age) {
  Double running_total = 0;
    for (auto fishery_iter : fishery_by_category_with_selectivity_) {
      if (fishery_iter.second.find(category_label) != fishery_iter.second.end())
        running_total += fishery_exploitation[fishery_iter.first] * fishery_by_category_with_selectivity_[fishery_iter.first][category_label]->GetResult(age);
  }
    return (fishery_exploitation[fishery_label] * fishery_by_category_with_selectivity_[fishery_label][category_label]->GetResult(age) / running_total);
}

} /* namespace processes */
} /* namespace niwa */
