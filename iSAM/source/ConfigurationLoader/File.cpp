/**
 * @file File.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 */

// Headers
#include "File.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Loader.h"
#include "Logging/Logging.h"
#include "Utilities/String.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {
namespace configuration {

namespace util = niwa::utilities;

/**
 * Destructor
 */
File::~File() {
  if (file_.is_open())
    file_.close();
}

/**
 * Attempt to open our configuration file
 *
 * @param file_name The name of the file to open
 * @return true on success, false on failure
 */
bool File::OpenFile(string file_name) {
  LOG_TRACE();

  file_name_ = file_name;

  file_.open(file_name_.c_str());
  if (file_.fail() || !file_.is_open())
    return false;

  return true;
}

/**
 * Parse the configuration file. Creating objects and loading
 * the parameter objects
 */
void File::Parse() {
  LOG_TRACE();

  if (file_.fail() || !file_.is_open())
    LOG_CODE_ERROR() << "Unable to parse the configuration file because a previous error has not been reported.\nFile: " << file_name_;

  /**
   * Iterate through our file parsing the contents
   */
  string    current_line        = "";
  while (getline(file_, current_line)) {
    ++line_number_;

    if (current_line.length() == 0)
      continue;

    // Handle comments
    HandleComments(current_line);

    if (current_line.length() == 0)
      continue;

    /**
     * Change tabs to spaces, remove any leading/trailing or multiple spaces
     * so we can be sure the input is nicely formatted
     */
    boost::replace_all(current_line, "\t", " ");
    boost::trim_all(current_line);
    LOG_FINEST() << "current_line == '" << current_line << "'";

    /**
     * Now we need to check if this line is an include line for a new
     * file.
     */
    if (current_line.length() > strlen(CONFIG_INCLUDE) + 2) {
      string lower_line = util::ToLowercase(current_line);
      if (current_line.substr(0, strlen(CONFIG_INCLUDE)) == CONFIG_INCLUDE) {
        string include_name = current_line.substr(strlen(CONFIG_INCLUDE));
        LOG_FINEST() << "Loading new configuration file via include " << include_name;

        boost::replace_all(include_name, "\"", "");
        boost::trim_all(include_name);
        File include_file(loader_);
        if (!include_file.OpenFile(include_name))
          LOG_FATAL() << "At line: " << line_number_ << " of " << file_name_
              << ": Include file '" << include_name << "' could not be opened. Does this file exist?";

        include_file.Parse();
        continue;
      }
    }


    /**
     * At this point everything is standard. We have a simple line of text that we now need to parse. All
     * comments etc have been removed and we've gone through any include_file directives
     */
    FileLine current_file_line;
    current_file_line.file_name_    = file_name_;
    current_file_line.line_number_  = line_number_;
    current_file_line.line_         = current_line;

    loader_.AddFileLine(current_file_line);
  } // while(get_line())
}

/**
 * This method will parse the line continually looking for comments
 * and removing them when they are found.
 *
 * @param current_line The current line to parse
 */
void File::HandleComments(string& current_line) {

  /**
   * Trigger is flagged when we find a comment in the line. If a comment
   * was triggered we'll re-loop to pick up any extra comments in the lines
   *
   */
  bool trigger = true;
  while (trigger) {
    trigger = false;

    /**
     * If we're not in a multi line comment, we're safe to remove any single line
     * comments from the line.
     */
    if (!multi_line_comment_) {
      size_t single_line_pos = current_line.find_first_of(CONFIG_SINGLE_COMMENT);
      size_t multi_comment_start = current_line.find_first_of(CONFIG_MULTI_COMMENT_START);

      bool process_single_line = false;
      if (multi_comment_start == string::npos && single_line_pos != string::npos)
        process_single_line = true;
      if (multi_comment_start != string::npos && single_line_pos != string::npos && single_line_pos < multi_comment_start)
        process_single_line = true;

      if (process_single_line) {
        trigger = true;
        current_line = current_line.substr(0, single_line_pos);
        continue;
      }
    }

    /**
     * If we're in a multi-line comment then we're ok to look for the end of it in the current line.
     */
    if (multi_line_comment_) {
      size_t pos = current_line.find_first_of(CONFIG_MULTI_COMMENT_END);
      if (pos == string::npos) {
        current_line = "";
        continue;
      }

      multi_line_comment_ = false;
      trigger = true;
      current_line = current_line.substr(pos + 1, current_line.length() - pos);
    }

    /**
     * Look for a new multi-line comment to begin. Then if we begin one look for the ending
     * which maybe right after it.
     */
    size_t start_pos = current_line.find_first_of(CONFIG_MULTI_COMMENT_START);
    if (start_pos != string::npos) {
      size_t end_pos = current_line.find_first_of(CONFIG_MULTI_COMMENT_END, start_pos);
      if (end_pos != string::npos) {
        current_line = current_line.substr(0, start_pos) + current_line.substr(end_pos+1);
        trigger = true;

      } else {
        multi_line_comment_ = true;
        current_line = current_line.substr(0, start_pos);
      }
    }
  }
}

} /* namespace configuration */
} /* namespace niwa */



















