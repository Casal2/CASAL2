/**
 * @file AgeLength.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 23/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * AgeLength classes relate to a category or group of categories that describes the relationship between age and length.
 */
#ifndef AGELENGTH_H_
#define AGELENGTH_H_

// headers
#include "Common/BaseClasses/Object.h"
#include "Common/Partition/Category.h"

// namespaces
namespace niwa {
class Model;
class Selectivity;

// classes
class AgeLength : public niwa::base::Object {
public:
  // methods
  AgeLength() = delete;
  explicit AgeLength(Model* model);
  virtual                     ~AgeLength() { };
  void                        Validate();
  void                        Build();
  void                        Reset();
  void                        DoAgeToLengthConversion(partition::Category* category,
                                const vector<Double>& length_bins, bool plus_grp, Selectivity* selectivity);
  void                        CummulativeNormal(Double mu, Double cv, vector<Double>& prop_in_length,
                                vector<Double> length_bins, string distribution, bool plus_grp);
  // accessors
  virtual Double              mean_length(unsigned year, unsigned age) = 0;
  virtual Double              mean_weight(unsigned year, unsigned age) = 0;
  virtual Double              cv(unsigned year, unsigned age, unsigned time_step) { return cvs_[year][age][time_step]; };
  virtual string              distribution() { return distribution_; };

protected:
  // methods

  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  void                        BuildCV();
  // members
  Model*                      model_ = nullptr;
  vector<Double>              time_step_proportions_;
  map<unsigned, map<unsigned, map<unsigned, Double>>>       cvs_;  //cvs[year][age][time_step]
  Double                      cv_first_ = 0.0;
  Double                      cv_last_;
  bool                        by_length_;
  string                      distribution_;
  bool                        casal_normal_cdf_ = false;
};

} /* namespace niwa */
#endif /* AGELENGTH_H_ */
