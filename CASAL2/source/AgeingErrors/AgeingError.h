/**
 * @file AgeingError.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 */
#ifndef AGEINGERROR_H_
#define AGEINGERROR_H_

// headers
#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
class Model;

using niwa::utilities::Double;

/**
 * Default Constructor
 */
class AgeingError : public niwa::base::Object {
public:
  // Methods
  AgeingError(Model* model);
  AgeingError() = delete;
  virtual                     ~AgeingError() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); }

  // Accessors
  unsigned                    min_age() const { return min_age_; }
  unsigned                    max_age() const { return max_age_; }
  bool                        age_plus() const { return age_plus_; }
  vector<vector<Double> >&    mis_matrix() { return mis_matrix_; }

protected:
  // Methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;

  // Members
  Model*                      model_ = nullptr;
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  bool                        age_plus_ = false;
  unsigned                    age_spread_ = 0;
  vector<vector<Double> >     mis_matrix_;
};

} /* namespace niwa */
#endif /* AGEINGERROR_H_ */
