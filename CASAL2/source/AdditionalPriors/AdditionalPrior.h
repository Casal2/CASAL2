/**
 * @file AdditionalPrior.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Additional priors are stand alone priors that don't need
 * an estimate or addressable; but still contribute to the
 * objective function
 */
#ifndef ADDITIONALPRIOR_H_
#define ADDITIONALPRIOR_H_

// headers
#include "BaseClasses/Executor.h"

// namespaces
namespace niwa {
class Model;

// forward decs
class AdditionalPrior;
typedef Double (AdditionalPrior::*ScoreFunction)();

// classes
class AdditionalPrior : public base::Object {
public:
  // methods
  AdditionalPrior(Model* model);
  virtual                     ~AdditionalPrior() = default;
  virtual Double              GetScore() = 0;
  void                        Validate();
  void                        Build();
  void                        Reset() { };

protected:
  // pure virtual methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;

  // members
  string											parameter_;
  Model*                      model_;
  ScoreFunction               score_function_ = 0;
};

} /* namespace niwa */

#endif /* ADDITIONALPRIOR_H_ */
