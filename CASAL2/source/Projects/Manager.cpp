/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "Manager.h"

#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Model/Objects.h"
#include "../Processes/Manager.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
Manager::Manager() {}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {}

/**
 * Build the objects - no model
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 * Build the objects
 */
void Manager::Build(shared_ptr<Model> model) {
  LOG_TRACE();
  if (model->run_mode() == RunMode::kProjection) {
    bool recruitment_multipliers_exist = false;
    for (auto project : objects_) {
      LOG_FINE() << "Building Project: " << project->label();
      project->Build();
      if (project->parameter().find(PARAM_RECRUITMENT_MULTIPLIERS) != string::npos)
        recruitment_multipliers_exist = true;
    }
    if (!recruitment_multipliers_exist) {
      for (auto process : model->managers()->process()->objects()) {
        if (process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT)
          LOG_ERROR() << process->location() << " does not have a @project command for recruitment_multipliers";
      }
    }
  }
}

/**
 * Update the objects for a specified year
 *
 * @param year The year
 */
void Manager::Update(unsigned current_year) {
  LOG_TRACE();
  for (auto project : objects_) project->Update(current_year);
}

/**
 * This function will store all parameter values to overwrite at in projection years
 * TODO: This is not how projections work. They will only modify values in the year they're called.
 *
 * @param year The year
 */
void Manager::StoreValues(unsigned current_year) {
  LOG_TRACE();
  // iterate over all @project blocks
  for (auto project : objects_) {
    project->StoreValue(current_year);
  }
}

/**
 * Return the project with the name passed in as a parameter.
 * If no process is found then an empty pointer will
 * be returned.
 *
 * @param label The name of the project to find
 * @return A pointer to the project or empty pointer
 */
Project* Manager::GetProject(const string& label) {
  for (auto project : objects_) {
    if (project->label() == label)
      return project;
  }

  return nullptr;
}


/*
* This function is called by model::RunProjections at the end of each run
* Sometimes values are not set at the end of each 
*/
void Manager::SetObjectsForNextIteration() {
  // iterate over all @project blocks
  for (auto project : objects_) {
    project->SetObjectForNextIteration();
  }
}
} /* namespace projects */
} /* namespace niwa */
