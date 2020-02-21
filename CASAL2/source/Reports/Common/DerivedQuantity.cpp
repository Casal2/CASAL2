/*
 * DerivedQuantity.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "DerivedQuantity.h"

#include "DerivedQuantities/Manager.h"

namespace niwa {
namespace reports {


/**
 *
 */
DerivedQuantity::DerivedQuantity(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);
}


/**
 *
 */
void DerivedQuantity::DoExecute() {
  LOG_TRACE();
  derivedquantities::Manager& manager = *model_->managers().derived_quantity();
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";

  auto derived_quantities = manager.objects();
  for (auto dq : derived_quantities) {
    string label =  dq->label();
    cache_ << label << " " << REPORT_R_LIST <<" \n";
    cache_ << "type: " << dq->type() << " \n";
    const vector<vector<Double>> init_values = dq->initialisation_values();
    for (unsigned i = 0; i < init_values.size(); ++i) {
      cache_ << "initialisation_phase["<< i + 1 << "]: ";
      Double value = init_values[i].back();
      cache_ << AS_VALUE(value) << " ";
      cache_ << "\n";
    }

    const map<unsigned, Double> values = dq->values();
    cache_ << "values " << REPORT_R_VECTOR <<"\n";
    for (auto iter = values.begin(); iter != values.end(); ++iter) {
      Double weight = iter->second;
      cache_ << iter->first << " " << AS_VALUE(weight) << "\n";
    }
    //cache_ <<"\n";
    cache_ << REPORT_R_LIST_END <<"\n";
  }

  ready_for_writing_ = true;
}


/**
 * Execute Tabular report
 */

void DerivedQuantity::DoExecuteTabular() {
  derivedquantities::Manager& manager = *model_->managers().derived_quantity();
  auto derived_quantities = manager.objects();

  if (first_run_) {
    first_run_ = false;
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    for (auto dq : derived_quantities) {
      const vector<vector<Double>> init_values = dq->initialisation_values();
      const map<unsigned, Double> values = dq->values();
      string derived_type = dq->type();
      for (unsigned i = 0; i < init_values.size(); ++i) {
        cache_ << derived_type << "[" << dq->label() <<"][initialisation_phase_" << i + 1 << "] ";
      }
      for (auto iter = values.begin(); iter != values.end(); ++iter)
        cache_ << derived_type << "[" << dq->label() <<"][" << iter->first << "] ";
    }
    cache_ << "\n";
  }
  for (auto dq : derived_quantities) {
    string derived_type = dq->type();
    const map<unsigned, Double> values = dq->values();
    const vector<vector<Double>> init_values = dq->initialisation_values();
    for (unsigned i = 0; i < init_values.size(); ++i) {
      Double value = init_values[i].back();
      cache_ << AS_VALUE(value) << " ";
    }
    for (auto iter = values.begin(); iter != values.end(); ++iter) {
      Double weight = iter->second;
      cache_ << AS_VALUE(weight) << " ";
    }
  }
  cache_ << "\n";
}

/**
 *
 */
void DerivedQuantity::DoFinaliseTabular() {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
