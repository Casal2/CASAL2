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
 * << Add Description >>
 */
#ifndef ADDITIONALPRIOR_H_
#define ADDITIONALPRIOR_H_

// headers
#include "BaseClasses/Executor.h"

// namespaces
namespace niwa {

class AdditionalPrior;
typedef Double (AdditionalPrior::*ScoreFunction)();

/**
 *
 */
class AdditionalPrior : public base::Object {
public:
  // methods
  AdditionalPrior();
  virtual                     ~AdditionalPrior() = default;
  Double                      GetScore();
  void                        Validate();
  void                        Build() { };
  void                        Reset() { };

  // pure virtual methods
  virtual void                DoValidate() = 0;

protected:
  // methods
  virtual Double              GetRatioScore() = 0;
  virtual Double              GetDifferenceScore() = 0;
  virtual Double              GetMeanScore() = 0;

  // members
  ScoreFunction               DoScoreFunction_ = 0;
  string                      method_;
};

// typedef
typedef boost::shared_ptr<niwa::AdditionalPrior> AdditionalPriorPtr;

} /* namespace niwa */

#endif /* ADDITIONALPRIOR_H_ */
