/**
 * @file Addressable.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/06/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print an addressable out during a year/time step
 * so you can track the value of it over time (useful when it's
 * been modified during a projection etc)
 */
#ifndef REPORTS_ADDRESSABLE_H_
#define REPORTS_ADDRESSABLE_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * class definition
 */
class Addressable : public niwa::Report {
public:
  // methods
  Addressable();
  virtual                     ~Addressable() = default;
  void                        DoValidate(shared_ptr<Model> model) final { };
  void                        DoBuild(shared_ptr<Model> model) final;
  void                        DoPrepare(shared_ptr<Model> model) final;
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoFinalise(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final { };

private:
  // members
  string                      parameter_ = "";
  Double*                     target_ = 0;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* REPORTS_ADDRESSABLE_H_ */
