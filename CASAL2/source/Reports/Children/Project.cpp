/**
 * @file Project.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Project.h"

#include <boost/algorithm/string/join.hpp>

#include "Projects/Manager.h"


// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * @param model Pointer to the current model context
 */
Project::Project(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kProjection);
  model_state_ = State::kFinalise;

  parameters_.Bind<string>(PARAM_PROJECT, &Project_label_, "Project label that is reported", "", "");
}

/**
 * An Empty Build
 */
void Project::DoBuild() {
}

/**
 * Execute this report
 */
void Project::DoExecute() {
  // Call manager
  projects::Manager& manager = *model_->managers().project();
  auto projections = manager.objects();

  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  // Iterate over all objects printing out the values of the parameters for each year
  for (auto proj : projections) {
    cache_ << "Year Value\n";
    auto projected_parameters = proj->projected_parameters();
    for (auto parameter : projected_parameters) {
      cache_ <<parameter.first << " " << parameter.second << "\n";
    }
  }
  ready_for_writing_ = true;
}


} /* namespace reports */
} /* namespace niwa */
