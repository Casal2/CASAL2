/**
 * @file CallBack.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2013 - www.niwa.co.nz
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
#include "../../../Minimisers/Common/DESolver/Engine.h"
#include "../../../Model/Model.h"
#include "../../../Utilities/Types.h"

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
  CallBack(shared_ptr<Model> model, unsigned vector_size, unsigned population_size, double tolerance);
  virtual ~CallBack();
  double EnergyFunction(vector<double> test_solution) override final;

private:
  // Members
  shared_ptr<Model> model_;
};

} /* namespace desolver */
} /* namespace minimisers */
} /* namespace niwa */
#endif /* MINIMISERS_DESOLVER_CALLBACK_H_ */
#endif /* USE_AUTODIFF */
