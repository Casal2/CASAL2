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
  kNullProcess, // Special process type for the process child nop
};

/**
 * Class Definition
 */
class Process : public niwa::base::Object {
public:
  // methods
  Process() = delete;
  explicit Process(Model* model);
  virtual                     ~Process() = default;
  void                        Validate();
  void                        Build();
  void                        Reset();
  void                        Execute(unsigned year, const string& time_step_label);
  void                        Print();
  void                        Subscribe(unsigned year, const string& time_step_label, Executor* executor);

  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  virtual void                DoExecute() = 0;

  // accessors
  void                        flag_print_report() { print_report_ = true; }
  bool                        print_report() const { return print_report_; }
  PartitionStructure          partition_structure() const { return partition_structure_; }
  ProcessType                 process_type() const { return process_type_; }
  map<string, vector<string>>& print_values() { return print_values_; }
  map<string, string>& 			 print_tabular_values() { return print_tabular_values_; }

protected:
  // methods
  template<typename T>
  void                        StoreForReport(const string& label, T value);
  template<typename T>
  void                        StoreForReport(const string& label, const vector<T>& value);
  template<typename T>
  void                        StoreForTabularReport(const string& label, T value);
  template<typename T>
  void                        StoreForTabularReport(const string& label, const vector<T>& value);
  // members
  Model*                      model_ = nullptr;
  ProcessType                 process_type_ = ProcessType::kUnknown;
  PartitionStructure          partition_structure_ = PartitionStructure::kInvalid;
  map<unsigned, map<string, vector<Executor*>>> executors_;

private:
  bool                        create_report_ = false;
  bool                        print_report_ = false;
  map<string, vector<string>> print_values_;
  map<string, string> 				print_tabular_values_;
};
} /* namespace niwa */

#include "Process-inl.h"
#endif /* PROCESS_H_ */
