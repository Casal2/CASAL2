/**
 * @file Project.cpp
 * @author C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "Project.h"

#include <boost/algorithm/string/join.hpp>

#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Projects/Manager.h"
#include "../../Projects/Project.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * @param model Pointer to the current model context
 */
Project::Project() {
  model_state_ = State::kIterationComplete;
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kSimulation | RunMode::kProjection);

  parameters_.Bind<string>(PARAM_PROJECT, &project_label_, "The project label that is reported", "", "");
}

/**
 * Build the relationships between this object and other objects
 */
void Project::DoBuild(shared_ptr<Model> model) {
  project_ = model->managers()->project()->GetProject(project_label_);
  if (!project_) {
    LOG_ERROR_P(PARAM_PROJECT) << "project " << project_label_ << " was not found.";
  }
}

/**
 * Execute this report
 */
void Project::DoExecute(shared_ptr<Model> model) {
  project_ = model->managers()->project()->GetProject(project_label_);
  if (!project_) {
    LOG_CODE_ERROR() << "!project: " << project_label_;
  }

  LOG_FINE() << " printing report " << label_ << " of type " << project_->type();
  map<unsigned, Double>& values = project_->projected_parameters();
  cache_ << ReportHeader(type_, label_);
  cache_ << "project: " << project_label_ << REPORT_EOL;
  cache_ << "values " << REPORT_R_VECTOR << REPORT_EOL;
  for (auto value : values) {
    cache_ << value.first << " " << AS_DOUBLE(value.second) << REPORT_EOL;
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
