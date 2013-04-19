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
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGEINGERROR_H_
#define AGEINGERROR_H_

// headers
#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// namespaces
namespace isam {

using isam::utilities::Double;

/**
 * Default Constructor
 */
class AgeingError : public isam::base::Object {
public:
  // Methods
  AgeingError();
  virtual                     ~AgeingError();
  virtual void                Validate();
  virtual void                Build();
  virtual void                Reset() = 0;

  // Accessors
  unsigned                    min_age() const { return min_age_; }
  unsigned                    max_age() const { return max_age_; }
  bool                        age_plus() const { return age_plus_; }
  vector<vector<Double> >&    mix_matrix() { return mis_matrix_; }
//  const vector<Double>&       expected();

protected:
  // Members
  unsigned                    min_age_;
  unsigned                    max_age_;
  bool                        age_plus_;
  unsigned                    age_spread_;
  vector<vector<Double> >     mis_matrix_;
};

// Typedef
typedef boost::shared_ptr<AgeingError> AgeingErrorPtr;

} /* namespace isam */
#endif /* AGEINGERROR_H_ */
