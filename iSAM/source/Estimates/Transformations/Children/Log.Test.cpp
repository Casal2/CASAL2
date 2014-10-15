/**
 * @file Log.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Log.h"

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
TEST(Estimates_Transformations, Log) {

  estimates::TransformationPtr transform = estimates::TransformationPtr(new Log());

  EXPECT_DOUBLE_EQ(-0.916290731874155, transform->Transform(0.4));
  EXPECT_DOUBLE_EQ(0.4, transform->Untransform(-0.916290731874155));

  EXPECT_DOUBLE_EQ(4.8283137373023015, transform->Transform(125));
  EXPECT_DOUBLE_EQ(125,   transform->Untransform(4.8283137373023015));

  EXPECT_DOUBLE_EQ(3.6888794541139363, transform->Transform(40));
  EXPECT_DOUBLE_EQ(40,    transform->Untransform(3.6888794541139363));
}

}
}
}
#endif /* ifdef TESTMODE */
