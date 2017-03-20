/*
 * Selectivity.h
 *
 *  Created on: 21/12/2012
 *      Author: Admin
 */

#ifndef SELECTIVITY_H_
#define SELECTIVITY_H_

// Headers
#include <map>

#include "BaseClasses/Object.h"
#include "Model/Model.h"
#include "Utilities/Types.h"

// Namespaces
namespace niwa {
class AgeLength;

// Using
using std::map;
using niwa::utilities::Double;

/**
 * Class Definition
 */
class Selectivity : public niwa::base::Object {
public:
  // Methods
  Selectivity() = delete;
  explicit Selectivity(Model* model);
  virtual                     ~Selectivity() = default;
  void                        Validate();
  virtual void                Build() { Reset(); };
  virtual void                Reset();
  virtual Double              GetResult(unsigned age, AgeLength* age_length);
  // accessor
  bool                        IsSelectivityLengthBased() {return length_based_;};
protected:
  // pure methods
  virtual Double              GetLengthBasedResult(unsigned age, AgeLength* age_length) = 0;
  virtual void                DoValidate() = 0;
  // Members
  Model*                      model_ = nullptr;
  map<unsigned, Double>       values_;
  unsigned                    n_quant_ = 5;
  vector<Double>              quantiles_;
  vector<Double>              quantiles_at_;
  bool                        length_based_ = false;
};
} /* namespace niwa */
#endif /* SELECTIVITY_H_ */
