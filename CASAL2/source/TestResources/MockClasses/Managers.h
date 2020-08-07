/**
 * @file Managers.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 11/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a mock class for the Model/Managers file
 */
#ifndef SOURCE_TESTRESOURCES_MOCKCLASSES_MANAGERS_H_
#define SOURCE_TESTRESOURCES_MOCKCLASSES_MANAGERS_H_

// Headers
#include "Model/Managers.h"

// Namespaces
namespace niwa {

/**
 * Class Definition
 */
class MockManagers : public niwa::Managers {
public:
  MockManagers(Model* model) : Managers(model) { };
  MOCK_METHOD0(time_step, timesteps::Manager*());
  MOCK_METHOD0(length_weight, lengthweights::Manager*());
};

} /* namespace niwa */

#endif /* SOURCE_TESTRESOURCES_MOCKCLASSES_MANAGERS_H_ */
