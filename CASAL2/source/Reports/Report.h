/**
 * @file Report.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
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
#include <fstream>
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>
#include <thread>

#include "../BaseClasses/Object.h"
#include "../Model/Model.h"

// Namespaces
namespace niwa {
class Model;
using std::cout;
using std::endl;
using std::ios_base;
using std::iostream;
using std::ofstream;
using std::ostringstream;
using std::streambuf;

/**
 * Class definition
 */
class Report : public base::Object {
public:
  // Methods
  Report();
  virtual ~Report() = default;
  void Validate(){};
  void Build(){};
  void Validate(shared_ptr<Model> model);
  void Build(shared_ptr<Model> model);
  void Prepare(shared_ptr<Model> model);
  void Reset(){};
  void Execute(shared_ptr<Model> model);
  void Finalise(shared_ptr<Model> model);
  void PrepareTabular(shared_ptr<Model> model);
  void ExecuteTabular(shared_ptr<Model> model);
  void FinaliseTabular(shared_ptr<Model> model);
  bool HasYear(unsigned year);
  void FlushCache();

  // Accessors
  RunMode::Type  run_mode() const { return run_mode_; }
  State::Type    model_state() const { return model_state_; }
  const string&  time_step() const { return time_step_; }
  const string&  file_name() const { return file_name_; }
  bool           ready_for_writing() const { return ready_for_writing_; }
  void           set_skip_tags(bool value) { skip_tags_ = value; }
  void           set_suffix(string_view suffix);
  void           set_write_mode(string_view write_mode) { write_mode_ = write_mode; }
  constexpr bool is_valid() const { return is_valid_; }
  void           set_is_default(bool value) { default_report_ = value; }

protected:
  // methods
  void SetUpInternalStates();
  // pure methods
  virtual void DoValidate(shared_ptr<Model> model) = 0;
  virtual void DoBuild(shared_ptr<Model> model)    = 0;
  virtual void DoPrepare(shared_ptr<Model> model){};
  virtual void DoExecute(shared_ptr<Model> model) = 0;
  virtual void DoFinalise(shared_ptr<Model> model){};
  virtual void DoPrepareTabular(shared_ptr<Model> model){};
  virtual void DoExecuteTabular(shared_ptr<Model> model){};
  virtual void DoFinaliseTabular(shared_ptr<Model> model){};
  string       ReportHeader(string type, string label, string format);

  // Members
  //  shared_ptr<Model>                      model_ = nullptr;
  RunMode::Type     run_mode_    = RunMode::kInvalid;
  State::Type       model_state_ = State::kInitialise;
  static std::mutex lock_;
  string            time_step_   = "";
  string            file_name_   = "";
  bool              first_write_ = true;
  bool              overwrite_   = true;
  string            last_suffix_ = "";
  string            write_mode_  = PARAM_OVERWRITE;
  vector<unsigned>  years_;
  ostringstream     cache_;
  bool              ready_for_writing_ = false;
  bool              skip_tags_         = false;
  string            suffix_            = "";
  string            format_            = PARAM_R;
  bool              is_valid_          = true;
  bool              default_report_    = false;
};

// Typedef
typedef std::shared_ptr<niwa::Report> ReportPtr;

} /* namespace niwa */
#endif /* REPORT_H_ */
