/**
 * @file Profile.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * A profile is a form of estimate. When you define a profile
 * you define the usual lower and upper bounds as well as
 * a steps parameter. The steps parameter tells Casal2 how many
 * steps it must make between the lower and upper bounds incrementing
 * the estimate value each time and then doing another minimisation.
 */
#ifndef PROFILE_H_
#define PROFILE_H_

// headers
#include "../BaseClasses/Object.h"
#include "../Estimates/Estimate.h"

// namespaces
namespace niwa {
class Model;

/**
 * class definition
 */
class Profile : public niwa::base::Object {
public:
  // methods
  Profile() = delete;
  explicit Profile(shared_ptr<Model> model);
  virtual ~Profile() = default;
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model);
  void Reset(){};
  void FirstStep();
  void NextStep();
  void RestoreOriginalValue();

  // accessors
  string   parameter() const { return parameter_; }
  vector<string>   same_parameters() const { return same_labels_; }
  unsigned steps() const { return steps_; }
  Double   value() const { return *target_; }
  vector<Double>   get_values() const { return values_; }

private:
  // members
  shared_ptr<Model> model_               = nullptr;
  unsigned          steps_               = 0;
  Double            lower_bound_         = 0;
  Double            upper_bound_         = 0;
  string            parameter_           = "";
  vector<string>    same_labels_;
  vector<Double*>   sames_;
  Double            step_size_           = 0;
  Double*           target_              = nullptr;
  Double            original_value_      = 0;
  Double            same_original_value_ = 0;
  vector<Double>    values_;
  Double            current_value_       =  0.0;
};
} /* namespace niwa */
#endif /* PROFILE_H_ */
