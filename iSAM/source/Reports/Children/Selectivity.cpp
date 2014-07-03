/*
 * Selectivity.cpp
 *
 *  Created on: 25/06/2014
 *      Author: Admin
 */

#include <Reports/Children/Selectivity.h>

#include "Model/Model.h"

namespace isam {
namespace reports {

Selectivity::Selectivity() {
  run_mode_     = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_  = State::kFinalise;

  parameters_.Bind<string>(PARAM_SELECTIVITY, &selectivity_label_, "Selectivity name", "");
}

void Selectivity::DoValidate() {
}

void Selectivity::DoBuild() {
  selectivity_ = selectivities::Manager::Instance().GetSelectivity(selectivity_label_);
  if (!selectivity_)
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITY) << " " << selectivity_label_ << " does not exist. Have you defined it?");
}

void Selectivity::DoExecute() {
  ModelPtr model = Model::Instance();
  cache_ << "[" << label_ << "]\n";
  cache_ << "report.type: " << PARAM_SELECTIVITY << "\n";
  cache_ << "selectivity.label: " << selectivity_->label() << "\n";

  const map<string, ParameterPtr> parameters = selectivity_->parameters().parameters();

  for (auto iter : parameters) {
    ParameterPtr x = iter.second;
    cache_ << "selectivity." << iter.first << " ";

    vector<string> values = x->values();
    for (string value : values)
      cache_ << value << " ";
    cache_ << "\n";
  }

  cache_ << "Age Value\n";
  for (unsigned i = model->min_age(); i <= model->max_age(); ++i)
    cache_ << i << " " << selectivity_->GetResult(i) << "\n";

  ready_for_writing_ = true;
}


} /* namespace reports */
} /* namespace isam */
