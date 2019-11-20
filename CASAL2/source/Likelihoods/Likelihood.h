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
class Model;
using niwa::utilities::Double;

/**
 * Class definition
 */
class Likelihood : public niwa::base::Object {
public:
  // Methods
  Likelihood(Model* model);
  virtual                     ~Likelihood() = default;
  void                        Validate();
  void                        Build() { };
  void                        Reset() override final { };
  virtual Double              AdjustErrorValue(const Double process_error, const Double error_value) = 0;
  virtual void                SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) { };
  virtual Double              GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) { return 0.0; };
  virtual void                GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) { };
  virtual void                DoValidate() { };

  // accessors
  void                        set_multiplier(double new_value) { multiplier_ = new_value; }
  void                        set_error_value_multiplier(double new_value) { error_value_multiplier_ = new_value; }
  void                        set_type(const string& type) { type_ = type; }

protected:
  // members
  Model*                      model_ = nullptr;
  double                      multiplier_ = 1.0;
  double                      error_value_multiplier_ = 1.0;
};
} /* namespace niwa */
#endif /* LIKELIHOOD_H_ */
