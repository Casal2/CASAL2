/**
 * @file Project.cpp
 * @author C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
      LOG_WARNING_P(PARAM_PROJECT) << ": the " << PARAM_PROJECT << " report with label '" << project_label_ << "' was requested, "
                                   << "but an @" << PARAM_PROJECT << " with that label was not found in the input configuration files. "
                                   << "Please check your input configuration files. The report will not be generated";
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

  LOG_FINE() << "Projection - printing report " << label_ << " of type " << project_->type();
  map<unsigned, Double>& values = project_->projected_parameters();
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "project: " << project_label_ << REPORT_EOL;
  cache_ << "values " << REPORT_R_VECTOR << REPORT_EOL;
  for (auto value : values) {
    cache_ << value.first << " " << AS_DOUBLE(value.second) << REPORT_EOL;
    LOG_FINE() << "Projection -  for label '" << label_ << "' for year = " << value.first;
  }

  ready_for_writing_ = true;
}

void Project::DoPrepareTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;

  // TODO: Fix this - this should access the project objects, but these do not appear to have been constructed yet.
  // TODO: Moved the code below to DoExecuteTabular() in the meantime until a better fix is implemented
  /*
    if (project_label_ != "") {
      LOG_FINE() << "Projection - specific label = " << label_;
      // single catchability provided not recommended
      string                 label  = project_->label();
      map<unsigned, Double>& values = project_->projected_parameters();
      LOG_FINE() << "Projection - for label '" << label << "' has size = " << values.size();
      for (auto value : values) {
        cache_ << "project[" << label << "]." << value.first << " ";
        LOG_FINE() << "Projection - for label '" << label << "' for year = " << value.first;
      }
    } else {
      LOG_FINE() << "Projection - Print all projections = " << label_;
      projects::Manager& ProjectManager = *model->managers()->project();
      for (auto object : ProjectManager.objects()) {
        string label = object->label();
        LOG_FINE() << "Projection - specific project = " << label;
        auto values = object->projected_parameters();
        LOG_FINE() << "Projection - for label '" << label << "' has size = " << values.size();
        for (auto value : values) {
          cache_ << "project[" << label << "]." << value.first << " ";
          LOG_FINE() << "Projection - for label '" << label << "' for year = " << value.first;
        }
      }
    }
    cache_ << REPORT_EOL;
  */
}

/**
 * Execute this report
 */
void Project::DoExecuteTabular(shared_ptr<Model> model) {
  if (!is_valid())
    return;

  // TODO: A fix for DoPrepareTabular() above not functioning as expected.
  if (first_time_) {
    if (project_label_ != "") {
      LOG_FINE() << "Projection - specific label = " << label_;
      // single catchability provided not recommended
      string                 label  = project_->label();
      map<unsigned, Double>& values = project_->projected_parameters();
      for (auto value : values) {
        cache_ << "project[" << label << "]." << value.first << " ";
      }
    } else {
      LOG_FINE() << "Projection - Print all projections = " << label_;
      projects::Manager& ProjectManager = *model->managers()->project();
      for (auto object : ProjectManager.objects()) {
        string label = object->label();
        LOG_FINE() << "Projection - specific project = " << label;
        auto values = object->projected_parameters();
        for (auto value : values) {
          cache_ << "project[" << label << "]." << value.first << " ";
        }
      }
    }
    cache_ << REPORT_EOL;
  }
  first_time_ = false;

  if (project_label_ != "") {
    // single catchability provided not recommended
    string                 label  = project_->label();
    map<unsigned, Double>& values = project_->projected_parameters();
    LOG_FINE() << "Projection (execute) - for label '" << label << "' has size = " << values.size();

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
