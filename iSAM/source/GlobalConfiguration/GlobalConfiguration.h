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

#include "BaseClasses/Object.h"
#include "Translations/Translations.h"
#include "Utilities/To.h"

// Namespaces
using std::map;
using std::vector;
using std::string;
using boost::shared_ptr;

namespace util = isam::utilities;

namespace isam {

/**
 * Class Definitiion
 */
class GlobalConfiguration : public isam::base::Object {
public:
  // Methods
  static shared_ptr<GlobalConfiguration> Instance();

  virtual                     ~GlobalConfiguration() = default;
  void                        Clear();
  void                        OverrideGlobalValues(const map<string, string>& override_values);

  // Accessors and Mutators
  void                        set_command_line_parameters(vector<string> &parameters) { command_line_parameters_ = parameters; }
  vector<string>&             command_line_parameters() { return command_line_parameters_; }
  void                        set_debug_mode(string value) { global_parameters_[PARAM_DEBUG] = value; }
  bool                        debug_mode();
  void                        set_random_seed(string value) { global_parameters_[PARAM_RANDOM_NUMBER_SEED] = value; }
  unsigned                    random_seed();
  void                        set_config_file(string value) { global_parameters_[PARAM_CONFIG_FILE] = value; }
  string                      config_file() { return global_parameters_[PARAM_CONFIG_FILE]; }
  void                        set_skip_config_file(string value) { global_parameters_[PARAM_SKIP_CONFIG_FILE] = value; }
  bool                        skip_config_file();
  void                        set_simulation_candidates(unsigned value) { simulation_candidates_ = value; }
  unsigned                    simulation_candidates() const { return simulation_candidates_; }

private:
  // Methods
  GlobalConfiguration();

  // Members
  map<string, string>         global_parameters_;
  vector<string>              command_line_parameters_;
  unsigned                    simulation_candidates_ = 0;

};

// Typedef
typedef boost::shared_ptr<GlobalConfiguration> GlobalConfigurationPtr;

} /* namespace isam */
#endif /* GLOBALCONFIGURATION_H_ */
