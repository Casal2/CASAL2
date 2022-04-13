/**
 * @file DoubleNormalSS3.Test.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>
#include <string>

#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/MockClasses/ModelLength.h"
#include "DoubleNormalSS3.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the results of our selectivity are correct
p1 <- 7.5
p2 <- -0.5
p3 <- 1
p4 <- 1
p5 <- -10
p6 <- 0.5

bin_vec <- 1:20
max_x_val <- max(bin_vec)
# from above, sent by Adam Langley on 2022-02-22
p1trans <- p1
p2trans <- p1trans + 1 + (0.99 + max_x_val - p1trans - 1)/(1 + exp(-1.0 * p2))
p3trans <- exp(p3)
p4trans <- exp(p4)
p5trans <- 1/(1 + exp(-1.0 * p5))
p6trans <- 1/(1 + exp(-1.0 * p6))
midbin <- bin_vec
asc <- exp(-((midbin - p1trans)^2/p3trans))
asc.scaled <- (p5trans + (1 - p5trans) * (asc - 0)/(1 - 0))
desc <- exp(-((midbin - p2trans)^2/p4trans))
stj <- exp(-((40 - p2trans)^2/p4trans))
des.scaled <- (1 + (p6trans - 1) * (desc - 1) /(stj - 1))
join1 <- 1/(1 + exp(-(20 * (midbin - p1trans)/(1 + abs(midbin - p1trans)))))
join2 <- 1/(1 + exp(-(20 * (midbin - p2trans)/(1 + abs(midbin - p2trans)))))
selex <- asc.scaled * (1 - join1) + join1 * (1 * (1 - join2) + des.scaled * join2)
selex[1] <- exp(p5)
 */
TEST(Selectivities, DoubleNormal_Age_ss3) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(1));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));
  niwa::selectivities::DoubleNormalSS3 double_normal_stock_synthesis(model);

  double_normal_stock_synthesis.parameters().Add(PARAM_LABEL, "unit_test_double_normal_ss3", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_PEAK, "7.5", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_Y0, "-10", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_Y1, "0.5", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_ASCENDING, "1", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_DESCENDING, "1", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_WIDTH, "-0.5", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_L, "1", __FILE__, __LINE__);
  double_normal_stock_synthesis.parameters().Add(PARAM_H, "20", __FILE__, __LINE__);


  double_normal_stock_synthesis.Validate();
  double_normal_stock_synthesis.Build();

  ASSERT_THROW(double_normal_stock_synthesis.GetAgeResult(0, nullptr), std::string);      
  // values from R
  EXPECT_NEAR(4.539993e-05, double_normal_stock_synthesis.GetAgeResult(1, nullptr), 1e-4); 
  EXPECT_NEAR(6.013177e-05, double_normal_stock_synthesis.GetAgeResult(2, nullptr), 1e-4); 
  EXPECT_NEAR(6.271479e-04, double_normal_stock_synthesis.GetAgeResult(3, nullptr), 1e-4); 
  EXPECT_NEAR(1.108184e-02, double_normal_stock_synthesis.GetAgeResult(4, nullptr), 1e-4); 
  EXPECT_NEAR(1.003758e-01, double_normal_stock_synthesis.GetAgeResult(5, nullptr), 1e-4); 
  EXPECT_NEAR(4.370698e-01, double_normal_stock_synthesis.GetAgeResult(6, nullptr), 1e-4); 
  EXPECT_NEAR(9.122483e-01, double_normal_stock_synthesis.GetAgeResult(7, nullptr), 1e-4); 
  EXPECT_NEAR(9.998883e-01, double_normal_stock_synthesis.GetAgeResult(8, nullptr), 1e-4); 
  EXPECT_NEAR(9.999965e-01, double_normal_stock_synthesis.GetAgeResult(9, nullptr), 1e-4); 
  EXPECT_NEAR(9.999994e-01, double_normal_stock_synthesis.GetAgeResult(10, nullptr), 1e-4); 
  EXPECT_NEAR(9.999995e-01, double_normal_stock_synthesis.GetAgeResult(11, nullptr), 1e-4);   
  EXPECT_NEAR(9.999972e-01 , double_normal_stock_synthesis.GetAgeResult(12, nullptr), 1e-4); 
  EXPECT_NEAR(9.998207e-01, double_normal_stock_synthesis.GetAgeResult(13, nullptr), 1e-4); 
  EXPECT_NEAR(9.235162e-01, double_normal_stock_synthesis.GetAgeResult(14, nullptr), 1e-4); 
  EXPECT_NEAR(7.394591e-01, double_normal_stock_synthesis.GetAgeResult(15, nullptr), 1e-4); 
  EXPECT_NEAR(6.442464e-01, double_normal_stock_synthesis.GetAgeResult(16, nullptr), 1e-4); 
  EXPECT_NEAR(6.244033e-01, double_normal_stock_synthesis.GetAgeResult(17, nullptr), 1e-4); 
  EXPECT_NEAR(6.225425e-01 , double_normal_stock_synthesis.GetAgeResult(18, nullptr), 1e-4); 
  EXPECT_NEAR(6.224610e-01, double_normal_stock_synthesis.GetAgeResult(19, nullptr), 1e-4); 
  EXPECT_NEAR(6.224594e-01, double_normal_stock_synthesis.GetAgeResult(20, nullptr), 1e-4); 
 

}

}  // namespace niwa

#endif /* ifdef TESTMODE */
