/*
 * Likelihood.cpp
 *
 *  Created on: 22/03/2013
 *      Author: Admin
 */

#include "Likelihood.h"

#include "Observations/DataWeights/Factory.h"

namespace niwa {

/**
 *
 */
//Double Likelihood::GetInitialScore(const vector<string> &keys, const vector<Double> &process_errors, const vector<Double> &error_values) {
//  return 0;
//}

/**
 *
 */
//bool Likelihood::set_data_weight(string data_weight_type, Double data_weight_value) {
//  data_weight_type_ = data_weight_type;
//  data_weight_value_ = data_weight_value;
//
//  if (std::find(allowed_data_weight_types_.begin(), allowed_data_weight_types_.end(), data_weight_type_) == allowed_data_weight_types_.end())
//    return false;
//
//  data_weight_ = observations::dataweights::Factory::Create(data_weight_type_);
//  if (!data_weight_)
//    return false;
//
//  return data_weight_->set_weight_value(data_weight_value_);
//}

} /* namespace niwa */

