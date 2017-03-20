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
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation| RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);
}


/**
 *
 */
void DerivedQuantity::DoExecute() {
  LOG_TRACE();
  derivedquantities::Manager& manager = *model_->managers().derived_quantity();
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";

  auto derived_quantities = manager.objects();
  for (auto dq : derived_quantities) {
    string label =  dq->label();
    cache_ << label << " " << REPORT_R_LIST <<" \n";
    cache_ << "derived_type: " << dq->type() << " \n";
    // report b0 and binitial
    if (model_->b0(label) > 0)
    cache_ << "B0: " << model_->b0(label) << "\n";
    if (model_->binitial(dq->label()) > 0)
    cache_ << "Binitial: " << model_->binitial(dq->label()) << "\n";

    const vector<vector<Double> > init_values = dq->initialisation_values();
    for (unsigned i = 0; i < init_values.size(); ++i) {
      cache_ << "Initial_phase_"<< i << ": ";
      for (unsigned j = 0; j < init_values[i].size(); ++j) {
      if (dq->type() == PARAM_BIOMASS) {
        Double weight = init_values[i][j];
        cache_ << weight << " ";
      } else
        cache_ << init_values[i][j] << " ";
      }
      cache_ << "\n";
    }


    const map<unsigned, Double> values = dq->values();
    cache_ << "values " << REPORT_R_VECTOR <<"\n";
    for (auto iter = values.begin(); iter != values.end(); ++iter) {
      if (dq->type() == PARAM_BIOMASS) {
        Double weight = iter->second;
        cache_ << iter->first << " " << weight << "\n";
      } else
        cache_ << iter->first << " " << iter->second << "\n";
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
    cache_ << "*derived_quant (derived_quantity)\n";
    cache_ << "values " << REPORT_R_DATAFRAME << "\n";
    for (auto dq : derived_quantities) {
      const map<unsigned, Double> values = dq->values();
      string derived_type = dq->type();
      for (auto iter = values.begin(); iter != values.end(); ++iter)
        cache_ << derived_type << "_" << iter->first << " ";
    }
    cache_ << "\n";
  }
  for (auto dq : derived_quantities) {
    string derived_type = dq->type();
    const map<unsigned, Double> values = dq->values();
    for (auto iter = values.begin(); iter != values.end(); ++iter) {
      if (derived_type == PARAM_BIOMASS) {
        Double weight = iter->second;
        cache_ << weight << " ";
      } else
        cache_ << iter->second << " ";
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
