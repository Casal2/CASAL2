/*
 * AddressableTransformation.cpp
 *
 *  Created on: Dec 7, 2015
 *      Author: Zaita
 */

#include "AddressableTransformation.h"

#include "../Utilities/String.h"
#include "../Utilities/To.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include "../Model/Model.h"
#include "../Model/Objects.h"
namespace niwa {
namespace utils = niwa::utilities;

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 */
AddressableTransformation::AddressableTransformation(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label for the transformation block", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of transformation", "");
  parameters_.Bind<string>(PARAM_PARAMETERS, &parameter_labels_, "The parameters used in the transformation", "");
  parameters_.Bind<bool>(PARAM_PRIOR_APPLIES_TO_RESTORED_PARAMETERS, &prior_applies_to_restored_parameters_, "If the prior applies to the parameters (true) with jacobian (if it exists) or prior applies to transformed_parameter (false) with no jacobian ", "", false);
}

/**
 * Validate
 * This checks addressables are usable
 * sets usage via the VerifyForAddressable()
 */
void AddressableTransformation::Validate() {
  parameters_.Populate(model_);
  // Print out parameters
  LOG_FINE() << "In " << label_ << " parameters supplied for transformation = " <<  parameter_labels_.size();
  for(auto param : parameter_labels_)
    LOG_FINE() << param;

  // Check parameters are valid addressable types
  string error = "";
  for(auto param : parameter_labels_) {
    if (!model_->objects().VerifyAddressableForUse(param, addressable::kEstimate, error)) {
      LOG_FATAL_P(PARAM_PARAMETERS) << "The parameter " << param << " could not be verified for use in an @parameter_tranformation block. Error: " << error;
    }
  }
  vector_and_u_map_indicies_.resize(parameter_labels_.size());
  string_map_indicies_.resize(parameter_labels_.size());


  // Because in Model::Managers::Validate() this class is validated at the end 
  // we can assume all objects are initialisaed in thier respective class and we can essentially
  // move teh build methods into the validate

  LOG_FINE();
  // bind our function pointer for the update function, original value and addressible pointer
  // bind our function pointer for the update function, original value and addressible pointer
  unsigned param_counter = 0;
  addressable::Type previous_addressable_type = addressable::kSingle;
  unsigned previous_indicies = 0;
  for(auto param : parameter_labels_) {
    string         new_parameter = param;

    auto           pair          = model_->objects().ExplodeParameterAndIndex(param);
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
    LOG_FINE() << "sizes = " << indexes.size() << " parameter = " << parameter << " index = " << index << " new param = " << new_parameter;
    addressable::Type addressable_type = model_->objects().GetAddressableType(new_parameter);
    LOG_FINE() << " addressable_type = " << addressable_type;

    // Check all parameters are the same type i.e. all maps, or all vectors or all scalars etc.
    // if they are maps or vectors they also need to have the same indicies i.e. 
    // parameter_labels process[Recruit_east].ycs_values process[Recruit_west].ycs_values // will work
    // parameter_labels process[Recruit_east].ycs_values{1993:2003} process[Recruit_west].ycs_values{1993:2003} // will work
    // parameter_labels process[Recruit_east].ycs_values{1993:2000} process[Recruit_west].ycs_values{1993:2003} // Wont' work, as different length indicies
    // We wont check if the indicies are the same so this will run but could be wrong
    // parameter_labels process[Recruit_east].ycs_values{1993:2003} process[Recruit_west].ycs_values{1983:1993} // will work as same length but different periods
    if(param_counter == 0) {
      previous_addressable_type = addressable_type;
      previous_indicies = indexes.size();
    } else {
      if(addressable_type != previous_addressable_type)
        LOG_ERROR_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " needs to be the same type as previous parameters. i.e. all need to be scalar, you can't mix vector values with scalars";
      if(previous_indicies != indexes.size()) {
        LOG_ERROR_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " is a map or vector and has different number of indicies to previous parameter, these need to be consistent.";
      }  
    }
    switch (addressable_type) {
      case addressable::kInvalid:
        LOG_ERROR_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " " << error;
        break;
      case addressable::kSingle:
        LOG_FINE() << "kSingle()";
        restore_function_ = &AddressableTransformation::set_single_values;
        addressables_.push_back(model_->objects().GetAddressable(new_parameter));
        init_values_.push_back(*addressables_[param_counter]);
        ++n_params_;
        break;
      case addressable::kVector:
        LOG_FINE() << "kVector()";
        if(indexes.size() == 1) {
          restore_function_    = &AddressableTransformation::set_single_values_for_multiple_vectors;
        } else {
          restore_function_    = &AddressableTransformation::set_vector_values;
        }
        addressable_vectors_.push_back(model_->objects().GetAddressableVector(new_parameter));
        for (string string_index : indexes) {
          unsigned u_index = 0;
          if (!utils::To<string, unsigned>(string_index, u_index))
            LOG_FATAL_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " could not be converted to an unsigned integer";
          if (u_index <= 0 || u_index > addressable_vectors_[param_counter]->size())
            LOG_FATAL_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " index not in range for this parameter, please check the input";
          vector_and_u_map_indicies_[param_counter].push_back(u_index);
          init_values_.push_back((*addressable_vectors_[param_counter])[u_index]);
          ++n_params_;
        }        
        break;
      case addressable::kUnsignedMap:
        LOG_FINE() << "kUnsignedMap()";
        if(indexes.size() == 1) {
          restore_function_    = &AddressableTransformation::set_single_values_for_multiple_maps;
        } else {
          restore_function_    = &AddressableTransformation::set_map_values;
        }
        addressable_maps_.push_back(model_->objects().GetAddressableUMap(new_parameter));
        for (string string_index : indexes) {
          unsigned u_index = 0;
          if (!utils::To<string, unsigned>(string_index, u_index))
            LOG_FATAL_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " could not be converted to an unsigned integer";
          if (addressable_maps_[param_counter]->find(u_index) == addressable_maps_[param_counter]->end())
            LOG_FATAL_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " could not find index " << string_index << " between {}";
          vector_and_u_map_indicies_[param_counter].push_back(u_index);
          init_values_.push_back((*addressable_maps_[param_counter])[u_index]);
          ++n_params_;
        }          
        break;
      case addressable::kStringMap:
        LOG_FINE() << "kStringMap()";
        if(indexes.size() == 1) {
          restore_function_    = &AddressableTransformation::set_single_values_for_multiple_string_maps;
        } else {
          restore_function_    = &AddressableTransformation::set_map_string_values;
        }   
        addressable_string_maps_.push_back(model_->objects().GetAddressableSMap(new_parameter));
        for (string string_index : indexes) {
          if (addressable_string_maps_[param_counter]->find(index) == addressable_string_maps_[param_counter]->end())
            LOG_FATAL_P(PARAM_PARAMETERS) << "parameter " << parameter_labels_[param_counter] << " could not find index " << string_index << " between {}";
          string_map_indicies_[param_counter].push_back(string_index);
          init_values_.push_back((*addressable_string_maps_[param_counter])[string_index]);
          ++n_params_;
        }
        break;             
      default:
        LOG_FATAL_P(PARAM_PARAMETERS) << "The addressable " << parameter_labels_[param_counter] << " provided for use in the @parameter_tranformation block: " << label_ << " is not a parameter of a type that is supported";
        break;
    }
    // Get Target Object variable.
    // Scott: Do we need this? We already have pointers to specific objects above
    target_objects_.push_back(model_->objects().FindObject(new_parameter));
    ++param_counter;
  }

  DoValidate();
}

