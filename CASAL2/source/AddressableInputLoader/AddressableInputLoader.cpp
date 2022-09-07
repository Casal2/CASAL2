/**
 * @file AddressableInputLoader.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "AddressableInputLoader.h"

#include "../Estimates/Manager.h"
#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Logging/Logging.h"
#include "../Model/Model.h"
#include "../Model/Objects.h"

// namespaces
namespace niwa {


void AddressableInputLoader::Validate() {
  LOG_INFO() << "Estimable parameters were loaded from the free parameter file: " << model_->global_configuration().get_free_parameter_input_file();
}

/**
 * Append a value to an addressable. Addressables have a list of
 * values that will be iterated through. One value is loaded
 * per model iteration.
 *
 * @param addressable_label The label of the addressable
 * @param value The value
 */
void AddressableInputLoader::AddValue(const string& addressable_label, Double value) {
  LOG_FINE() << "AddValue Addressable " << addressable_label;
  addressable_values_[addressable_label].push_back(value);
}

/**
 * Return a vector of estimable labels
 *
 * @return a vector of estimable labels
 */
vector<string> AddressableInputLoader::GetAddressableLabels() const {
  vector<string> result;
  for (auto iter : addressable_values_) result.push_back(iter.first);

  return result;
}

/**
 * Return the number of addressable values thay have been loaded.
 * All addressables will have the same number of values loaded, so
 * we can just return the size of the vector on the first addressable.
 *
 * @return the number of estimable values, or 0 if there are none
 */
unsigned AddressableInputLoader::GetValueCount() const {
  if (addressable_values_.size() == 0)
    return 0;

  auto iter = addressable_values_.begin();
  return iter->second.size();
}
/**
 * Return a map of addressable values for a specified index. The index
 * is the nth value loaded for those addressables. When looking at the
 * input file, this is the nth line of values provided.
 *
 * @param index The index
 * @return the map of addressable values, or an empty map if there are none
 */
map<string, Double> AddressableInputLoader::GetValues(unsigned index) const {
  map<string, Double> result;
  for (auto iter : addressable_values_) result[iter.first] = iter.second[index];

  return result;
}

/**
 * Load the nth (index) set of values into the addressables so that they can be
 * used in the current model iteration.
 *
 * @param index The index
 */
void AddressableInputLoader::LoadValues(unsigned index) {
  /**
   * load our estimables if they haven't been loaded already
   */
  if (addressables_.size() == 0) {
    string error = "";
    for (auto iter : addressable_values_) {
      if (!model_->objects().VerifyAddressableForUse(iter.first, addressable::kInputRun, error)) {
        LOG_FATAL() << "The addressable " << iter.first << " could not be verified for use in -i run. Error: " << error;
      }
      Double* ptr               = model_->objects().GetAddressable(iter.first);
      addressables_[iter.first] = ptr;
    }
  }

  /**
   * Verify that we're only using @estimate parameters if this has been defined
   */
  if (!model_->global_configuration().force_overwrite_of_addressables()) {
    vector<Estimate*> estimates = model_->managers()->estimate()->GetIsEstimated();
    for (auto estimate : estimates) {
      if (addressable_values_.find(estimate->parameter()) == addressable_values_.end())
        LOG_FATAL() << "The estimate " << estimate->parameter() << " was not found in the input configuration file";
    }
    if (estimates.size() != addressable_values_.size())
      LOG_FATAL() << "The free parameters file does not have the correct number of estimables defined. Expected " << estimates.size() << ", parsed " << addressable_values_.size();
  }

  unsigned estimate_count = 0;
  for (auto iter : addressables_) {
    if (index >= addressable_values_[iter.first].size())
      LOG_CODE_ERROR() << "index >= addressable_values_[iter.first].size()";
    *iter.second = addressable_values_[iter.first][index];

    // Update estimate with new value if it exists
    // so that we can automatically update the 'same' targets
    auto estimate = model_->managers()->estimate()->GetEstimate(iter.first);
    if (estimate != nullptr) {
      estimate->set_value(*iter.second);
      estimate->set_initial_value(*iter.second);
      estimate->flag_value_has_been_initialised();
      ++estimate_count;
    }
  }
  

  if (model_->global_configuration().force_overwrite_of_addressables()) {
    int AdditionalAddressables = addressable_values_.size() - estimate_count;
    LOG_IMPORTANT() << AdditionalAddressables
                    << " additional non-estimated addressable parameters were found in the free parameter file: " << model_->global_configuration().get_free_parameter_input_file();
  }
}

} /* namespace niwa */
