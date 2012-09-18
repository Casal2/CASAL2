/**
 * @file GlobalConfiguration.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a singleton object that holds some global
 * configuration data for the application.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef GLOBALCONFIGURATION_H_
#define GLOBALCONFIGURATION_H_

// Headers
#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "../Translations/Translations.h"
#include "../Utilities/To.h"

// Namespaces
using std::map;
using std::vector;
using std::string;
using boost::shared_ptr;

namespace util = iSAM::Utilities;

namespace iSAM {

/**
 * Class Definitiion
 */
class GlobalConfiguration {
public:
  // Methods
  static shared_ptr<GlobalConfiguration> Instance();

  virtual                     ~GlobalConfiguration();
  void                        Clear();

  // Accessors and Mutators
  void                        set_command_line_parameters(vector<string> &parameters) { command_line_parameters_ = parameters; }
  vector<string>&             command_line_parameters() { return command_line_parameters_; }
  void                        set_debug_mode(string value) { parameters_[PARAM_DEBUG] = value; }
  bool                        debug_mode() { return util::To<bool>(parameters_[PARAM_DEBUG]); }
  void                        set_random_seed(string value) { parameters_[PARAM_RANDOM_NUMBER_SEED] = value; }
  int                         random_seed();
  void                        set_config_file(string value) { parameters_[PARAM_CONFIG_FILE] = value; }
  string                      config_file() { return parameters_[PARAM_CONFIG_FILE]; }


private:
  // Methods
  GlobalConfiguration();

  // Members
  map<string, string>         parameters_;
  vector<string>              command_line_parameters_;

};

// Typedef
typedef boost::shared_ptr<GlobalConfiguration> GlobalConfigurationPtr;

} /* namespace iSAM */
#endif /* GLOBALCONFIGURATION_H_ */
