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

#include "../AgeLengths/Manager.h"
#include "../Catchabilities/Manager.h"
#include "../DerivedQuantities/Manager.h"
#include "../Estimates/Manager.h"
#include "../Model/Model.h"
#include "../Model/Managers.h"
#include "../Processes/Manager.h"
#include "../Selectivities/Manager.h"
#include "../LengthWeights/Manager.h"
#include "../InitialisationPhases/Manager.h"
#include "../TimeVarying/Manager.h"
#include "../Observations/Manager.h"
#include "../Utilities/String.h"
#include "../Utilities/To.h"

// namespaces
namespace niwa {

namespace util = niwa::utilities;

/**
 * Default constructor
 */
Objects::Objects(shared_ptr<Model> model) : model_(model) {
}

/**
 * This method verifies the existence of the addressable and its usage flag. This check is done
 * prior to any other call to this object to ensure the addressable can be found and used.
 *
 * @param parameter_absolute_name The absolute name for the addressable (e.g., 'block[label].variable{index}')
 * @param usage The intended usage for this addressable
 * @param error A string to hold any returning errors in
 * @return True if the verification was successful, false otherwise
 */
bool Objects::VerfiyAddressableForUse(const string& parameter_absolute_name, addressable::Usage usage, string& error) {
  LOG_TRACE();
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";
  error = "";

  ostringstream str;

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    str << "The syntax for " << parameter_absolute_name << " is invalid. The correct syntax is 'block[label].variable{index}'";
    error = str.str();
    return false;
  }

