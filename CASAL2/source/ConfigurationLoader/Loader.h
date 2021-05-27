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
#include <string>
#include <vector>

#include "../BaseClasses/Object.h"
#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Model/Model.h"
#include "../ParameterList/Table.h"

// Namespaces
namespace niwa {
namespace configuration {

using niwa::base::Object;
using std::string;
using std::vector;

/**
 * @brief FileLine struct holes one line of text from a configuration file.
 * We also hold the name of the file and the line number in that file
 * so that we can use this for error reporting to the user.
 *
 * This information is used by the ParameterList system and LOG_X_P() macros
 * to easily display to the user what file and line the configuration
 * had an issue with.
 */
struct FileLine {
public:
  string   file_name_   = "";
  unsigned line_number_ = 0;
  string   line_        = "";
};

/**
 * @brief Configuration loading class.
 * This is the main class used to load configuration files
 * from disk to memory and then create/populate internal
 * objects.
 */
class Loader {
  friend class LoaderTest;

public:
  // Methods
  Loader()          = default;
  virtual ~Loader() = default;
  bool LoadFromDiskToMemory(GlobalConfiguration& global_config, const string& override_file_name = "");
  void StoreLine(FileLine line);
  void Build(vector<shared_ptr<Model>>& model_list);

  // accessors
  vector<FileLine>&         file_lines() { return file_lines_; }
  vector<vector<FileLine>>& blocks() { return blocks_; }
  string                    model_type() const { return model_type_; }
  string                    minimiser_type() const { return minimiser_type_; }

protected:
  // Methods
  void HandleInlineDefinitions();
  void CreateBlocksFromInput();
  void ParseBlock(shared_ptr<Model> model, vector<FileLine>& block);
  // void HandleInlineDefinitions(shared_ptr<Model> model, FileLine& file_line, const string& parent_label);
  void FindActiveMinimiserType();

  // Members
  vector<FileLine>         file_lines_;
  vector<vector<FileLine>> blocks_;
  parameters::Table*       current_table_ = nullptr;
  map<string, unsigned>    inline_count_;
  string                   model_type_     = "";
  string                   minimiser_type_ = "";
};

} /* namespace configuration */
} /* namespace niwa */
#endif /* CONFIGURATION_LOADER_H_ */
