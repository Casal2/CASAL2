/**
 * @file Creator.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 22/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Creator.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Estimates/Manager.h"
#include "Estimates/Factory.h"
#include "ObjectsFinder/ObjectsFinder.h"
#include "Utilities/String.h"
#include "Utilities/To.h"

// namespaces
namespace isam {
namespace estimates {

namespace utils = isam::utilities;
using isam::utilities::Map;

/**
 *
 */
Creator::Creator() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "", "", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "", "", "");
  parameters_.Bind<double>(PARAM_LOWER_BOUND, &lower_bounds_, "", "");
  parameters_.Bind<double>(PARAM_UPPER_BOUND, &upper_bounds_, "", "");
  parameters_.Bind<string>(PARAM_PRIOR, &prior_label_, "", "", "");
  parameters_.Bind<string>(PARAM_SAME, &same_labels_, "", "", "");
  parameters_.Bind<string>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "", "", "");
  parameters_.Bind<string>(PARAM_MCMC, &mcmc_, "", "", "");

  update_label_ = false;
  update_parameter_ = false;
}

/**
 * Create the estimates that were defined by the @estimate block that makes up this creator
 */
void Creator::CreateEstimates() {
  parameters_.Populate();
  type_ = utilities::ToLowercase(type_);

  if (label_ != "" && parameter_ == "") {
    parameter_ = label_;
    update_parameter_ = true;
  } else if (label_ == "" && parameter_ != "") {
    label_ = parameter_;
    update_label_ = true;
  } else if (label_ == "" && parameter_ == "") {
    LOG_ERROR(parameters_.location(PARAM_LABEL) << " or parameter have not been defined. Please ensure you define at least one to indicate the target estimable");
  } else {
    update_parameter_ = true;
  }

  /**
   * At this point we need to determine if we need to split this estimate in to multiple estimates.
   */
  base::ObjectPtr target = objects::FindObject(parameter_);
  if (!target)
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_ << " is not a valid estimable in the system");

  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";
  objects::ExplodeString(parameter_, type, label, parameter, index);
  string new_label = type + "[" + label + "]." + parameter;

  vector<string> indexes;
  if (index != "") {
    indexes = utilities::strings::explode(index);
    if (index != "" && indexes.size() == 0) {
      LOG_ERROR(parameters_.location(PARAM_PARAMETER) << " could be split up to search for indexes because the format was invalid. "
          << "Please ensure you are using correct indexes and only the operators , and : (range) are supported");
    }
  }

  if (indexes.size() == 1 || target->GetEstimableType(parameter) == Estimable::kSingle) {
    /**
     * This estimate is only for a single object. So we will validate based on that
     */
    if (lower_bounds_.size() != 1)
      LOG_ERROR(parameters_.location(PARAM_LOWER_BOUND) << " values specified (" << lower_bounds_.size() << " must match number of target estimables (1)");
    if (upper_bounds_.size() != 1)
      LOG_ERROR(parameters_.location(PARAM_UPPER_BOUND) << " values specified (" << upper_bounds_.size() << " must match number of target estimables (1)");

    update_parameter_ = false;
    EstimatePtr estimate = CreateEstimate(label_, 0, target->GetEstimable(parameter));

  } else if (indexes.size() != 0) {
    /**
     * Here we have a specified number of estimates we want to reference. So we'll find them in the target object
     * and create new estimates for each of these.
     */
    if (lower_bounds_.size() != indexes.size())
      LOG_ERROR(parameters_.location(PARAM_LOWER_BOUND) << " values specified (" << lower_bounds_.size() << " must match number of target estimables (" << indexes.size() << ")");
    if (upper_bounds_.size() != indexes.size())
      LOG_ERROR(parameters_.location(PARAM_UPPER_BOUND) << " values specified (" << upper_bounds_.size() << " must match number of target estimables (" << indexes.size() << ")");
    if (same_labels_.size() != 0 && same_labels_.size() != indexes.size())
      LOG_ERROR(parameters_.location(PARAM_SAME) << " values specified (" << same_labels_.size() << " must match number of target estimables (" << indexes.size() << ")");

    switch(target->GetEstimableType(parameter)) {
    case Estimable::kVector:
    {
      vector<Double>* targets = target->GetEstimableVector(parameter);

      for (string string_index : indexes) {
        unsigned offset = 0;
        if (!utils::To<string, unsigned>(string_index, offset))
          LOG_ERROR(parameters_.location(PARAM_PARAMETER) << " index " << string_index << " could not be converted to a numeric type");
        if (offset >= targets->size())
          LOG_ERROR(parameters_.location(PARAM_PARAMETER) << " index " << string_index << " exceeds the amount of objects registered (" << targets->size() << ")");

        CreateEstimate(new_label + "(" + string_index + ")", offset, &(*targets)[offset]);
      }
    }
    break;

    case Estimable::kUnsignedMap:
    {
      map<unsigned, Double>* targets = target->GetEstimableUMap(parameter);
      unsigned offset = 0;
      for (string string_index : indexes) {
        unsigned u_index = 0;
        if (!utils::To<string, unsigned>(string_index, u_index))
          LOG_ERROR(parameters_.location(PARAM_PARAMETER) << " index " << string_index << " could not be converted to a numeric type");
        if (targets->find(u_index) == targets->end())
          LOG_ERROR(parameters_.location(PARAM_PARAMETER) << " value " << string_index << " could not be found in the objects registered");

        CreateEstimate(new_label + "(" + string_index + ")", offset, &(*targets)[u_index]);
        offset++;
      }
    }
    break;

    case Estimable::kStringMap:
    {
      Map<string, Double>* targets = target->GetEstimableSMap(parameter);
      unsigned offset = 0;
      for (string index : indexes) {
        if (targets->find(index) == targets->end())
          LOG_ERROR(parameters_.location(PARAM_PARAMETER) << " value " << index << " could not be found in the objects registered");

        CreateEstimate(new_label + "(" + index + ")", offset, &(*targets)[index]);
        offset++;
      }
    }
    break;

    default:
      LOG_CODE_ERROR("This type of estimable is not supported: " << (unsigned)target->GetEstimableType(parameter));
      break;
    }
  } else {
    /**
     * Here we need to handle when a user defines an entire container as the target for an estimate.
     * We'll estimate every element separately.
     */
    unsigned n = indexes.size();
    if (n == 0)
      n = target->GetEstimableSize(parameter);

    if (lower_bounds_.size() != n)
      LOG_ERROR(parameters_.location(PARAM_LOWER_BOUND) << " values specified (" << lower_bounds_.size() << " must match number of target estimables (" << n << ")");
    if (upper_bounds_.size() != n)
      LOG_ERROR(parameters_.location(PARAM_UPPER_BOUND) << " values specified (" << upper_bounds_.size() << " must match number of target estimables (" << n << ")");

    // force label change so we can actually print the indexes of each estimate
    // in the objective function
    update_label_ = true;

    switch(target->GetEstimableType(parameter)) {
    case Estimable::kVector:
    {
      vector<Double>* targets = target->GetEstimableVector(parameter);
      for (unsigned i = 0; i < targets->size(); ++i)
        CreateEstimate(new_label + "(" + utilities::ToInline<unsigned, string>(i + 1) + ")", i, &(*targets)[i]);

      break;
    }
    case Estimable::kUnsignedMap:
    {
      map<unsigned, Double>* targets = target->GetEstimableUMap(parameter);
      unsigned offset = 0;
      for (auto iter : (*targets)) {
        CreateEstimate(new_label + "(" + utilities::ToInline<unsigned, string>(iter.first) + ")", offset, &(*targets)[iter.first]);
        offset++;
      }
      break;
    }
    case Estimable::kStringMap:
    {
      Map<string, Double>* targets = target->GetEstimableSMap(parameter);
      unsigned offset = 0;
      for (auto iter : (*targets)) {
        CreateEstimate(new_label + "(" + iter.first + ")", offset, &(*targets)[iter.first]);
        offset++;
      }
      break;
    }
    default:
      LOG_CODE_ERROR("This type of estimable is not supported: " << (unsigned)target->GetEstimableType(parameter));
      break;
    }
  }
}

