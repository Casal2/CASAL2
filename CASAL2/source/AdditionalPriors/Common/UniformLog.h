/**
 * @file Beta.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef ADDITIONALPRIORS_UNIFORM_LOG_H_
#define ADDITIONALPRIORS_UNIFORM_LOG_H_

// headers
#include "../../AdditionalPriors/AdditionalPrior.h"

// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class UniformLog : public AdditionalPrior {
public:
  // methods
  UniformLog(shared_ptr<Model> model);
  virtual ~UniformLog() = default;
  void   DoValidate() final{};
  void   DoBuild() final;
  Double GetScore() final;

protected:
  // members
  Double* addressable_ = nullptr;
  Double  score_       = 0.0;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONALPRIORS_UNIFORM_LOG_H_ */
