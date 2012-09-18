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

#include "../BaseClasses/Object.h"

// Namespaces
using std::vector;
using std::string;
using iSAM::Base::Object;

namespace iSAM {
namespace Configuration {

/**
 * Class Definition
 */
class Loader {
public:
  // Methods
  Loader();
  virtual                     ~Loader();
  void                        LoadConfigFile(bool skip_loading_file = false);
  void                        LoadIntoCache(vector<string> &lines);

protected:
  // Methods
  void                        ProcessBlock();
  string                      GetTypeFromCurrentBlock();
  void                        LoadConfigIntoCache(string file_name);
  void                        AssignParameters(shared_ptr<Object> object);
  void                        AssignTableParameters(shared_ptr<Object> object, int &current_index);
  void                        SplitLineIntoVector(string line, vector<string> &parameters);

  // Members
  vector<string>              lines_;
  vector<string>              current_block_;
};

} /* namespace Configuration */
} /* namespace iSAM */
#endif /* LOADER_H_ */
