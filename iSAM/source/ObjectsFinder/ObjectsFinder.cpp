/**
 * @file ObjectFinder.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ObjectsFinder.h"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include "Catchabilities/Manager.h"
#include "Processes/Manager.h"
#include "Selectivities/Manager.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace objects {

namespace util = isam::utilities;

/**
 * See Header file for definition.
 */
void ExplodeString(const string& source_parameter, string &type, string& label, string& parameter, string& index) {

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
 * Implode the string so it's back to a target parameter. This basically handles any issues we have with
 * case sensitivity.
 */
void ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter) {
  target_parameter = util::ToLowercase(type) + "[" + label + "]." + util::ToLowercase(parameter);
  if (index != "")
    target_parameter += "(" + index + ")";
}


/**
 * See header file for description
 */
isam::base::ObjectPtr FindObject(const string& parameter_absolute_name) {
  base::ObjectPtr result;

  string type         = "";
  string label        = "";
  string parameter    = "";
  string index        = "";

  ExplodeString(parameter_absolute_name, type, label, parameter, index);
  LOG_INFO("FindObject; type: " << type << "; label: " << label << "; parameter: " << parameter << "; index: " << index);

  if (type == PARAM_PROCESS) {
    result = processes::Manager::Instance().GetProcess(label);

  } else if (type == PARAM_CATCHABILITY) {
    result = catchabilities::Manager::Instance().GetCatchability(label);

  } else if (type == PARAM_SELECTIVITY) {
    result = selectivities::Manager::Instance().GetSelectivity(label);
  }

  return result;
}

/**
 * See header for dexcription
 */
Double* FindEstimable(const string& estimable_absolute_name) {
  isam::base::ObjectPtr object = FindObject(estimable_absolute_name);
  if (!object)
    return 0;

  Double* result = 0;

  string type         = "";
  string label        = "";
  string parameter    = "";
  string index        = "";

  ExplodeString(estimable_absolute_name, type, label, parameter, index);
  if (index != "")
    result = object->GetEstimable(parameter, index);
  else
    result = object->GetEstimable(parameter);

  return result;
}

} /* namespace objects */
} /* namespace isam */
