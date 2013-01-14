/**
 * @file KnifeEdge.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Model/Model.h"

class MockModel : public isam::Model {
public:
  MOCK_CONST_METHOD0(min_age, unsigned());
};

using ::testing::AtLeast;



#endif
