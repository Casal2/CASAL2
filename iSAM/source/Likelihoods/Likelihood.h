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
  void                        Validate() { };
  void                        Build() { };
  virtual Double              AdjustErrorValue(const Double process_error, const Double error_value) = 0;
  virtual void                SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) { };
  virtual Double              GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) { return 0.0; };
  virtual void                GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) { };

  // accessors
  void                        set_multiplier(Double new_value) { multiplier_ = new_value; }
  void                        set_error_value_multiplier(Double new_value) { error_value_multiplier_ = new_value; }

protected:
  // members
  Double                      multiplier_ = 1.0;
  Double                      error_value_multiplier_ = 1.0;
};

// Typedef
typedef boost::shared_ptr<niwa::Likelihood> LikelihoodPtr;

} /* namespace niwa */
#endif /* LIKELIHOOD_H_ */
