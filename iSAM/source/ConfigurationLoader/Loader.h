/**
 * @file Loader.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for loading the configuration files into
 * memory and then passes each @config block off to the appropriate objects
 * after calling the factory.
 *
 * This class really creates a bunch of Base::Object objects and then assigns
 * parameters to them for verification and validation.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LOADER_H_
#define LOADER_H_

// Headers
#include <vector>
#include <string>

#include "BaseClasses/Object.h"
#include "ParameterList/Table.h"

// Namespaces
using std::vector;
using std::string;
using isam::base::Object;
using isam::base::ObjectPtr;
using isam::parameterlist::TablePtr;

namespace isam {
namespace configuration {

/**
 * FileLine Structure
 */
struct FileLine {
public:
  string    file_name_      = "";
  unsigned  line_number_    = 0;
  string    line_           = "";
};

/**
 * Class Definition
 */
class Loader {
public:
  // Methods
  Loader() = default;
  virtual                     ~Loader() = default;
  void                        LoadConfigFile(const string& override_file_name = "");
  void                        ClearFileLines() { file_lines_.clear(); }
  void                        AddFileLine(FileLine line);
  void                        ParseFileLines();

private:
  // Methods
  void                        ParseBlock(vector<FileLine> &block);
  ObjectPtr                   CreateObject(const string &block_type, const string &object_type);
  bool                        HandleOperators(vector<string> &line_values);
  void                        HandleInlineDefinitions(FileLine& file_line, const string& parent_label);

  // Members
  vector<FileLine>            file_lines_;
};

} /* namespace configuration */
} /* namespace isam */
#endif /* LOADER_H_ */
