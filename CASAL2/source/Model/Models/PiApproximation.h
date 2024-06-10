/**
 * @file PiApproximation.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */
#ifndef USE_AUTODIFF
#ifndef SOURCE_MODEL_MODELS_PIAPPROXIMATION_H_
#define SOURCE_MODEL_MODELS_PIAPPROXIMATION_H_

// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa::model {

// class declaration
class PiApproximation : public Model {
public:
  PiApproximation();
  virtual ~PiApproximation() = default;
  bool Start(RunMode::Type run_mode) override;
  void DoValidate() override;
  void FullIteration() override;

private:
  bool     use_random_      = false;
  double   objective_score_ = 0.0;
  unsigned inside_          = 0;
  unsigned outside_         = 0;
  double   size_            = 0;
};

}  // namespace niwa::model

#endif /* SOURCE_MODEL_MODELS_PIAPPROXIMATION_H_ */
#endif
