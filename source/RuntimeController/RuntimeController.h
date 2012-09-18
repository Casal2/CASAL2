/**
 * @file RuntimeController.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is really the execution-switch for this application. It's
 * reponsible for taking the command line parameters, calling the configuration
 * loader and then firing off the execution call to the target objects based
 * on the run mode.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef RUNTIMECONTROLLER_H_
#define RUNTIMECONTROLLER_H_

// Headers
#include <boost/shared_ptr.hpp>

#include "../GlobalConfiguration/GlobalConfiguration.h"

// Namespaces
namespace iSAM {

namespace RunMode {
typedef enum {
  INVALID,
  LICENSE,
  VERSION,
  HELP,
  BASIC,
  ESTIMATION
} Type;
}

using boost::shared_ptr;

/**
 * Class Definition
 */
class RuntimeController {
public:
  // Methods
  static shared_ptr<RuntimeController> getInstance();

  virtual                     ~RuntimeController();
  void                        parseCommandLine(int argc, const char* argv[]);
  string                      getCommandLineUsage() { return sCommandLineUsage; }
  RunMode::Type               getRunMode() { return eRunMode; }

private:
  // Methods
  RuntimeController();

  // Members
  GlobalConfigurationPtr      pGlobalConfig;
  RunMode::Type               eRunMode;
  string                      sCommandLineUsage;
};

// Typdef
typedef boost::shared_ptr<RuntimeController> RuntimeControllerPtr;

} /* namespace iSAM */
#endif /* RUNTIMECONTROLLER_H_ */
