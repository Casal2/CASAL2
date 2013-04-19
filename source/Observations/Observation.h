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
#include "Likelihoods/Likelihood.h"
#include "Selectivities/Selectivity.h"
#include "Utilities/Types.h"

/**
 * Struct Definitions
 */
namespace isam {
namespace observations {

struct Comparison {
  string    key_ = "";
  unsigned  age_ = 0;
  Double    expected_;
  Double    observed_;
  Double    error_value_;
  Double    score_;
};

} /* namespace observations */
} /* namespace isam */

// Namespaces
namespace isam {
using isam::utilities::Double;
namespace obs = isam::observations;

/**
 * Class Definition
 */
class Observation : public isam::base::Object {
public:
  // methods
  Observation();
  virtual                     ~Observation();
  virtual void                Validate();
  virtual void                Build();
  virtual void                PreExecute() { };
  virtual void                Execute() = 0;
  void                        Reset();
  virtual void                PostExecute() { };

  // accessors
  Double                      score() const { return score_; }
  unsigned                    year() const { return year_; }
  const string&               time_step() const { return time_step_label_; }
  const vector<obs::Comparison>& comparisons() const { return comparisons_; }

protected:
  // methods
  void                        SaveComparison(string key, unsigned age, Double expected, Double observed, Double error_value, Double score);
  void                        SaveComparison(string key, Double expected, Double observed, Double erroe_value, Double score);

  // members
  string                      type_;
  Double                      score_;
  unsigned                    year_;
  string                      time_step_label_;
  Double                      time_step_proportion_;
  string                      time_step_proportion_method_;
  bool                        mean_proportion_method_;
  vector<string>              category_labels_;
  vector<string>              selectivity_labels_;
  string                      likelihood_type_;
  string                      simulation_likelihood_label_;
  bool                        run_in_simulation_mode_;
  vector<SelectivityPtr>      selectivities_;
  LikelihoodPtr               likelihood_;
  vector<obs::Comparison>     comparisons_;
};

// Typedef
typedef boost::shared_ptr<isam::Observation> ObservationPtr;

} /* namespace isam */
#endif /* OBSERVATION_H_ */
