/**
 * @file File.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents an instance of an open configuration file. It's controlled by the
 * configuration::Loader class.
 */
#ifndef CONFIGURATION_FILE_H_
#define CONFIGURATION_FILE_H_

// headers
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "../ConfigurationLoader/Loader.h"


// namespaces
namespace niwa {
namespace configuration {

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

// classes
class File {
public:
  // Methods
  File(Loader& loader) : loader_(loader){};
  virtual ~File();
  bool OpenFile(string file_name);
  void LoadIntoMemory();

private:
  // Methods
  void StripComments(string& current_line);

  // Members
  Loader&  loader_;
  string   file_name_ = "not specified";
  ifstream file_;
  unsigned line_number_         = 0;
  bool     multi_line_comment_  = false;
  bool     single_line_comment_ = false;
};

} /* namespace configuration */
} /* namespace niwa */
#endif /* CONFIGURATION_FILE_H_ */
