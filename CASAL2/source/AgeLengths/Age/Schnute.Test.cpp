/**
 * @file Schnute.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../TestResources/MockClasses/Managers.h"
#include "../../TestResources/MockClasses/Model.h"
#include "../../TimeSteps/Manager.h"
#include "Schnute.h"

// Namespaces
namespace niwa {
namespace agelengths {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  MockTimeStepManager()     = default;
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class MockSchnute : public Schnute {
public:
  MockSchnute(shared_ptr<Model> model, Double y1, Double y2, Double tau1, Double tau2, Double a, Double b, bool by_length, Double cv_first, Double cv_last,
              vector<Double> time_step_proportions) :
      Schnute(model) {
    y1_                    = y1;
    y2_                    = y2;
    tau1_                  = tau1;
    tau2_                  = tau2;
    a_                     = a;
    b_                     = b;
    by_length_             = by_length;
    cv_first_              = cv_first;
    cv_last_               = cv_last;
    time_step_proportions_ = time_step_proportions;

    year_offset_ = model ->start_year();
    age_offset_ = model ->min_age();    
    model_years_ = model->years();
    min_age_ = model ->min_age(); 
    max_age_ = model ->min_age(); 
    // allocate memory for cvs; this is usually done in the Build() but difficult to mock
    cvs_.resize(model->years().size());
    for(unsigned year_ndx = 0; year_ndx < cvs_.size(); ++year_ndx) {
      cvs_[year_ndx].resize(model->time_steps().size());
      for(unsigned time_step_ndx = 0; time_step_ndx < cvs_[year_ndx].size(); ++time_step_ndx) {
        cvs_[year_ndx][time_step_ndx].resize(model->age_spread(), 0.0);
      }
    } 
    // allocate memory for mean weight and length
    mean_length_by_timestep_age_.resize(model_->time_steps().size());
    mean_weight_by_timestep_age_.resize(model_->time_steps().size());
    for (unsigned time_step_ndx = 0; time_step_ndx < mean_weight_by_timestep_age_.size(); ++time_step_ndx) {
      mean_length_by_timestep_age_[time_step_ndx].resize(model_->age_spread(), 0.0);
      mean_weight_by_timestep_age_[time_step_ndx].resize(model_->age_spread(), 0.0);
    }  

  }

  void MockPopulateCV() { this->PopulateCV(); }
  };


/**
 * Test the results of our Schnute are correct
 */
TEST(AgeLengths, Schnute) {
  shared_ptr<MockModel> model         = shared_ptr<MockModel>(new MockModel());
  MockManagersPtr       mock_managers = MockManagersPtr(new MockManagers(model));
  MockTimeStepManager   time_step_manager;
  time_step_manager.time_step_index_ = 0;
  vector<string>   time_steps        = {"0", "1", "2"};
  vector<unsigned> years             = {1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999};

  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(*model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(*model, managers()).WillRepeatedly(Return(mock_managers));
  EXPECT_CALL(*model, time_steps()).WillRepeatedly(ReturnRef(time_steps));
  EXPECT_CALL(*model, years()).WillRepeatedly(Return(years));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(10 - 5 + 1));

  EXPECT_CALL(*mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 1.5, 0.01, {0.0});
  schnute.MockPopulateCV();
  EXPECT_DOUBLE_EQ(69.024873822523432, schnute.mean_length(0, 5));
  EXPECT_DOUBLE_EQ(74.848134092163818, schnute.mean_length(0, 6));
  EXPECT_DOUBLE_EQ(79.70651775663795, schnute.mean_length(0, 7));
  EXPECT_DOUBLE_EQ(83.803556291745934, schnute.mean_length(0, 8));
  EXPECT_DOUBLE_EQ(87.285326700186346, schnute.mean_length(0, 9));
  EXPECT_DOUBLE_EQ(90.261388412893822, schnute.mean_length(0, 10));

  //ASSERT_NO_THROW(schnute.MockBuildCV());
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 5));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 6));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 7));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 8));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 9));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 10));

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_managers.get()));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_ByLength_Proportion) {
  shared_ptr<MockModel> model         = shared_ptr<MockModel>(new MockModel());
  MockManagersPtr       mock_managers = MockManagersPtr(new MockManagers(model));
  MockTimeStepManager   time_step_manager;
  time_step_manager.time_step_index_ = 0;
  EXPECT_CALL(*mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));
  vector<string>   time_steps = {"0", "1", "2"};
  vector<unsigned> years      = {1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999};

  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(*model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(*model, managers()).WillRepeatedly(Return(mock_managers));
  EXPECT_CALL(*model, time_steps()).WillRepeatedly(ReturnRef(time_steps));
  EXPECT_CALL(*model, years()).WillRepeatedly(Return(years));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(10 - 5 + 1));

  MockSchnute  schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 1.5, 7, {0.3});
  schnute.MockPopulateCV();

  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 5));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 6));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 7));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 8));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 9));
  EXPECT_DOUBLE_EQ(1.5, schnute.cv(1990, 0, 10));

  EXPECT_DOUBLE_EQ(70.88858658180817, schnute.mean_length(0, 5));
  EXPECT_DOUBLE_EQ(76.396278285380305, schnute.mean_length(0, 6));
  EXPECT_DOUBLE_EQ(81.008050505923777, schnute.mean_length(0, 7));
  EXPECT_DOUBLE_EQ(84.907129213795571, schnute.mean_length(0, 8));
  EXPECT_DOUBLE_EQ(88.226983724973564, schnute.mean_length(0, 9));
  EXPECT_DOUBLE_EQ(91.068783855484241, schnute.mean_length(0, 10));

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_managers.get()));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

