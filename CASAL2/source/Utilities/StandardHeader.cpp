/**
 * @file StandardHeader.cpp
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 30/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 */

// headers
#include "StandardHeader.h"

#include <math.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Model/Model.h"
#include "../Version.h"

// Namespaces
using std::cerr;
using std::cout;
using std::endl;
using std::ostringstream;

// namespaces
namespace niwa {
namespace utilities {

/**
 *
 */
void StandardHeader::PrintTop(GlobalConfiguration& global_config) {
  // Variables
  start_time_ = time(NULL);

  // Linux Vars
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  times(&cpu_start);
#endif

  ostringstream header;
  header << "Casal2" << endl;
  /**
   * Build the Command line
   */
  header << "Call: ";
  vector<string>& commandLine = global_config.command_line_parameters();
  for (unsigned i = 0; i < commandLine.size(); ++i) header << commandLine[i] << " ";
  header << endl;
  /**
   *  Print the date and time
   */
  header << "Date: ";
  time_t t;
  t = time(NULL);
  header << ctime(&t);
  /**
   * Version information
   */
  header << "Version: v" << VERSION << endl;
  header << "Copyright (c) 2017-" << SOURCE_CONTROL_YEAR << ", NIWA (www.niwa.co.nz)" << endl;
  /**
   * User and Computer Information
   */
#ifdef __MINGW32__
  header << "Environment: User=" << getenv("USERNAME") << " Machine=" << getenv("COMPUTERNAME") << " OS=" << getenv("OS") << " PID=" << _getpid() << endl;
#elif _MSC_VER
#else
  cout << "Environment: User=";
  auto* cUsername = getenv("LOGNAME");
  if (cUsername != NULL)
    header << cUsername << endl;
  else {
    header << "-----" << endl;
  }
  struct utsname names;
  uname(&names);
  header << " Machine=" << names.nodename << " (" << names.sysname << " " << names.release << " " << names.machine << ")" << endl;
  header << " PID=" << getpid() << endl;
#endif

  if (!global_config.disable_standard_report()) {
    cout << header.str() << endl << endl;
    cerr << "\n";
  }

  global_config.set_standard_header(header.str());
}
/**
 *
 */
void StandardHeader::PrintBottom(GlobalConfiguration& global_config) {
  if (global_config.disable_standard_report())
    return;

  LOG_INFO() << "Completed";

#if !defined(__MINGW32__) && !defined(_MSC_VER)
  times(&cpu_stop);
  double cpu_time
      = (static_cast<double>(cpu_stop.tms_utime) + static_cast<double>(cpu_stop.tms_stime)) - (static_cast<double>(cpu_start.tms_utime) + static_cast<double>(cpu_start.tms_stime));
  // Turn into seconds
  cpu_time /= static_cast<double>(sysconf(_SC_CLK_TCK));
  // Turn into hours
  cpu_time = cpu_time / 3600.0;
  int P    = (int)floor(log10(cpu_time)) + 4;
  LOG_INFO() << "Total CPU time: " << std::setprecision(P) << cpu_time << (cpu_time == 1 ? " hour" : " hours");
#endif

  double elapsed_time = static_cast<double>(time(NULL) - start_time_);

  if (elapsed_time < 60) {
    int P = (int)floor(log10(elapsed_time)) + 4;
    LOG_INFO() << "Total elapsed time: " << std::setprecision(P) << fmax(1, elapsed_time) << (elapsed_time == 1 ? " second" : " seconds");
  } else if ((elapsed_time / 60.0) < 60) {
    elapsed_time /= 60.0;
    int P = (int)floor(log10(elapsed_time)) + 4;
    LOG_INFO() << "Total elapsed time: " << std::setprecision(P) << elapsed_time << (elapsed_time == 1 ? " minute" : " minutes");
  } else {
    elapsed_time /= 3600.0;
    int P = (int)floor(log10(elapsed_time)) + 4;
    LOG_INFO() << "Total elapsed time: " << std::setprecision(P) << elapsed_time << (elapsed_time == 1 ? " hour" : " hours");
  }
}

} /* namespace utilities */
} /* namespace niwa */