/**
 * Build relationships
 * get the pointers to our target values
 */
void AddressableTransformation::Build() {
  // Build child specific relationships
  DoBuild();
}

/**
 * Change the value of the addressable
 *
 * @param value The value to assign to the addressable
 */
void AddressableTransformation::set_single_values(vector<Double> values) {
  LOG_FINE(); 
  for(unsigned i = 0; i < values.size(); ++i)
    *addressables_[i] = values[i];
}

/**
 * Change a single value to mulitple maps
 *
 * @param values The value to assign to the addressable
 * assuemes each index of values relates to each index in parameter_label
 */
void AddressableTransformation::set_single_values_for_multiple_maps(vector<Double> values) {
  LOG_FINE(); 
  if(values.size() != addressable_maps_.size())
    LOG_CODE_ERROR() << "values.size() != addressable_maps_.size()";
  for(unsigned param_ndx = 0; param_ndx < values.size(); ++param_ndx) {
    if(vector_and_u_map_indicies_[param_ndx].size() != 1)
      LOG_CODE_ERROR() << "vector_and_u_map_indicies_[param_ndx].size() != 1";
    (*addressable_maps_[param_ndx])[vector_and_u_map_indicies_[param_ndx][0]] = values[param_ndx];
  }
}
/**
 * Change a single value to string maps
 *
 * @param values The value to assign to the addressable. Assumes each index of values relates to each index in parameter_label
 */
