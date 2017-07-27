/**
 * @file Beta.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef ADDITIONALPRIORS_UNIFORM_LOG_H_
#define ADDITIONALPRIORS_UNIFORM_LOG_H_

// headers
#include "Common/AdditionalPriors/AdditionalPrior.h"


// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class UniformLog : public AdditionalPrior {
public:
  // methods
	UniformLog(Model* model);
  virtual                     ~UniformLog() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  Double                      GetScore() override final;

protected:
  // members
  Double* 										addressable_ = nullptr;
  Double											score_ = 0.0;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONALPRIORS_UNIFORM_LOG_H_ */
