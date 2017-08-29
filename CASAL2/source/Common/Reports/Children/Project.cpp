/**
 * @file Project.cpp
 * @author C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// headers
#include "Project.h"

#include <boost/algorithm/string/join.hpp>

#include "Common/Model/Managers.h"
#include "Common/Model/Model.h"
#include "Common/Projects/Manager.h"
#include "Common/Projects/Project.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * @param model Pointer to the current model context
 */
Project::Project(Model* model) : Report(model) {
  model_state_ = State::kIterationComplete;
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kSimulation | RunMode::kProjection);

  parameters_.Bind<string>(PARAM_PROJECT, &project_label_, "Project label that is reported", "", "");
}

/**
 * Build our relationships between this object and other objects
 */
void Project::DoBuild() {
  project_ = model_->managers().project()->GetProject(project_label_);
  if (!project_) {
    LOG_ERROR_P(PARAM_PROJECT) << "project " << project_label_ << " could not be found. Have you defined it?";
  }

}

/**
 * Execute this report
 */
void Project::DoExecute() {
  LOG_FINE() <<" printing report " << label_ << " of type " << project_->type();
  map<unsigned,Double>& values = project_->projected_parameters();
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "project: " << project_label_ << "\n";
  cache_ << "values " << REPORT_R_VECTOR <<"\n";
  for(auto value : values) {
    cache_ << value.first << " " << value.second << "\n";
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