void AddressableTransformation::set_single_values_for_multiple_string_maps(vector<Double> values) {
  LOG_FINE(); 
  if(values.size() != addressable_string_maps_.size())
    LOG_CODE_ERROR() << "values.size() != addressable_string_maps_.size()";
  for(unsigned param_ndx = 0; param_ndx < values.size(); ++param_ndx) {
    if(string_map_indicies_[param_ndx].size() != 1)
      LOG_CODE_ERROR() << "string_map_indicies_[param_ndx].size() != 1";
    (*addressable_string_maps_[param_ndx])[string_map_indicies_[param_ndx][0]] = values[param_ndx];
  }
}
/**
 * Change a single value to mulitple vectors
 *
 * @param values The value to assign to the addressable. Assumes each index of values relates to each index in parameter_label
 */
void AddressableTransformation::set_single_values_for_multiple_vectors(vector<Double> values) {
  LOG_FINE(); 
  if(values.size() != addressable_vectors_.size())
    LOG_CODE_ERROR() << "values.size() != addressable_vectors_.size()";
  for(unsigned param_ndx = 0; param_ndx < values.size(); ++param_ndx) {
    if(vector_and_u_map_indicies_[param_ndx].size() != 1)
      LOG_CODE_ERROR() << "vector_and_u_map_indicies_[param_ndx].size() != 1";
    (*addressable_vectors_[param_ndx])[vector_and_u_map_indicies_[param_ndx][0]] = values[param_ndx];
  }
}

/**
 * Change the values for all or a subset of a single addressable vector
 *
 * @param value The value to add to the addressable vector
 */
void AddressableTransformation::set_vector_values(vector<Double> values) {
  if(addressable_vectors_.size() > 1)
    LOG_CODE_ERROR() << "addressable_vector_.size() > 1";
  for(unsigned i = 0; i < values.size(); ++i)
    (*addressable_vectors_[0])[vector_and_u_map_indicies_[0][i]] = values[i];
}

/**
 * Change the values for all or a subset of a single addressable Umap
 *
 * @param values The value to add to the addressable Umap
 */
void AddressableTransformation::set_map_values(vector<Double> values) {
  if(addressable_maps_.size() > 1)
    LOG_CODE_ERROR() << "addressable_maps_.size() > 1";
  for(unsigned i = 0; i < values.size(); ++i)
    (*addressable_maps_[0])[vector_and_u_map_indicies_[0][i]] = values[i];
}
/**
 * Change the values for all or a subset of a single addressable string map
 *
 * @param values The value to add to the addressable string map
 */
void AddressableTransformation::set_map_string_values(vector<Double> values) {
  if(addressable_string_maps_.size() > 1)
    LOG_CODE_ERROR() << "addressable_string_maps_.size() > 1";
  for(unsigned i = 0; i < values.size(); ++i)
    (*addressable_string_maps_[0])[string_map_indicies_[0][i]] = values[i];
}

/**
 * This method Will take an updated transformed parameters
 * and back transform it into model space and update the target addressables
 */
void AddressableTransformation::Restore() {
  LOG_TRACE();
  DoRestore();
}

}  // namespace niwa
/* namespace niwa */
