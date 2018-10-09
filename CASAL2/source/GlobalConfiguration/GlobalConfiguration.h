/**
 * @file GlobalConfiguration.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
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
#include <vector>
#include <string>

#include "BaseClasses/Object.h"
#include "Translations/Translations.h"
#include "Utilities/RunParameters.h"
#include "Utilities/To.h"

// Namespaces
using std::map;
using std::vector;
using std::string;

namespace util = niwa::utilities;

namespace niwa {
class Model;

/**
 * Class Definitiion
 */
class GlobalConfiguration {
public:
  // Methods
  GlobalConfiguration() = default;
  virtual                     ~GlobalConfiguration() = default;
  void                        Clear();
  void                        ParseOptions(Model* model);

  // Accessors and Mutators
  void                  set_command_line_parameters(vector<string> &parameters) { command_line_parameters_ = parameters; }
  vector<string>&       command_line_parameters() { return command_line_parameters_; }
  void                  set_run_parameters(utilities::RunParameters& options) { options_ = options; }
  bool                  debug_mode() { return options_.debug_mode_; }
  unsigned              random_seed() { return options_.random_number_seed_; }
  string                config_file() { return options_.config_file_; }
  unsigned              simulation_candidates() const { return options_.simulation_candidates_; }
  unsigned              projection_candidates() const { return options_.projection_candidates_; }
  string                estimable_value_file() const { return options_.estimable_value_input_file_; }
  bool                  force_estimable_values_file() { return options_.force_estimables_as_named_; }
  bool                  disable_standard_report() { return options_.no_std_report_; }
  bool                  resume() const { return options_.resume_mcmc_chain_; }
  string                mcmc_sample_file() const { return options_.mcmc_sample_file_; }
  string                mcmc_objective_file() const { return options_.mcmc_objective_file_; }
  bool                  single_step() const { return options_.single_step_model_; }
  bool                  print_tabular() const { return options_.tabular_reports_; }
  string                object_to_query() const { return options_.query_object_; }
  bool                  skip_loading_config_file() const { return skip_loading_config_file_; }
  void                  flag_skip_config_file() { skip_loading_config_file_ = true; }
  bool                  create_mpd_file() const { return options_.create_mpd_file_; }
  unsigned              estimation_phases() const { return options_.estimation_phases_; }

private:
  // Members
  vector<string>              command_line_parameters_;
  utilities::RunParameters    options_;
  bool                        skip_loading_config_file_;
};
} /* namespace niwa */
#endif /* GLOBALCONFIGURATION_H_ */
