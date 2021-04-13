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
#include "../BaseClasses/Object.h"

// Namespaces
namespace niwa {
class Model;
class AgeWeight;
class AgeLength;
class LengthWeight;

/**
 * Struct Definition
 */
struct CategoryInfo {
  string name_ = "";
  vector<unsigned>  years_;
  unsigned          min_age_ = 0;
  unsigned          max_age_ = 0;
  AgeLength*        age_length_ = nullptr;
  LengthWeight*     length_weight_ = nullptr;
  AgeWeight*        age_weight_ = nullptr;

};

/**
 * Class Definition
 */
class Categories : public niwa::base::Object {
  friend class Model;
  friend class MockCategories;
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
  vector<string>                ExpandLabels(const vector<string> &category_labels, const string& parameter_location);
  string                        GetCategoryLabels(const string& lookup_string, const string& parameter_location);
  vector<string>                GetCategoryLabelsV(const string& lookup_string, const string& parameter_location);
  virtual bool                  HasAgeLengths() const { return age_length_labels_.size() != 0; }

  // Accessors
  string                        format() const { return format_; }
  virtual vector<string>        category_names() const { return category_names_;}
  unsigned                      min_age(const string& category_name);
  unsigned                      max_age(const string& category_name);
  vector<unsigned>              years(const string& category_name);
  virtual AgeLength*            age_length(const string& category_name);
  LengthWeight*                 length_weight(const string& category_name);
  AgeWeight*                    age_weight(const string& category_name);

protected:
  // Methods
  Categories() = delete;
  explicit Categories(shared_ptr<Model> model);

  virtual map<string, string> GetCategoryLabelsAndValues(const string& lookup, const string& parameter_location);

  // Members
  shared_ptr<Model>                      model_ = nullptr;
  string                      format_;
  vector<string>              names_;
  vector<string>              years_;
  vector<string>              category_names_;
  vector<string>              age_weight_labels_;
  vector<string>              age_length_labels_;
  vector<string>              length_weight_labels_;
  map<string, string>         category_age_length_labels_;
  map<string, string>         category_length_weight_labels_;
  map<string, string>         category_age_weight_labels_;
  map<string, CategoryInfo>   categories_;
};
} /* namespace niwa */

#endif /* CATEGORIES_H_ */
