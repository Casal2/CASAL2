/**
 * @file RandomNumberSeed.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 13/04/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_REPORTS_CHILDREN_RANDOMNUMBERSEED_H_
#define SOURCE_REPORTS_CHILDREN_RANDOMNUMBERSEED_H_

// headers
#include "../../Reports/Report.h"

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
  virtual ~RandomNumberSeed() = default;

protected:
  // pure methods
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final{};
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_RANDOMNUMBERSEED_H_ */
