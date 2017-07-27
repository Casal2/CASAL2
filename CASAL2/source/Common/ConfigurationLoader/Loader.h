/**
 * @file Loader.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for loading the configuration files into
 * memory and then passes each @config block off to the appropriate objects
 * after calling the factory.
 *
 * This class really creates a bunch of Base::Object objects and then assigns
 * parameters to them for verification and validation.
 */
#ifndef CONFIGURATION_LOADER_H_
#define CONFIGURATION_LOADER_H_

// Headers
#include <vector>
#include <string>

#include "Common/BaseClasses/Object.h"
#include "Common/Model/Model.h"
#include "Common/ParameterList/Table.h"

// Namespaces
namespace niwa {
namespace configuration {

using std::vector;
using std::string;
using niwa::base::Object;

// structs
struct FileLine {
public:
  string    file_name_      = "";
  unsigned  line_number_    = 0;
  string    line_           = "";
};

// classes
class Loader {
  friend class LoaderTest;
public:
  // Methods
  Loader(Model& model) : model_(model) { };
  virtual                     ~Loader() = default;
  bool                        LoadConfigFile(const string& override_file_name = "");
  void                        ClearFileLines() { file_lines_.clear(); }
  void                        AddFileLine(FileLine line);
  void                        ParseFileLines();

  // accessors
  vector<FileLine>&           file_lines() { return file_lines_; }

private:
  // Methods
  void                        ParseBlock(vector<FileLine> &block);
  bool                        HandleOperators(vector<string>& line_values, string &error);
  void                        HandleAssignment(const string& input_line, string& output_line);
  void                        HandleInlineDefinitions(FileLine& file_line, const string& parent_label);
  string                      RangeSplit(const string& range_value);

  // Members
  Model&                      model_;
  vector<FileLine>            file_lines_;
  parameters::Table*          current_table_ = nullptr;
  map<string, unsigned>       inline_count_;
};

} /* namespace configuration */
} /* namespace niwa */
#endif /* CONFIGURATION_LOADER_H_ */
