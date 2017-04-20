/**
 * @file Selectivity.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */
#include "Selectivity.h"

#include "Model/Model.h"
#include "Selectivities/Manager.h"

namespace niwa {
namespace reports {

Selectivity::Selectivity(Model* model) : Report(model) {
  run_mode_     = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_  = State::kFinalise;

  parameters_.Bind<string>(PARAM_SELECTIVITY, &selectivity_label_, "Selectivity name", "");
}

void Selectivity::DoValidate() {
}

void Selectivity::DoBuild() {
  selectivity_ = model_->managers().selectivity()->GetSelectivity(selectivity_label_);
  if (!selectivity_)
    LOG_ERROR_P(PARAM_SELECTIVITY) << " " << selectivity_label_ << " does not exist. Have you defined it?";
  if (selectivity_->IsSelectivityLengthBased()) {
    LOG_WARNING() << "Cannot report length based selectivity values, This can be done in the R package see info on the R package in the user manual. This report (" << label_ << ") is being ignored";
  }
}

void Selectivity::DoExecute() {
  if (selectivity_->IsSelectivityLengthBased()) {
    cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
    const map<string, Parameter*> parameters = selectivity_->parameters().parameters();

    for (auto iter : parameters) {
      Parameter* x = iter.second;
      cache_  << iter.first << ": ";

      vector<string> values = x->current_values();
      for (string value : values)
        cache_ << value << " ";
      cache_ << "\n";
    }

    cache_ << "Values " << REPORT_R_VECTOR << "\n";
    for (unsigned i = model_->min_age(); i <= model_->max_age(); ++i)
      cache_ << i << " " << AS_DOUBLE(selectivity_->GetResult(i, nullptr)) << "\n";
    ready_for_writing_ = true;
  }
}


} /* namespace reports */
} /* namespace niwa */
