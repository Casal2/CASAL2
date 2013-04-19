/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef REPORT_FACTORY_H_
#define REPORT_FACTORY_H_

// Headers
#include <string>

#include "Reports/Report.h"

// Namespaces
namespace isam {
namespace reports {

using std::string;

/**
 * Class definition
 */
class Factory {
public:
  static ReportPtr Create(const string& block_type, const string& report_type);
private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace reports */
} /* namespace isam */
#endif /* REPORTS_FACTORY_H_ */
