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
class LengthWeight;
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
  void         RebuildCache(); // should only be called by time-varying class
  void         UpdateYearContainers(); // should only be called by this class or its manager

  // accessors
  string          weight_units();
  const Double&   cv(unsigned year, unsigned time_step, unsigned age) { return cvs_[year - year_offset_][time_step - time_step_offset_][age - age_offset_]; };
  virtual string  distribution_label() { return distribution_label_; };
  Distribution    distribution() const { return distribution_; }
  string          compatibility() const { return compatibility_; }
  const Double&   mean_weight(unsigned time_step, unsigned age) {return mean_weight_by_timestep_age_[time_step - time_step_offset_][age - age_offset_];};
  const Double&   mean_length(unsigned time_step, unsigned age) {return mean_length_by_timestep_age_[time_step - time_step_offset_][age - age_offset_];};
  Double          mean_weight_by_length(Double length, unsigned age, unsigned year, unsigned time_step);

  void            BuildAgeLengthMatrixForTheseYears(vector<unsigned> years);
  // four versions of this function, these variations are to do with a selectivity, and using bespoke length bins.
  void            populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length, Selectivity* selectivity);
  void            populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length); // overloaded for the case with no selectivity
  void            populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length, Selectivity* selectivity, vector<int>& map_length_bin_ndx); // overloaded for the case where class has bespoke length bins
  void            populate_numbers_at_length(vector<Double> numbers_at_age, vector<Double>& numbers_at_length, vector<int>& map_length_bin_ndx); // overloaded for the case with no selectivity and class has bespoke length bins
  void            populate_numbers_at_age_with_length_based_exploitation(vector<Double>& numbers_at_age, vector<Double>& numbers_at_age_with_exploitation, Double& exploitation_by_length, unsigned model_length_bin_ndx,  Selectivity* selectivity); // 

  // For reporting in the AgeLength 
  void            FillReportCache(ostringstream& cache);

protected:
  // methods

  virtual void      DoValidate()     = 0;
  virtual void      DoBuild()        = 0;
  virtual void      DoReset()        = 0;
  virtual  Double   calculate_mean_length(unsigned year, unsigned time_step, unsigned age) = 0;

  void              PopulateCV();
  void              PopulateAgeLengthMatrix();
  void              UpdateAgeLengthMatrixForThisYear(unsigned year);

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
  string                                              length_weight_label_;
  LengthWeight*                                       length_weight_ = nullptr;
  // these objects get allocated memory in Dobuild and get populated by DoRebuildCache() in each child
  vector<vector<Double>>    mean_length_by_timestep_age_;  // mean_length_by_timestep_age_[time_step][age]
  vector<vector<Double>>    mean_weight_by_timestep_age_;  // mean_weight_by_timestep_age_[time_step][age]
  vector<unsigned>          age_length_matrix_years_;
  map<unsigned, unsigned>   age_length_matrix_year_key_;  // [year, dimension]
  // because this may not be in sequential order (see method BuildAgeLengthMatrixForTheseYears) we have a key which maps the right year with first dimension of age_length_transition_matrix_
  vector<vector<Double>>                 numbers_by_age_length_transition_; // age x length used as a temporarey container
  vector<vector<vector<vector<Double>>>> age_length_transition_matrix_; // dims years x timesteps x age x length
  // these members are used in the functions
  unsigned            this_year_ = model_->start_year();
  unsigned            this_time_step_ = 0;
  unsigned            year_dim_in_age_length_ = 0;
  unsigned            min_age_ = model_->min_age();
  unsigned            max_age_ = model_->max_age();
};

} /* namespace niwa */
#endif /* AGELENGTH_H_ */
