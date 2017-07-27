/**
 * @file CategoryInfo.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print out all of the information about the categories that
 * have been specified in the configuration file. This report is mostly
 * useful when debugging functionality of a configuration file.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef REPORTS_CATEGORYINFO_H_
#define REPORTS_CATEGORYINFO_H_

// Headers
#include "Common/Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Class Definition
 */
class CategoryInfo : public niwa::Report {
public:
  // Methods
  CategoryInfo(Model* model);
  virtual                     ~CategoryInfo() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_CATEGORYINFO_H_ */
