/**
 * @file LoadPartition.h
 * @author your name (you@domain.com)
 * @brief This process will load the partition with the information from a table. It's primary use is to set the
 * partition to a known state during a unit test.
 * @version 0.1
 * @date 2021-06-26
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SOURCE_PROCESSES_CHILDREN_LOADPARTITION_H_
#define SOURCE_PROCESSES_CHILDREN_LOADPARTITION_H_

// headers
#include "../../Processes/Process.h"

// namespaces
namespace niwa::processes {

// class definition
class LoadPartition : public Process {
public:
  // methods
  explicit LoadPartition(shared_ptr<Model> model);
  virtual ~LoadPartition();
  void DoValidate() final;
  void DoBuild() final;
  void DoReset() final{};
  void DoExecute() final{};

private:
  parameters::Table* data_table_ = nullptr;
};

}  // namespace niwa::processes

#endif /* SOURCE_PROCESSES_CHILDREN_LOADPARTITION_H_ */
