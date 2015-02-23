/**
 * @file Process.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
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
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Object.h"
#include "BaseClasses/Executor.h"
#include "Model/Model.h"

namespace niwa {

enum class ProcessType {
  kUnknown,
  kAgeing,
  kMaturation,
  kMortality,
  kRecruitment,
  kTransition,
  kTaggingWithMortality
};

/**
 * Class Definition
 */
class Process : public niwa::base::Object {
public:
  // methods
  Process();
  virtual                     ~Process() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };
  void                        Execute(unsigned year, const string& time_step_label);
  void                        Print();
  void                        Subscribe(unsigned year, const string& time_step_label, ExecutorPtr executor);

  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  virtual void                DoExecute() = 0;

  // accessors
  bool                        print_report() const { return print_report_; }
  PartitionStructure          partition_structure() const { return partition_structure_; }
  ProcessType                 process_type() const { return process_type_; }

protected:
  // members
  string                      type_ = "";
  ProcessType                 process_type_ = ProcessType::kUnknown;
  PartitionStructure          partition_structure_ = PartitionStructure::kInvalid;
  bool                        print_report_ = false;
  map<string, vector<string>> print_values_;
  map<unsigned, map<string, vector<ExecutorPtr>>> executors_;

};

/**
 * Typedef
 */
typedef boost::shared_ptr<Process> ProcessPtr;

} /* namespace niwa */
#endif /* PROCESS_H_ */
