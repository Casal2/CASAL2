/**
 * @file RandomNumberSeed.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 13/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_REPORTS_CHILDREN_RANDOMNUMBERSEED_H_
#define SOURCE_REPORTS_CHILDREN_RANDOMNUMBERSEED_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Class definition
 */
class RandomNumberSeed : public niwa::Report {
public:
  // methods
  RandomNumberSeed();
  virtual                     ~RandomNumberSeed() = default;

protected:
  // pure methods
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_RANDOMNUMBERSEED_H_ */
