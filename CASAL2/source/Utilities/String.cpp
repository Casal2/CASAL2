/**
 * @file String.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "String.h"

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Utilities/To.h"
#include "Logging/Logging.h"

// Namespaces
namespace niwa {
namespace utilities {

namespace util = niwa::utilities;

std::string String::find_invalid_characters(const std::string& test_string) {
 if (test_string.length() == 0)
   return "";

 std::string special_chars = ".-_";

 std::string invalid = "";
 std::for_each(test_string.begin(), test_string.end(),
#ifdef _MSC_VER
	 [&special_chars, &invalid](char c) { if (!isalpha(c) && !isdigit(c) && special_chars.find(c) == string::npos) invalid += c; }
#else
	 [&special_chars, &invalid](char c) { if (!std::isalpha(c) && !std::isdigit(c) && special_chars.find(c) == string::npos) invalid += c; }
#endif
 );





 return invalid;
}

vector<std::string> String::explode(const std::string& source) {
  vector<std::string> result;

  vector<std::string> chunks;
  boost::split(chunks, source, boost::is_any_of(","));

  for (std::string index : chunks) {
    if (index.find(":") != std::string::npos) {
      vector<string> pieces;
      boost::split(pieces, index, boost::is_any_of(":"));
      if (pieces.size() != 2)
        return vector<string>();

      unsigned first = 0;
      unsigned second = 0;

      if (!utilities::To<std::string, unsigned>(pieces[0], first))
        return vector<string>();
      if (!utilities::To<std::string, unsigned>(pieces[1], second))
        return vector<string>();

      if (first < second) {
        for (unsigned i = first; i <= second; ++i)
          result.push_back(utilities::ToInline<unsigned, std::string>(i));
      } else {
        for (unsigned i = first; i >= second; --i)
          result.push_back(utilities::ToInline<unsigned, std::string>(i));
      }

    } else
      result.push_back(index);
  }

  for (auto& str_value: result)
    boost::trim(str_value);

  return result;
}

/**
 *
 */
bool String::TrimOperators(vector<string>& line_values) {
  string line = boost::algorithm::join(line_values, " ");

  // join operator
  boost::replace_all(line, " +", "+");
  boost::replace_all(line, "+ ", "+");
  // sub-component operator
  boost::replace_all(line, " .", ".");
  boost::replace_all(line, ". ", ".");
  // list operator
  boost::replace_all(line, " ,", ",");
  boost::replace_all(line, ", ", ",");
  // range operator
  boost::replace_all(line, " :", ":");
  boost::replace_all(line, ": ", ":");
  // wildcard operator
  boost::replace_all(line, " *", "*");
  boost::replace_all(line, "* ", "*");
  // assignment operator
  boost::replace_all(line, " =", "=");
  boost::replace_all(line, "= ", "=");

  boost::split(line_values, line, boost::is_any_of(" "));
  return true;
}

/**
 * This method will search through the line for values that have operators in them and then
 * rebuild the vector with the values properly split out as they should be.
 *
 * Operators:
 * + = Join 2 values together as a single value
 * , = These values are 1 parameter (e.g 1,2,4,5
 * : = Split integer range values out (e.g 1:3 = 1,2,3
 *
 * @param line_values The vector containing the split parts we want to modify
 * @return true on success, false on failure
 */
