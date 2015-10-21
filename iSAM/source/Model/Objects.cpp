/**
 * @file Objects.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 1/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Objects.h"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include "Catchabilities/Manager.h"
#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "Model/Managers.h"
#include "Processes/Manager.h"
#include "Selectivities/Manager.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {

namespace util = niwa::utilities;


Objects::Objects(Model* model) : model_(model) {

}
/**
 * This method will find the type of estimable in the system defined by the absolute
 * parameter name.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @param error The variable to populate with the error message if one occurs
 * @return The Estimable type, set to invalid if it's not found
 */
Estimable::Type Objects::GetEstimableType(const string& parameter_absolute_name, string& error) {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    error = parameter_absolute_name + " is not in the correct format. Correct format is object_type[label].estimable(array index)";
    return Estimable::kInvalid;
  }

  base::Object* target = FindObject(parameter_absolute_name, error);
  if (!target)
    return Estimable::kInvalid;

  if (!target->HasEstimable(parameter)) {
    error = parameter + " is not a valid estimable on the " + type + " with label " + label;
    return Estimable::kInvalid;
  }

  return target->GetEstimableType(parameter);
}

/**
 * This method will search the model for the specific estimable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @param error The variable to populate with the error message if one occurs
 * @return Pointer to the estimable or nullptr if none exists
 */
Double* Objects::GetEstimable(const string& parameter_absolute_name, string& error) {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    error = parameter_absolute_name + " is not in the correct format. Correct format is object_type[label].estimable(array index)";
    return nullptr;;
  }

  base::Object* target = FindObject(parameter_absolute_name, error);
  if (!target)
    return nullptr;

  if (index != "")
    return target->GetEstimable(parameter, index);

  return target->GetEstimable(parameter);
}

/**
 * This method will search the model for the specific estimable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @param error The variable to populate with the error message if one occurs
 * @return Pointer to the estimable or nullptr if none exists
 */
map<unsigned, Double>* Objects::GetEstimableUMap(const string& parameter_absolute_name, string& error) {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    error = parameter_absolute_name + " is not in the correct format. Correct format is object_type[label].estimable(array index)";
    return nullptr;;
  }

  base::Object* target = FindObject(parameter_absolute_name, error);
  if (!target)
    return nullptr;

  return target->GetEstimableUMap(parameter);
}

/**
 * This method will search the model for the specific estimable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @param error The variable to populate with the error message if one occurs
 * @return Pointer to the estimable or nullptr if none exists
 */
OrderedMap<string, Double>* Objects::GetEstimableSMap(const string& parameter_absolute_name, string& error) {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    error = parameter_absolute_name + " is not in the correct format. Correct format is object_type[label].estimable(array index)";
    return nullptr;;
  }

  base::Object* target = FindObject(parameter_absolute_name, error);
  if (!target)
    return nullptr;

  return target->GetEstimableSMap(parameter);
}

/**
 * This method will search the model for the specific estimable and
 * return a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name e.g. process[recruitment].r0
 * @param error The variable to populate with the error message if one occurs
 * @return Pointer to the estimable or nullptr if none exists
 */
vector<Double>* Objects::GetEstimableVector(const string& parameter_absolute_name, string& error) {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    error = parameter_absolute_name + " is not in the correct format. Correct format is object_type[label].estimable(array index)";
    return nullptr;;
  }

  base::Object* target = FindObject(parameter_absolute_name, error);
  if (!target)
    return nullptr;

  return target->GetEstimableVector(parameter);
}

/**
 * This method will find the object in iSAM and return an Object pointer to it.
 *
 * @param object_absolute_name The absolute name for the parameter. This includes the object details (e.g process[mortality].m
 * @param error The string to populate the error if one occurs
 * @return Pointer to object or empty pointer if it's not found
 */
base::Object* Objects::FindObject(const string& parameter_absolute_name, string& error) {
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

  } else if (type == PARAM_ESTIMATE) {
    result = model_->managers().estimate()->GetEstimateByLabel(label);

  } else if (type == PARAM_CATCHABILITY) {
    result = model_->managers().catchability()->GetCatchability(label);

  } else if (type == PARAM_SELECTIVITY) {
    result = model_->managers().selectivity()->GetSelectivity(label);
  }

  // TODO: Populate Error
  return result;
}

Double* Objects::FindEstimable(const string& estimable_absolute_name, string& error) {
  LOG_TRACE();

  niwa::base::Object* object = FindObject(estimable_absolute_name, error);
  if (!object)
    return 0;

  Double* result = 0;

  string type         = "";
  string label        = "";
  string parameter    = "";
  string index        = "";

  ExplodeString(estimable_absolute_name, type, label, parameter, index);
  LOG_FINEST() << "type: " << type << "; label: " << label << "; parameter: " << parameter << "; index: " << index;
  if (index != "")
    result = object->GetEstimable(parameter, index);
  else
    result = object->GetEstimable(parameter);

  return result;
}

/**
 *
 */
void Objects::ExplodeString(const string& source_parameter, string &type, string& label, string& parameter, string& index) {
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
   tokenizer tokens(source_parameter, seperator);

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
   boost::char_separator<char> seperator2("()");
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
    target_parameter += "(" + index + ")";
}

} /* namespace niwa */
