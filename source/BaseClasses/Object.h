/**
 * @file BaseObject.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the highest level class in our object tree. All objects
 * that have to be created from a configuration block need to inherit from this class.
 *
 * This class creates access to some central global objects within the system
 * including: GlobalConfiguration, TheModel, ParameterList etc
 *
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef BASE_OBJECT_H_
#define BASE_OBJECT_H_

// Headers
#include <string>
#include <boost/shared_ptr.hpp>

#include "ParameterList/ParameterList.h"
#include "Translations/Translations.h"
#include "Utilities/NoCopy.h"

// Namespaces
using std::string;

namespace isam {
namespace base {

/**
 * Class Definition
 */
class Object {
public:
  // Methods
  Object();
  virtual                     ~Object();
  string                      Label() { return label_; }

  // Accessors and Mutators
  ParameterList&              parameters() { return parameters_; }
  void                        set_block_type(string value) { block_type_ = value; }
  void                        set_defined_file_name(string value) { defined_file_name_ = value; }
  void                        set_defined_line_number(unsigned value) { defined_line_number_ = value; }

protected:
  // Methods
  void                        CheckForRequiredParameter(const string &label);

  // Members
  string                      block_type_           = "";
  string                      label_                = "";
  string                      defined_file_name_    = "";
  unsigned                    defined_line_number_  = 0;
  ParameterList               parameters_;

  DISALLOW_COPY_AND_ASSIGN(Object);
};

/**
 * Typedef
 */
typedef boost::shared_ptr<isam::base::Object> ObjectPtr;


} /* namespace base */
} /* namespace isam */
#endif /* BASE_OBJECT_H_ */
