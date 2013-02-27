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

#include "Processes/Manager.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace objects {

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
   vector<string> tokenList;

   typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
   boost::char_separator<char> seperator(".[]()");
   tokenizer tokens(source_parameter, seperator);

   for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
     tokenList.push_back(*tok_iter);

   /**
    * Now, we've populated vector<string> called tokenList with
    * all of the split objects. We should expect their to be
    * either 3 or 4 (4 if index is specified)
    */
   if (tokenList.size() != 3 && tokenList.size() != 4)
     return;

   if (tokenList.size() == 4) {
        index = tokenList[3];
   }

   type       = utilities::ToLowercase(tokenList[0]);
   label      = tokenList[1];
   parameter  = utilities::ToLowercase(tokenList[2]);
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
  }


  return result;
}

} /* namespace objects */
} /* namespace isam */