/**
 *
 */
TEST(AgeLengths, Schnute_BuildCV_ByLength_ProportionAndTimeStep) {
  shared_ptr<MockModel> model         = shared_ptr<MockModel>(new MockModel());
  MockManagersPtr       mock_managers = MockManagersPtr(new MockManagers(model));
  MockTimeStepManager   time_step_manager;
  time_step_manager.time_step_index_ = 1;
  vector<string>   time_steps        = {"0", "1", "2"};
  vector<unsigned> years             = {1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999};

  EXPECT_CALL(*mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));

  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(*model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(*model, managers()).WillRepeatedly(Return(mock_managers));
  EXPECT_CALL(*model, time_steps()).WillRepeatedly(ReturnRef(time_steps));
  EXPECT_CALL(*model, years()).WillRepeatedly(Return(years));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(10 - 5 + 1));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, true, 0.2, 0.9, {0.25, 0.5});
  ASSERT_NO_THROW(schnute.MockPopulateCV());
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 1, 5));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 1, 6));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 1, 7));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 1, 8));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 1, 9));
  EXPECT_DOUBLE_EQ(0.20000000000000001, schnute.cv(1990, 1, 10));

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_managers.get()));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

/**
 *  Since I made the change to BuildCV() to identify if the cv_last->was_defined(); the test doesn't recognise what parameters have been populated
 *  This component will be tested elsewhere-> TestModels/ORH has cv_last and cv_first by_length. So if that passe the test I am happy that this funcitonality works
 */
TEST(AgeLengths, Schnute_BuildCV_LinearInterpolation) {
  shared_ptr<MockModel> model         = shared_ptr<MockModel>(new MockModel());
  MockManagersPtr       mock_managers = MockManagersPtr(new MockManagers(model));
  MockTimeStepManager   time_step_manager;
  time_step_manager.time_step_index_ = 0;
  vector<string>   time_steps        = {"0", "1", "2"};
  vector<unsigned> years             = {1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999};

  EXPECT_CALL(*mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));

  EXPECT_CALL(*model, min_age()).WillRepeatedly(Return(5));
  EXPECT_CALL(*model, max_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model, start_year()).WillRepeatedly(Return(1990));
  EXPECT_CALL(*model, final_year()).WillRepeatedly(Return(1999));
  EXPECT_CALL(*model, managers()).WillRepeatedly(Return(mock_managers));
  EXPECT_CALL(*model, time_steps()).WillRepeatedly(ReturnRef(time_steps));
  EXPECT_CALL(*model, years()).WillRepeatedly(Return(years));
  EXPECT_CALL(*model, age_spread()).WillRepeatedly(Return(10 - 5 + 1));

  MockSchnute schnute(model, 24.5, 104.8, 1, 20, 0.131, 1.70, false, 0.1, 0.9, {1.0});
  ASSERT_NO_THROW(schnute.MockPopulateCV());
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 0, 5));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 0, 6));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 0, 7));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 0, 8));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 0, 9));
  EXPECT_DOUBLE_EQ(0.10000000000000001, schnute.cv(1990, 0, 10));

  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_managers.get()));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(model.get()));
}

} /* namespace agelengths */
} /* namespace niwa */

#endif
