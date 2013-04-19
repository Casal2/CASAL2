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

/**
 * Class definition
 */
class Report : public base::Object {
public:
  // Methods
  Report();
  virtual                     ~Report() = default;
  virtual void                Validate();
  virtual void                Build() {};
  virtual void                Prepare() {};
  virtual void                Execute() = 0;
  virtual void                Reset() {};
  virtual void                Finalise() {};

  // Accessors
  State::Type                 model_state() const { return model_state_; }
  const string&               time_step() const { return time_step_; }
  unsigned                    year() const { return year_; }

protected:
  // Members
  State::Type                 model_state_ = State::kInitialise;
  string                      time_step_   = "";
  unsigned                    year_ = 0;
};

// Typedef
typedef boost::shared_ptr<isam::Report> ReportPtr;

} /* namespace isam */
#endif /* REPORT_H_ */
