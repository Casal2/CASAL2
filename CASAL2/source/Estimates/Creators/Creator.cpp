/**
 * @file Creator.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 22/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Creator.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../../Estimates/Factory.h"
#include "../../Estimates/Manager.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Utilities/String.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace estimates {

namespace utils = niwa::utilities;

/**
 *
 */
Creator::Creator(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "TThe label of the estimate", "", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The prior type for the estimate", "");
  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The name of the variable to estimate in the model", "");
  parameters_.Bind<double>(PARAM_LOWER_BOUND, &lower_bounds_, "The lower bound for the parameter", "");
  parameters_.Bind<double>(PARAM_UPPER_BOUND, &upper_bounds_, "The upper bound for the parameter", "");
  //  parameters_.Bind<string>(PARAM_PRIOR, &prior_label_, "TBA", "", "");
  parameters_.Bind<string>(PARAM_SAME, &same_labels_, "The list of parameters that are constrained to have the same value as this parameter", "", "");
  parameters_.Bind<string>(PARAM_ESTIMATION_PHASE, &estimation_phase_, "The estimation phase", "", "");
  parameters_.Bind<string>(PARAM_MCMC, &mcmc_, "Indicates if this parameter is fixed at the point estimate during an MCMC run", "", "");
}

/**
 * Create the estimates that were defined by the @estimate block that makes up this creator
 */