  base::Object* object = this->FindObjectOrNull(parameter_absolute_name);
  if (!object) {
    str << "Parent object for " << parameter_absolute_name << " is not valid.";
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
 * This method finds the type of addressable in the system defined by the absolute
 * parameter name.
 *
 * @param parameter_absolute_name The absolute parameter name, e.g., 'process[recruitment].r0'
 * @return The addressable type, set to invalid if it is not found
 */
addressable::Type Objects::GetAddressableType(const string& parameter_absolute_name) {
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  LOG_FINEST() << "parameter type = " << parameter_index.first << " index = " << parameter_index.second;
  if (parameter_index.second != "") {
    if (utilities::String::explode(parameter_index.second).size() == 1)
      return addressable::kSingle;
    else
      return addressable::kMultiple;
  }

  base::Object* target = FindObjectOrNull(parameter_absolute_name); // TODO: Mock FIndObject() so we can unit test this.
  if (target == nullptr)
    LOG_CODE_ERROR() << "target == nullptr: " << parameter_absolute_name;
  return target->GetAddressableType(parameter_index.first);
}

/**
 * This method searches the model for the specific addressable and
 * returns a pointer to it if it exists.
 *
 * @param addressable_absolute_name The absolute parameter name, e.g., 'process[recruitment].r0'
 * @return Pointer to the addressable
 */
Double* Objects::GetAddressable(const string& addressable_absolute_name) {
  base::Object* target = FindObject(addressable_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(addressable_absolute_name);
  if (parameter_index.second != "")
    return target->GetAddressable(parameter_index.first, parameter_index.second);

  return target->GetAddressable(parameter_index.first);
}

/**
 * This method returns a vector of double pointers to a specific subset of an addressable
 *
 * @param addressable_absolute_name The absolute parameter name, e.g., 'process[recruitment].r0'
 * @return Pointer to a vector or addressable pointers
 */
vector<Double*>* Objects::GetAddressables(const string& addressable_absolute_name) {
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(addressable_absolute_name);
  if (parameter_index.second == "") {
    LOG_CODE_ERROR() << addressable_absolute_name << " is not a multiple type of addressable lookup";
  }

  string absolute_parameter = parameter_index.first + "{" + parameter_index.second + "}";
  vector<string> indexes = utilities::String::explode(parameter_index.second);

  base::Object* target = FindObject(addressable_absolute_name);
  return target->GetAddressables(absolute_parameter, indexes);
}

/**
 * This method searchs the model for the specific addressable and
 * returns a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name, e.g., 'process[recruitment].r0'
 * @param error The variable to populate with the error message if one occurs
 * @return Pointer to the addressable or nullptr if none exists
 */
map<unsigned, Double>* Objects::GetAddressableUMap(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  return target->GetAddressableUMap(parameter_index.first);
}

/**
 * This method searches the model for the specific addressable and
 * returns a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name, e.g., 'process[recruitment].r0'
 * @return Pointer to the addressable
 */
OrderedMap<string, Double>* Objects::GetAddressableSMap(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  return target->GetAddressableSMap(parameter_index.first);
}

/**
 * This method searches the model for the specific addressable and
 * returns a pointer to it if it exists.
 *
 * @param parameter_absolute_name The absolute parameter name, e.g., 'process[recruitment].r0'
 * @return Pointer to the addressable
 */
vector<Double>* Objects::GetAddressableVector(const string& parameter_absolute_name) {
  base::Object* target = FindObject(parameter_absolute_name);
  std::pair<string, string> parameter_index = ExplodeParameterAndIndex(parameter_absolute_name);
  return target->GetAddressableVector(parameter_index.first);
}

/**
 * This method finds the object and returns an Object pointer to it
 *
 * @param object_absolute_name The absolute name for the parameter. This includes the object details (e.g., 'process[mortality].m')
 * @return Pointer to object or empty pointer
 */
base::Object* Objects::FindObjectOrNull(const string& parameter_absolute_name) {
  LOG_FINE() << "Looking for object: " << parameter_absolute_name;
  base::Object* result = nullptr;

  string type         = "";
  string label        = "";
  string parameter    = "";
  string index        = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  LOG_FINEST() << "FindObject; type: " << type << "; label: " << label << "; parameter: " << parameter << "; index: " << index;
  if (type == "" || label == "" || parameter == "")
    return nullptr;

  if (type == PARAM_PROCESS) {
    result = model_->managers()->process()->GetProcess(label);

  } else if (type == PARAM_DERIVED_QUANTITY) {
    result = model_->managers()->derived_quantity()->GetDerivedQuantity(label);

  } else if (type == PARAM_AGE_LENGTH) {
    result = model_->managers()->age_length()->FindAgeLength(label);

  } else if (type == PARAM_LENGTH_WEIGHT) {
    result = model_->managers()->length_weight()->GetLengthWeight(label);

  } else if (type == PARAM_INITIALISATION_PHASE) {
    result = model_->managers()->initialisation_phase()->GetInitPhase(label);

  } else if (type == PARAM_ESTIMATE) {
    result = model_->managers()->estimate()->GetEstimateByLabel(label);

  } else if (type == PARAM_CATCHABILITY) {
    result = model_->managers()->catchability()->GetCatchability(label);

  } else if (type == PARAM_SELECTIVITY) {
    result = model_->managers()->selectivity()->GetSelectivity(label);

  } else if (type == PARAM_TIME_VARYING) {
    result = model_->managers()->time_varying()->GetTimeVarying(label);

  } else if (type == PARAM_OBSERVATION) {
    result = model_->managers()->observation()->GetObservation(label);

  } else {
    LOG_FATAL() << "Currently the type " << type << " is not registered for addressable finding, first please check you have spelt it correctly, if you are "
        << "confident you have it may not be coded to find addressable, please add it the class to FindObject() "
        << "in Model/Objects.cpp by contacting the development team";
  }

  return result;
}

/**
 * This method finds the object and returns an Object pointer to it.
 *
 * @param object_absolute_name The absolute name for the parameter. This includes the object details (e.g., 'process[mortality].m')
 * @return Pointer to object or throw error if it does not exist.
 */
base::Object* Objects::FindObject(const string& parameter_absolute_name) {
  base::Object* result = FindObjectOrNull(parameter_absolute_name);

  if (!result) {
    LOG_CODE_ERROR() << "Parameter absolute " << parameter_absolute_name << " was not found. "
      << "Please check that VerfiyAddressableForUse() was called prior to any model_.objects() methods";
  }

  return result;
}

/**
 * This method takes an addressable full name and returns the addressable label
 * and index components.
 *
 * Submitting process[myprocess].addressable{1,2} would return pair<"addressable", "1,2">.
 * This is then used on a base::Object to locate the specific addressable.
 *
 * @parameter The absolute name of the addressable/parameter (e.g., 'process[x].a{1}')
 * @return std::pair<"addressable", "index">
 *
 */
std::pair<string, string> Objects::ExplodeParameterAndIndex(const string& parameter_absolute_name) {
  string blank        = "";
  std::pair<string, string> result;

  ExplodeString(parameter_absolute_name, blank, blank, result.first, result.second);
  return result;
}

/**
 * This method takes an absolute parameter name and breaks it into the different parts so it
 * can be used to find the binary object in memory
 *
 * @parameter parameter_absolute_name The name of the object in either type[label].addressable{index} or label.addressable{index}
 * @parameter type Return parameter to hold the object type (e.g., process, derived quantity)
 * @parameter label the label of the object
 * @parameter addressable The addressable parameter
 * @parameter index The index into the object. This could be multiple values in one string.
 */
void Objects::ExplodeString(const string& parameter_absolute_name, string &type, string& label, string& addressable, string& index) {
  LOG_TRACE();

  type       = "";
  label      = "";
  addressable  = "";
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
   addressable  = utilities::ToLowercase(token_list[2].substr(1));

   /**
    * Now check for index
    */
   boost::char_separator<char> seperator2("{}");
   tokenizer tokens2(addressable, seperator2);

   token_list.clear();
   for (tokenizer::iterator tok_iter = tokens2.begin(); tok_iter != tokens2.end(); ++tok_iter)
     token_list.push_back(*tok_iter);

   if (token_list.size() == 2) {
     addressable  = utilities::ToLowercase(token_list[0]);
     index      = token_list[1];
   }
}

/**
 * This method takes the different parts of a full addressable path and joins them back
 * into a single string.
 * e.g. type=process, label=myproc, addressable=a, index=1 would return
 * process[myproc].a{1}
 *
 * @parameter type The type of object the addressable belongs to (e.g., process)
 * @parameter label The label of the object the addressable is on (e.g., my_process)
 * @parameter parameter The parameter that is being addressed (e.g., b0)
 * @parameter index The index into the addressable for vector, map types, etc.
 * @parameter target_parameter This is the return value, the full string (e.g., 'process[myproc].a{1}')
 */
void Objects::ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter) {
  target_parameter = util::ToLowercase(type) + "[" + label + "]." + util::ToLowercase(parameter);
  if (index != "")
    target_parameter += "{" + index + "}";
}

} /* namespace niwa */
