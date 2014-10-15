/**
 * @file SquareRoot.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "SquareRoot.h"

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
TEST(Estimates_Transformations, SquareRoot) {

  estimates::TransformationPtr transform = estimates::TransformationPtr(new SquareRoot());

  EXPECT_DOUBLE_EQ(0.63245553203367588, transform->Transform(0.4));
  EXPECT_DOUBLE_EQ(0.4, transform->Untransform(0.63245553203367588));

  EXPECT_DOUBLE_EQ(11.180339887498949, transform->Transform(125));
  EXPECT_DOUBLE_EQ(125,   transform->Untransform(11.180339887498949));

  EXPECT_DOUBLE_EQ(6.324555320336759, transform->Transform(40));
  EXPECT_DOUBLE_EQ(40,    transform->Untransform(6.324555320336759));
}

}
}
}
#endif /* ifdef TESTMODE */
