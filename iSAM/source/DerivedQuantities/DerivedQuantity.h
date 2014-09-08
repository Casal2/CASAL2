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
 * << Add Description >>
 */
#ifndef DERIVEDQUANTITY_H_
#define DERIVEDQUANTITY_H_

// headers
#include "BaseClasses/Executor.h"
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Selectivities/Selectivity.h"

// namespaces
namespace isam {

namespace accessor = isam::partition::accessors;

/**
 * class definition
 */
class DerivedQuantity : public isam::base::Executor {
public:
  // methods
  DerivedQuantity();
  virtual                     ~DerivedQuantity() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  bool                        IsAssignedToInitialisationPhase(const string& label);
  void                        PreExecute() override final { };
  Double                      GetValue(unsigned year);
  Double                      GetInitialisationValue(unsigned phase = 0, unsigned index = 0);
  Double                      GetLastValueFromInitialisation(unsigned phase);

  // accessors
  const string&               time_step() { return time_step_label_; }
  const vector<vector<Double> >& initialisation_values() { return initialisation_values_; }
  const map<unsigned, Double>& values() { return values_; }

protected:
  // Members
  string                      time_step_label_;
  vector<string>              initialisation_time_step_labels_;
  unsigned                    current_initialisation_phase_;
  vector<vector<Double> >     initialisation_values_;
  map<unsigned, Double>       values_;
  vector<string>              selectivity_labels_;
  vector<SelectivityPtr>      selectivities_;
  vector<string>              category_labels_;
  ModelPtr                    model_;
  accessor::Categories        partition_;
};

// typedef
typedef boost::shared_ptr<DerivedQuantity> DerivedQuantityPtr;

} /* namespace isam */
#endif /* DERIVEDQUANTITY_H_ */
