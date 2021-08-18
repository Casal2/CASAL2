/**
 * @file RunParameters.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 12, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science (c)2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains the struct definition for the object we will
 * pass in to the Casal2 DLL holding all of the optional information
 * we need to give it.
 *
 * If we need to keep information between the run modes/dll/shared object
 * calls then we store it in this object so that it can be passed back and
 * forth without having to write to disk etc.
 */
#ifndef RUNPARAMETERS_H_
#define RUNPARAMETERS_H_

// headers
#include <ctime>
#include <string>

#include "../Utilities/RunMode.h"

// namespaces
namespace niwa {
namespace utilities {
using std::string;

// structs
struct RunParameters {
  // top level options
  RunMode::Type run_mode_    = RunMode::kInvalid;
  string        model_type_  = "";
  string        config_file_ = "config.csl2";
  string        log_level_   = "warning";

  // basic run mode options
  bool single_step_model_ = false;

  // mcmc configuration options
  bool   estimate_before_mcmc_ = false;
  string mcmc_mpd_file_name_   = "";
  bool   resume_mcmc_chain_    = false;
  string mcmc_objective_file_  = "";
  string mcmc_sample_file_     = "";
  string mpd_data_             = "";  // String to hold MPD record from estimation for the MCMC

  // estimation configuration options
  string   minimiser_                       = "";
  unsigned estimation_phases_               = 1;
  string   estimable_value_input_file_      = "";
  bool     force_overwrite_of_addressables_ = false;
  bool     estimation_is_for_mcmc_          = false;  // are we running in estimation run mode before a MCMC

  // query run mode options
  string query_object_ = "";  // name of the Casal2 object to query

  // simulation run mode options
  unsigned simulation_candidates_ = 1u;

  // projection run mode options
  unsigned projection_candidates_ = 1u;

  // random number options
  unsigned random_number_seed_          = 123u;   // default random number seed
  bool     override_random_number_seed_ = false;  // override rng seed can come from command line parameters
  unsigned override_rng_seed_value_     = 123u;   // seed to override our default with

  // reporting options
  bool        tabular_reports_         = false;  // print reports in tabular format
  bool        disable_all_reports_     = false;  // disable all reports
  bool        no_std_report_           = false;  // skip standard header
  bool        print_std_report_header_ = true;   // print top part of std header
  bool        print_std_report_footer_ = true;   // print bottom part of std header
  std::time_t std_report_time_         = time(0);

  // unknown options
  string output_            = "";
  bool   create_mpd_file_   = false;
  string mpd_output_append_ = "";
  string mpd_file_name_     = "";
};

}  // namespace utilities
}  // namespace niwa

#endif /* RUNPARAMETERS_H_ */
