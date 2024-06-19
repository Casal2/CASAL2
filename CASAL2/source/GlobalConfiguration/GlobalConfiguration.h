/**
 * @file GlobalConfiguration.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
#include <string>
#include <vector>

#include "../BaseClasses/Object.h"
#include "../Translations/Translations.h"
#include "../Utilities/RunParameters.h"
#include "../Utilities/To.h"

// Namespaces
using std::map;
using std::string;
using std::vector;

namespace util = niwa::utilities;

namespace niwa {
class Model;

/**
 * Class Definitiion
 */
class GlobalConfiguration {
public:
  // Methods
  GlobalConfiguration()          = default;
  virtual ~GlobalConfiguration() = default;
  void Clear();
  void ParseOptions();

  // Accessors and Mutators
  void            set_command_line_parameters(vector<string>& parameters) { command_line_parameters_ = parameters; }
  vector<string>& command_line_parameters() { return command_line_parameters_; }
  void            set_run_parameters(utilities::RunParameters& options) { options_ = options; }
  // Generic Options
  void          set_standard_header(const string& header) { standard_header_ = header; }
  const string& standard_header() const { return standard_header_; }
  unsigned      random_seed() { return options_.random_number_seed_; }
  vector<string>& config_file() { return options_.config_file_; }
  bool          disable_all_reports() const { return options_.disable_all_reports_; }

  unsigned simulation_candidates() const { return options_.simulation_candidates_; }
  unsigned projection_candidates() const { return options_.projection_candidates_; }

  void   set_disable_standard_report(bool value) { options_.no_std_report_ = value; }
  bool   disable_standard_report() { return options_.no_std_report_; }
  bool   single_step() const { return options_.single_step_model_; }
  bool   print_tabular() const { return options_.tabular_reports_; }
  string object_to_query() const { return options_.query_object_; }

  void set_continue_pass_verify(bool value) { options_.continue_pass_verify_ = value; }
  bool get_continue_pass_verify() const { return options_.continue_pass_verify_; }

  // Estimation options
  bool   create_mpd_output_file() const { return options_.create_mpd_output_file_; }
  string get_mpd_output_file() const { return options_.mpd_output_file_; }
  void   set_mpd_output_file(const string& value) { options_.mpd_output_file_ = value; }
  string mpd_write_mode() const { return options_.mpd_write_mode_; }

  string get_mpd_input_file() const { return options_.mpd_input_file_; }
  void   set_mpd_input_file(const string& value) { options_.mpd_input_file_ = value; }
  bool   force_overwrite_of_addressables() { return options_.force_overwrite_of_addressables_; }

  bool   create_free_parameter_output_file() const { return options_.create_free_parameter_output_file_; }
  string get_free_parameter_output_file() const { return options_.free_parameter_output_file_; }
  void   set_free_parameter_output_file(const string& value) { options_.free_parameter_output_file_ = value; }
  string free_parameter_write_mode() const { return options_.free_parameter_write_mode_; }

  string get_free_parameter_input_file() const { return options_.free_parameter_input_file_; }
  void   set_free_parameter_input_file(const string& value) { options_.free_parameter_input_file_ = value; }

  unsigned estimation_phases() const { return options_.estimation_phases_; }

  // MCMC options
  bool   resume_mcmc() const { return options_.resume_mcmc_chain_; }
  void   set_estimate_before_mcmc(bool value) { options_.estimate_before_mcmc_ = value; }
  bool   estimate_before_mcmc() const { return options_.estimate_before_mcmc_; }
  string mcmc_sample_file() const { return options_.mcmc_sample_file_; }
  string mcmc_objective_file() const { return options_.mcmc_objective_file_; }
  bool   estimation_is_for_mcmc() const { return options_.estimation_is_for_mcmc_; }

  // Test options
  bool skip_loading_config_file() const { return skip_loading_config_file_; }
  void set_skip_config_file(bool value) { skip_loading_config_file_ = value; }

private:
  // Members
  vector<string>           command_line_parameters_;
  utilities::RunParameters options_;
  bool                     skip_loading_config_file_ = false;
  string                   standard_header_          = "";
};
} /* namespace niwa */
#endif /* GLOBALCONFIGURATION_H_ */
