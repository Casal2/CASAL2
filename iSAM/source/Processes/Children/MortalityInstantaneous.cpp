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
#include "Penalties/Manager.h"
#include "Selectivities/Manager.h"
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
MortalityInstantaneous::MortalityInstantaneous() {
//  : time_steps_manager_(timesteps::Manager::Instance()) {

  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;

  catches_table_    = TablePtr(new parameters::Table(PARAM_CATCHES));
  fisheries_table_  = TablePtr(new parameters::Table(PARAM_FISHERIES));

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Categories", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to run this process in", "");
  parameters_.BindTable(PARAM_CATCHES, catches_table_, "Table of catch data", "", true, false);
  parameters_.BindTable(PARAM_FISHERIES, fisheries_table_, "Table of fishery data", "", true, false);
  parameters_.Bind<Double>(PARAM_U_MAX, &u_max_, "U Max", "", 0.99)->set_range(0.0, 1.0);
  parameters_.Bind<Double>(PARAM_M, &m_, "Mortality rates", "")->set_range(0.0, 1.0);
//  parameters_.Bind<Double>(PARAM_TIME_STEP_RATIO, &ratios_, "Time step ratios for M", "", true)->set_range(0.0, 1.0);

  RegisterAsEstimable(PARAM_U_MAX, &u_max_);
  RegisterAsEstimable(PARAM_M, &m_);

  model_ = Model::Instance();
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when we cannot use bind<>() overloads
 *
 * Note: all parameters are populated from configuration files
 */
void MortalityInstantaneous::DoValidate() {
  if (!catches_table_->Populate<string, unsigned, Double>(PARAM_FISHERY, years_, fishery_by_year_with_catch_))
    return;

  vector<string> table_columns { PARAM_CATEGORY, PARAM_SELECTIVITY };
  if (!fisheries_table_->Populate<string, string, string>(PARAM_FISHERY, table_columns, fishery_table_data_))
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
//  vector<TimeStepPtr> time_steps = time_steps_manager_.time_steps();
//  LOG_FINEST() << "time_steps.size(): " << time_steps.size();
//  vector<unsigned> active_time_steps;
//  for (unsigned i = 0; i < time_steps.size(); ++i) {
//    if (time_steps[i]->HasProcess(label_))
//      active_time_steps.push_back(i);
//  }
//
//  if (ratios_.size() == 0) {
//    for (unsigned i : active_time_steps)
//      time_step_ratios_[i] = 1.0;
//  } else {
//    if (ratios_.size() != active_time_steps.size())
//      LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << " length (" << ratios_.size()
//          << ") does not match the number of time steps this process has been assigned to (" << active_time_steps.size() << ")";
//
//    for (Double value : ratios_) {
//      if (value <= 0.0 || value > 1.0)
//        LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << " value (" << value << ") must be between 0.0 (exclusive) and 1.0 (inclusive)";
//    }
//
//    for (unsigned i = 0; i < ratios_.size(); ++i)
//      time_step_ratios_[active_time_steps[i]] = ratios_[i];
//  }
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void MortalityInstantaneous::DoBuild() {
  partition_.Init(category_labels_);

  selectivities::Manager& selectivity_manager = selectivities::Manager::Instance();
  for (auto fishery_iter : fishery_table_data_) {
    for (auto category_iter : fishery_iter.second) {
      SelectivityPtr selectivity = selectivity_manager.GetSelectivity(category_iter.second);
      if (!selectivity)
        LOG_ERROR() << "selectivity " << category_iter.second << " could not be found ";

      fishery_by_category_with_selectivity_[fishery_iter.first][category_iter.first] = selectivity;
    }
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
  /*
  map<string, map<unsigned, Double>>        fishery_by_year_with_catch_;
  map<string, map<string, SelectivityPtr>>  fishery_by_category_with_selectivity_;
   */

//  unsigned time_step = time_steps_manager_.current_time_step();
//  Double ratio = time_step_ratios_[time_step];

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
                * exp(-0.5 * /* ratio */ m_[m_offset]);

            fishery_vulnerability[fishery_iter.first] += vulnerable;
          }
        }
      }
      ++m_offset;
    }

    /**
     * Work out the exploitation rate to remove (catch/vulnerable)
     */
    map<string, Double> fishery_exploitation;

    for (auto fishery_iter : fishery_by_year_with_catch_) {
      Double exploitation = fishery_iter.second[model_->current_year()] / utilities::doublecompare::ZeroFun(fishery_vulnerability[fishery_iter.first]);
      fishery_exploitation[fishery_iter.first] = exploitation;
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

      if (uobs_f > u_max_) {
        fishery_exploitation[fishery_iter.first] *= u_max_ / fishery_exploitation[fishery_iter.first];
        recalculate_age_exploitation = true;

        if (penalty_)
          penalty_->Trigger(label_, fishery_by_year_with_catch_[fishery_iter.first][model_->current_year()], fishery_vulnerability[fishery_iter.first] * u_max_);
      }
    }

    /**
     * recalculate age exploitation if we triggered penalty
     */
    if (recalculate_age_exploitation) {
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
      categories->data_[i] *= exp(-m_[m_offset]) * (1 - category_by_age_with_exploitation[categories->name_][categories->min_age_ + i]);
    }

    ++m_offset;
  }
}

} /* namespace processes */
} /* namespace niwa */
