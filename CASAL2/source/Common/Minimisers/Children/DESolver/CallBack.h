/**
 * @file CallBack.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
#ifndef MINIMISERS_DESOLVER_CALLBACK_H_
#define MINIMISERS_DESOLVER_CALLBACK_H_

// Headers
#include "Common/Minimisers/Children/DESolver/Engine.h"
#include "Common/Model/Model.h"
#include "Common/Utilities/Types.h"

// Namespaces
namespace niwa {
namespace minimisers {
namespace desolver {

/**
 * Class Definition
 */
class CallBack : public niwa::minimisers::desolver::Engine {
public:
  // Methods
  CallBack(Model* model, unsigned vector_size, unsigned population_size, double tolerance);
  virtual                     ~CallBack();
  double                      EnergyFunction(vector<double> test_solution) override final;

private:
  // Members
  Model*                    model_;
};

} /* namespace desolver */
} /* namespace minimisers */
} /* namespace niwa */
#endif /* MINIMISERS_DESOLVER_CALLBACK_H_ */
#endif /* USE_AUTODIFF */
