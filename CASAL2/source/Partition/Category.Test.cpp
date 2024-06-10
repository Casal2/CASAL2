/**
 * @file Category.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 07/03/2018
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

// Headers
#include "Category.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include "../AgeLengths/Age/VonBertalanffy.h"
#include "../Categories/Categories.h"
#include "../Selectivities/Common/Logistic.h"
#include "../TestResources/MockClasses/Managers.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TimeSteps/Manager.h"

// namespaces
namespace niwa {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class MockCategories : public Categories {
public:
  MockCategories(shared_ptr<Model> model) : Categories(model) {
    vector<string> names;
    vector<string> sexes  = {"male", "female"};
    vector<string> stages = {"immature", "mature"};

    for (string sex : sexes)
      for (string stage : stages) names.push_back(sex + "." + stage);

    set_block_type(PARAM_CATEGORIES);
    parameters().Add(PARAM_FORMAT, "sex.stage", __FILE__, __LINE__);
    parameters().Add(PARAM_NAMES, names, __FILE__, __LINE__);
  };

  map<string, string> GetCategoryLabelsAndValues(const string& lookup, const string& parameter_location) override final {
    return Categories::GetCategoryLabelsAndValues(lookup, parameter_location);
  }

  AgeLength* age_length_ = nullptr;
  AgeLength* age_length(const string& category_name) override final { return age_length_; }
};

class MockVonBertalanffy : public agelengths::VonBertalanffy {
public:
  MockVonBertalanffy(Distribution distribution = Distribution::kNormal) : VonBertalanffy(nullptr) { distribution_ = distribution; }

  MockVonBertalanffy(shared_ptr<Model> model, Double linf, Double k, Double t0, bool by_length, Double cv_first, Double cv_last, vector<double> time_step_proportions,
                     string compatibility = PARAM_CASAL2, Distribution distribution = Distribution::kNormal) :
      VonBertalanffy(model) {
    linf_                  = linf;
    k_                     = k;
    t0_                    = t0;
    by_length_             = by_length;
    cv_first_              = cv_first;
    cv_last_               = cv_last;
    time_step_proportions_ = time_step_proportions;
    compatibility_         = compatibility;
    distribution_          = distribution;
    year_offset_ = model ->start_year();
    age_offset_ = model ->min_age();    
    model_years_ = model->years();
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

} /* namespace niwa */
#endif
