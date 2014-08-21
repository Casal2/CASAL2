/**
 * @file TimeStep.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TIMESTEP_H_
#define TIMESTEP_H_

// Headers
#include "BaseClasses/Object.h"
#include "DerivedQuantities/Manager.h"
#include "Processes/Process.h"

// Namespaces
namespace isam {

/**
 * Class Definition
 */
class TimeStep : public isam::base::Object {
public:
  // Methods
  TimeStep();
  virtual                     ~TimeStep() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() {};
  void                        ExecuteForInitialisation(unsigned phase);
  void                        Execute(unsigned year);

  // accessors
  const vector<string>&       process_names() const { return process_names_; };

private:
  // methods
  void                        ExecuteDerivedQuantities();
  void                        ExecuteInitialisationDerivedQuantities(unsigned phase);

  // Members
  vector<string>              process_names_;
  vector<ProcessPtr>          processes_;
  vector<DerivedQuantityPtr>  initialisation_derived_quantities_;
  vector<DerivedQuantityPtr>  derived_quantities_;
  unsigned                    block_start_process_index_;
  unsigned                    block_end_process_Index_;
};

/**
 * Typedef
 */
typedef boost::shared_ptr<TimeStep> TimeStepPtr;

} /* namespace isam */
#endif /* TIMESTEP_H_ */
