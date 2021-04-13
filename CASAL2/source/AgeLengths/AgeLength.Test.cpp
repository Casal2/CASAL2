/**
 * @file AgeLength.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 29/01/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "AgeLength.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../TestResources/MockClasses/Managers.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace agelengths {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  MockTimeStepManager() = default;
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};


} /* namespace agelengths */
} /* namespace niwa */


#endif
