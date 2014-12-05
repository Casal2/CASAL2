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
DerivedQuantity::DerivedQuantity() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kFinalise;
}

/**
 *
 */
void DerivedQuantity::DoExecute() {
  LOG_TRACE();

  cache_ << "derived_quantity: " << label_ << "\n";

  derivedquantities::Manager& manager = derivedquantities::Manager::Instance();

  vector<DerivedQuantityPtr> derived_quantities = manager.GetObjects();

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

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
