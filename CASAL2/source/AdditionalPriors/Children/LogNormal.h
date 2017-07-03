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
 * This class along with the the uniformLog class were specifically implemented so that Casal2 could emulate the nuisance Q functionality that CASAL does.
 */
#ifndef ADDITIONALPRIORS_LOGNORMAL_H_
#define ADDITIONALPRIORS_LOGNORMAL_H_

// headers
#include "AdditionalPriors/AdditionalPrior.h"


// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class LogNormal : public AdditionalPrior {
public:
  // methods
	LogNormal(Model* model);
  virtual                     ~LogNormal() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  Double                      GetScore() override final;

protected:
  // members
  Double* 										addressable_ = nullptr;
  Double											score_ = 0.0;
  Double											mu_;
  Double											cv_;
  Double											sigma_;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONALPRIORS_LOGNORMAL_H_ */
