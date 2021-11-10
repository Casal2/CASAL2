/**
 * @file Process.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the parent process for all process objects in the model.
 * Anything that acts on the partition during a time step is considered
 * a process (e.g mortality, recruitment, ageing etc)
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PROCESS_H_
#define PROCESS_H_

// Headers
#include "../BaseClasses/Executor.h"
#include "../BaseClasses/Object.h"
#include "../Model/Model.h"

namespace niwa {

enum class ProcessType {
  kUnknown,
  kAgeing,
  kMaturation,
  kMortality,
  kRecruitment,
  kTransition,
  kNullProcess,  // Special process type for the process child nop
};

/**
 * Class Definition
 */
class Process : public niwa::base::Object {
public:
  // methods
  Process() = delete;
  explicit Process(shared_ptr<Model> model);
  virtual ~Process() = default;
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model){};
  void Reset();
  void Execute(unsigned year, const string& time_step_label);
  void Subscribe(unsigned year, const string& time_step_label, Executor* executor);

  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;
  virtual void DoReset()    = 0;
  virtual void DoExecute()  = 0;
  virtual void FillReportCache(ostringstream& cache){};
  virtual void FillTabularReportCache(ostringstream& cache, bool first_run){};

  // accessors
  PartitionType partition_structure() const { return partition_structure_; }
  ProcessType   process_type() const { return process_type_; }

protected:
  // members
  shared_ptr<Model>                             model_               = nullptr;
  ProcessType                                   process_type_        = ProcessType::kUnknown;
  PartitionType                                 partition_structure_ = PartitionType::kInvalid;
  map<unsigned, map<string, vector<Executor*>>> executors_;
};
} /* namespace niwa */

#endif /* PROCESS_H_ */
