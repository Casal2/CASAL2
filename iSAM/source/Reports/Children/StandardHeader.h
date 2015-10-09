//============================================================================
// Name        : CStandardHeaderReport.h
// Author      : S.Rasmussen
// Date        : 5/01/2009
// Copyright   : Copyright NIWA Science ©2008 - www.niwa.co.nz
// Description :
// $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
//============================================================================
#ifndef CSTANDARDHEADERREPORT_H_
#define CSTANDARDHEADERREPORT_H_

// Global Headers
#include <string>

#ifdef __MINGW32__
  #include <process.h>
#elif _MSC_VER
#else
  #include <sys/utsname.h>
  #include <sys/times.h>
  #include <unistd.h>
#endif

// headers
#include "Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {
/**
 * Class Definition
 */
class StandardHeader : public niwa::Report {
public:
  // Functions
  StandardHeader(Model* model);
  virtual                     ~StandardHeader();
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoPrepare() override final;
  void                        DoExecute() override final;
  void                        DoFinalise() override final;

protected:
  // Variables
  time_t                      time_start_;

  // Linux Vars
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  tms                         cpu_start;
  tms                         cpu_stop;
#endif
};

} /* namespace report */
} /* namespace niwa */

#endif /* CSTANDARDHEADERREPORT_H_ */
