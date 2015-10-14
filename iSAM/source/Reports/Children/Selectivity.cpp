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
}

void Selectivity::DoExecute() {
  Model* model = Model::Instance();

  /*
  cache_ << "[" << label_ << "]\n";
  cache_ << "report.type: " << PARAM_SELECTIVITY << "\n";
  cache_ << "selectivity.label: " << selectivity_->label() << "\n";

  const map<string, ParameterPtr> parameters = selectivity_->parameters().parameters();

  for (auto iter : parameters) {
    ParameterPtr x = iter.second;
    cache_ << "selectivity." << iter.first << " ";

    vector<string> values = x->current_values();
    for (string value : values)
      cache_ << value << " ";
    cache_ << "\n";
  }

  cache_ << "Age Value\n";
  for (unsigned i = model->min_age(); i <= model->max_age(); ++i)
    cache_ << i << " " << selectivity_->GetResult(i) << "\n";
 */
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  // cache_ << ".label: " << selectivity_->label() << "\n";

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
  for (unsigned i = model->min_age(); i <= model->max_age(); ++i)
    cache_ << i << " " << selectivity_->GetResult(i) << "\n";
  ready_for_writing_ = true;
}


} /* namespace reports */
} /* namespace niwa */
