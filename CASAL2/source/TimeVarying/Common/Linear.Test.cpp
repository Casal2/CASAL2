/**
 * @file Linear.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/04/16
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "Linear.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Model/Managers.h"
#include "Model/Model.h"
#include "Reports/Report.h"
#include "Reports/Test/AddressableValue.h"
#include "TestResources/Models/TwoSex.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TimeVarying/Manager.h"

namespace niwa::timevarying {
using niwa::testfixtures::InternalEmptyModel;

TEST_F(InternalEmptyModel, Timevarying_Linear) {
  ASSERT_EQ(true, true);
}

}  // namespace niwa::timevarying
#endif  // TESTMODE