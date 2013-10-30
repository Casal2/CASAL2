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
namespace isam {

// Using
using std::map;
using isam::utilities::Double;

/**
 * Class Definition
 */
class Selectivity : public isam::base::Object {
public:
  // Methods
  Selectivity() = delete;
  explicit Selectivity(ModelPtr model);
  virtual                     ~Selectivity() = default;
  void                        Validate();
  virtual void                Build() { Reset(); };
  virtual void                Reset() {};
  virtual Double              GetResult(unsigned age_or_length);

  // pure methods
  virtual void                DoValidate() = 0;

protected:
  // Members
  ModelPtr                    model_;
  map<unsigned, Double>       values_;
};



/**
 * Typedef
 */
typedef boost::shared_ptr<Selectivity> SelectivityPtr;

} /* namespace isam */
#endif /* SELECTIVITY_H_ */
