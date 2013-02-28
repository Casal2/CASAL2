/**
 * @file Minimiser.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * A minimiser is responsible for taking the model
 * and trying to find the best fit of parameters using the
 * estimates and their bounds.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MINIMISER_H_
#define MINIMISER_H_

// Headers
#include "BaseClasses/Object.h"

// Namespaces
namespace isam {

/**
 * Class Definition
 */
class Minimiser : public isam::base::Object {
public:
  // Methods
  Minimiser();
  virtual                     ~Minimiser();
  virtual void                Validate();
  virtual void                Build();
  virtual void                Execute() = 0;

  // Acessors
  bool                        active() const { return active_; }
  void                        set_active(bool new_value) { active_ = new_value; }

protected:
  // Members
  string                      type_;
  bool                        active_;
  bool                        build_covariance_;
};

// Typdef
typedef boost::shared_ptr<isam::Minimiser> MinimiserPtr;

} /* namespace isam */
#endif /* MINIMISER_H_ */
