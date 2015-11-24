/**
 * @file DerivedQuantity.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * A derived quantity is a value that is derived from the model
 * at some point during execution. This can be anything
 * from the amount of objects in the partition, to the weight
 * of objects in the partition etc.
 */
#ifndef DERIVEDQUANTITY_H_
#define DERIVEDQUANTITY_H_

// headers
#include "BaseClasses/Executor.h"
#include "Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
namespace accessor = niwa::partition::accessors;

class Model;
class Selectivity;

// classes
class DerivedQuantity : public niwa::base::Executor {
public:
  // methods
  DerivedQuantity() = delete;
  explicit DerivedQuantity(Model* model);
  virtual                     ~DerivedQuantity() = default;
  void                        Validate();
  void                        Build();
  void                        Reset();
  Double                      GetValue(unsigned year);
  Double                      GetInitialisationValue(unsigned phase = 0, unsigned index = 0);
  Double                      GetLastValueFromInitialisation(unsigned phase);

  // accessors
  const string&               time_step() { return time_step_label_; }
  const vector<vector<Double> >& initialisation_values() { return initialisation_values_; }
  const map<unsigned, Double>& values() { return values_; }

protected:
  // Members
  Model*                      model_ = nullptr;
  string                      time_step_label_ = "";
  unsigned                    current_initialisation_phase_ = 0;
  vector<vector<Double>>      initialisation_values_;
  map<unsigned, Double>       values_;
  map<unsigned, Double>       override_values_;
  Double                      cache_value_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  vector<string>              category_labels_;
  accessor::Categories        partition_;
  string                      proportion_method_;
  Double                      time_step_proportion_;
  bool                        mean_proportion_method_;
};
} /* namespace niwa */
#endif /* DERIVEDQUANTITY_H_ */
