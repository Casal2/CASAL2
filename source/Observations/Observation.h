/**
 * @file Observation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBSERVATION_H_
#define OBSERVATION_H_

// Headers
#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {

using isam::utilities::Double;

/**
 * Class Definition
 */
class Observation : public isam::base::Object {
public:
  // Methods
  Observation();
  virtual                     ~Observation();
  virtual void                Validate();
  virtual void                Build() { };
  virtual void                PreExecute() { };
  virtual void                Execute() { };
  virtual void                PostExecute() { };

protected:
  // members
  string                      type_;
  Double                      score_;
  unsigned                    year_;
  string                      time_step_label_;
  Double                      time_step_proportion_;
  string                      time_step_proportion_method_;
  vector<string>              category_labels_;
  vector<string>              selectivity_labels_;
  string                      likelihood_label_;
  string                      simulation_likelihood_label_;
  bool                        run_in_simulation_mode_;
};

} /* namespace isam */
#endif /* OBSERVATION_H_ */
