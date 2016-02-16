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
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = (State::Type)(State::kIterationComplete| State::kFinalise);
}


/**
 *
 */
void DerivedQuantity::DoExecute() {
  LOG_TRACE();

  derivedquantities::Manager& manager = *model_->managers().derived_quantity();

  auto derived_quantities = manager.objects();


  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  unsigned count = 1;
  for (auto dq : derived_quantities) {
    string label =  dq->label();
    cache_ << label << " " << REPORT_R_LIST << "\n";

    // report b0 and binitial
    if (model_->b0(label) > 0)
    cache_ << "B0: " << model_->b0(label) << "\n";
    if (model_->binitial(dq->label()) > 0)
    cache_ << "Binitial: " << model_->binitial(dq->label()) << "\n";

    const vector<vector<Double> > init_values = dq->initialisation_values();
    for (unsigned i = 0; i < init_values.size(); ++i) {
      cache_ << "Initial_phase_"<< i << ": ";
      for (unsigned j = 0; j < init_values[i].size(); ++j)
        cache_ << AS_DOUBLE(init_values[i][j]) << " ";
      cache_ << "\n";
    }

    const map<unsigned, Double> values = dq->values();
    cache_ << "values " << REPORT_R_VECTOR <<"\n";
    for (auto iter = values.begin(); iter != values.end(); ++iter) {
      cache_ << iter->first << " " << AS_DOUBLE(iter->second) << "\n";
    }
    cache_ << REPORT_R_LIST_END << "\n";

    count++;
  }

  ready_for_writing_ = true;
}


/**
 * Execute Tabular report
 */

void DerivedQuantity::DoExecuteTabular() {
	skip_tags_ = true;
	derivedquantities::Manager& manager = *model_->managers().derived_quantity();
	auto derived_quantities = manager.objects();

	for (auto dq : derived_quantities) {
	  const map<unsigned, Double> values = dq->values();
	    string derived_type = dq->type();
		   if (first_run_) {
			   cache_ << "*derived_quant (derived_quantity)\n";
			   cache_ << "values " << REPORT_R_DATAFRAME << "\n";
			   for (auto iter = values.begin(); iter != values.end(); ++iter)
			   cache_ << derived_type << "_" << iter->first << " ";
			   first_run_ = false;
		   } else {
		   for (auto iter = values.begin(); iter != values.end(); ++iter)
	     cache_ << AS_DOUBLE(iter->second) << " ";
	   }
    }
    ready_for_writing_ = true;
}

/**
 *
 */
void DerivedQuantity::DoFinaliseTabular() {
  cache_ << CONFIG_END_REPORT << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
