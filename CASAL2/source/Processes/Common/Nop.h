/**
 * @file NullProcess.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 4/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This process does nothing. It's mostly used for debugging time steps
 */
#ifndef SOURCE_PROCESSES_CHILDREN_NOP_H_
#define SOURCE_PROCESSES_CHILDREN_NOP_H_

// headers
#include "../../Processes/Process.h"

// namespaces
namespace niwa {
namespace processes {

/**
 * Class definition
 */
class NullProcess : public Process {
public:
  // methods
  explicit NullProcess(shared_ptr<Model> model);
  virtual ~NullProcess() = default;
  void DoValidate();
  void DoBuild() override final{};
  void DoReset() override final{};
  void DoExecute() override final{};
};

} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_NOP_H_ */
