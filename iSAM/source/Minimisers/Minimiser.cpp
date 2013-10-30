/**
 * @file Minimiser.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Boost defines
#define NDEBUG 1
#define BOOST_UBLAS_TYPE_CHECK 0
#define BOOST_UBLAS_SINGULAR_CHECK 1

// Headers
#include "Minimiser.h"

#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "Estimates/Manager.h"
#include "Utilities/DoubleCompare.h"

// Namespaces
namespace isam {

namespace dc = isam::utilities::doublecompare;
namespace ublas = boost::numeric::ublas;

/**
 * Default constructor
 */
Minimiser::Minimiser() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of minimiser to use");
  parameters_.Bind<bool>(PARAM_ACTIVE, &active_, "True if this minimiser is active", false);
  parameters_.Bind<bool>(PARAM_COVARIANCE, &build_covariance_, "True if a covariance matrix should be created", true);

  hessian_ = 0;
  hessian_size_ = 0;
}

/**
 * Destructor
 */
Minimiser::~Minimiser() {
  if (hessian_ != 0) {
    for (unsigned i = 0; i < hessian_size_; ++i)
      delete [] hessian_[i];
    delete [] hessian_;
  }
}

/**
 * Validate the parameters for this minimisers
 */
void Minimiser::Validate() {
  parameters_.Populate();

  DoValidate();
}

/**
 * Build the parameters for this minimiser
 */
void Minimiser::Build() {
  hessian_size_ = estimates::Manager::Instance().GetEnabledCount();

  hessian_ = new double*[hessian_size_];
  for (unsigned i = 0; i < hessian_size_; ++i) {
    hessian_[i] = new double[hessian_size_];
    for (unsigned j = 0; j < hessian_size_; ++j)
      hessian_[i][j] = 0.0;
  }

  DoBuild();
}

/**
 * Build the covariance matrix to be used by our MCMC
 */
void Minimiser::BuildCovarianceMatrix() {
  if (hessian_ == 0)
    LOG_CODE_ERROR("Cannot build the covariance matrix as the hessian has not been allocated");

  ublas::matrix<double> hessian_matrix(hessian_size_, hessian_size_);
  vector<bool> zero_row;
  for (unsigned i = 0; i < hessian_size_; ++i) {
    zero_row.push_back(1);
    for (unsigned j = 0; j < hessian_size_; ++j) {
      hessian_matrix(i,j) = hessian_[i][j];
      if( !dc::IsZero( hessian_matrix(i,j) ) ) zero_row[i] = 0;
    }
    if( zero_row[i] ) hessian_matrix(i,i) = 1.0;
  }

  // Convert Hessian to Covariance
  ublas::permutation_matrix<> permutation_matrix(hessian_matrix.size1());
  ublas::matrix<double> copied_matrix = ublas::matrix<double>(hessian_matrix);
  ublas::lu_factorize(copied_matrix,permutation_matrix);

  ublas::matrix<double> identity_matrix(ublas::identity_matrix<double>(copied_matrix.size1()));
  ublas::lu_substitute(copied_matrix,permutation_matrix,identity_matrix);

  covariance_matrix_.swap(identity_matrix);

  correlation_matrix_ = covariance_matrix_;
  double diag;
  for (unsigned i = 0; i < hessian_size_; ++i) {
    diag = correlation_matrix_(i,i);
    for (unsigned j = 0; j < hessian_size_; ++j) {
      correlation_matrix_(i,j) = correlation_matrix_(i,j) / sqrt(diag);
      correlation_matrix_(j,i) = correlation_matrix_(j,i) / sqrt(diag);
    }
  }
}

} /* namespace isam */
