/**
 * @file Managers.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 11/08/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This is a mock class for the Model/Managers file
 */
#ifndef SOURCE_TESTRESOURCES_MOCKCLASSES_MANAGERS_H_
#define SOURCE_TESTRESOURCES_MOCKCLASSES_MANAGERS_H_
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>

#include "../../Model/Managers.h"

// Namespaces
namespace niwa {

/**
 * Class Definition
 */
class MockManagers : public niwa::Managers {
public:
  MockManagers() = delete;
  MockManagers(shared_ptr<Model> model) : Managers(model){};
  MOCK_METHOD0(time_step, timesteps::Manager*());
  MOCK_METHOD0(length_weight, lengthweights::Manager*());
};

typedef std::shared_ptr<MockManagers> MockManagersPtr;

} /* namespace niwa */

#endif  // TESTMODE
#endif  /* SOURCE_TESTRESOURCES_MOCKCLASSES_MANAGERS_H_ */
