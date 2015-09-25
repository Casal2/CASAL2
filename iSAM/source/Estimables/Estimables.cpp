/**
 * @file Estimables.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Estimables.h"

#include "Estimates/Manager.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Model.h"
#include "ObjectsFinder/ObjectsFinder.h"
#include "Logging/Logging.h"

// namespaces
namespace niwa {

/**
 * Default constructor
 */
Estimables::Estimables() {
}

/**
 * Instance method
 */
Estimables& Estimables::Instance() {
  static Estimables instance;
  return instance;
}

/**
 *
 */
void Estimables::AddValue(const string& estimable_label, Double value) {
  estimable_values_[estimable_label].push_back(value);
}

/**
 *
 */
vector<string> Estimables::GetEstimables() const {
  vector<string> result;
  for (auto iter : estimable_values_)
    result.push_back(iter.first);

  return result;
}

/**
 *
 */
unsigned Estimables::GetValueCount() const {
  if (estimable_values_.size() == 0)
    return 0;

  auto iter = estimable_values_.begin();
  return iter->second.size();
}
/**
 *
 */
map<string, Double> Estimables::GetValues(unsigned index) const {
  map<string, Double> result;
  for (auto iter : estimable_values_)
    result[iter.first] = iter.second[index];

  return result;
}

/**
 *
 */
void Estimables::LoadValues(unsigned index) {
  /**
   * load our estimables if they haven't been loaded already
   */
  if (estimables_.size() == 0) {
    for (auto iter : estimable_values_) {
      Double* ptr = objects::FindEstimable(iter.first);
      if (ptr == 0)
        LOG_FATAL() << "The estimable " << iter.first << " was defined in the estimable's input value file but was not registered in the model";
      estimables_[iter.first] = ptr;
    }

    /**
     * Verify that we're only using @estimate parameters if this has been defined
     */
    if (GlobalConfiguration::Instance()->get_force_estimable_values_file()) {
      vector<Estimate*> estimates = estimates::Manager::Instance().GetEnabled();
      for (auto estimate : estimates) {
        if (estimable_values_.find(estimate->parameter()) == estimable_values_.end())
          LOG_FATAL() << "The estimate " << estimate->parameter() << " has not been defined in the input file, even though force-estimates has been enabled";
      }

      if (estimates.size() != estimable_values_.size())
        LOG_FATAL() << "The estimate value file does not have the correct number of estimables defined. Expected " << estimates.size() << " but got " << estimable_values_.size();
    }
  }

  for (auto iter : estimables_) {
    if (index >= estimable_values_[iter.first].size())
      LOG_CODE_ERROR() << "index >= estimable_values_[iter.first].size()";
    (*estimables_[iter.first]) = estimable_values_[iter.first][index];
  }
}




} /* namespace niwa */
