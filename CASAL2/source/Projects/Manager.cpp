/**
 * @file Manager.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Manager.h"

#include "Model/Model.h"
#include "Model/Managers.h"
#include "Processes/Manager.h"
#include "Model/Objects.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
Manager::Manager() {
}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {
}

/**
 * Build the objects - no model
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 * Build the objects
 */
void Manager::Build(Model* model) {
  LOG_TRACE();
  if (model->run_mode() == RunMode::kProjection) {
    bool ycs_values_exist = false;
    for (auto project : objects_) {
      LOG_FINE() << "Building Project: " << project->label();
      project->Build();
      if (project->parameter().find(PARAM_YCS_VALUES) != string::npos)
        ycs_values_exist = true;
    }
    if (!ycs_values_exist) {
      for (auto process : model->managers().process()->objects()) {
        if (process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT)
          LOG_ERROR() << process->location() << " process " << process->label()
            << " does not contain a @project for ycs_values, but this model is running in projection mode";
      }
    }
  }
}

/**
 * Update the objects for a specified year
 *
 * @param year The year
 */
void Manager::Update(unsigned year) {
  LOG_TRACE();
  for (auto project : objects_)
    project->Update(year);
}

/**
 * This function will store all parameter values to overwrite at in projection years
 * TODO: This is not how projections work. They will only modify values in the year they're called.
 *
 * @param year The year
 */
void Manager::StoreValues(unsigned year) {
  LOG_TRACE();
  // iterate over all @project blocks
  for (auto project : objects_) {
    project->StoreValue(year);
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

} /* namespace projects */
} /* namespace niwa */
