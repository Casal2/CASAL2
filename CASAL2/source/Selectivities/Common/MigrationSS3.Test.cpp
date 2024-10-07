/**
 * @file MigrationSS3.Test.cpp
 * @author  S. Datta
 * @version 1.0
 * @date 2024
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2024-10-02 12:19:00 +1300 (Wed, 02 Oct 2024) $
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>
#include <string>

#include "../../TestResources/MockClasses/Model.h"
#include "../../TestResources/MockClasses/ModelLength.h"
#include "MigrationSS3.h"

// Namespaces
namespace niwa {

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Test the results of our selectivity are correct

# R function

migration_stock_synthesis <- function(x,L_, H_, y1_, y2_,
		alt1_ = F, alt1_rate_ = 0,
		alt2_ = F, alt2_rate_ = 0,
		r_ = 1e-4){
		#
		# migration rate for ages between L & H interpolated
		# rates at L is y1, at H is y2
		# rate gounger ages than L = y1 (unless alt1 is false, then alt1_rate)
		# similarly for age older than H
		#
	y1dash = y1_ + r_
	y2dash = y2_ + r_
	k      = log(y2dash/y1dash)/(H_ - L_)
	if(alt1_) yyoung = alt1_rate_ else yyoung = y1dash 
	if(alt2_) yold   = alt2_rate_ else yold   = y2dash 
	
	rate   = rep(0,length(x))
	younger= x < L_
	if(length(younger[younger]) > 0) rate[younger] <- yyoung
	old    = x > H_
	if(length(old[old]) > 0) rate[old] <- yold
	
	IN     = x >= L_ & x <= H_
	if(length(IN[IN]) > 0) rate[IN] <- y1dash * exp(k*(x[IN] - L_))
	
	return(data.frame(x=x,y=rate))
}
	
y <- migration_stock_synthesis(x,5,12,0.3,0.05,T,0.05,T,0.02)  #usual case
plot(y$x,y$y,type="l",lwd=3,ylim=c(0,0.35),las=1,ylab="Migration rate",xlab="Age")
text(c(5,12),rep(0.01,2),labels=c("L","H"))
abline(v=c(5,12),lty=2)

y$y

 */
 
TEST(Selectivities, Migration_Age_ss3) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(1));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(20));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));
  niwa::selectivities::MigrationSS3 migration_stock_synthesis(model);

  migration_stock_synthesis.parameters().Add(PARAM_LABEL, "unit_test_migration_ss3", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_L, "5", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_H, "12", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_Y1, "0.3", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_Y2, "0.05", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_ALT_RATE_LOWER_SWITCH, "1", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_ALT_RATE_UPPER_SWITCH, "1", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_ALT_RATE_LOWER, "0.05", __FILE__, __LINE__);
  migration_stock_synthesis.parameters().Add(PARAM_ALT_RATE_UPPER, "0.02", __FILE__, __LINE__);


  migration_stock_synthesis.Validate();
  migration_stock_synthesis.Build();
  migration_stock_synthesis.Reset();

  ASSERT_THROW(migration_stock_synthesis.GetAgeResult(0, nullptr), std::string);      
  // values from R
  EXPECT_NEAR(0.05000000, migration_stock_synthesis.GetAgeResult(1, nullptr), 1e-4); 
  EXPECT_NEAR(0.05000000, migration_stock_synthesis.GetAgeResult(2, nullptr), 1e-4); 
  EXPECT_NEAR(0.05000000, migration_stock_synthesis.GetAgeResult(3, nullptr), 1e-4); 
  EXPECT_NEAR(0.05000000, migration_stock_synthesis.GetAgeResult(4, nullptr), 1e-4); 
  EXPECT_NEAR(0.30010000, migration_stock_synthesis.GetAgeResult(5, nullptr), 1e-4); 
  EXPECT_NEAR(0.23238325, migration_stock_synthesis.GetAgeResult(6, nullptr), 1e-4); 
  EXPECT_NEAR(0.17994659, migration_stock_synthesis.GetAgeResult(7, nullptr), 1e-4); 
  EXPECT_NEAR(0.13934213, migration_stock_synthesis.GetAgeResult(8, nullptr), 1e-4); 
  EXPECT_NEAR(0.10789996, migration_stock_synthesis.GetAgeResult(9, nullptr), 1e-4); 
  EXPECT_NEAR(0.08355262, migration_stock_synthesis.GetAgeResult(10, nullptr), 1e-4); 
  EXPECT_NEAR(0.06469920, migration_stock_synthesis.GetAgeResult(11, nullptr), 1e-4);   
  EXPECT_NEAR(0.05010000, migration_stock_synthesis.GetAgeResult(12, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(13, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(14, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(15, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(16, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(17, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(18, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(19, nullptr), 1e-4); 
  EXPECT_NEAR(0.02000000, migration_stock_synthesis.GetAgeResult(20, nullptr), 1e-4); 

}

}  // namespace niwa

#endif /* ifdef TESTMODE */
