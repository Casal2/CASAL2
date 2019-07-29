/**
 * @file Free.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Free catchabilities are normal addressables that require a prior, but are considered a nuisance parameter in the estimation problem.
 */
#ifndef CATCHABILITIES_FREE_H_
#define CATCHABILITIES_FREE_H_

// headers
#include "Catchabilities/Catchability.h"

// namespaces
namespace niwa {
namespace catchabilities {

// classes
class Free : public Catchability {
public:
  Free() = delete;
  explicit                    Free(Model* model);
  virtual                     ~Free() { };
  Double                      q() const override final { return q_; }
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };

private:
  // members
  Double                      q_;
};

} /* namespace catchabilities */
} /* namespace niwa */

#endif /* FREE_H_ */
