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
  virtual Double              GetResult(unsigned age_or_length);

  // pure methods
  virtual void                DoValidate() = 0;

protected:
  // Members
  Model*                      model_ = nullptr;
  map<unsigned, Double>       values_;
};
} /* namespace niwa */
#endif /* SELECTIVITY_H_ */
