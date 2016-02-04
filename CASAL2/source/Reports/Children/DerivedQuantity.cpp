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
  model_state_ = State::kIterationComplete;
}


/**
 *
 */
void DerivedQuantity::DoExecute() {
  LOG_TRACE();

  /*
  cache_ << "derived_quantity: " << label_ << "\n";

  derivedquantities::Manager& manager = derivedquantities::Manager::Instance();

  vector<DerivedQuantityPtr> derived_quantities = manager.objects();

  unsigned count = 1;
  for (DerivedQuantityPtr dq : derived_quantities) {
    cache_ << "derived quantity: " << dq->label() << "\n";

    const vector<vector<Double> > init_values = dq->initialisation_values();
    for (unsigned i = 0; i < init_values.size(); ++i) {
      cache_ << "Init phase: " << i << " = ";
      for (unsigned j = 0; j < init_values[i].size(); ++j)
        cache_ << AS_DOUBLE(init_values[i][j]) << " ";
      cache_ << "\n";
    }

    const map<unsigned, Double> values = dq->values();
    cache_ << "Values [Year = Value]:\n";
    for (auto iter = values.begin(); iter != values.end(); ++iter) {
      cache_ << iter->first << " = " << AS_DOUBLE(iter->second) << "\n";
    }

    count++;
  }
  */

  derivedquantities::Manager& manager = *model_->managers().derived_quantity();

  auto derived_quantities = manager.objects();


  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  unsigned count = 1;
  for (auto dq : derived_quantities) {
    cache_ << dq->label() << " " << REPORT_R_LIST << "\n";

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
