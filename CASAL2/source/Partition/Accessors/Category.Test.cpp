/**
 * @file Category.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 24/01/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../Categories/Categories.h"
#include "../../Partition/Partition.h"
#include "../../TestResources/MockClasses/Model.h"
#include "Category.h"

// Namespaces
namespace niwa {

using ::testing::Return;

/**
 *
 */
TEST(PartitionAccessors, Category) {
  //  boost::shared_ptr<MockModel> model = boost::shared_ptr<MockModel>(new MockModel);
  //  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(10));
  //  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(20));
  //  EXPECT_CALL(*model.get(), age_spread()).WillRepeatedly(Return(11));
  //
  //  CategoriesPtr categories = Categories::Instance();
  //  categories->set_block_type(PARAM_CATEGORIES);
  //  categories->parameters().Add(PARAM_NAMES, "male", __FILE__, __LINE__);
  //  categories->parameters().Add(PARAM_NAMES, "female", __FILE__, __LINE__);
  //  categories->Validate();
  //
  //  Partition& partition = Partition::Instance();
  //  partition.Build();

  //  niwa::partition::accessors::Category accessor;
  //  accessor.begin();
}

}  // namespace niwa
#endif /* #ifdef TESTMODE */
