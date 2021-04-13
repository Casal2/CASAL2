/**
 * @file Multivariate.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 */
#ifndef USE_AUTODIFF
#ifndef SOURCE_MODEL_MODELS_MULTIVARIATE_H_
#define SOURCE_MODEL_MODELS_MULTIVARIATE_H_

// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa {
namespace model {

// class declaration
class Multivariate : public Model {
public:
	Multivariate();
	virtual 										~Multivariate();
	bool                				Start(RunMode::Type run_mode) override;
	void												DoValidate() override;
	void												FullIteration() override;


private:
	bool												use_random_ = false;
	double											objective_score_ = 0.0;
};

} /* namespace model */
} /* namespace niwa */

#endif /* SOURCE_MODEL_MODELS_MULTIVARIATE_H_ */
#endif