void Creator::CreateEstimates() {
  LOG_TRACE();
  type_ = utilities::ToLowercase(type_);

  /**
   * At this point we need to determine if we need to split this estimate in to multiple estimates.
   */
  string error = "";
  if (!model_->objects().VerifyAddressableForUse(parameter_, addressable::kEstimate, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << " could not be verified for use in an @estimate block. Error: " << error;
  }

  string         new_parameter = parameter_;
  auto           pair          = model_->objects().ExplodeParameterAndIndex(parameter_);
  string         parameter     = pair.first;
  string         index         = pair.second;
  vector<string> indexes;
  if (index != "") {
    indexes = utilities::String::explode(index);
    if (index != "" && indexes.size() == 0) {
      LOG_FATAL_P(PARAM_PARAMETER) << " could not be split up to search for indexes because the format was invalid. "
                                   << "Check the indices. Only the operators ',' and ':' (range) are supported";
    }

    new_parameter = new_parameter.substr(0, new_parameter.find('{'));
  }

  auto target = model_->objects().FindObject(parameter_);
  // set estimated flag
  target->set_estimated(true);
  if (target->GetAddressableType(parameter) == addressable::kSingle) {
    /**
     * This estimate is only for a single object. So we will validate based on that
     */
    if (lower_bounds_.size() != 1)
      LOG_FATAL_P(PARAM_LOWER_BOUND) << "values specified (" << lower_bounds_.size() << " must match number of target addressables (1)";
    if (upper_bounds_.size() != 1)
      LOG_FATAL_P(PARAM_UPPER_BOUND) << "values specified (" << upper_bounds_.size() << " must match number of target addressables (1)";

    CreateEstimate(parameter_, 0, target->GetAddressable(parameter));

  } else if (indexes.size() != 0) {
    /**
     * Here we have a specified number of estimates we want to reference. So we'll find them in the target object
     * and create new estimates for each of these.
     */
    if (lower_bounds_.size() != indexes.size())
      LOG_FATAL_P(PARAM_LOWER_BOUND) << "values specified (" << lower_bounds_.size() << " must match number of target addressables (" << indexes.size() << ")";
    if (upper_bounds_.size() != indexes.size())
      LOG_FATAL_P(PARAM_UPPER_BOUND) << "values specified (" << upper_bounds_.size() << " must match number of target addressables (" << indexes.size() << ")";

    switch (target->GetAddressableType(parameter)) {
      case addressable::kVector: {
        vector<Double>* targets = target->GetAddressableVector(parameter);

        unsigned offset = 0;
        for (string string_index : indexes) {
          unsigned u_index = 0;
          if (!utils::To<string, unsigned>(string_index, u_index))
            LOG_FATAL_P(PARAM_PARAMETER) << "index " << string_index << " could not be converted to an unsigned integer";
          if (u_index <= 0 || u_index > targets->size())
            LOG_FATAL_P(PARAM_PARAMETER) << "index " << string_index << " is out of range 1-" << targets->size();

          CreateEstimate(new_parameter + "{" + string_index + "}", offset, &(*targets)[u_index - 1]);
          offset++;
        }
      } break;
      case addressable::kUnsignedMap: {
        bool                   create_missing = false;
        map<unsigned, Double>* targets        = target->GetAddressableUMap(parameter, create_missing);
        unsigned               offset         = 0;
        for (string string_index : indexes) {
          unsigned u_index = 0;
          if (!utils::To<string, unsigned>(string_index, u_index))
            LOG_FATAL_P(PARAM_PARAMETER) << "index " << string_index << " could not be converted to an unsigned integer";
          if (targets->find(u_index) == targets->end() && !create_missing)
            LOG_FATAL_P(PARAM_PARAMETER) << "value " << string_index << " was not found in the objects registered";
          if (targets->find(u_index) == targets->end() && create_missing)
            (*targets)[u_index] = lower_bounds_[offset];

          CreateEstimate(new_parameter + "{" + string_index + "}", offset, &(*targets)[u_index]);
          offset++;
        }
      } break;
      case addressable::kStringMap: {
        utils::OrderedMap<string, Double>* targets = target->GetAddressableSMap(parameter);
        unsigned                           offset  = 0;
        for (string index : indexes) {
          if (targets->find(index) == targets->end())
            LOG_FATAL_P(PARAM_PARAMETER) << "value " << index << " was not found in the objects registered";

          CreateEstimate(new_parameter + "{" + index + "}", offset, &(*targets)[index]);
          offset++;
        }
      } break;
      default:
        LOG_CODE_ERROR() << "This type of addressable is not supported: " << (unsigned)target->GetAddressableType(parameter);
        break;
    }
  } else {
    /**
     * Here we need to handle when a user defines an entire container as the target for an estimate.
     * We'll estimate every element separately.
     */
    unsigned n = indexes.size();
    if (n == 0)
      n = target->GetAddressableSize(parameter);

    if (lower_bounds_.size() != n)
      LOG_FATAL_P(PARAM_LOWER_BOUND) << "values specified (" << lower_bounds_.size() << " must match number of target addressables (" << n << ")";
    if (upper_bounds_.size() != n)
      LOG_FATAL_P(PARAM_UPPER_BOUND) << "values specified (" << upper_bounds_.size() << " must match number of target addressables (" << n << ")";

    switch (target->GetAddressableType(parameter)) {
      case addressable::kVector:
      case addressable::kVectorStringMap: {
        vector<Double>* targets = target->GetAddressableVector(parameter);
        for (unsigned i = 0; i < targets->size(); ++i) CreateEstimate(new_parameter + "{" + utilities::ToInline<unsigned, string>(i + 1) + "}", i, &(*targets)[i]);

        break;
      }
      case addressable::kUnsignedMap: {
        map<unsigned, Double>* targets = target->GetAddressableUMap(parameter);
        unsigned               offset  = 0;
        for (auto iter : (*targets)) {
          CreateEstimate(new_parameter + "{" + utilities::ToInline<unsigned, string>(iter.first) + "}", offset, &(*targets)[iter.first]);
          offset++;
        }
        break;
      }
      case addressable::kStringMap: {
        utils::OrderedMap<string, Double>* targets = target->GetAddressableSMap(parameter);
        unsigned                           offset  = 0;
        for (auto iter : (*targets)) {
          CreateEstimate(new_parameter + "{" + iter.first + "}", offset, &(*targets)[iter.first]);
          offset++;
        }
        break;
      }
      default:
        LOG_CODE_ERROR() << "This type of addressable is not supported: " << (unsigned)target->GetAddressableType(parameter);
        break;
    }
  }

  HandleSameParameter();
}

/**
 * This method is responsible for handling the same parameter on the @estimate
 * block.
 *
 * Sames are objects added to the estimate that will be modified when the estimate
 * is modified. The code below is quite complex as it needs to do all of the
 * label expanding the code above does
 *
 * Also checks for duplicate sames etc.
 */
void Creator::HandleSameParameter() {
  if (!parameters_.Get(PARAM_SAME)->has_been_defined())
    return;

  vector<string>        labels;
  vector<Double*>       targets;
  map<string, unsigned> same_count;

  auto sames = parameters_.Get(PARAM_SAME)->values();
  for (auto same : sames) {
    /**
     * At this point we need to determine if we need to split this estimate in to multiple estimates.
     */
    string error = "";
    if (!model_->objects().VerifyAddressableForUse(same, addressable::kEstimate, error)) {
      LOG_FATAL_P(PARAM_SAME) << "could not be verified for use in an @estimate.same block. Error: " << error;
    }

    string new_parameter = same;
    auto   pair          = model_->objects().ExplodeParameterAndIndex(same);
    string parameter     = pair.first;
    string index         = pair.second;
    LOG_FINEST() << "same: " << same << "; new_parameter: " << new_parameter;

    vector<string> indexes;
    if (index != "") {
      indexes = utilities::String::explode(index);
      if (index != "" && indexes.size() == 0) {
        LOG_FATAL_P(PARAM_SAME) << " could not be split up to search for indexes because the format was invalid. "
                                << "Check the indices. Only the operators ',' and ':' (range) are supported";
      }

      new_parameter = new_parameter.substr(0, new_parameter.find('{'));
    }

    auto target = model_->objects().FindObject(same);
    target->set_estimated(true);
    if (target->GetAddressableType(parameter) == addressable::kSingle) {
      /**
       * Handle when our sames are referencing a single object
       */
      labels.push_back(same);
      targets.push_back(target->GetAddressable(parameter));

    } else if (indexes.size() != 0) {
      /**
       * Handle sames that are using index values
       */
      switch (target->GetAddressableType(parameter)) {
        case addressable::kVector: {
          vector<Double>* temp   = target->GetAddressableVector(parameter);
          unsigned        offset = 0;
          for (string string_index : indexes) {
            unsigned u_index = 0;
            if (!utils::To<string, unsigned>(string_index, u_index))
              LOG_FATAL_P(PARAM_SAME) << "index " << string_index << " could not be converted to an unsigned integer";
            if (u_index <= 0 || u_index > temp->size())
              LOG_FATAL_P(PARAM_SAME) << "index " << string_index << " is out of range 1-" << temp->size();

            labels.push_back(new_parameter + "{" + string_index + "}");
            targets.push_back(&(*temp)[u_index - 1]);
            offset++;
          }
        } break;
        case addressable::kUnsignedMap: {
          bool                   create_missing = false;
          map<unsigned, Double>* temps          = target->GetAddressableUMap(parameter, create_missing);
          unsigned               offset         = 0;
          for (string string_index : indexes) {
            unsigned u_index = 0;
            if (!utils::To<string, unsigned>(string_index, u_index))
              LOG_FATAL_P(PARAM_PARAMETER) << "index " << string_index << " could not be converted to an unsigned integer";
            if (temps->find(u_index) == temps->end() && !create_missing)
              LOG_FATAL_P(PARAM_PARAMETER) << "value " << string_index << " was not found in the objects registered";
            if (temps->find(u_index) == temps->end() && create_missing)
              (*temps)[u_index] = lower_bounds_[offset];

            labels.push_back(new_parameter + "{" + string_index + "}");
            targets.push_back(&(*temps)[u_index]);
            offset++;
          }
        } break;
        case addressable::kStringMap: {
          utils::OrderedMap<string, Double>* temp   = target->GetAddressableSMap(parameter);
          unsigned                           offset = 0;
          for (string index : indexes) {
            if (temp->find(index) == temp->end())
              LOG_FATAL_P(PARAM_PARAMETER) << "value " << index << " was not found in the objects registered";

            labels.push_back(new_parameter + "{" + index + "}");
            targets.push_back(&(*temp)[index]);
            offset++;
          }
        } break;
        default:
          LOG_CODE_ERROR() << "This type of addressable is not supported: " << (unsigned)target->GetAddressableType(parameter);
          break;
      }
    } else {
      /**
       * Here we need to handle when a user defines an entire container as the target for a same.
       * We'll same every element separately.
       */
      switch (target->GetAddressableType(parameter)) {
        case addressable::kVector:
        case addressable::kVectorStringMap: {
          vector<Double>* temps = target->GetAddressableVector(parameter);
          for (unsigned i = 0; i < temps->size(); ++i) {
            labels.push_back(new_parameter + "{" + utilities::ToInline<unsigned, string>(i + 1) + "}");
            targets.push_back(&(*temps)[i]);
          }

          break;
        }
        case addressable::kUnsignedMap: {
          map<unsigned, Double>* temps  = target->GetAddressableUMap(parameter);
          unsigned               offset = 0;
          for (auto iter : (*temps)) {
            labels.push_back(new_parameter + "{" + utilities::ToInline<unsigned, string>(iter.first) + "}");
            targets.push_back(&(*temps)[iter.first]);
            offset++;
          }
          break;
        }
        case addressable::kStringMap: {
          utils::OrderedMap<string, Double>* temps  = target->GetAddressableSMap(parameter);
          unsigned                           offset = 0;
          for (auto iter : (*temps)) {
            labels.push_back(new_parameter + "{" + iter.first + "}");
            targets.push_back(&(*temps)[iter.first]);
            offset++;
          }
          break;
        }
        default:
          LOG_CODE_ERROR() << "This type of addressable is not supported: " << (unsigned)target->GetAddressableType(parameter);
          break;
      }
    }
  }

  /**
   * Do checks against the number of estimates to ensure it's compat.
   */
  if (estimates_.size() == 1 && labels.size() == 0) {
    LOG_ERROR_P(PARAM_SAME) << " Did not create any objects within the system when there are " << estimates_.size() << " estimates";
  } else if (estimates_.size() != 1 && (estimates_.size() != labels.size())) {
    LOG_ERROR_P(PARAM_SAME) << " created " << labels.size() << " same parameters against " << estimates_.size() << " estimates. These must match";
  }

  /**
   * Check for Duplicates
   */
  for (string same : labels) {
    same_count[same]++;
    if (same_count[same] > 1) {
      LOG_ERROR_P(PARAM_SAME) << ": same parameter '" << same << "' is a duplicate.";
    }
  }

  /**
   * Assign Sames to Estimates
   */
  if (estimates_.size() == 1) {
    for (unsigned i = 0; i < labels.size(); ++i) {
      estimates_[0]->AddSame(labels[i], targets[i]);
    }
  } else {
    for (unsigned i = 0; i < estimates_.size(); ++i) {
      estimates_[i]->AddSame(labels[i], targets[i]);
    }
  }
}

/**
 * Create an instance of an estimate
 */
niwa::Estimate* Creator::CreateEstimate(string parameter, unsigned index, Double* target) {
  niwa::Estimate* estimate = estimates::Factory::Create(model_, block_type_, type_);
  if (!estimate)
    LOG_FATAL_P(PARAM_TYPE) << " " << type_ << " is invalid when creating an estimate.";

  CopyParameters(estimate, index);
  estimate->set_target(target);
  estimate->parameters().Get(PARAM_PARAMETER)->set_value(parameter);
  estimate->set_creator_parameter(parameter_);
  estimate->set_block_type(PARAM_ESTIMATE);

  estimate->parameters().Populate(model_);

  estimates_.push_back(estimate);
  return estimate;
}

/**
 * Copy parameters
 * @param estimate
 * @param index
 */
void Creator::CopyParameters(niwa::Estimate* estimate, unsigned index) {
  estimate->parameters().CopyFrom(parameters_, PARAM_LABEL);
  estimate->parameters().CopyFrom(parameters_, PARAM_TYPE);
  estimate->parameters().CopyFrom(parameters_, PARAM_PARAMETER);
  //  estimate->parameters().CopyFrom(parameters_, PARAM_PRIOR);
  estimate->parameters().CopyFrom(parameters_, PARAM_ESTIMATION_PHASE);
  estimate->parameters().CopyFrom(parameters_, PARAM_MCMC);
  estimate->parameters().CopyFrom(parameters_, PARAM_PRIOR_APPLIES_TO_TRANSFORM);

  estimate->parameters().CopyFrom(parameters_, PARAM_TRANSFORMATION, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_TRANSFORM_WITH_JACOBIAN, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_LOWER_BOUND, index);
  estimate->parameters().CopyFrom(parameters_, PARAM_UPPER_BOUND, index);

  DoCopyParameters(estimate, index);
}
} /* namespace estimates */
} /* namespace niwa */
