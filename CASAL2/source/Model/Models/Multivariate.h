/**
 * @file Multivariate.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science (c)2019 - www.niwa.co.nz
 */
#ifndef USE_AUTODIFF
#ifndef SOURCE_MODEL_MODELS_MULTIVARIATE_H_
#define SOURCE_MODEL_MODELS_MULTIVARIATE_H_

// headers
#include <Eigen/Eigen>

#include "../../Model/Model.h"

// namespaces
namespace niwa {
namespace model {

// class definition
class Multivariate : public Model {
public:
  Multivariate();
  virtual ~Multivariate() = default;
  bool Start(RunMode::Type run_mode) final;
  void DoValidate() final;
  void FullIteration() final;

private:
  // methods
  double pdf(const Eigen::VectorXd& x) const;

  // members
  bool            use_random_      = false;
  double          objective_score_ = 0.0;
  Eigen::VectorXd mean_;
  Eigen::MatrixXd sigma_;
};

} /* namespace model */
} /* namespace niwa */

#endif /* SOURCE_MODEL_MODELS_MULTIVARIATE_H_ */
#endif
