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
#include "Observations/Comparison.h"
#include "Selectivities/Selectivity.h"
#include "Utilities/Types.h"

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
  virtual                     ~Observation() = default;
  void                        Validate();
  void                        Build();
  void                        Reset();


  // pure methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  virtual void                PreExecute() = 0;
  virtual void                Execute() = 0;
  virtual void                PostExecute() = 0;
  virtual void                CalculateScore() = 0;
  virtual bool                HasYear(unsigned year) const = 0;

  // accessors
  const map<unsigned, Double>&  scores() const { return scores_; }
  const string&                 time_step() const { return time_step_label_; }
  vector<obs::Comparison>&      comparisons(unsigned year) { return comparisons_[year]; }
  map<unsigned, vector<obs::Comparison> >& comparisons() { return comparisons_; }

protected:
  // methods
  void                        SaveComparison(string category, unsigned age, Double expected, Double observed,
      Double process_error, Double error_value, Double delta, Double score);
  void                        SaveComparison(string category, Double expected, Double observed,
      Double process_error, Double error_value, Double delta, Double score);

  // members
  string                      type_;
  map<unsigned, Double>       scores_;
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
  map<unsigned, vector<obs::Comparison> > comparisons_;
};

// Typedef
typedef boost::shared_ptr<isam::Observation> ObservationPtr;

} /* namespace isam */
#endif /* OBSERVATION_H_ */
