/**
 * @file MPDLoader.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 2/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "MPDLoader.h"

#include <string>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "Minimisers/Manager.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Model/Objects.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace configuration {
namespace ublas = boost::numeric::ublas;

using std::streambuf;
using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;
using std::ios_base;

/**
 * Check if the file we're looking for exists
 */
inline bool DoesFileExist(const string& file_name) {
  ifstream  file(file_name.c_str());
  if (file.fail() || !file.is_open())
    return false;

  file.close();
  return true;
}


/**
 * Default constructor
 */
MPDLoader::MPDLoader(Model* model) : model_(model) {
}

/**
 * This method will load our MPD file. The file contains
 * a list of values to be assigned to estimables and
 * a covariance matrix for our minimiser
 */
bool MPDLoader::LoadMPDFile() {
//
//  string mpd_file_name = "mpd_out";
//  /**
//   * Figure out the name of the mpd file we want to use. We need to
//   * look for the last created one.
//   */
//  string good_file_name = "";
//  if (!DoesFileExist(mpd_file_name)) {
//    LOG_FATAL() << "MPD file " << mpd_file_name << " does not exist";
//  }
//
//  LOG_FINE() << "Loading MPD from file: " << mpd_file_name;
//
//  ifstream  file(mpd_file_name.c_str());
//
//  string current_line = "";
//  if (!getline(file, current_line) && current_line != "* MPD") {
//    LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format. Please use a valid MPD file";
//  }
//
//  if (!getline(file, current_line) && current_line != "estimate_values:") {
//    LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format. Please use a valid MPD file";
//  }
//
//  /**
//   * Load the next line which should contain a space separated list of
//   * estimable labels
//   */
//  if (!getline(file, current_line)) {
//    LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format. Please use a valid MPD file";
//  }
//
//  vector<string> estimable_labels;
//  boost::split(estimable_labels, current_line, boost::is_any_of(" "), boost::token_compress_on);
//
//  /**
//   * Load the next line which should contain a space separated list
//   * of estimable values
//   */
//  if (!getline(file, current_line)) {
//    LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format. Please use a valid MPD file";
//  }
//
//  vector<string> estimable_values;
//  boost::split(estimable_values, current_line, boost::is_any_of(" "), boost::token_compress_on);
//
//  /**
//   * Load estimable values into a map
//   */
//  if (estimable_labels.size() != estimable_values.size()) {
//    LOG_FATAL() << "The number of estimable labels (" << estimable_labels.size() << ") does not match the " <<
//        "number of estimable values (" << estimable_values.size() << ")";
//  }
//
//  map<string, Double> estimables;
//  for (unsigned i = 0; i << estimable_labels.size(); ++i) {
//    Double value = 0;
//    if (!utilities::To<string, Double>(estimable_values[i], value)) {
//      LOG_FATAL() << "Could not convert the value " << estimable_values[i] << " to a numeric for the estimable "
//          << estimable_labels[i];
//    }
//
//    estimables[estimable_labels[i]] = value;
//  }
//
//  /**
//   * Populate our estimables with new values
//   */
//  string error = "";
//  for (auto estimable : estimables) {
//    Double* target = model_->objects().FindEstimable(estimable.first, error);
//    if (target == nullptr) {
//      LOG_FATAL() << "MPD file " << mpd_file_name << " produced error: " << error;
//    }
//
//    *target = estimable.second;
//  }
//
//  /**
//   * Check for covariance header line
//   */
//  if (!getline(file, current_line) && current_line != "covariance_matrix:") {
//    LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format. Please use a valid MPD file";
//  }
//
//  /**
//   * Load Covariance Matrix
//   */
//  unsigned count = estimable_labels.size();
//  LOG_FINE() << "count: " << count;
//  auto covariance_matrix = model_->managers().minimiser()->active_minimiser()->covariance_matrix();
//  covariance_matrix.resize(count, count);
//  vector<string> split_values;
//  for (unsigned i = 0; i < count; ++i) {
//    if (!getline(file, current_line)) {
//      LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format. Failed to load line "
//          << i + 1 << " of the covariance matrix";
//    }
//
//    boost::split(split_values, current_line, boost::is_any_of(" "), boost::token_compress_on);
//    if (split_values.size() != estimable_labels.size()) {
//      LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format. Please use a valid MPD file";
//    }
//
//    for (unsigned j = 0; j < count; ++j) {
//      LOG_FINE() << "i: " << i << ", j: " << j << ", value: " << split_values[j];
//      Double value = 0;
//      if (!utilities::To<string, Double>(split_values[j], value)) {
//        LOG_FATAL() << "MPD file " << mpd_file_name << " is not in the correct format."
//            << " Value " << split_values[j] << " could not be converted to a numeric";
//      }
//
//      covariance_matrix(i, j) = value;
//    }
//  }

  return true;
}

} /* namespace configuration */
} /* namespace niwa */
