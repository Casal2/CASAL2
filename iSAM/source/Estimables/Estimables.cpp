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

#include "Utilities/Logging/Logging.h"

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
shared_ptr<Estimables> Estimables::Instance() {
  static EstimablesPtr instance = EstimablesPtr(new Estimables());
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

} /* namespace niwa */
