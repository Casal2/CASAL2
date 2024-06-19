/**
 * @file RunParameters.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 12, 2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
#include <vector>

#include "../Translations/Translations.h"
#include "../Utilities/RunMode.h"

// namespaces
namespace niwa {
namespace utilities {
using std::string;
using std::vector;

// structs
struct RunParameters {
  // top level options
  RunMode::Type run_mode_             = RunMode::kInvalid;
  string        model_type_           = "";
  vector<string> config_file_         = {"config.csl2"};
  string        log_level_            = "information";
  bool          continue_pass_verify_ = false;  // Skip verify halting execution if warnings are produced

  // basic run mode options
  bool single_step_model_ = false;

  // mcmc configuration options
  bool   estimate_before_mcmc_ = false;
  bool   resume_mcmc_chain_    = false;
  string mcmc_objective_file_  = "";
  string mcmc_sample_file_     = "";
  string mpd_data_             = "";  // String to hold MPD record from estimation for the MCMC

  // estimation configuration options
  string   minimiser_                       = "";
  unsigned estimation_phases_               = 1;
  bool     force_overwrite_of_addressables_ = false;
  bool     estimation_is_for_mcmc_          = false;  // are we running in estimation run mode before a MCMC

  // query run mode options
  string query_object_ = "";  // name of the Casal2 object to query

  // simulation run mode options
  unsigned simulation_candidates_ = 1u;

  // projection run mode options
  unsigned projection_candidates_ = 1u;

  // random number options
  unsigned random_number_seed_ = 123u;  // default random number seed

  // reporting options
  bool        tabular_reports_         = false;  // print reports in tabular format
  bool        disable_all_reports_     = false;  // disable all reports
  bool        no_std_report_           = false;  // skip standard header
  bool        print_std_report_header_ = true;   // print top part of std header
  bool        print_std_report_footer_ = true;   // print bottom part of std header
  std::time_t std_report_time_         = time(0);

  // MPD and free parameter file options
  string mpd_input_file_         = "";
  string mpd_output_file_        = "";
  bool   create_mpd_output_file_ = false;
  string mpd_write_mode_         = PARAM_OVERWRITE;

  string free_parameter_input_file_         = "";
  string free_parameter_output_file_        = "";
  bool   create_free_parameter_output_file_ = false;
  string free_parameter_write_mode_         = PARAM_OVERWRITE;
};

}  // namespace utilities
}  // namespace niwa

#endif /* RUNPARAMETERS_H_ */
