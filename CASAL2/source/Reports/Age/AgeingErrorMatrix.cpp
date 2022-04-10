/*
 * AgeingErrorMatrix.cpp
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#include "AgeingErrorMatrix.h"

#include "../../AgeingErrors/Manager.h"

namespace niwa {
namespace reports {
namespace age {

/**
 * Default constructor
 */
AgeingErrorMatrix::AgeingErrorMatrix() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kFinalise;

  parameters_.Bind<string>(PARAM_AGEING_ERROR, &ageing_error_label_, "The ageing error label", "");
}

/**
 * Build our report
 * @param model Any shared_ptr<Model> from the threaded list that can be queried
 */
void AgeingErrorMatrix::DoBuild(shared_ptr<Model> model) {
  auto ageingerror = model->managers()->ageing_error()->get(ageing_error_label_);
  if (!ageingerror)
    LOG_ERROR_P(PARAM_AGEING_ERROR) << "Ageing error label (" << ageing_error_label_ << ") was not found.";
}

/**
 * Execute the report
 */
void AgeingErrorMatrix::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  auto ageingerror = model->managers()->ageing_error()->get(ageing_error_label_);
  if (!ageingerror)
    LOG_CODE_ERROR() << "!ageingerror: " << ageing_error_label_;
  vector<vector<Double>>& mis_matrix = ageingerror->mis_matrix();

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "values " << REPORT_R_MATRIX << REPORT_EOL;

  for (unsigned i = 0; i < mis_matrix.size(); ++i) {
    for (unsigned j = 0; j < mis_matrix[i].size(); ++j) {
      cache_ << AS_DOUBLE(mis_matrix[i][j]) << " ";
      if (j == (mis_matrix[i].size() - 1))
        cache_ << REPORT_EOL;
    }
  }
  ready_for_writing_ = true;
}

void AgeingErrorMatrix::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_AGEING_ERROR << " has not been implemented";
}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
