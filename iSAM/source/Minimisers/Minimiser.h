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
#include <boost/numeric/ublas/matrix.hpp>

#include "BaseClasses/Object.h"

// Namespaces
namespace niwa {
namespace ublas = boost::numeric::ublas;

class Model;

/**
 * Class Definition
 */
class Minimiser : public niwa::base::Object {
public:
  // Methods
  Minimiser() = delete;
  explicit Minimiser(Model* model);
  virtual                     ~Minimiser();
  void                        Validate();
  void                        Build();
  void                        Reset() { DoReset(); };
  void                        BuildCovarianceMatrix();

  // pure methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoReset() = 0;
  virtual void                Execute() = 0;

  // Acessors
  bool                        active() const { return active_; }
  void                        set_active(bool new_value) { active_ = new_value; }
  ublas::matrix<double>       covariance_matrix() const { return covariance_matrix_; }
  ublas::matrix<double>       correlation_matrix() const { return correlation_matrix_; }
  double**                    hessian_matrix() const { return hessian_; }
  unsigned                    hessian_size() const { return hessian_size_; }

protected:
  // Members
  Model*                      model_;
  string                      type_;
  bool                        active_;
  double**                    hessian_;
  unsigned                    hessian_size_;
  bool                        build_covariance_;
  ublas::matrix<double>       covariance_matrix_;
  ublas::matrix<double>       correlation_matrix_;
};
} /* namespace niwa */
#endif /* MINIMISER_H_ */
