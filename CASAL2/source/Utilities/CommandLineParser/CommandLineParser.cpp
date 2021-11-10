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
 * @param options The options object to file with the values
 */
void CommandLineParser::Parse(int argc, char* argv[], RunParameters& options) {
  // Build Options menu
  // clang-format off
  options_description oDesc("Usage");
  oDesc.add_options()
    ("help,h", "Display help")
    ("license,l", "Display the reference for the Casal2 license")
    ("version,v", "Display the Casal2 version")
    ("config,c", value<string>(), "Configuration [file]")
    ("run,r", "Do a model run (without estimation)")
    ("estimate,e", "Estimate the MPD")
    ("Estimate,E", value<string>(), "Estimate the MPD and create an MPD [file] containing the MPD and covariance for use in an MCMC")
    ("mcmc,m", "Estimate the MPD and then do an MCMC")
    ("mcmc-from-estimate,M", value<string>(), "Do an MCMC using a previously estimated MPD using [file] (from -E)")
    ("resume,R", value<string>(), "Resume a previously interrupted MCMC using the MPD [file], and sample and objective files")
    ("objective-file", value<string>(), "Objectives [file] for resuming the MCMC")
    ("sample-file", value<string>(), "Samples [file] for resuming the MCMC")
    // other
    ("profile,p", "Do a likelihood profile")
    ("simulation,s", value<unsigned>(), "Do a simulation [n] = number of candidates)")
    ("projection,f", value<unsigned>(), "Do projections [n] = number of projections per set of input values)")
    ("input,i", value<string>(), "Load free parameter values from [file]")
    ("input-force,I",  value<string>(), "Load free parameters from [file], and force overwriting of non-estimated addressable parameters")
    ("output,o", value<string>(), "Create a free parameter report and write to [file]")
   // ("Output,O", value<string>(), "Append a free parameter report to the [file]")
    ("seed,g", value<unsigned>(), "Set the random number seed [n]")
    ("query", value<string>(), "Query an object type to view its syntax description. Argument = [object_type.sub_type, e.g., process.recruitment_constant]")
    ("nostd", "Do not print the standard header report")
    ("loglevel", value<string>(), "Set message log level. Defaults to [info]. See the Manual for further options")
    ("single-step", "Single step the model each year with new estimable values")
    ("tabular,t", "Print reports using the Tabular format")
    ("unittest", "Run the unit tests for Casal2")
    ("skip-verify", "Skip halting if warnings are created during verification of the model");

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
    LOG_FATAL() << "Command line error: An error occurred while processing the command line. There was an " << ex.what();
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
    cout << "Release ID: v" << VERSION_NUMBER << " " << SOURCE_CONTROL_VERSION << "" << endl;
    cout << "Copyright (c) 2017-" << SOURCE_CONTROL_YEAR << ", NIWA (www.niwa.co.nz)" << endl;
    cout << "Date: ";
    time_t t;
    t = time(NULL);
    cout << ctime(&t) << endl;
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
  if (parameters.count("config"))
    options.config_file_ = parameters["config"].as<string>();
  if (parameters.count("input")) {
    if (parameters.count("input-force"))
      LOG_FATAL() << "Do not specify both -i (input file) and -I (input-force file) at the same time";
    options.free_parameter_input_file_       = parameters["input"].as<string>();
    options.force_overwrite_of_addressables_ = false;
  }
  if (parameters.count("input-force")) {
    if (parameters.count("input"))
      LOG_FATAL() << "Do not specify both -i (input file) and -I (input-force file) at the same time";
    options.free_parameter_input_file_       = parameters["input-force"].as<string>();
    options.force_overwrite_of_addressables_ = true;
  }
  if (parameters.count("nostd"))
    options.no_std_report_ = true;
  if (parameters.count("output")) {
    options.create_free_parameter_output_file_ = true;
    options.free_parameter_output_file_        = parameters["output"].as<string>();
    options.free_parameter_write_mode_         = PARAM_OVERWRITE;
  }
  //  if (parameters.count("Output")) {
  //    options.create_free_parameter_output_file_ = true;
  //    options.free_parameter_output_file_        = parameters["output"].as<string>();
  //    options.free_parameter_write_mode_         = PARAM_APPEND;
  //  }
  if (parameters.count("single-step"))
    options.single_step_model_ = true;
  if (parameters.count("tabular"))
    options.tabular_reports_ = true;
  if (parameters.count("phases"))
    options.estimation_phases_ = parameters["phases"].as<unsigned>();
  if (parameters.count("Estimate")) {
    options.create_mpd_output_file_ = true;
    options.mpd_output_file_        = parameters["Estimate"].as<string>();
  }
  if (parameters.count("mcmc"))
    options.estimate_before_mcmc_ = true;
  if (parameters.count("mcmc-from-estimate")) {
    options.estimate_before_mcmc_ = false;
    options.mpd_input_file_       = parameters["mcmc-from-estimate"].as<string>();
  }

  /**
   * At this point we know we've been asked to do an actual model
   * run. So we need to check to ensure the command line makes
   * sense.
   */
  unsigned run_mode_count = 0;
  run_mode_count += parameters.count("run");
  run_mode_count += parameters.count("estimate");
  run_mode_count += parameters.count("Estimate");
  run_mode_count += parameters.count("mcmc");
  run_mode_count += parameters.count("mcmc-from-estimate");
  run_mode_count += parameters.count("resume");
  run_mode_count += parameters.count("profile");
  run_mode_count += parameters.count("simulation");
  run_mode_count += parameters.count("projection");
  run_mode_count += parameters.count("query");
  run_mode_count += parameters.count("unittest");

  if (run_mode_count == 0)
    LOG_FATAL() << "Command line error: No valid run mode has been specified on the command line. Please specify a valid run mode (e.g., '-r')";
  if (run_mode_count > 1)
    LOG_FATAL() << "Command line error: Multiple run modes have been specified on the command line. Please specify one run mode only";

  if (parameters.count("run")) {
    options.run_mode_ = RunMode::kBasic;
  } else if (parameters.count("estimate") || parameters.count("Estimate")) {
    options.run_mode_ = RunMode::kEstimation;
  } else if (parameters.count("mcmc") || parameters.count("mcmc-from-estimate") || parameters.count("resume")) {
    options.run_mode_ = RunMode::kMCMC;
    if (parameters.count("resume")) {
      options.estimate_before_mcmc_ = false;
      options.mpd_input_file_       = parameters["resume"].as<string>();
      if (!(parameters.count("objective-file") && parameters.count("sample-file"))) {
        LOG_FATAL() << "Command line error: Resuming an MCMC chain requires the objective-file and sample-file parameters";
      }
      options.mcmc_objective_file_ = parameters["objective-file"].as<string>();
      options.mcmc_sample_file_    = parameters["sample-file"].as<string>();
      options.resume_mcmc_chain_   = true;
    }

    if (!options.estimate_before_mcmc_ && options.mpd_input_file_ == "")
      LOG_FATAL() << "Command line error: Cannot start a MCMC run without an estimation if there is no MPD file name specified";
    if (options.estimate_before_mcmc_ && options.mpd_input_file_ != "")
      LOG_FATAL() << "Command line error: Cannot specify the --mcmc-from-estimate and --resume at the same time";

    if (options.create_free_parameter_output_file_)
      LOG_FATAL() << "Command line error: Cannot specify an MCMC and a free parameter output file with --output at the same time";

  } else if (parameters.count("profile")) {
    options.run_mode_ = RunMode::kProfiling;
  } else if (parameters.count("simulation")) {
    if (!(parameters.count("input") || !parameters.count("input-force")))
      LOG_FATAL() << "A free parameter file must be supplied (using -i/-I) for simulations";
    options.run_mode_              = RunMode::kSimulation;
    options.simulation_candidates_ = parameters["simulation"].as<unsigned>();
  } else if (parameters.count("projection")) {
    if (!(parameters.count("input") || parameters.count("input-force")))
      LOG_FATAL() << "A free parameter file must be supplied (using -i/-I) for projections";
    options.run_mode_              = RunMode::kProjection;
    options.projection_candidates_ = parameters["projection"].as<unsigned>();
  } else {
    LOG_FATAL() << "Command line error: An invalid or unknown run mode has been specified";
  }

  if (parameters.count("skip-verify")) {
    options.skip_verify_ = true;
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
