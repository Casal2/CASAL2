/**
 * @file Initialisation.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */
#include "Initialisation.h"

#include "../../InitialisationPhases/Age/Iterative.h"
#include "../../InitialisationPhases/InitialisationPhase.h"
#include "../../InitialisationPhases/Length/Iterative.h"
#include "../../InitialisationPhases/Manager.h"
#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"

namespace niwa {
namespace reports {

/**
 *
 */
Initialisation::Initialisation() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kInitialise;
  skip_tags_   = true;
}

/**
 * Execute the report
 */
void Initialisation::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();

  niwa::partition::accessors::All all_view(model);

  // Print the header
  // this report is slightly unique. Instead of the label it will use the name of the initialisation phase
  // The reason this was done was to deal with multi initialisation phases.
  label_          = model->get_current_initialisation_phase_label();
  initialisation_ = model->managers()->initialisation_phase()->GetInitPhase(label_);

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << PARAM_TYPE << ": " << initialisation_->type() << REPORT_EOL;

  if (initialisation_->type() == PARAM_ITERATIVE) {
    years_             = initialisation_->GetTestConvergenceYears();
    lambda_            = initialisation_->GetTestConvergenceLambda();
    Double true_lambda = initialisation_->GetConvergenceLambda();
    cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
    cache_ << PARAM_YEAR << " " << PARAM_LAMBDA << " " << PARAM_VALUE << " converged" << REPORT_EOL;
    for (unsigned i = 0; i < years_.size(); ++i) {
      if (true_lambda <= lambda_[i])
        cache_ << years_[i] << " " << true_lambda << " " << lambda_[i] << " FALSE" << REPORT_EOL;
      else
        cache_ << years_[i] << " " << true_lambda << " " << lambda_[i] << " TRUE" << REPORT_EOL;
    }
  }
  cache_ << REPORT_END << REPORT_EOL;
  ready_for_writing_ = true;
}

void Initialisation::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_INITIALISATION << " has not been implemented";
}

void Initialisation::DoExecuteTabular(shared_ptr<Model> model) {}

void Initialisation::DoFinaliseTabular(shared_ptr<Model> model) {}

} /* namespace reports */
} /* namespace niwa */
