/**
 * @file Categories.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents the category configuration for the partition
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef CATEGORIES_H_
#define CATEGORIES_H_

// Headers
#include "BaseClasses/Object.h"

// Namespaces
namespace niwa {
class Model;
class AgeLength;

/**
 * Struct Definition
 */
struct CategoryInfo {
  string name_ = "";
  vector<unsigned>  years_;
  unsigned          min_age_ = 0;
  unsigned          max_age_ = 0;
  AgeLength*        age_length_ = nullptr;
};

/**
 * Class Definition
 */
class Categories : public niwa::base::Object {
  friend class Model;
public:
  // Methods
  virtual                       ~Categories() = default;
  void                          Validate();
  void                          Build();
  void                          Reset() { };
  bool                          IsValid(const string& label) const;
  bool                          IsCombinedLabels(const string& label) const;
  unsigned                      GetNumberOfCategoriesDefined(const string& label) const;
  void                          Clear();

  vector<string>                ExpandLabels(const vector<string> &category_labels, const Parameter* source_parameter);
  string                        GetCategoryLabels(const string& lookup_string, const Parameter* source_parameter);
  vector<string>                GetCategoryLabelsV(const string& lookup_string, const Parameter* source_parameter);

  // Accessors
  string                        format() const { return format_; }
  vector<string>                category_names() const { return category_names_; }
  unsigned                      min_age(const string& category_name);
  unsigned                      max_age(const string& category_name);
  vector<unsigned>              years(const string& category_name);
  AgeLength*                    age_length(const string& category_name);

private:
  // Methods
  Categories() = delete;
  explicit Categories(Model* model);

  // Members
  Model*                      model_ = nullptr;
  string                      format_;
  vector<string>              names_;
  vector<string>              years_;
  vector<string>              ages_;
  vector<string>              category_names_;
  vector<string>              age_length_labels_;
  map<string, string>         category_age_length_labels_;
  map<string, CategoryInfo>   categories_;
};
} /* namespace niwa */

#endif /* CATEGORIES_H_ */
