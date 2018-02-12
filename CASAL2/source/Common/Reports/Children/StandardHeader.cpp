//============================================================================
// Name        : CStandardHeaderReport.cpp
// Author      : S.Rasmussen
// Date        : 5/01/2009
// Copyright   : Copyright NIWA Science ©2008 - www.niwa.co.nz
// Description :
// $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
//============================================================================

// Headers
#include "StandardHeader.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <math.h>
#include <ctime>
#include <sstream>

#include "Version.h"
#include "Common/GlobalConfiguration/GlobalConfiguration.h"
#include "Common/Model/Model.h"

// Namespaces
using std::endl;
using std::cout;
using std::cerr;
using std::ostringstream;

namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
StandardHeader::StandardHeader(Model* model) : Report(model) {

  // Variables
  time_start_   = time(NULL);

  // Linux Vars
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  times(&cpu_start);
#endif
}

/**
 * Destructor
 */
StandardHeader::~StandardHeader() {
}

/**
 * Prepare the header. This involves building
 * all of the information we need so it can be printed.
 */
void StandardHeader::DoPrepare() {

  ostringstream               header;
  header << "CASAL2" << endl;

  /**
   * Build the Command line
   */
  header << "Call: ";
  vector<string>& commandLine = model_->global_configuration().command_line_parameters();
  for (unsigned i = 0; i < commandLine.size(); ++i)
    header << commandLine[i] << " ";
  header << endl;

    // Build Date
  header << "Date: " << ctime(&time_start_);

  /**
   * Version information
   */
  header << "Version: " << SOURCE_CONTROL_VERSION << endl;
  header << "Copyright (c) 2018 - " << SOURCE_CONTROL_YEAR << ", NIWA (www.niwa.co.nz)" << endl;

  /**
   * User and Computer Information
   */

#ifdef __MINGW32__
  header << "Environment: machine:" << getenv("COMPUTERNAME") << ", user:" << getenv("USERNAME") << ", os:" << getenv("OS") << ", pid:" << _getpid() << endl;  
#elif _MSC_VER
#else
  header << "-- Username: ";
  char* cUsername = getenv("LOGNAME");
  if (cUsername != NULL)
    header << cUsername << endl;
  else {
    header << "-----" << endl;
  }

    struct utsname names;
  uname(&names);
  header << "-- Machine: " << names.nodename << " (" << names.sysname << " " << names.release << " " << names.machine << ")" << endl;
  header << "-- Process Id: " << getpid() << endl;
#endif

  // Print Header
  cout << header.str() << endl;
  header_ = header.str();
}

/**
 * This report does it's printing during the start and finalise methods
 * so we do not need to have any logic within the run method.
 */
void StandardHeader::DoExecute() { }

/**
 * Finalise our report. This will calculate how much time it took
 * to do the execution of our application and print this to the
 * console.
 */
void StandardHeader::DoFinalise() {
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  times(&cpu_stop);
  double cpu_time=(static_cast<double>(cpu_stop.tms_utime)+static_cast<double>(cpu_stop.tms_stime))-(static_cast<double>(cpu_start.tms_utime) + static_cast<double>(cpu_start.tms_stime));
  // Turn into seconds
  cpu_time /= static_cast<double>(sysconf(_SC_CLK_TCK));
  // Turn into hours
  cpu_time = cpu_time / 3600.0;
  int P = (int) floor(log10(cpu_time))+4;
  cout << "Total CPU time: " << std::setprecision(P) << cpu_time << (cpu_time==1?" hour":" hours") << ".\n";
#endif

  double elapsed_time = static_cast<double>(time(NULL)-time_start_);
  if(elapsed_time < 60) {
    int P = (int) floor(log10(elapsed_time))+4;
    cout << "Total elapsed time: " << std::setprecision(P) << fmax(1,elapsed_time) << (elapsed_time<=1?" second":" seconds") << endl;
  } else if((elapsed_time/60.0) < 60) {
    elapsed_time /= 60.0;
    int P = (int) floor(log10(elapsed_time))+4;
    cout << "Total elapsed time: " << std::setprecision(P) << elapsed_time << (elapsed_time==1?" minute":" minutes") << endl;
  } else {
    elapsed_time /= 3600.0;
    int P = (int) floor(log10(elapsed_time))+4;
    cout << "Total elapsed time: " << std::setprecision(P) << elapsed_time << (elapsed_time==1?" hour":" hours") << endl;
  }
  cout << "Completed" << endl;
}

} /* namespace report */
} /* namespace niwa */
