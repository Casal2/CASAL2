/**
 * @file Estimables.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Estimables.h"

#include "../Estimates/Manager.h"
#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Logging/Logging.h"
#include "../Model/Model.h"
#include "../Model/Objects.h"

// namespaces
namespace niwa {

/**
 * Append a value to an estimable
 *
 * @param estimable_label The label of the estimable
 * @param value The value
 */
void Estimables::AddValue(const string& estimable_label, Double value) {
  estimable_values_[estimable_label].push_back(value);
}

/**
 * Return a vector of estimable labels
 *
 * @return a vector of estimable labels
 */
vector<string> Estimables::GetEstimables() const {
  vector<string> result;
  for (auto iter : estimable_values_) result.push_back(iter.first);

  return result;
}

/**
 * Return the number of estimable values
 *
 * @return the number of estimable values, or 0 if there are none
 */
unsigned Estimables::GetValueCount() const {
  if (estimable_values_.size() == 0)
    return 0;

  auto iter = estimable_values_.begin();
  return iter->second.size();
}
/**
 * Return a map of estimable values for a specified index
 *
 * @param index The index
 * @return the map of estimable values, or an empty map if there are none
 */
map<string, Double> Estimables::GetValues(unsigned index) const {
  map<string, Double> result;
  for (auto iter : estimable_values_) result[iter.first] = iter.second[index];

  return result;
}

/**
 * Load values into an estimable map for a specified index
 *
 * @param index The index
 */
void Estimables::LoadValues(unsigned index) {
  /**
   * load our estimables if they haven't been loaded already
   */
  if (estimables_.size() == 0) {
    string error = "";
    for (auto iter : estimable_values_) {
      if (!model_->objects().VerifyAddressableForUse(iter.first, addressable::kInputRun, error)) {
        LOG_FATAL() << "The addressable " << iter.first << " could not be verified for use in -i run. Error: " << error;
      }
      Double* ptr             = model_->objects().GetAddressable(iter.first);
      estimables_[iter.first] = ptr;
    }
  }

  /**
   * Verify that we're only using @estimate parameters if this has been defined
   */
  if (!model_->global_configuration().force_overwrite_of_addressables()) {
    vector<Estimate*> estimates = model_->managers()->estimate()->GetIsEstimated();
    for (auto estimate : estimates) {
      if (estimable_values_.find(estimate->parameter()) == estimable_values_.end())
        LOG_FATAL() << "The estimate " << estimate->parameter() << " has not been found in the input configuration file";
    }
    if (estimates.size() != estimable_values_.size())
      LOG_FATAL() << "The estimate value file does not have the correct number of estimables defined. Expected " << estimates.size() << ", parsed " << estimable_values_.size();
  }

  for (auto iter : estimables_) {
    if (index >= estimable_values_[iter.first].size())
      LOG_CODE_ERROR() << "index >= estimable_values_[iter.first].size()";
    (*estimables_[iter.first]) = estimable_values_[iter.first][index];

    auto estimate = model_->managers()->estimate()->GetEstimate(iter.first);
    if (estimate != nullptr)
      estimate->set_value(estimable_values_[iter.first][index]);
  }

  LOG_INFO() << "Estimable parameters were loaded from the free parameter file: " << model_->global_configuration().estimable_value_file();

  if (model_->global_configuration().force_overwrite_of_addressables()) {
    int AdditionalAddressables = estimable_values_.size() - (model_->managers()->estimate()->GetIsEstimated()).size();
    LOG_IMPORTANT() << AdditionalAddressables
                    << " additional non-estimated addressable parameters were found in the free parameter file: " << model_->global_configuration().estimable_value_file();
  }
}

} /* namespace niwa */