bool String::HandleOperators(vector<string>& line_values, string &error) {
  TrimOperators(line_values);

  string line = boost::algorithm::join(line_values, " ");
  line_values.clear();
  LOG_FINEST() << "HandleOperators: " << line;

  /**
   * Branch the line back in to values now that we've removed
   * any weird spaces or tabs.
   */
  vector<string> temp_line_values;
  boost::split(temp_line_values, line, boost::is_any_of(" "));
  for (string line_value : temp_line_values) {

    if (line_value.find("=") != string::npos) {
      // handle the assignment format
      string output_line = "";
      HandleAssignment(line_value, output_line);
      line_values.push_back(output_line);

    } else {
      // the chunks is a 2D vector we're going to build then
      // put back together into the output
      vector<vector<string>> chunks;

      // break each line value in to segments (with . character)
      vector<string> line_value_segments;
      boost::split(line_value_segments, line_value, boost::is_any_of("."));

      for (unsigned i = 0; i < line_value_segments.size(); ++i) {
        chunks.push_back(vector<string>());

        vector<string> segment_chunks;
        boost::split(segment_chunks, line_value_segments[i], boost::is_any_of(","));
        for (string chunk : segment_chunks) {
          if (chunk.find(":") != string::npos) {
            string split_chunk = RangeSplit(chunk);
            vector<string> split_chunk_pieces;
            boost::split(split_chunk_pieces, split_chunk, boost::is_any_of(","));
            for (auto c : split_chunk_pieces)
              chunks[i].push_back(c);

          } else if (chunk.find("*") != string::npos && chunk != "*" && chunk != "*+") {
            /**
             * Handle the * multiplier operator. We split it on the *
             * character and then multiple the first segment by the
             * amount specified in the second segment
             */
            vector<string> temp;
            boost::split(temp, chunk, boost::is_any_of("*"));
            if (temp.size() != 2) {
              error = "Could not split line with * multiplier";
              return false;
            }

            unsigned multiplier = 0;
            if (!util::To<unsigned>(temp[1], multiplier)) {
              error = "Could not convert " + temp[1] + " to an unsigned int";
              return false;
            }

            for (unsigned index = 0; index < multiplier; ++index)
              chunks[i].push_back(temp[0]);
          } else
            chunks[i].push_back(chunk);
            // default is to do nothing
        }
      }

      /**
       * Build our outputs up
       */
      vector<string> combinations = (*chunks.rbegin());
      for (auto iter = chunks.rbegin() + 1; iter != chunks.rend(); ++iter) {
        vector<string> temp = combinations;
        combinations.clear();
        for (unsigned j = 0; j < (*iter).size(); ++j) {
          for (string value : temp) {
            value = (*iter)[j] + "." + value;
            combinations.push_back(value);
          }
        }
      }

      for (string combination : combinations)
        line_values.push_back(combination);
    }
  }// for line_value

  return true;
}

/**
 * This method will handle splitting pieces
 * of a line in to a range. When inputting a value
 * with a : that indicates a range it'll return
 * a comma separated list of values
 * e.g input = 2000:2003
 * output = 2000,2001,2002,2003
 *
 * @param range_value The value to parse and range
 * @return a string containing the values
 */
string String::RangeSplit(const string& range_value) {
  string result;

  LOG_FINEST() << "Range splitting: " << range_value;
  /**
   * Handle the : range operator. We split the chunk
   * in to 2 pieces and iterate between the two filling
   * in the range
   */
  vector<string> numerics;
  boost::split(numerics, range_value, boost::is_any_of(":"));
  if (numerics.size() != 2) {
    LOG_FATAL() << "line " << range_value << " could not be split into 2 pieces for a range";
  }

  int start_value;
  int end_value;
  vector<string> range;
  if (util::To<int>(numerics[0], start_value) && util::To<int>(numerics[1], end_value)) {
    LOG_FINEST() << "Ranging values: " << start_value << " to " << end_value;
    if (start_value < end_value) {
      for (int value = start_value; value <= end_value; ++value)
        range.push_back(util::ToInline<int, string>(value));
    } else {
      for (int value = start_value; value >= end_value; --value)
        range.push_back(util::ToInline<int, string>(value));
    }

    result = boost::algorithm::join(range, ",");

  }  else {
    LOG_FINE() << "Could not convert either " << numerics[0] << " or " << numerics[1] << " to an int";
    result = range_value;
  }

  return result;
}

/**
 * This method is responsible for parsing lines that
 * are doing assignments to a category
 */
void String::HandleAssignment(const string& input_line, string& output_line) {

    /**
     * We need to split based on the = operator now so we can
     * process each chunk individually.
     */
    vector<string> equals_split_pieces;
    boost::split(equals_split_pieces, input_line, boost::is_any_of("="));
    vector<vector<string>> new_equals_split_pieces(equals_split_pieces.size());

    for (unsigned i = 0; i < equals_split_pieces.size(); ++i) {
      string equals_split_chunk = equals_split_pieces[i];
      LOG_FINEST() << "equals_split_chunk: " << equals_split_chunk;

      vector<string> comma_split_pieces;
      boost::split(comma_split_pieces, equals_split_chunk, boost::is_any_of(","));
      for (auto comma_split_piece : comma_split_pieces) {
        LOG_FINEST() << "comma_split_piece: " << comma_split_piece;

        if (comma_split_piece.find(":") != string::npos) {
          string range = RangeSplit(comma_split_piece);
          new_equals_split_pieces[i].push_back(range);
        } else {
          new_equals_split_pieces[i].push_back(comma_split_piece);
        }
      }
    }

    vector<string> equals_bits;
    for (auto vec : new_equals_split_pieces) {
      string line = boost::algorithm::join(vec, ",");
      equals_bits.push_back(line);
    }

    output_line = boost::algorithm::join(equals_bits, "=");
}

} /* namespace utilities */
} /* namespace niwa */
