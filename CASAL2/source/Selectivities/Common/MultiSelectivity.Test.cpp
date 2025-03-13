/**
 * @file MultiSelectivity.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2025/03/13
 * @section LICENSE
 *
 * Copyright Casal2 Project 2025 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

#include "MultiSelectivity.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "../../Selectivities/Manager.h"
#include "../../TestResources/MockClasses/Managers.h"
#include "../../TestResources/MockClasses/Model.h"
#include "InverseLogistic.h"
#include "Logistic.h"

namespace niwa {
using ::testing::_;
using ::testing::Invoke;

class MockSelectivitymanager : public selectivities::Manager {
public:
  MockSelectivitymanager()  = default;
  ~MockSelectivitymanager() = default;
};

/*
 * This selectivity is more challenging to unit test because it needs to reference
 * other selectivities and relies on the model current year to determine which selectivity
 * to use for the specific call.
 */
TEST(Selectivities, MultiSelectivity) {
  auto             model         = std::make_shared<MockModel>();
  auto             mock_managers = std::make_shared<MockManagers>(model);
  vector<unsigned> years         = {1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999};

  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(*model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(*model, managers()).WillRepeatedly(Return(mock_managers));
  EXPECT_CALL(*model, years()).WillRepeatedly(Return(years));
  EXPECT_CALL(*model, years_all()).WillRepeatedly(Return(years));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(10 - 5 + 1));
  EXPECT_CALL(*model, current_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));

  MockSelectivitymanager selectivity_manager;
  EXPECT_CALL(*mock_managers, selectivity()).WillRepeatedly(Return(&selectivity_manager));
  ASSERT_EQ(mock_managers, model->managers());
  ASSERT_EQ(&selectivity_manager, model->managers()->selectivity());

  niwa::selectivities::InverseLogistic* inverse_logistic = new niwa::selectivities::InverseLogistic(model);
  inverse_logistic->parameters().Add(PARAM_LABEL, "inverse_logistic", __FILE__, __LINE__);
  inverse_logistic->parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  inverse_logistic->parameters().Add(PARAM_A50, "2", __FILE__, __LINE__);
  inverse_logistic->parameters().Add(PARAM_ATO95, "7", __FILE__, __LINE__);
  inverse_logistic->Validate();
  inverse_logistic->Build();
  selectivity_manager.AddObject(inverse_logistic);

  niwa::selectivities::Logistic* logistic = new niwa::selectivities::Logistic(model);
  logistic->parameters().Add(PARAM_LABEL, "logistic", __FILE__, __LINE__);
  logistic->parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
  logistic->parameters().Add(PARAM_A50, "2", __FILE__, __LINE__);
  logistic->parameters().Add(PARAM_ATO95, "7", __FILE__, __LINE__);
  logistic->Validate();
  logistic->Build();
  selectivity_manager.AddObject(logistic);

  niwa::selectivities::MultiSelectivity* multi              = new niwa::selectivities::MultiSelectivity(model);
  vector<string>                         selectivity_years  = {"1990", "1991", "1992"};
  vector<string>                         selectivity_labels = {"inverse_logistic", "inverse_logistic", "inverse_logistic"};
  multi->parameters().Add(PARAM_LABEL, "multi", __FILE__, __LINE__);
  multi->parameters().Add(PARAM_TYPE, "n_a", __FILE__, __LINE__);
  multi->parameters().Add(PARAM_YEARS, selectivity_years, __FILE__, __LINE__);
  multi->parameters().Add(PARAM_SELECTIVITY_LABELS, selectivity_labels, __FILE__, __LINE__);
  multi->parameters().Add(PARAM_DEFAULT_SELECTIVITY, "logistic", __FILE__, __LINE__);
  multi->parameters().Add(PARAM_PROJECTION_SELECTIVITY, "logistic", __FILE__, __LINE__);
  multi->Validate();
  multi->Build();
  selectivity_manager.AddObject(multi);

  EXPECT_DOUBLE_EQ(0.15675987867112151, inverse_logistic->GetAgeResult(6, nullptr));  // At model->min_age()
  EXPECT_DOUBLE_EQ(0.10878902775125598, inverse_logistic->GetAgeResult(7, nullptr));
  EXPECT_DOUBLE_EQ(0.074206130458434716, inverse_logistic->GetAgeResult(8, nullptr));
  EXPECT_DOUBLE_EQ(0.15675987867112151, multi->GetAgeResult(6, nullptr));  // At model->min_age()
  EXPECT_DOUBLE_EQ(0.10878902775125598, multi->GetAgeResult(7, nullptr));
  EXPECT_DOUBLE_EQ(0.074206130458434716, multi->GetAgeResult(8, nullptr));

  EXPECT_CALL(*model, current_year()).WillRepeatedly(Return(1993));
  EXPECT_DOUBLE_EQ(0.84324012132887849, logistic->GetAgeResult(6, nullptr));  // At model->min_age()
  EXPECT_DOUBLE_EQ(0.89121097224874402, logistic->GetAgeResult(7, nullptr));
  EXPECT_DOUBLE_EQ(0.92579386954156528, logistic->GetAgeResult(8, nullptr));
  EXPECT_DOUBLE_EQ(0.84324012132887849, multi->GetAgeResult(6, nullptr));  // At model->min_age()
  EXPECT_DOUBLE_EQ(0.89121097224874402, multi->GetAgeResult(7, nullptr));
  EXPECT_DOUBLE_EQ(0.92579386954156528, multi->GetAgeResult(8, nullptr));

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_managers.get()));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

}  // namespace niwa
#endif  // TESTMODE