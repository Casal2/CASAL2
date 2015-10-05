/**
 * @file TimeStep.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TIMESTEP_H_
#define TIMESTEP_H_

// Headers
#include "BaseClasses/Object.h"
#include "BaseClasses/Executor.h"
#include "Processes/Process.h"
#include "Utilities/Map.h"

// Namespaces
namespace niwa {

using std::pair;
using base::Executor;

/**
 * Class Definition
 */
class TimeStep : public niwa::base::Object {
public:
  // Methods
  TimeStep();
  virtual                     ~TimeStep() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() {};
  void                        ExecuteForInitialisation(const string& phase_label);
  void                        Execute(unsigned year);
  bool                        HasProcess(const string& label) { return std::find(process_names_.begin(), process_names_.end(), label) != process_names_.end(); }
  void                        Subscribe(Executor* executor, unsigned year) { executors_[year].push_back(executor); }
  void                        SubscribeToInitialisationBlock(Executor* executor) { initialisation_block_executors_.push_back(executor); }
  void                        SubscribeToBlock(Executor* executor);
  void                        SubscribeToBlock(Executor* executor, unsigned year) { block_executors_[year].push_back(executor); }
  Process*                    SubscribeToProcess(Executor* executor, unsigned year, string process_label);
  Process*                    SubscribeToProcess(Executor* executor, const vector<unsigned>& years, string process_label);
  void                        SetInitialisationProcessLabels(const string& initialisation_phase_label, vector<string> process_labels_);
  void                        BuildInitialisationProcesses();

  // accessors
  const vector<Process*>&     processes() const { return processes_; }
  vector<string>              process_labels() const { return process_names_; }
  vector<string>              initialisation_process_labels(const string& initialisation_phase) { return initialisation_process_labels_[initialisation_phase]; }

private:
  // Members
  vector<string>                      process_names_;
  vector<Process*>                    processes_;
  map<unsigned, vector<Executor*> >   executors_;
  map<unsigned, vector<Executor*> >   block_executors_;
  vector<Executor*>                   initialisation_block_executors_;
  pair<unsigned, unsigned>            mortality_block_ = pair<unsigned,unsigned>(0u, 0u);
  map<string, vector<string>>         initialisation_process_labels_;
  map<string, vector<Process*>>       initialisation_processes_;
  map<string, pair<unsigned, unsigned>> initialisation_mortality_blocks_;
  map<unsigned, map<unsigned, vector<Executor*>>> process_executors_; // year/process index
};
} /* namespace niwa */
#endif /* TIMESTEP_H_ */
