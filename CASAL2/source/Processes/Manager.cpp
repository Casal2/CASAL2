/*
 * Manager.cpp
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#include "Manager.h"

#include "../Logging/Logging.h"
#include "../Model/Model.h"
#include "Verification/Verification.h"

namespace niwa {
namespace processes {

/**
 * Default constructor
 */
Manager::Manager() {}

/**
 * Destructor
 */
Manager::~Manager() noexcept(true) {}

/**
 * Validate the objects - no model
 */
void Manager::Validate() {
  LOG_TRACE();
  LOG_CODE_ERROR() << "This method is not supported";
}

/**
 * Validate the objects
 */
void Manager::Validate(shared_ptr<Model> model) {
  LOG_TRACE();
  base::Manager<niwa::processes::Manager, niwa::Process>::Validate();

  if (objects_.size() == 0 && model->partition_type() == PartitionType::kAge)
    LOG_ERROR() << "Casal2 requires that at least one process has been specified in the input configuration file, e.g., @recruitment, @mortality, @ageing";

  PartitionType partition_type = model->partition_type();

  for (auto process : objects_) {
    LOG_FINEST() << "Validating process" << process->label();
    if ((PartitionType)(process->partition_structure() & PartitionType::kInvalid) == PartitionType::kInvalid)
      LOG_CODE_ERROR() << "Process: " << process->label() << " has not been properly configured to have a partition structure";

    if ((PartitionType)(process->partition_structure() & partition_type) != partition_type) {
      string     label = "unknown";
      Parameter* param = process->parameters().Get(PARAM_LABEL);
      if (param)
        label = param->values()[0];

      LOG_ERROR() << process->location() << "the process " << label << " is not allowed to be created when the model type is set to " << model->type();
    }
  }
}

/**
 * @brief
 *
 * @param model
 */
void Manager::Verify(shared_ptr<Model> model) {
  for (auto process : objects_) process->Verify(model);
  verification::DoVerification(model);
}

/**
 * Return the process with the name passed in as a parameter.
 * If no process is found then an empty pointer will
 * be returned.
 *
 * @param label The name of the process to find
 * @return A pointer to the process or empty pointer
 */
Process* Manager::GetProcess(const string& label) {
  for (auto process : objects_) {
    if (process->label() == label)
      return process;
  }

  LOG_MEDIUM() << "Process label '" << label << "' was not found.";

  return nullptr;
}

} /* namespace processes */
} /* namespace niwa */
