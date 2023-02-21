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
  run_mode_    = (RunMode::Type)(RunMode::kProjection);

  parameters_.Bind<string>(PARAM_PROJECT, &project_label_, "The project label that is reported", "", "");
}
/**
 * Validate object
 */
void Project::DoValidate(shared_ptr<Model> model) {
  if (!model->global_configuration().print_tabular()) {
    if (project_label_ == "")
      project_label_ = label_;
  }
}

/**
 * Build the relationships between this object and other objects
 */
void Project::DoBuild(shared_ptr<Model> model) {
  if (project_label_ != "") {
    project_ = model->managers()->project()->GetProject(project_label_);
    if (!project_) {
#ifndef TESTMODE
      LOG_WARNING_Q(PARAM_PROJECT) << "the report for " << PARAM_PROJECT << " with label '" << project_label_ << "' was requested. This " << PARAM_PROJECT
                                   << " was not found in the input configuration file. The report will not be generated";
#endif
      is_valid_ = false;
    }
  }
}

/**
 * Execute this report
 */
void Project::DoExecute(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  LOG_FINE() << " printing report " << label_ << " of type " << project_->type();
  map<unsigned, Double>& values = project_->projected_parameters();
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "project: " << project_label_ << REPORT_EOL;
  cache_ << "values " << REPORT_R_VECTOR << REPORT_EOL;
  for (auto value : values) {
    cache_ << value.first << " " << AS_DOUBLE(value.second) << REPORT_EOL;
  }

  ready_for_writing_ = true;
}

void Project::DoPrepareTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;
  LOG_FINE() << "label = " << project_label_;
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
  if (project_label_ != "") {
    LOG_FINE() << "specific project = " << label_;

    // single catchability provided not recommended
    string                 label  = project_->label();
    map<unsigned, Double>& values = project_->projected_parameters();
    for (auto value : values) {
      cache_ << "project[" << label << "]." << value.first << " ";
    }
  } else {
    LOG_FINE() << "print all project = " << label_;
    projects::Manager& ProjectManager = *model->managers()->project();
    for (auto object : ProjectManager.objects()) {
      string label = object->label();
      LOG_FINE() << "project = " << label;

      auto values = object->years();
      for (auto value : values) {
        cache_ << "project[" << label << "]." << value << " ";
        LOG_FINE() << " year = " << value;
      }
    }
  }
  cache_ << REPORT_EOL;
}

/**
 * Execute this report
 */
void Project::DoExecuteTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;
  if (project_label_ != "") {
    // single catchability provided not recommended
    string                 label  = project_->label();
    map<unsigned, Double>& values = project_->projected_parameters();
    for (auto value : values) {
      cache_ << value.second << " ";
    }
  } else {
    projects::Manager& ProjectManager = *model->managers()->project();
    for (auto object : ProjectManager.objects()) {
      map<unsigned, Double>& values = object->projected_parameters();
      for (auto value : values) {
        cache_ << value.second << " ";
      }
    }
  }
  cache_ << REPORT_EOL;
}

void Project::DoFinaliseTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
