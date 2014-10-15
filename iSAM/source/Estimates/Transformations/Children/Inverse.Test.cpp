/**
 * @file Inverse.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Inverse.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

// Namespaces
namespace isam {
namespace estimates {
namespace transformations {


using ::testing::Return;

/**
 * Test the results of our KnifeEdge are correct
 */
TEST(Estimates_Transformations, Inverse) {

  estimates::TransformationPtr transform = estimates::TransformationPtr(new Inverse());


  EXPECT_DOUBLE_EQ(2.5, transform->Transform(0.4));
  EXPECT_DOUBLE_EQ(0.4, transform->Untransform(2.5));

  EXPECT_DOUBLE_EQ(0.008, transform->Transform(125));
  EXPECT_DOUBLE_EQ(125,   transform->Untransform(0.008));

  EXPECT_DOUBLE_EQ(0.025, transform->Transform(40));
  EXPECT_DOUBLE_EQ(40,    transform->Untransform(0.025));
}

}
}
}
#endif /* ifdef TESTMODE */
