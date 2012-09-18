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
#include "../BaseClasses/Object.h"

namespace iSAM {
/**
 * Class definition
 */
class Report : public Base::Object {
public:
  // Methods
  Report();
  virtual                     ~Report();
  virtual void                Prepare() = 0;
  virtual void                Run() = 0;
  virtual void                Finalise() = 0;
};

} /* namespace iSAM */
#endif /* REPORT_H_ */
