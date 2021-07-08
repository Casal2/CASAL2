/**
 * @file CommandLineParser.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "CommandLineParser.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <sstream>

#include "../../License.h"
#include "../../Version.h"

// Namespaces
namespace niwa {
namespace utilities {

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;
using std::cout;
using std::endl;
using std::ostringstream;

/**
 * This method will take the raw command line input from the main() method
 * and process them into something more useful.
 *
 * @param argc The number of arguments that have been provided
 * @param argv Pointer to an array containing the arguments
 * @param options The options object to fille with the values
 */
void CommandLineParser::Parse(int argc, char* argv[], RunParameters& options) {
  // Build Options menu
  // clang-format off
  options_description oDesc("Usage");
  oDesc.add_options()
    ("help,h", "Print help")
    ("license,l", "Display the Casal2 license")
    ("version,v", "Display the Casal2 version")
    ("config,c", value<string>(), "Configuration file")
    ("run,r", "Basic model run mode")
    ("estimate,e", "Point estimation run mode")
    // mcmc options
    ("mcmc,m", "Markov chain Monte Carlo run mode")
    ("estimate-before-mcmc", "Estimate the model before starting an MCMC")
    ("mcmc-mpd-file-name", value<string>(), "Load the specified MPD file for the MCMC run. When creating, this will overwrite")
    ("resume", "Resume the MCMC chain")
    ("objective-file", value<string>(), "Objectives file for resuming the MCMC chain")
    ("sample-file", value<string>(), "Samples file for resuming the MCMC chain")
    // other
    ("profiling,p", "Profiling run mode")
    ("simulation,s", value<unsigned>(), "Simulation mode (arg = number of candidates)")
    ("projection,f", value<unsigned>(), "Projection mode (arg = number of projections per set of input values)")
    ("input,i", value<string>(), "Load free parameter values from file")
    ("fi", "Force the input file to allow @estimate parameters only (basic run mode only)")
    ("seed,g", value<unsigned>(), "Random number seed")
    ("query,q", value<string>(), "Query an object type to see its description and parameters. Argument object_type.sub_type, e.g., process.recruitment_constant")
    ("debug,d", "Run in debug mode (with debug output)")
    ("nostd", "Do not print the standard header report")
    ("loglevel", value<string>(), "Set log level: medium, fine, finest, trace, none (default)")
    ("output,o", value<string>(), "Create estimate value report directed to [file]")
    ("single-step", "Single step the model each year with new estimable values")
    ("tabular", "Print reports in Tabular mode")
    ("unittest", "Run the unit tests for Casal2")
    ("no-mpd", "Do not create an MPD file");
  // clang-format on

  ostringstream o;
  o << oDesc;
  command_line_usage_ = o.str();

  // Read our Parameters
  variables_map parameters;

  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, oDesc), parameters);
    notify(parameters);

  } catch (boost::program_options::unknown_option& ex) {
    cout << "An error occurred while processing the command line. " << ex.what() << endl;
  }

  /**
   * Determine what run mode we should be in. If we're
   * in help, version or license then we don't need to continue.
   */
  if ((parameters.count("help")) || (parameters.size() == 0)) {
    options.run_mode_ = RunMode::kHelp;
    cout << command_line_usage_ << endl;
    return;
  } else if (parameters.count("version")) {
    options.run_mode_ = RunMode::kVersion;
    cout << "Casal2 Version: v" << VERSION << endl;
    cout << "Release ID: v" << VERSION_MUMBER << " " << SOURCE_CONTROL_VERSION << "" << endl;
    cout << "Copyright (c) 2017-" << SOURCE_CONTROL_YEAR << ", NIWA (www.niwa.co.nz)" << endl;
    return;
  } else if (parameters.count("license")) {
    options.run_mode_ = RunMode::kLicense;
    cout << license << endl;
    return;
  } else if (parameters.count("query")) {
    options.query_object_ = parameters["query"].as<string>();
    options.run_mode_     = RunMode::kQuery;
    return;
  } else if (parameters.count("unittest")) {
    options.run_mode_ = RunMode::kUnitTest;
    return;
  }

  /**
   * Load any variables into the global config that need to be available
   * immediately
   */
  if (parameters.count("loglevel")) {
    options.log_level_ = parameters["loglevel"].as<string>();
    Logging::Instance().SetLogLevel(options.log_level_);
  }

  LOG_TRACE();
  if (parameters.count("debug"))
    options.debug_mode_ = true;
  if (parameters.count("config"))
    options.config_file_ = parameters["config"].as<string>();
  if (parameters.count("input"))
    options.estimable_value_input_file_ = parameters["input"].as<string>();
  if (parameters.count("fi"))
    options.force_estimables_as_named_ = true;
  if (parameters.count("nostd"))
    options.no_std_report_ = true;
  if (parameters.count("output"))
    options.output_ = parameters["output"].as<string>();
  if (parameters.count("single-step"))
    options.single_step_model_ = true;
  if (parameters.count("tabular"))
    options.tabular_reports_ = true;
  if (parameters.count("phases"))
    options.estimation_phases_ = parameters["phases"].as<unsigned>();
  if (parameters.count("no-mpd"))
    options.create_mpd_file_ = false;
  if (parameters.count("estimate-before-mcmc"))
    options.estimate_before_mcmc_ = true;
  if (parameters.count("mcmc-mpd-file-name"))
    options.mcmc_mpd_file_name_ = parameters["mcmc-mpd-file-name"].as<string>();

  /**
   * At this point we know we've been asked to do an actual model
   * run. So we need to check to ensure the command line makes
   * sense.
   */
  unsigned run_mode_count = 0;
  run_mode_count += parameters.count("run");
  run_mode_count += parameters.count("estimate");
  run_mode_count += parameters.count("mcmc");
  run_mode_count += parameters.count("profiling");
  run_mode_count += parameters.count("simulation");
  run_mode_count += parameters.count("projection");
  run_mode_count += parameters.count("query");
  run_mode_count += parameters.count("unittest");

  if (run_mode_count == 0)
    LOG_ERROR() << "No valid run mode has been specified. Please specify a valid run mode (e.g., '-r')";
  if (run_mode_count > 1)
    LOG_ERROR() << "Multiple run modes have been specified. Please specify one run mode only";

  if (parameters.count("run"))
    options.run_mode_ = RunMode::kBasic;
  else if (parameters.count("estimate"))
    options.run_mode_ = RunMode::kEstimation;
  else if (parameters.count("mcmc")) {
    options.run_mode_ = RunMode::kMCMC;
    if (parameters.count("resume")) {
      if (!parameters.count("objective-file") || !parameters.count("sample-file")) {
        LOG_ERROR() << "Resuming an MCMC chain requires the objective-file and sample-file parameters";
        return;
      }

      options.mcmc_objective_file_ = parameters["objective-file"].as<string>();
      options.mcmc_sample_file_    = parameters["sample-file"].as<string>();
      options.resume_mcmc_chain_   = true;
    }

    if (options.estimate_before_mcmc_ == false && options.mcmc_mpd_file_name_ == "")
      LOG_ERROR() << "Cannot start a MCMC run without an estimation if there is no mcmc-mpd-file-name specified";
    if (options.estimate_before_mcmc_ && options.mcmc_mpd_file_name_ != "")
      LOG_ERROR() << "Cannot specify the --mcmc-mpd-file-name and --estimate-before-mcmc together";

  } else if (parameters.count("profiling"))
    options.run_mode_ = RunMode::kProfiling;
  else if (parameters.count("simulation")) {
    options.run_mode_              = RunMode::kSimulation;
    options.simulation_candidates_ = parameters["simulation"].as<unsigned>();
  } else if (parameters.count("projection")) {
    options.run_mode_              = RunMode::kProjection;
    options.projection_candidates_ = parameters["projection"].as<unsigned>();
  } else {
    LOG_ERROR() << "An invalid or unknown run mode has been specified.";
  }

  /**
   * Now we store any variables we want to use to override global defaults.
   */
  if (parameters.count("seed")) {
    options.override_rng_seed_value_     = parameters["seed"].as<unsigned>();
    options.override_random_number_seed_ = true;
  }
}

} /* namespace utilities */
} /* namespace niwa */
