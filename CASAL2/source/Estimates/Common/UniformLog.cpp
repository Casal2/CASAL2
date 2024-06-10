/**
 * @file UniformLog.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "UniformLog.h"

// Namespaces
namespace niwa {
namespace estimates {

/**
 * Default Constructor
 */
UniformLog::UniformLog(shared_ptr<Model> model) : Estimate(model) {}

/**
 * Calculate and return the score
 *
 * @return Score as log(param)
 */
Double UniformLog::GetScore() {
  if(value() <= 0.0)
    LOG_FATAL() << "value for " << label_ << " was less than zero. The uniform log is only suitable for strictly positive parameters.";
  Double score_ = log(value());
  return score_;
}
/**
 * GetPriorValues()
 *
 * @return empty vector no prior parameters for this estimate type
 */
vector<Double>   UniformLog::GetPriorValues() {
  vector<Double> result = {0};
  return result;
}
vector<string>   UniformLog::GetPriorLabels() {
  vector<string> result = {"none"};
  return result;
}

} /* namespace estimates */
} /* namespace niwa */
