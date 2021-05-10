/**
 * @file RunParameters.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 12, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains the struct definition for the object we will
 * pass in to the CASAL2 DLL holding all of the optional information
 * we need to give it.
 */
#ifndef RUNPARAMETERS_H_
#define RUNPARAMETERS_H_

// headers
#include <string>

#include "../Utilities/RunMode.h"

// namespaces
namespace niwa {
namespace utilities {
using std::string;

// structs
struct RunParameters {
  RunMode::Type run_mode_                   = RunMode::kInvalid;
  string        config_file_                = "config.csl2";
  bool          resume_mcmc_chain_          = false;
  bool          skip_estimation_            = false;
  string        mcmc_objective_file_        = "";
  string        mcmc_sample_file_           = "";
  unsigned      estimation_phases_          = 1;
  string        estimable_value_input_file_ = "";
  bool          force_estimables_as_named_  = false;
  unsigned      random_number_seed_         = 123u;
  string        query_object_               = "";
  bool          debug_mode_                 = false;
  bool          no_std_report_              = false;
  string        log_level_                  = "warning";
  string        output_                     = "";
  bool          single_step_model_          = false;
  bool          tabular_reports_            = false;
  unsigned      simulation_candidates_      = 1u;
  unsigned      projection_candidates_      = 1u;

  bool     override_random_number_seed_ = false;
  unsigned override_rng_seed_value_     = 123u;

  string model_type_                = "";
  string minimiser_                 = "";
  bool   create_mpd_file_           = true;
  string mpd_file_name_             = "mpd.out";
  bool   skip_mpd_loading_for_mcmc_ = false;
};

}  // namespace utilities
}  // namespace niwa

#endif /* RUNPARAMETERS_H_ */
