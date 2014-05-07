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
 * This is the base report class for all reports that are created within iSAM.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef REPORT_H_
#define REPORT_H_

// Headers
#include <string>
#include <iostream>
#include <fstream>
#include <ostream>

#include "BaseClasses/Object.h"
#include "Model/Model.h"

// Namespaces
namespace isam {

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
  Report();
  virtual                     ~Report() = default;
  void                        Validate();
  void                        Build() { DoBuild(); };
  virtual void                Prepare() {};
  virtual void                Reset() {};
  virtual void                Finalise() {};
  bool                        HasYear(unsigned year);
  void                        FlushCache();

  // pure methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                Execute() = 0;

  // Accessors
  RunMode::Type               run_mode() const { return run_mode_; }
  State::Type                 model_state() const { return model_state_; }
  const string&               time_step() const { return time_step_; }
  bool                        ready_for_writing() const { return ready_for_writing_; }

protected:
  // Members
  RunMode::Type               run_mode_    = RunMode::kInvalid;
  State::Type                 model_state_ = State::kInitialise;
  string                      time_step_   = "";
  string                      file_name_   = "";
  bool                        overwrite_   = true;
  string                      last_suffix_ = "";
  vector<unsigned>            years_;
  ostringstream               cache_;
  bool                        ready_for_writing_ = false;
};

// Typedef
typedef boost::shared_ptr<isam::Report> ReportPtr;

} /* namespace isam */
#endif /* REPORT_H_ */
