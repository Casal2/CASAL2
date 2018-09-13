
/**
 * @file CallBack.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
// Headers
#include <Minimisers/Common/DESolver/CallBack.h>

#include "Estimates/Manager.h"
#include "EstimateTransformations/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Logging/Logging.h"

// Namespaces
namespace niwa {
namespace minimisers {
namespace desolver {

/**
 * Default constructor
 */
CallBack::CallBack(Model* model, unsigned vector_size, unsigned population_size, double tolerance)
  : niwa::minimisers::desolver::Engine(vector_size, population_size, tolerance),
  model_(model) {
}

/**
 * Destructor
 */
CallBack::~CallBack() {
}

/**
 * The energy function used to test the solution against
 * our model.
 *
 * @param test_solution The test solution to use
 * @return The score from the energy function
 */
double CallBack::EnergyFunction(vector<double> test_solution) {
  vector<Estimate*> estimates = model_->managers().estimate()->GetIsEstimated();

  if (test_solution.size() != estimates.size()) {
    LOG_CODE_ERROR() << "The number of enabled estimates does not match the number of test solution values";
  }

  for (unsigned i = 0; i < test_solution.size(); ++i)
    estimates[i]->set_value(test_solution[i]);

  model_->managers().estimate_transformation()->RestoreEstimates();
  model_->FullIteration();

  ObjectiveFunction& objective = model_->objective_function();
  objective.CalculateScore();

  model_->managers().estimate_transformation()->TransformEstimates();
  return objective.score();
}

} /* namespace desolver */
} /* namespace minimisers */
} /* namespace niwa */
#endif
