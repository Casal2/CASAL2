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
#ifndef ADDITIONALPRIORS_BETA_H_
#define ADDITIONALPRIORS_BETA_H_

// headers
#include "../../AdditionalPriors/AdditionalPrior.h"

// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class Beta : public AdditionalPrior {
public:
  // methods
  Beta(shared_ptr<Model> model);
  virtual ~Beta() = default;
  void   DoValidate() override final;
  void   DoBuild() override final;
  Double GetScore() override final;

protected:
  // members
  Double* addressable_ = nullptr;

  Double mu_    = 0.0;
  Double sigma_ = 0.0;
  Double a_     = 0.0;
  Double b_     = 0.0;
  Double v_     = 0.0;
  Double t_     = 0.0;
  Double m_     = 0.0;
  Double n_     = 0.0;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONALPRIORS_BETA_H_ */
