/**
 * @file Likelihood.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
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
#ifndef LIKELIHOOD_H_
#define LIKELIHOOD_H_

// Headers
#include "BaseClasses/Object.h"
#include "Observations/Comparison.h"
#include "Utilities/Types.h"

// Namespaces
namespace niwa {

using niwa::utilities::Double;


/**
 * Class definition
 */
class Likelihood : public niwa::base::Object {
public:
  // Methods
  Likelihood() = default;
  virtual                     ~Likelihood() = default;
  virtual void                Validate() { };
  virtual void                Build() { };
  virtual Double              AdjustErrorValue(const Double process_error, const Double error_value) = 0;
  virtual void                GetResult(vector<Double> &scores, const vector<Double> &expecteds, const vector<Double> &observeds,
                                const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) = 0;
  virtual void                SimulateObserved(const vector<string> &keys, vector<Double> &observeds, const vector<Double> &expecteds,
                                const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) = 0;
  virtual Double              GetInitialScore(const vector<string> &keys, const vector<Double> &process_errors,
                                const vector<Double> &error_values);

  virtual void                SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) { };
  virtual Double              GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) { return 0.0; };
  virtual void                GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) { };
};

// Typedef
typedef boost::shared_ptr<niwa::Likelihood> LikelihoodPtr;

} /* namespace niwa */
#endif /* LIKELIHOOD_H_ */