/**
 * Create an instance of an estimate
 */
isam::EstimatePtr Creator::CreateEstimate(string label, unsigned index, Double* target) {
  isam::EstimatePtr estimate = estimates::Factory::Create(block_type_, type_);
  if (!estimate)
    LOG_ERROR(parameters_.location(PARAM_TYPE) << " " << type_ << " is invalid when creating an estimate.");

  CopyParameters(estimate, index);
  estimate->set_target(target);
  if (update_label_)
    estimate->parameters().Get(PARAM_LABEL)->set_value(label);
  if (update_parameter_)
    estimate->parameters().Get(PARAM_PARAMETER)->set_value(label);
  return estimate;
}

/**
 *
 */
void Creator::CopyParameters(isam::EstimatePtr estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_LABEL);
  estimate->parameters().CopyFrom(parameters_, PARAM_TYPE);
  estimate->parameters().CopyFrom(parameters_, PARAM_PARAMETER);
  estimate->parameters().CopyFrom(parameters_, PARAM_PRIOR);
  estimate->parameters().CopyFrom(parameters_, PARAM_ESTIMATION_PHASE);
  estimate->parameters().CopyFrom(parameters_, PARAM_MCMC);

  estimate->parameters().CopyFrom(parameters_, PARAM_LOWER_BOUND, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_UPPER_BOUND, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_SAME, index);

  DoCopyParameters(estimate, index);
}
} /* namespace estimates */
} /* namespace isam */

