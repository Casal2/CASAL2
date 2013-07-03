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
#include "BaseClasses/Object.h"

// namespaces
namespace isam {

/**
 * class definition
 */
class DerivedQuantity : public isam::base::Object {
public:
  // methods
  DerivedQuantity();
  virtual                     ~DerivedQuantity() = default;
  void                        Validate();
  void                        Build() { DoBuild(); };
  void                        Reset() { DoReset(); };
  void                        Calculate();
  void                        CalculateForInitialisationPhase(unsigned initialisation_phase);

  // accessors
  const string&               time_step() { return time_step_label_; }

protected:
  // methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;

private:
  // Members
  string                      time_step_label_;
  vector<string>              initialisation_time_step_labels_;
  unsigned                    current_initialisation_phase_;
  vector<Double>              initialisation_values_;
  vector<Double>              values_;
};

// typedef
typedef boost::shared_ptr<DerivedQuantity> DerivedQuantityPtr;

} /* namespace isam */
#endif /* DERIVEDQUANTITY_H_ */
