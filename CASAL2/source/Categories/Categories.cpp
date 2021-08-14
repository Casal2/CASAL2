/**
 * @file Categories.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 */

// Headers
#include "Categories.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../AgeLengths/Age/None.h"
#include "../AgeLengths/AgeLength.h"
#include "../AgeLengths/Factory.h"
#include "../AgeLengths/Manager.h"
#include "../AgeWeights/Age/None.h"
#include "../AgeWeights/AgeWeight.h"
#include "../AgeWeights/Factory.h"
#include "../AgeWeights/Manager.h"
#include "../LengthWeights/Common/None.h"
#include "../LengthWeights/Factory.h"
#include "../LengthWeights/LengthWeight.h"
#include "../LengthWeights/Manager.h"
#include "../Logging/Logging.h"
#include "../Model/Model.h"
#include "../Utilities/String.h"
#include "../Utilities/To.h"

// Namespaces
namespace niwa {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Categories::Categories(shared_ptr<Model> model) : model_(model) {
  parameters_.Bind<string>(PARAM_FORMAT, &format_, "The format that the category names use", "");
  parameters_.Bind<string>(PARAM_NAMES, &names_, "The names of the categories", "");
  // Years commented out due to GitHub Issue: https://github.com/NIWAFisheriesModelling/CASAL2/issues/367
  // parameters_.Bind<string>(PARAM_YEARS, &years_, "The years that individual categories will be activated (if different from the model for these categories)", "", true);
  parameters_.Bind<string>(PARAM_AGE_LENGTHS, &age_length_labels_, "The age-length relationship labels for each category", "", true)->set_partition_type(PartitionType::kAge);
  parameters_.Bind<string>(PARAM_AGE_WEIGHT, &age_weight_labels_, "The age-weight relationships labelsfor each category", "", true)->set_partition_type(PartitionType::kAge);
}

/**
 * Populate any parameters,
 * Validate values are within expected ranges when bind<>() overloads cannot be used
 *
 * Note: all parameters are populated from configuration files
 */
void Categories::Validate() {
  if (model_->partition_type() == PartitionType::kPiApprox)
    return;

  // Check that we actually had a categories block
  if (block_type_ == "")
    LOG_ERROR() << "The @categories command is missing from the input configuration file. This is required";

  parameters_.Populate(model_);

  /**
   * Parameter: Names
   *
   * 1. Verify we have some names present
   * 2. Verify the names match the format string properly (right amount of segments)
   * 3. Create a new CategoryInfo object for each category and store it
   */

  // Parameter: Names
  // build the default years
  vector<unsigned> default_years;
  for (auto year : model_->years()) default_years.push_back(year);

  if (model_->partition_type() == PartitionType::kAge) {
    // Check the user hasn't specified both age_length and age_weight subcommands
    if (parameters_.Get(PARAM_AGE_WEIGHT)->has_been_defined() && parameters_.Get(PARAM_AGE_LENGTHS)->has_been_defined())
      LOG_ERROR_P(PARAM_AGE_WEIGHT) << "Both age_lengths and age_weights cannot be specified at the same time in @categories. Specify either one or the other.";
    if (parameters_.Get(PARAM_AGE_WEIGHT)->has_been_defined()) {
      if (age_weight_labels_.size() != names_.size())
        LOG_ERROR_P(PARAM_AGE_WEIGHT) << " number of age-weight labels (" << age_weight_labels_.size() << " were specified) must be the same as the number of categories ("
                                      << names_.size() << ")";
    }
    // get the age sizes
    if (parameters_.Get(PARAM_AGE_LENGTHS)->has_been_defined()) {
      if (age_length_labels_.size() != names_.size())
        LOG_ERROR_P(PARAM_AGE_LENGTHS) << " number of age-length labels (" << age_length_labels_.size() << " were specified) must be the same as the number of categories ("
                                       << names_.size() << ")";
    }
    vector<string> format_chunks;
    boost::split(format_chunks, format_, boost::is_any_of("."), boost::token_compress_on);
    // build our categories vector
    for (unsigned i = 0; i < names_.size(); ++i) {
      if (parameters_.Get(PARAM_AGE_LENGTHS)->has_been_defined()) {
        if (age_length_labels_.size() > i)
          category_age_length_labels_[names_[i]] = age_length_labels_[i];
      }

      if (parameters_.Get(PARAM_AGE_WEIGHT)->has_been_defined()) {
        if (age_weight_labels_.size() > i)
          category_age_weight_labels_[names_[i]] = age_weight_labels_[i];
      }
      // expand the names.
      vector<string> category_chunks;
      boost::split(category_chunks, names_[i], boost::is_any_of("."), boost::token_compress_on);
      if (category_chunks.size() != category_chunks.size())
        LOG_ERROR_P(PARAM_NAMES) << "The category named " << names_[i] << " does not match the format " << format_;

      // Create a new CategoryInfo object
      CategoryInfo new_category_info;
      new_category_info.name_    = names_[i];
      new_category_info.min_age_ = model_->min_age();
      new_category_info.max_age_ = model_->max_age();
      new_category_info.years_   = default_years;
      categories_[names_[i]]     = new_category_info;

      category_names_.push_back(names_[i]);
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    LOG_FATAL() << "Length-based partition models have not yet been implemented";
    // get the age sizes
    if (length_weight_labels_.size() > 0 && length_weight_labels_.size() != names_.size())
      LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << " number of length-weight labels (" << length_weight_labels_.size() << " were specified) must be the same as the number of categories ("
                                       << names_.size() << ")";

    vector<string> format_chunks;
    boost::split(format_chunks, format_, boost::is_any_of("."), boost::token_compress_on);
    // build our categories vector
    for (unsigned i = 0; i < names_.size(); ++i) {
      if (length_weight_labels_.size() > i)
        category_length_weight_labels_[names_[i]] = length_weight_labels_[i];

      // expand the names.
      vector<string> category_chunks;
      boost::split(category_chunks, names_[i], boost::is_any_of("."), boost::token_compress_on);
      if (category_chunks.size() != category_chunks.size())
        LOG_ERROR_P(PARAM_NAMES) << "The category named " << names_[i] << " does not match the format " << format_;

      // Create a new CategoryInfo object
      CategoryInfo new_category_info;
      new_category_info.name_    = names_[i];
      new_category_info.min_age_ = model_->min_age();
      new_category_info.max_age_ = model_->max_age();
      new_category_info.years_   = default_years;
      categories_[names_[i]]     = new_category_info;

      category_names_.push_back(names_[i]);
    }
  } else {
    LOG_FATAL() << "There is no functionality for model types that are not age or length";
  }

  for (string label : category_names_) {
    string invalid_characters = utilities::String::find_invalid_characters(label);
    if (invalid_characters != "")
      LOG_ERROR_P(PARAM_NAMES) << " category label " << label << " contains the invalid character(s) " << invalid_characters;
  }

  /**
   * Handle individual years for a category
   */
  map<string, string> category_values;
  vector<string>      years_split;
  auto                model_years = model_->years();
  for (auto year_lookup : years_) {
    category_values = GetCategoryLabelsAndValues(year_lookup, parameters_.Get(PARAM_YEARS)->location());
    for (auto iter : category_values) {
      if (categories_.find(iter.first) == categories_.end()) {
        LOG_FATAL_P(PARAM_YEARS) << "category " << iter.first << " does not exist.";
      }

      categories_[iter.first].years_.clear();
      boost::split(years_split, iter.second, boost::is_any_of(","), boost::token_compress_on);
      for (auto year : years_split) {
        unsigned actual_value = 0;
        if (!utilities::To<string, unsigned>(year, actual_value)) {
          LOG_FATAL_P(PARAM_YEARS) << "year " << year << " could not be converted to an unsigned integer";
        }

        if (std::find(categories_[iter.first].years_.begin(), categories_[iter.first].years_.end(), actual_value) != categories_[iter.first].years_.end()) {
          LOG_ERROR_P(PARAM_YEARS) << "value " << actual_value << " has already been defined for "
                                   << "the category " << iter.first;
        }

        if (std::find(model_years.begin(), model_years.end(), actual_value) == model_years.end()) {
          LOG_ERROR_P(PARAM_YEARS) << "value " << actual_value << " is not a valid year in the model";
        }
        categories_[iter.first].years_.push_back(actual_value);
      }
    }
  }
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */

void Categories::Build() {
  if (model_->partition_type() == PartitionType::kAge) {
    /**
     * Get our age length objects if age based partition model
     */
    if (parameters_.Get(PARAM_AGE_LENGTHS)->has_been_defined()) {
      agelengths::Manager* age_sizes_manager = model_->managers()->age_length();
      auto                 iter              = category_age_length_labels_.begin();
      for (; iter != category_age_length_labels_.end(); ++iter) {
        AgeLength* age_size = age_sizes_manager->FindAgeLength(iter->second);
        if (!age_size)
          LOG_ERROR_P(PARAM_AGE_LENGTHS) << "Age-length label (" << iter->second << ") was not found.";

        categories_[iter->first].age_length_ = age_size;
      }
    } else if (parameters_.Get(PARAM_AGE_WEIGHT)->has_been_defined()) {
      ageweights::Manager* age_weight_manager = model_->managers()->age_weight();
      auto                 iter               = category_age_weight_labels_.begin();
      for (; iter != category_age_weight_labels_.end(); ++iter) {
        AgeWeight* age_weight = age_weight_manager->FindAgeWeight(iter->second);
        if (!age_weight)
          LOG_ERROR_P(PARAM_AGE_WEIGHT) << "Age-weight label (" << iter->second << ") was not found.";

        categories_[iter->first].age_weight_ = age_weight;
      }
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    lengthweights::Manager* length_weight_manager = model_->managers()->length_weight();
    auto                    iter                  = category_length_weight_labels_.begin();
    for (; iter != category_length_weight_labels_.end(); ++iter) {
      LengthWeight* length_weight = length_weight_manager->GetLengthWeight(iter->second);
      if (!length_weight)
        LOG_ERROR_P(PARAM_LENGTH_WEIGHT) << "Length-weight label (" << iter->second << ") was not found.";

      categories_[iter->first].length_weight_ = length_weight;
    }
  }
}

/**
 * This method expands short-hand syntax into a vector with all of the elements
 * separated.
 *
 * e.g., male.immature sex=female == male.immature female.immature female.mature == 3 elements
 *
 * @param category_labels A vector of category definitions to iterate over and expand
 * @param parameter_location The location string for the parameter to print in case of error
 * @return a singular vector with each category as it's own element
 */
vector<string> Categories::ExpandLabels(const vector<string>& category_labels, const string& parameter_location) {
  vector<string> result;

  vector<string> temp;
  for (const string& label : category_labels) {
    temp = GetCategoryLabelsV(label, parameter_location);
    result.insert(result.end(), temp.begin(), temp.end());
  }

  return result;
}

/**
 * This method takes a lookup string and searches for the category labels.
 * The parameter_location parameter is passed in to print proper error messages.
 *
 * @param category_labels A vector of category definitions to iterate over and expand
 * @param parameter_location The location string for the parameter to print in case of error
 * @return a vector of category label
 */
vector<string> Categories::GetCategoryLabelsV(const string& lookup_string, const string& parameter_location) {
  string temp = GetCategoryLabels(lookup_string, parameter_location);

  vector<string> result;
  boost::split(result, temp, boost::is_any_of(" "));
  return result;
}

/**
 * This method takes a lookup string and parses it looking for the short-hand
 * syntax. The parameter_location parameter is passed in to print proper error messages.
 *
 * @param lookup_string The category definition to parse, short-hand or not
 * @param parameter_location Location of the parameter in the configuration file
 * @return String containing the new lookup string once it has been parsed
 */
string Categories::GetCategoryLabels(const string& lookup_string, const string& parameter_location) {
  /**
   * if we're asking for all categories then get a list of them all joined
   * by the + symbol
   */
  if (lookup_string == "*+") {
    string all = category_names_[0];
    for (unsigned i = 1; i < category_names_.size(); ++i) all += "+" + category_names_[i];

    return all;

  } else if (lookup_string == "*") {
    string all = category_names_[0];
    for (unsigned i = 1; i < category_names_.size(); ++i) all += " " + category_names_[i];

    return all;
  }

  /**
   * Check if we're just asking for a single category
   * or collection without any short-hand syntax
   */
  if (lookup_string.find("=") == string::npos)
    return lookup_string;

  /**
   * From here forward we're working with short-hand syntax.
   * Either
   * <format_chunk>=<lookup_string>[+]
   * or
   * format=<lookup_string_0>.<lookup_string_1>.etc[+]
   */
  string result = "";

  vector<string> pieces;
  boost::split(pieces, lookup_string, boost::is_any_of("="), boost::token_compress_on);

  if (pieces.size() != 2) {
    LOG_ERROR() << parameter_location << " short-hand category string (" << lookup_string << ") is not in the correct format, e.g., <format_chunk>=<lookup_chunk>";
  }

  boost::replace_all(pieces[0], " ", "");
  boost::replace_all(pieces[1], " ", "");

  string format = pieces[0];
  string lookup = pieces[1];

  bool use_plus_join = false;
  if (lookup.find("+") != string::npos) {
    use_plus_join = true;
    boost::replace_all(lookup, "+", "");
  }

  // organise categories split into segments to match against format
  map<string, vector<string>> category_pieces;
  for (string category : category_names_) {
    vector<string> temp;
    boost::split(temp, category, boost::is_any_of("."));
    category_pieces[category] = temp;
  }

  vector<string> matched_categories = category_names_;
  if (utilities::ToLowercase(format) == PARAM_FORMAT) {
    /**
     * this is a full length lookup string. It must have the
     * same number of pieces as our actual format string
     * e.g
     * format sex.stage.tag[+]
     * lookup_string male.*.*
     */
    boost::split(pieces, format_, boost::is_any_of("."));
    unsigned format_pieces = pieces.size();
    boost::split(pieces, lookup, boost::is_any_of("."));

    if (pieces.size() != format_pieces) {
      LOG_ERROR() << parameter_location << " short-hand category string ( " << lookup_string << ") does not have the correct number of sections. Expected " << format_pieces
                  << " but parsed " << pieces.size() << ". Pieces are chunks of the string separated with a '.' character";
    }

    for (unsigned i = 0; i < pieces.size(); ++i) {
      if (pieces[i] == "*")
        continue;

      vector<string> comma_separated_pieces;
      boost::split(comma_separated_pieces, pieces[i], boost::is_any_of(","));
      matched_categories.erase(
          std::remove_if(matched_categories.begin(), matched_categories.end(),
                         [&i, &category_pieces, &comma_separated_pieces](string& category) {
                           if (std::find(comma_separated_pieces.begin(), comma_separated_pieces.end(), category_pieces[category][i]) == comma_separated_pieces.end())
                             return true;

                           return false;
                         }),
          matched_categories.end());
    }

    LOG_FINEST() << "Full format parse of categories returned " << matched_categories.size() << " results";
  } else {
    /**
     * Here we have the shorter form of syntax
     * category_chunk=filter
     * e.g
     * sex=male
     */
    if (lookup.find(".") != string::npos) {
      LOG_ERROR() << parameter_location << " short-hand category string (" << lookup_string << ") is not in the correct format. The lookup component (" << lookup
                  << ") cannot contain any '.' characters";
    }

    // Verify we've actually got a good part of the format here.
    boost::split(pieces, format_, boost::is_any_of("."));
    unsigned format_offset = pieces.size();
    for (unsigned i = 0; i < pieces.size(); ++i) {
      if (pieces[i] == format) {
        format_offset = i;
        break;
      }
    }
    if (format_offset == pieces.size()) {
      LOG_ERROR() << parameter_location << " short-hand category syntax (" << lookup_string << ") is using an invalid format chunk (" << format << ") for its lookup. "
                  << "Valid format chunks must be taken from the format (" << format_ << ")";
    }

    if (lookup != "*") {
      matched_categories.erase(std::remove_if(matched_categories.begin(), matched_categories.end(),
                                              // lambda start
                                              [&format_offset, &parameter_location, &lookup, &lookup_string](string& category) {
                                                vector<string> chunks;
                                                boost::split(chunks, category, boost::is_any_of("."));
                                                if (chunks.size() <= format_offset) {
                                                  LOG_ERROR() << parameter_location << " short-hand category syntax (" << lookup_string << ") could not be compared to category ("
                                                              << category << ") because the category was malformed";
                                                }

                                                vector<string> comma_separated_pieces;
                                                boost::split(comma_separated_pieces, lookup, boost::is_any_of(","));
                                                if (std::find(comma_separated_pieces.begin(), comma_separated_pieces.end(), chunks[format_offset]) == comma_separated_pieces.end())
                                                  return true;
                                                return false;
                                              }),
                               matched_categories.end());  // End of .erase();
    }

    LOG_FINEST() << "Short format parse of categories returned " << matched_categories.size() << " results";
  }

  if (matched_categories.size() == 0) {
    LOG_ERROR() << parameter_location << " short-hand format string (" << lookup_string << ") did not match any of the categories.";
  }

  result = matched_categories[0];
  for (unsigned i = 1; i < matched_categories.size(); ++i) {
    if (use_plus_join)
      result += "+" + matched_categories[i];
    else
      result += " " + matched_categories[i];
  }

  return result;
}

/**
 * This method parses the lookup string and matches it to categories
 * that have been defined so that the values can be set.
 *
 * This supports
 * <category_name>=<value>
 * format=<lookup=<value>
 * <tag>=<lookup>=<value>
 *
 * This is used when setting individual lengths/ages/years on specific categories
 *
 * @param lookup The string
 * @param parameter_location String with file name and line of the parameter
 * @return a map of <category:value>
 */
map<string, string> Categories::GetCategoryLabelsAndValues(const string& lookup, const string& parameter_location) {
  map<string, string> results;

  vector<string> pieces;
  boost::split(pieces, lookup, boost::is_any_of("="), boost::token_compress_on);
  if (pieces.size() != 2 && pieces.size() != 3) {
    LOG_FATAL() << parameter_location << " short-hand category string (" << lookup << ") is not in the correct format, e.g., <format_chunk>=<lookup_chunk>=values";
  }

  string temp_lookup = pieces[0];
  if (pieces.size() == 3)
    temp_lookup += "=" + pieces[1];
  vector<string> categories = GetCategoryLabelsV(temp_lookup, parameter_location);
  for (auto category : categories) {
    if (results.find(category) != results.end()) {
      LOG_ERROR() << parameter_location << " category " << category << " is being assigned a value more than once";
    }

    results[category] = pieces.size() == 2 ? pieces[1] : pieces[2];
  }

  return results;
}

/**
 * Check if the parameter category label is a valid
 * category or not.
 *
 * @param label The label of the category to check for
 * @return true if category exists, false otherwise
 */
bool Categories::IsValid(const string& label) const {
  return std::find(category_names_.begin(), category_names_.end(), label) != category_names_.end();
}

/**
 * Check to see if the category label is a combination
 * of multiple categories. Combination categories contain the '+' symbol and
 * are used to aggregate partition elements together
 *
 * @param label Category label (e.g., male, or male+female)
 * @return True if the category is combination, false if not
 */
bool Categories::IsCombinedLabels(const string& label) const {
  return (label.find_first_of("+") != string::npos);
}

/**
 * Get the number of categories that have been defined in the combined label
 * e.g., this will return 2 for male+female, 3 for male+female+unsexed, etc.
 * 3 for male+female+unsexed etc
 *
 * @param The category label containing combined categories to check
 * @return The number of categories defined in the combined label
 */
unsigned Categories::GetNumberOfCategoriesDefined(const string& label) const {
  vector<string> category_labels;
  boost::split(category_labels, label, boost::is_any_of("+"));

  return category_labels.size();
}

/**
 * Get the minimum age for the target category
 *
 * @param category_name The name of the category
 * @return The minimum age allowed for the category
 */
unsigned Categories::min_age(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR() << "Could not find category_name " << category_name << " in the list of loaded categories";

  return categories_[category_name].min_age_;
}

/**
 * Get the maximum age for the target category
 *
 * @param category_name The name of the category
 * @return The maximum age allowed for the category
 */
unsigned Categories::max_age(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR() << "Could not find category_name " << category_name << " in the list of loaded categories";

  return categories_[category_name].max_age_;
}

/**
 * Get the vector of years for the target category
 *
 * @param category_name The name of the category
 * @return The vector of years for the category
 */
vector<unsigned> Categories::years(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR() << "Could not find category_name " << category_name << " in the list of loaded categories";

  return categories_[category_name].years_;
}

/**
 *  Return the corresponding age length pointer for this category
 * @param category_name The name of the category
 * @return The AgeLength instance for the category
 */
AgeLength* Categories::age_length(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR() << "Could not find category_name " << category_name << " in the list of loaded categories";

  return categories_[category_name].age_length_;
}

/**
 *  Return the corresponding age weight pointer for this category
 *
 * @param category_name The name of the category
 * @return The AgeWeight instance for the category
 */
AgeWeight* Categories::age_weight(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR() << "Could not find category_name " << category_name << " in the list of loaded categories";
  if (!categories_[category_name].age_weight_) {
    categories_[category_name].age_weight_ = ageweights::Factory::Create(model_, PARAM_AGE_WEIGHT, PARAM_NONE);
  }

  return categories_[category_name].age_weight_;
}

/**
 *  Return the corresponding length weight pointer for this category
 *
 * @param category_name The name of the category
 * @return The LengthWeight instance for the category
 */
LengthWeight* Categories::length_weight(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR() << "Could not find category_name " << category_name << " in the list of loaded categories";
  if (!categories_[category_name].length_weight_) {
    categories_[category_name].length_weight_ = lengthweights::Factory::Create(model_, PARAM_LENGTH_WEIGHT, PARAM_NONE);
  }

  return categories_[category_name].length_weight_;
}

/**
 * This method removes all of the information from the loaded categories
 */
void Categories::Clear() {
  parameters_.Clear();
  format_ = "";
  names_.clear();
  category_names_.clear();
  categories_.clear();
  age_length_labels_.clear();
  category_age_length_labels_.clear();
  age_weight_labels_.clear();
  category_age_weight_labels_.clear();
  length_weight_labels_.clear();
  category_length_weight_labels_.clear();
}

} /* namespace niwa */
