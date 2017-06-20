/**
 * @file Objects.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 1/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Objects.h"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include "Catchabilities/Manager.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Model/Managers.h"
#include "Processes/Manager.h"
#include "Selectivities/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "TimeVarying/Manager.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

namespace util = niwa::utilities;

/**
 * Default constructor
 */
Objects::Objects(Model* model) : model_(model) {
}

/**
 * This method will verify the existence of the addressable and it's usage flag. This check should
 * be done prior to any other call to this object to ensure the addressable can be found and used.
 *
 * @param parameter_absolute_name The absolute name for the addressable (e.g block[label].variable{index}
 * @param usage The intended usage for this addressable
 * @param error A string to hold any returning errors in
 * @return True if the verification was successful, false otherwise
 */
bool Objects::VerfiyAddressableForUse(const string& parameter_absolute_name, addressable::Usage usage, string& error) {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  ostringstream str;

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    str << "Syntax for " << parameter_absolute_name << " is invalid. Correct syntax is block[label].variable{index}";
    error = str.str();
    return false;
  }

  base::Object* object = this->FindObject(parameter_absolute_name);
  if (!object) {
    str << "Parent object for " << parameter_absolute_name << " is not valid. Please double check spelling";
    error = str.str();
    return false;
  }

  if (!object->HasAddressable(parameter)) {
    error = parameter + " is not a valid addressable on " + type + "." + label;
    return false;
  }

  if (!object->HasAddressableUsage(parameter, usage)) {
    error = parameter + " on " + type + "." + label + " cannot be used for this purpose due to usage restrictions";
    return false;
  }

  return true;
}


/**
 * This method will find the type of addressable in the system defined by the absolute
 * parameter name.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @return The addressable type, set to invalid if it's not found
 */
addressable::Type Objects::GetAddressableType(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  LOG_FINEST() << "parameter type = " << parameter_index.first << " index = " << parameter_index.second;
  if (parameter_index.second != "")
    return addressable::kSingle;

  return target->GetAddressableType(parameter_index.first);
}

/**
 * This method will search the model for the specific addressable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @return Pointer to the addressable
 */
Double* Objects::GetAddressable(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  if (parameter_index.second != "")
    return target->GetAddressable(parameter_index.first, parameter_index.second);

  return target->GetAddressable(parameter_index.first);
}

/**
 * This method will search the model for the specific addressable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @param error The variable to populate with the error message if one occurs
 * @return Pointer to the addressable or nullptr if none exists
 */
map<unsigned, Double>* Objects::GetAddressableUMap(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  return target->GetAddressableUMap(parameter_index.first);
}

/**
 * This method will search the model for the specific addressable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @return Pointer to the addressable
 */
OrderedMap<string, Double>* Objects::GetAddressableSMap(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  return target->GetAddressableSMap(parameter_index.first);
}

/**
 * This method will search the model for the specific addressable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @return Pointer to the addressable
 */
vector<Double>* Objects::GetAddressableVector(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  return target->GetAddressableVector(parameter_index.first);
}

/**
 * This method will find the object in Casal2 and return an Object pointer to it.
 *
 * @param object_absolute_name The absolute name for the parameter. This includes the object details (e.g process[mortality].m
 * @return Pointer to object or empty pointer if it's not found
 */

base::Object* Objects::FindObject(const string& parameter_absolute_name) {
  LOG_FINE() << "Looking for object: " << parameter_absolute_name;
  base::Object* result = nullptr;

  string type         = "";
  string label        = "";
  string parameter    = "";
  string index        = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  LOG_FINEST() << "FindObject; type: " << type << "; label: " << label << "; parameter: " << parameter << "; index: " << index;

  if (type == PARAM_PROCESS) {
    result = model_->managers().process()->GetProcess(label);

  } else if (type == PARAM_DERIVED_QUANTITY) {
    result = model_->managers().derived_quantity()->GetDerivedQuantity(label);

  } else if (type == PARAM_INITIALISATION_PHASE) {
    result = model_->managers().initialisation_phase()->GetInitPhase(label);

  } else if (type == PARAM_ESTIMATE) {
    result = model_->managers().estimate()->GetEstimateByLabel(label);

  } else if (type == PARAM_CATCHABILITY) {
    result = model_->managers().catchability()->GetCatchability(label);

  } else if (type == PARAM_SELECTIVITY) {
    result = model_->managers().selectivity()->GetSelectivity(label);

  } else if (type == PARAM_TIME_VARYING) {
    result = model_->managers().time_varying()->GetTimeVarying(label);
  }

  if (!result) {
    LOG_CODE_ERROR() << parameter_absolute_name << " could not be located. "
      << "Please ensure VerfiyAddressableForUse() was called prior to any model_.objects() methods";
  }

  return result;
}

/**
 *
 */
std::pair<string, string> Objects::ExplodeParameterAndIndex(const string& parameter_absolute_name) {
  string blank        = "";
  std::pair<string, string> result;

  ExplodeString(parameter_absolute_name, blank, blank, result.first, result.second);
  return result;
}

/**
 *
 */
void Objects::ExplodeString(const string& parameter_absolute_name, string &type, string& label, string& parameter, string& index) {
  LOG_TRACE();

  type       = "";
  label      = "";
  parameter  = "";
  index      = "";
   /**
     * This snippet of code will split the parameter from
     * objectType[ObjectName].ObjectParam(Index)
     */
   vector<string> token_list;

   typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
   boost::char_separator<char> seperator("[]");
   tokenizer tokens(parameter_absolute_name, seperator);

   for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
     token_list.push_back(*tok_iter);

   if (token_list.size() != 3)
     return;

   type = utilities::ToLowercase(token_list[0]);
   label      = token_list[1];
   parameter  = utilities::ToLowercase(token_list[2].substr(1));

   /**
    * Now check for index
    */
   boost::char_separator<char> seperator2("{}");
   tokenizer tokens2(parameter, seperator2);

   token_list.clear();
   for (tokenizer::iterator tok_iter = tokens2.begin(); tok_iter != tokens2.end(); ++tok_iter)
     token_list.push_back(*tok_iter);

   if (token_list.size() == 2) {
     parameter  = utilities::ToLowercase(token_list[0]);
     index      = token_list[1];
   }
}

/**
 *
 */
void Objects::ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter) {
  target_parameter = util::ToLowercase(type) + "[" + label + "]." + util::ToLowercase(parameter);
  if (index != "")
    target_parameter += "{" + index + "}";
}

} /* namespace niwa */
