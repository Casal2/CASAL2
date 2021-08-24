/**
 * @file MPD.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Feb 9, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

// headers
#include "MPD.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <ostream>
#include <vector>

#include "../Estimables/Estimables.h"
#include "../Estimates/Manager.h"
#include "../Logging/Logging.h"
#include "../MCMCs/Manager.h"
#include "../Minimisers/Manager.h"
#include "../Minimisers/Minimiser.h"
#include "../Model/Managers.h"
#include "../Model/Model.h"
#include "../Utilities/To.h"
#include "../Utilities/Types.h"

// namespaces
namespace niwa {
using niwa::utilities::Double;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostringstream;
using std::vector;

/**
 * @brief Load an MPD block from a string into a proper
 * MPD that can be used by the system.
 * We just split the string based on new-lines and add
 * it to the same vector we would if we're loading it from disk
 *
 * @param mpd_block
 */
void MPD::ParseString(const string& mpd_block) {
  vector<string> lines;
  boost::split(lines, mpd_block, boost::is_any_of("\n"));
  for (string line : lines) file_lines_.push_back(line);

  ParseFile();
}

/**
 * @brief
 *
 * @param file_name
 * @return true
 * @return false
 */
bool MPD::LoadFromDiskToMemory(const string& file_name) {
  ifstream file;
  file.open(file_name.c_str());
  if (file.fail() || !file.is_open()) {
    LOG_FATAL() << "Failed to find the MPD file " << file_name;
  }

  string line = "";
  while (getline(file, line)) file_lines_.push_back(line);

  if (file_lines_.size() == 0)
    LOG_FATAL() << "File " << file_name << " was empty. Could not load any MPD data";
  if (file_lines_[0].substr(0, 4) != "*mpd")
    LOG_FATAL() << "The file " << file_name << " is not a valid MPD file. It should start with '*mpd' and contain the MPD point estimates and the covariance matrix";

  file_name_ = file_name;
  file.close();

  ParseFile();
  LOG_INFO() << "Loading MPD from file " << file_name;
  return true;
}

/**
 * @brief Parse the MPD file data that has been loaded into the file_lines_
 * vector. We store it in a vector so we can populate it manually
 * during unit tests instead of parsing as we load from disk.
 */
void MPD::ParseFile() {
  unsigned idx = 0;

  // Lambda to get the next line from our vector easily
  auto get_next_line = [&idx, this]() {
    if (idx >= this->file_lines_.size())
      LOG_FATAL() << "Could not get line " << idx + 1 << " from the MPD file " << this->file_name_;

    string s = this->file_lines_[idx];
    boost::replace_all(s, "\t", " ");
    boost::trim_all(s);
    ++idx;
    return s;
  };

  string line = get_next_line();
  if (line.substr(0, 4) != "*mpd")
    LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. It should start with '*mpd' and contain the MPD point estimates and the covariance matrix";

  line = get_next_line();
  if (line != "estimate_values:")
    LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. The second line must be 'estimate_values:'";

  // third line should contain list of addressables
  line = get_next_line();
  vector<string> parameters;
  boost::split(parameters, line, boost::is_any_of(" "));

  if (parameters.size() == 0)
    LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. Parameters must be space separated on line 3";

  // fourth line is the value for each addressable
  line = get_next_line();
  vector<string> values;
  boost::split(values, line, boost::is_any_of(" "));
  if (values.size() != parameters.size())
    LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. The number of values supplied was " << values.size() << " when " << parameters.size()
                << " were expected";

  for (unsigned i = 0; i < values.size(); ++i) {
    boost::trim_all(parameters[i]);
    boost::trim_all(values[i]);

    double numeric = 0.0;
    if (!utilities::To<double>(values[i], numeric))
      LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. Could not convert the value " << values[i] << " to a double";

    auto estimate = model_->managers()->estimate()->GetEstimate(parameters[i]);
    if (!estimate)
      LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. Estimate " << parameters[i] << " was defined in MPD file but was not found";

    estimate->set_value(numeric);
  }

  // fifth line should be 'covariance_matrix:'
  line = get_next_line();
  if (line != "covariance_matrix:")
    LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. The second line of MPD data must be 'covariance_matrix:'";

  // now we load the covariance matrix data
  auto estimate_count = model_->managers()->estimate()->GetIsEstimatedCount();
  auto active_mcmc    = model_->managers()->mcmc()->active_mcmc();
  if (active_mcmc == nullptr)
    LOG_FATAL() << "No active MCMC to set covariance matrix on";
  auto& covariance_matrix = model_->managers()->mcmc()->active_mcmc()->covariance_matrix();
  covariance_matrix.resize(estimate_count, estimate_count);
  for (unsigned i = 0; i < estimate_count; ++i) {
    line = get_next_line();
    if (line == "")
      LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. Failed to load line " << i + 1 << " of the covariance matrix";

    // split the line
    vector<string> estimable_values;
    boost::trim_all(line);
    boost::split(estimable_values, line, boost::is_any_of(" "), boost::token_compress_on);
    if (estimate_count != estimable_values.size()) {
      LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. Line " << i + 1 << " of the covariance matrix had " << estimable_values.size() << " values when "
                  << estimate_count << " were expected";
    }

    double value = 0;
    for (unsigned j = 0; j < estimate_count; ++j) {
      if (!utilities::To<string, double>(estimable_values[j], value))
        LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. The value " << estimable_values[j] << " could not be converted to a double";
      covariance_matrix(i, j) = value;
    }
  }

  line = get_next_line();
  if (line != REPORT_END)
    LOG_FATAL() << "The file " << file_name_ << " is not a valid MPD file. The last line must be '" << REPORT_END << "'";
}

/**
 * @brief Create a MPD in this object and store it in value_
 * from the master model parameter
 *
 * @param master_model The master model with current minimiser
 */
void MPD::CreateMPD(shared_ptr<Model> master_model) {
  auto minimiser_manager = master_model->managers()->minimiser();
  auto minimiser         = minimiser_manager->active_minimiser();
  if (!minimiser)
    LOG_CODE_ERROR() << "No active minimiser found";

  ostringstream cache;
  cache << "*mpd\n";

  /**
   * Print our Estimate Values
   */
  cache << "estimate_values:\n";
  auto estimates = master_model->managers()->estimate()->GetIsEstimated();
  for (auto estimate : estimates) cache << estimate->parameter() << " ";
  cache << REPORT_EOL;

  for (auto estimate : estimates) cache << AS_DOUBLE(estimate->value()) << " ";
  cache << REPORT_EOL;

  /**
   * Print our covariance matrix
   */
  cache << "covariance_matrix:\n";
  auto covariance_matrix = master_model->managers()->minimiser()->active_minimiser()->covariance_matrix();
  for (unsigned i = 0; i < covariance_matrix.size1(); ++i) {
    for (unsigned j = 0; j < covariance_matrix.size2(); ++j) cache << covariance_matrix(i, j) << " ";
    cache << REPORT_EOL;
  }
  cache << REPORT_END << "\n";

  value_ = cache.str();
}

} /* namespace niwa */
