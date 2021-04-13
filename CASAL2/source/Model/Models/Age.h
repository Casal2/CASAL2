/**
 * @file Age.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 *
 * @description
 * This is a specialisation of our core model object that is for age based models.
 */
#ifndef SOURCE_MODEL_MODELS_AGE_H_
#define SOURCE_MODEL_MODELS_AGE_H_

// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa {
namespace model {

// class declaration
class Age : public Model {
public:
	// methods
	Age();
	virtual ~Age();
//	bool 												Start(RunMode::Type run_mode);
	void													DoValidate() override;
};

} /* namespace model */
} /* namespace niwa */

#endif /* SOURCE_MODEL_MODELS_AGE_H_ */
