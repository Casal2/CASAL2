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

#include "Common/Model/Model.h"
#include "Common/Model/Managers.h"
#include "Common/Processes/Manager.h"
#include "Common/Model/Objects.h"

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
 * Build
 */
void Manager::Build() {
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 * Build
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
          LOG_ERROR() << process->location() << " process " << process->label() << " does not contain a @project for ycs_values, but you are running in projection mode";
      }
    }
  }
}

/**
 *
 */
void Manager::Update(unsigned current_year) {
  LOG_TRACE();
  for (auto project : objects_)
    project->Update(current_year);
}

/**
 *  @param current_year The year this is called.
 *
 *  This function will store all parameter values that we may want to overwrite at in projection years
 *  TODO: This is not how projections work. They will only modify values in the year they're called.
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

} /* namespace projects */
} /* namespace niwa */
