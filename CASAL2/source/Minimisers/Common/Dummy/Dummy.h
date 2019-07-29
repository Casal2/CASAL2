/**
 * @file Dummy.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Feb 9, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a dummy minimiser that is used in the shared
 * libraries to store estimate values and covariance matrix
 * for the MCMCs. It's used when the specified minimiser is an
 * auto-differentiating one but we want to run the MCMC through
 * a standard configuration.
 *
 * This class will hold information for the MCMC.
 */
#ifndef SOURCE_MINIMISERS_CHILDREN_DUMMY_H_
#define SOURCE_MINIMISERS_CHILDREN_DUMMY_H_

// headers
#include "../../Minimiser.h"

// namespaces
namespace niwa {
namespace minimisers {

class Dummy : public Minimiser {
public:
  Dummy(Model* model);
  virtual                     ~Dummy() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoReset() override final { };
  void                        Execute() override final;
};
} /* namespace minimisers */
} /* namespace niwa */

#endif /* SOURCE_MINIMISERS_CHILDREN_DUMMY_H_ */
