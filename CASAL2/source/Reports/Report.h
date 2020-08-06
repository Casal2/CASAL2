/**
 * @file Report.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is the base report class for all reports that are created within CASA2.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef REPORT_H_
#define REPORT_H_

#ifndef BOOST_USE_WINDOWS_H
#define BOOST_USE_WINDOWS_H
#endif

// Headers
#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <thread>
#include <mutex>

#include "BaseClasses/Object.h"
#include "Model/Model.h"

// Namespaces
namespace niwa {
class Model;
using std::streambuf;
using std::ofstream;
using std::cout;
using std::endl;
using std::ios_base;
using std::iostream;
using std::ostringstream;


/**
 * Class definition
 */
class Report : public base::Object {
public:
  // Methods
  Report() = delete;
  explicit Report(Model* model);
  virtual                     ~Report() = default;
  void                        Validate();
  void                        Build();
  void                        Prepare();
  void                        Reset() {};
  void                        Execute();
  void                        Finalise();
  void                        PrepareTabular();
  void                        ExecuteTabular();
  void                        FinaliseTabular();
  bool                        HasYear(unsigned year);
  void                        FlushCache();

  // Accessors
  RunMode::Type               run_mode() const { return run_mode_; }
  State::Type                 model_state() const { return model_state_; }
  const string&               time_step() const { return time_step_; }
  bool                        ready_for_writing() const { return ready_for_writing_; }
  void                        set_skip_tags(bool value) { skip_tags_ = value; }
  void                        set_write_mode(string value) { write_mode_ = value; }

protected:
  // methods
  void                        SetUpInternalStates();
  // pure methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoPrepare() { };
  virtual void                DoExecute() = 0;
  virtual void                DoFinalise() { };
  virtual void                DoPrepareTabular() { };
  virtual void                DoExecuteTabular() = 0;
  virtual void                DoFinaliseTabular() { };

  // Members
  Model*                      model_;
  RunMode::Type               run_mode_    = RunMode::kInvalid;
  State::Type                 model_state_ = State::kInitialise;
  static std::mutex           lock_;
  string                      time_step_   = "";
  string                      file_name_   = "";
  bool                        first_write_ = true;
  bool                        overwrite_   = true;
  string                      last_suffix_ = "";
  string                      write_mode_ = PARAM_OVERWRITE;
  vector<unsigned>            years_;
  ostringstream               cache_;
  bool                        ready_for_writing_ = false;
  bool                        skip_tags_ = false;
};

// Typedef
typedef std::shared_ptr<niwa::Report> ReportPtr;

} /* namespace niwa */
#endif /* REPORT_H_ */
