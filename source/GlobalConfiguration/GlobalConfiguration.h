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
  static shared_ptr<GlobalConfiguration> getInstance();

  virtual                     ~GlobalConfiguration();
  void                        clear();
  void                        addCommandLineParameter(string parameter) { vCommandLineParameters.push_back(parameter); }
  vector<string>&             getCommandLineParameters() { return vCommandLineParameters; }
  bool                        getDebugMode() { return util::To<bool>(mParameters[PARAM_DEBUG]); }
  void                        setDebugMode(string value) { mParameters[PARAM_DEBUG] = value; }
  int                         getRandomSeed();
  void                        setRandomSeed(string value) { mParameters[PARAM_RANDOM_NUMBER_SEED] = value; }
  string                      getConfigFile() { return mParameters[PARAM_CONFIG_FILE]; }
  void                        setConfigFile(string value) { mParameters[PARAM_CONFIG_FILE] = value; }

private:
  // Methods
  GlobalConfiguration();

  // Members
  map<string, string>         mParameters;
  vector<string>              vCommandLineParameters;

};

// Typedef
typedef boost::shared_ptr<GlobalConfiguration> GlobalConfigurationPtr;

} /* namespace iSAM */
#endif /* GLOBALCONFIGURATION_H_ */
