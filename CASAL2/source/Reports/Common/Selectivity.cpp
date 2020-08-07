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

/**
 * Default constructor
 */
Selectivity::Selectivity(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation| RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);

  parameters_.Bind<string>(PARAM_SELECTIVITY, &selectivity_label_, "The selectivity name", "");
}

/**
 * Validate
 */
void Selectivity::DoValidate() {
}

/**
 * Build
 */
void Selectivity::DoBuild() {
  selectivity_ = model_->managers().selectivity()->GetSelectivity(selectivity_label_);
  if (!selectivity_)
    LOG_FATAL_P(PARAM_SELECTIVITY) << " " << selectivity_label_ << " was not found.";
  if (selectivity_->IsSelectivityLengthBased()) {
    LOG_WARNING() << "Cannot report the length-based selectivity values. This report (" << label_ << ") is being ignored.\n"
      << "This can be done using the Casal2 R package. See the User Manual for more info. ";
  }
}

/**
 * Execute the report
 */
void Selectivity::DoExecute() {
  LOG_TRACE();
  if (!selectivity_->IsSelectivityLengthBased()) {
    LOG_FINEST() << "Printing age-based selectivity";
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
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
      cache_ << i << " " << AS_VALUE(selectivity_->GetAgeResult(i, nullptr)) << "\n";
    ready_for_writing_ = true;
  }
}

/**
 * Execute the tabular report
 */
void Selectivity::DoExecuteTabular() {
  if (!selectivity_->IsSelectivityLengthBased()) {
    if (first_run_) {
      first_run_ = false;
      cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
      cache_ << "values " << REPORT_R_DATAFRAME << "\n";
      string age, selectivity_by_age_label;

      for (unsigned i = model_->min_age(); i <= model_->max_age(); ++i) {
        if (!utilities::To<unsigned, string>(i, age))
          LOG_CODE_ERROR() << "Could not convert the value " << i << " to a string for storage in the tabular report";
        selectivity_by_age_label = "selectivity[" + selectivity_->label() + "]." + age;
        cache_ << selectivity_by_age_label << " ";
      }
      cache_ << "\n";
    }
    for (unsigned i = model_->min_age(); i <= model_->max_age(); ++i) {
      cache_ << AS_VALUE(selectivity_->GetAgeResult(i, nullptr)) << " ";
    }
    cache_ << "\n";
  }
}

/**
 * Finalise the tabular report
 */
void Selectivity::DoFinaliseTabular() {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
