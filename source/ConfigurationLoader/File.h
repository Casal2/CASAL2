/**
 * @file File.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents an instance of an open configuration file. It's controlled by the
 * configuration::Loader class.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef FILE_H_
#define FILE_H_

// Headers
#include <string>
#include <fstream>
#include <iostream>
#include <boost/shared_ptr.hpp>

// Namespaces
using std::ifstream;
using std::cout;
using std::endl;
using std::string;

namespace isam {
namespace configuration {

// Declarations
class Loader;

/**
 * Class definition
 */
class File {
public:
  // Methods
  File(Loader* loader);
  virtual                     ~File();
  bool                        OpenFile(string file_name);
  void                        Parse();

private:
  // Members
  Loader*                     loader_;
  string                      file_name_        = "not specified";
  ifstream                    file_;
  unsigned                    line_number_      = 0;
};

/**
 * Typedefs
 */
typedef boost::shared_ptr<File> FilePtr;

} /* namespace configuration */
} /* namespace isam */
#endif /* FILE_H_ */
