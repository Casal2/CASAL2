/**
 * @file Observation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
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
#include "BaseClasses/Executor.h"
#include "Likelihoods/Likelihood.h"
#include "Observations/Comparison.h"
#include "Utilities/Types.h"

// Namespaces
namespace niwa {
namespace obs = niwa::observations;

class Model;

using niwa::utilities::Double;

/**
 * Class Definition
 */
class Observation : public niwa::base::Executor {
public:
  // methods
  Observation() = delete;
  explicit Observation(shared_ptr<Model> model);
  virtual ~Observation() = default;
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model){};
  void Reset();

  // pure methods
  virtual void DoValidate()                 = 0;
  virtual void DoBuild()                    = 0;
  virtual void DoReset()                    = 0;
  virtual void CalculateScore()             = 0;
  virtual bool HasYear(unsigned year) const = 0;

  // accessors
  const map<unsigned, Double>&             scores() const { return scores_; }
  string&                                  likelihood() { return likelihood_type_; }
  vector<obs::Comparison>&                 comparisons(unsigned year) { return comparisons_[year]; }
  map<unsigned, vector<obs::Comparison> >& comparisons() { return comparisons_; }

protected:
  // methods
  void SaveComparison(string category, unsigned age, Double length, Double expected, double observed, Double process_error, double error_value, Double adjusted_error, Double delta,
                      Double score);

  void SaveComparison(string category, Double expected, double observed, Double process_error, double error_value, Double adjusted_error, Double delta, Double score);

  // members
  shared_ptr<Model>     model_ = nullptr;
  map<unsigned, Double> scores_;
  Double                proportion_of_time_          = 0;
  bool                  mean_proportion_method_      = false;
  string                likelihood_type_             = "";
  string                simulation_likelihood_label_ = "";
  Double                delta_                       = 0;
  bool                  run_in_simulation_mode_      = false;
  Likelihood*           likelihood_                  = nullptr;
  vector<string>        allowed_likelihood_types_;
  Double                error_value_multiplier_ = 1.0;
  Double                likelihood_multiplier_  = 1.0;
  vector<string>        category_labels_;
  unsigned              expected_selectivity_count_;

  map<unsigned, vector<obs::Comparison> > comparisons_;
};
} /* namespace niwa */
#endif /* OBSERVATION_H_ */
