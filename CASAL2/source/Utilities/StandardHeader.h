/**
 * @file StandardHeader.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 30/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 */
#ifndef SOURCE_UTILITIES_STANDARDHEADER_H_
#define SOURCE_UTILITIES_STANDARDHEADER_H_

// headers
#include <string>

#ifdef __MINGW32__
  #include <process.h>
#elif _MSC_VER
#else
  #include <sys/utsname.h>
  #include <sys/times.h>
  #include <unistd.h>
#endif

#include "../GlobalConfiguration/GlobalConfiguration.h"

// namespaces
namespace niwa {
namespace utilities {

// class declaration
class StandardHeader {
public:
	void												PrintTop(GlobalConfiguration& global_config);
	void												PrintBottom(GlobalConfiguration& global_config);

private:
  // Variables
  time_t                      time_start_;
  std::string                 header_ = "";

  // Linux Vars
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  tms                         cpu_start;
  tms                         cpu_stop;
#endif
};

} /* namespace utilities */
} /* namespace niwa */

#endif /* SOURCE_UTILITIES_STANDARDHEADER_H_ */
