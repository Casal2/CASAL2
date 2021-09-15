/**
 * @file AgeLength.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 23/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * AgeLength classes relate to a category or group of categories that describes the relationship between age and length.
 */
#ifndef AGELENGTH_H_
#define AGELENGTH_H_

// headers
#include "../BaseClasses/Object.h"
#include "../Partition/Category.h"
#include "../Utilities/Distribution.h"

// namespaces
namespace niwa {
class Model;
class Selectivity;

// classes
class AgeLength : public niwa::base::Object {
public:
  // enums

  // methods
  AgeLength() = delete;
  explicit AgeLength(shared_ptr<Model> model);
  virtual ~AgeLength(){};
  void         Validate();
  void         Build();
  void         Reset();
  virtual void RebuildCache();

  // accessors
  virtual Double GetMeanLength(unsigned year, unsigned time_step, unsigned age)                = 0;
  virtual Double GetMeanWeight(unsigned year, unsigned time_step, unsigned age, Double length) = 0;
  virtual string weight_units()                                                                = 0;

  virtual Double cv(unsigned year, unsigned time_step, unsigned age) { return cvs_[year - year_offset_][time_step - time_step_offset_][age - age_offset_]; };
  virtual string distribution_label() { return distribution_label_; };
  Distribution   distribution() const { return distribution_; }
  string         compatibility() const { return compatibility_; }
  bool           varies_by_years() const { return varies_by_year_; }
  // Methods
  virtual Double mean_weight(unsigned time_step, unsigned age) = 0;
  virtual Double mean_length(unsigned time_step, unsigned age) = 0;

protected:
  // methods

  virtual void DoValidate()     = 0;
  virtual void DoBuild()        = 0;
  virtual void DoReset()        = 0;
  virtual void DoRebuildCache() = 0;

  void         PopulateCV();
  // members
  shared_ptr<Model> model_ = nullptr;
  vector<Double>    time_step_proportions_;
  Double            cv_first_ = 0.0;
  Double            cv_last_  = 0.0;
  bool              by_length_;
  string            distribution_label_;
  Distribution      distribution_;
  string            compatibility_  = "";
  bool              varies_by_year_ = false;
  unsigned          year_offset_;
  unsigned          age_offset_;
  unsigned          time_step_offset_ = 0;// time-steps are already vector friendly i.e start at 0
  vector<vector<vector<Double>>>    cvs_;  // cvs[year][time_step][age]
  vector<unsigned>  model_years_;
};

} /* namespace niwa */
#endif /* AGELENGTH_H_ */
