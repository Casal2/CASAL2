/*
 * LogisticNormal.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: Zaita
 */

#include <Likelihoods/Children/LogisticNormal.h>
#include "Utilities/DoubleCompare.h"
#include "Utilities/Math.h"
#include "Utilities/RandomNumberGenerator.h"

namespace niwa {
namespace likelihoods {

using std::set;
namespace dc = niwa::utilities::doublecompare;
namespace math = niwa::utilities::math;

LogisticNormal::LogisticNormal() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label for Logisitic Normal Likelihood", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of likelihood", "");
  covariance_table_ = new parameters::Table(PARAM_COVARIANCE_MATRIX);

  parameters_.Bind<Double>(PARAM_RHO, &rho_, "The auto-correlation parameter $\rho$", "");
  parameters_.BindTable(PARAM_COVARIANCE_MATRIX, covariance_table_, "User defined Covariance matrix", "",false,true);
  parameters_.Bind<Double>(PARAM_SIGMA, &sigma_, "Sigma parameter in the likelihood", "");
  parameters_.Bind<bool>(PARAM_ARMA, &arma_, "Defines if two rho parameters supplied then covar is assumed to have the correlation matrix of an ARMA(1,1) process", "");
  parameters_.Bind<unsigned>(PARAM_BIN_LABELS, &bins_, "If no covariance matrix parameter then list a vector of bin labels that the covariance matrix will be built for, can be ages or lengths.", "",false);
  parameters_.Bind<bool>(PARAM_SEXED, &sexed_, "Will the observation be split by sex?", "",false);
  parameters_.Bind<bool>(PARAM_SEPERATE_BY_SEX, &sep_by_sex_, "If data is sexed, shold the correlation structure be seperated by sex?", "",false);

}

LogisticNormal::~LogisticNormal() {
  // TODO Auto-generated destructor stub
}

void LogisticNormal::DoValidate() {
  LOG_TRACE();
  //parameters_.Populate();
  /**
   * Build our covariance matrix with user defined values

  if (parameters_.GetTable(PARAM_COVARIANCE_MATRIX)->has_been_defined()) {
    LOG_FINEST() << "Converting user defined covariance matrix";
    vector<vector<string>>& covariance_values_data = covariance_table_->data();
    int rows = covariance_values_data.size();
    int cols = covariance_values_data[0].size();
    if(rows != cols) {
      LOG_ERROR_P(PARAM_COVARIANCE_MATRIX) << "Covariance matrix must be square (equal rows and columns). rows = " << rows << " cols = " << cols;
    }
    covariance_matrix_(rows,cols);
    // Covariance should be an age by age matrix or age x sex by age x sex matrix; if comp data given by sex.
    LOG_FINEST() << " has " << covariance_values_data.size() << " rows defined";
    unsigned col_iter = 0;
    for (vector<string>& covariance_values_data_line : covariance_values_data) {
      unsigned bin = 0;
      vector<Double> temp;
      if (!utilities::To<unsigned>(covariance_values_data_line[0], bin)) {
        LOG_ERROR_P(PARAM_COVARIANCE_MATRIX) << " value " << covariance_values_data_line[0] << " could not be converted in to an unsigned integer. It should be the year for this line";
      } else {
        for (unsigned i = 1; i < covariance_values_data_line.size(); ++i) {
          Double value = 0;
          if (!utilities::To<Double>(covariance_values_data_line[i], value)) {
            LOG_ERROR_P(PARAM_SCANNED) << " value (" << covariance_values_data_line[i] << ") could not be converted to a double";
          }
          covariance_matrix_(bin, value);
        }
      }
      bins_.push_back(bin);
      ++col_iter;
    }
    n_bins_ = bins_.size();
  } else {
    LOG_FINEST() << "Calculating Covariance matrix with user specified parameters";
    n_bins_ = bins_.size();
    LOG_FINEST() << "number of bins = " << n_bins_;
    // Create a covariance matrix with the user defined parameters
    calculate_covariance(sigma_, rho_,n_bins_,sep_by_sex_,arma_, sexed_);
  }

  LOG_FINEST() << "Printing Covariance matrix";
  for (unsigned k = 0; k < covariance_matrix_.size1(); ++k) {
    for (unsigned j = 0; j < covariance_matrix_.size2(); ++j) {
      LOG_FINEST() << "row = " << k << " col = " << j << " val = " << covariance_matrix_(k,j) << " ";
    }
  }*/
}
/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double LogisticNormal::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (process_error > 0.0 && error_value > 0.0)
    return (1.0/(1.0/error_value + 1.0/process_error));

  return error_value;
}

/**
 *
 */
void LogisticNormal::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
/*  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;

      Double score = math::LnFactorial(error_value * comparison.observed_)
                      - error_value * comparison.observed_ * log(dc::ZeroFun(comparison.expected_, comparison.delta_));
      comparison.score_ = score * multiplier_;
    }
  }*/
}

/**
 * Grab the initial score for this likelihood
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

Double LogisticNormal::GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) {
  Double score = 0.0;
/*

  observations::Comparison& comparison = comparisons[year][0];

    Double temp_score = -math::LnFactorial(AdjustErrorValue(comparison.process_error_, comparison.error_value_)  * error_value_multiplier_);
    LOG_FINEST() << "Adding: " << temp_score << " = LnFactorial(AdjustErrorValue(" << comparison.process_error_ << ", " << comparison.error_value_ << ")  * " << error_value_multiplier_ << ")";
    score += temp_score;
    //stopper += 1;
  //}
*/

  return score * multiplier_;
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void LogisticNormal::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
/*  LOG_TRACE();
  // Generate a multivariate variabel X
  vector<Double> age_totals(n_bins_, 0.0);
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  if (!parameters_.GetTable(PARAM_COVARIANCE_MATRIX)->has_been_defined() && rho_[0] == 0.0) {
    LOG_FINEST() << "sigma_ = " << sigma_;
    // We just need to generate an independent multivariate normal distribtuion
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      unsigned age_category_iter = 0;
      for (observations::Comparison& comparison : year_iterator->second) {
        comparison.observed_ = exp(rng.normal(0.0,1.0) * sigma_ + log(comparison.expected_));
        LOG_FINEST() << "random deviate " << rng.normal(0.0,1.0) << " age = " << comparison.age_ << " simuolated val = " << comparison.observed_  << " expected = " << comparison.expected_ ;
        age_totals[age_category_iter] += comparison.observed_;
        ++age_category_iter;
      }
    }
  } else {
    if (!parameters_.GetTable(PARAM_COVARIANCE_MATRIX)->has_been_defined())
      calculate_covariance(sigma_, rho_, n_bins_, sep_by_sex_, arma_, sexed_);
    if (!DoCholeskyDecmposition())
      LOG_FATAL()<< "Cholesky decomposition failed. Cannot continue Simulating from a Logisitic Normal likelihood";
    //
    for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
      unsigned age_category_iter = 0;
      unsigned nbins = year_iterator->second.size();
      vector<Double> normals(nbins, 0.0);
      for (unsigned i = 0; i < nbins; ++i) {
        normals[i] = rng.normal();
      }
      for (observations::Comparison& comparison : year_iterator->second) {
        Double row_sum = 0.0;
        for (unsigned j = 0; j < nbins; ++j) {
          row_sum += covariance_matrix_lt(j,age_category_iter) * normals[j];
        }
        comparison.observed_ = row_sum * log(comparison.expected_);
        age_totals[age_category_iter] += comparison.observed_;
        ++age_category_iter;
      }
    }
  }
  // Do the logistic transformation to get our desired values.
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    LOG_FINEST() << "year = " << year_iterator->first;
    unsigned age_category_iter = 0;
    for (observations::Comparison& comparison : year_iterator->second) {
      comparison.observed_ /= age_totals[age_category_iter];
      LOG_FINEST() << "Simulated val = " << comparison.observed_ << " expected = " << comparison.expected_;
      ++age_category_iter;

    }
  }
  LOG_FINEST() << "check out the totals";
  for(auto num :age_totals)
    LOG_FINEST() << num ;*/
}

void LogisticNormal::calculate_covariance(Double sigma_, vector<Double> rho_, unsigned N_ages, bool sepbysex, bool ARMA, bool sexed) {
  LOG_TRACE();
/*  unsigned n_phi = rho_.size();
  vector<Double> rho;
  unsigned Nbins;
  if (sexed)
    Nbins = N_ages * 2;
  else
    Nbins = N_ages;
  vector<vector<Double>> covar;
  covar.resize(Nbins, vector<Double>(Nbins, 0.0));

  LOG_FINEST() << "covariance rows = " << covar.size() << " cols = " << covar[0].size();
  // initialise covar as the identity matrix
  for (unsigned diag = 0; diag < covar.size(); ++ diag)
    covar[diag][diag] = 1.0 * sigma_ * sigma_;

  if (rho_[0] == 0.0) {
    // Do nothing all zeros in the off diagonal
  } else if (sexed && sepbysex) {

      LOG_FINEST() << "Calculating covar for sexed patrition with sepbysex = true, covar dims = " << covar.size()  << " " << covar[0].size();
      for (unsigned i = 0; i < 1; ++i) {
        rho = GetRho(rho_,N_ages,ARMA);
          for (unsigned diag = 0; diag < N_ages; ++diag) {
            for (int row = 0; row < (int)Nbins; ++row) {
              for (int col = 0; col < (int)Nbins; ++col) {
                if (fabs(row - col) == diag + 1)
                  covar[row + (i * N_ages)][col + (i * N_ages)] = rho[diag] * sigma_ * sigma_;
              }
            }
          }
        }

    } else if (sexed && !sepbysex) {
      vector<int> binlab;
      int ages = 1;
      for (unsigned i = 1; i <= Nbins; ++i, ++ages) {
        if (ages > (int)N_ages)
          ages = 1;
        binlab.push_back(ages);
        cout << ages << " ";
      }
      if (ARMA && n_phi == 2)
        rho = GetRho(rho_,N_ages,ARMA);
      else if (!ARMA && n_phi == 2)
        rho = GetRho(rho_,N_ages + 1,ARMA);
        vector<int> col_vec, row_vec, offset_vec;

        for (unsigned row = 0; row < Nbins; ++row) {
          for (unsigned col = 0; col < Nbins; ++col) {
            col_vec.push_back(binlab[col]);
            row_vec.push_back(binlab[row]);
          }
        }
        // now calculate an offset vector;
        for (unsigned index = 0; index < col_vec.size(); ++index) {
          offset_vec.push_back(fabs(row_vec[index] - col_vec[index]) + 1.0);
          cout << offset_vec[index] << " ";
        }
        if (covar.size() * covar[0].size() != offset_vec.size())
          LOG_CODE_ERROR() << "covar.size() * covar[0].size() != offset_vec.size(). get_mat_index will fail as index vector needs to be same size as rows * col of the mat.";
        for (unsigned index = 1; index <= N_ages; ++index) {
          for (unsigned index2 = 0; index2 < offset_vec.size(); ++index2) {
            if ((int)index == offset_vec[index2]) {
              vector<unsigned> indexes = math::get_mat_index(covar.size(), covar[0].size(), index2);
              covar[indexes[0]][indexes[1]] = rho[index - 1];
              //cout << "row = " << indexes[0] << " col = " << indexes[1] << " index = " << index2 << endl;
            }
          }
        }


      // Add the identity mat
      for (unsigned row = 0; row < Nbins; ++row)
        covar[row][row] = 1.0;
      // Store in the Covariance matrix
      for (unsigned row = 0; row < Nbins; ++row) {
        for (unsigned col = 0; col < Nbins; ++col) {
          covar[row][col] *= sigma_ * sigma_;
        }
      }
    } else {
      // Unisex
      rho = GetRho(rho_,N_ages,ARMA);
      for (int diag = 0; diag < (int)N_ages; ++ diag) {
        for (int row = 0; row < (int)N_ages; ++ row) {
          for (int col = 0; col < (int)N_ages; ++ col) {
            if (fabs(row - col) == diag + 1) {
              covar[row][col] = rho[diag] * sigma_ * sigma_;
            }
          }
        }
      }
    }
    covariance_matrix_(Nbins,Nbins);
    for(unsigned i = 0; i < covariance_matrix_.size1(); ++i){
      for(unsigned j = 0; j < covariance_matrix_.size2(); ++j)
        covariance_matrix_(i,j) = covar[i][j];
    }*/
}

vector<Double> LogisticNormal::GetRho(vector<Double>& Phi, unsigned nBin, bool ARMA) {
  LOG_TRACE();
  // declare all variables that will be used in this function
  vector<Double> rhovec;
  /*
  vector<Double> ar, ma,final_acf,Cor;
  vector<vector<Double> > A, ind;
  vector<Double> psi, theta, Acf;


  if (Phi.size() == 1) {
    LOG_FINEST() <<  "Found single Rho parameter";
    rhovec.assign(nBin,0.0);
    //calculation of AR(1) acf for  LN2
    for(unsigned i = 1; i <= nBin - 1; i++)
      rhovec[i - 1] = pow(Phi[0],i);
  } else {
    // we are doing ARMAacf function
    unsigned p, q, r;
    if (ARMA) {
      q = 1;
      p = 1;
      ar.push_back(Phi[0]);
    } else {
      q = 0;
      p = 2;
      ar = Phi;
    }
    r = fmax(p, q + 1);
    if (p > 0) {
      if (r > 1) {
        if (r > p) {
          LOG_FINEST() << "calculating rho from an ARMA(1,1) process";
          for (unsigned i = 0; i < (r - p); ++i)
            ar.push_back(0.0);
          p = r;
        }
        LOG_FINEST() << "Structureing A";

        A.resize(p + 1, vector<Double>(2 * p + 1, 0.0));
        ind.resize(2 * p + 1, vector<Double>(p + 1, 0.0));
        for (int i = 0; i < (int)ind.size(); ++i) {
          for (int j = 0; j < (int)ind[i].size(); ++j) {
            ind[i][0] = i + 1;
            ind[i][1] = (i + 1) + (j + 1) - (i + 1);
          }
        }

        for (unsigned i = 0; i < A.size(); ++i) {
          A[i][i] = 1.0;
           A[i][i + 1] = -ar[0];
           A[i][i + 2] = -ar[1];
        }
        LOG_FINEST() << "Populate A. the second ar value" << ar[1];
        // Declare Eigen variables
        Eigen::Matrix3d A_eig; // 3f means 3x3 elements of type double
        Eigen::Vector3d rhs;
        // initialise rhs, which will be used to solve the following problem, that is Ax = b where b = rhs, so x = A^-1 b
        rhs(0) = 1.0;
        rhs(1) = 0.0;
        rhs(2) = 0.0;
        if (q > 0) {
          LOG_FINEST() << "Calculate ma coef";
          // deal with the ma coeffecient
          psi.push_back(1.0);
          psi.push_back(Phi[0] + Phi[1]);
          theta.push_back(1.0);
          theta.push_back(Phi[1]);
          for (unsigned i = 0; i <= q; ++i)
            theta.push_back(0.0);
          // Calculate rhs
          for (unsigned i = 0; i <= q; ++i) {
            Double x1, x2;
            x1 = psi[0] * theta[i];
            x2 = psi[1] * theta[i + q];
            rhs(i) = math::Sum( { x1, x2 });
          }
          rhs(2) = 0.0;
        }
        LOG_FINEST() << "Calculate seq parameter";
        // Use the eigen library yo solve the inverse of for A with known vector B
        //vector<double> Ind;
        vector<unsigned> seq;
        for (unsigned i = 0; i <= p; ++i) {
          seq.push_back(p - i);
        }
        for (unsigned i = 0; i <= p; ++i) {
          for (unsigned j = 0; j <= p; ++j) {
            //LOG_FINEST() << ": i = " << i << " j = " << j << " i index = " << seq[i] << " j index = " << seq[j] << " mat value = " << A[seq[i]][seq[j]];
            if (j == 2)
              A_eig(i,j) = A[i][j];
            else
              A_eig(i,j) = A[i][j] + A[i][2 * p  - j];
          }
        }
        for (unsigned i = 0; i <= p; ++i) {
          for (unsigned j = 0; j <= p ; ++j) {
            A_eig(i,j) =  A_eig(seq[i],seq[j]);
          }
        }
        // the bodge
        A_eig(1,2) = 0.0;

        LOG_FINEST() << "Check A mat that we are inverting\n" << A_eig;

        // Solve for A_eig given rhs
        Eigen::Vector3d x = A_eig.colPivHouseholderQr().solve(rhs);
        LOG_FINEST() << "solution = " << x;

        for (unsigned i = 1; i <= 2; ++i) {
          final_acf.push_back(x(i) / x(0));
        }
        LOG_FINEST() << "Final Acf";
        for (auto num : final_acf)
          LOG_FINEST() << num << " ";

        Cor = RecursiveFilter(Phi, nBin, final_acf);
        LOG_FINEST() << " Print Cor";
        for (auto num : Cor)
          LOG_FINEST()  << num << " ";
        // Add the initial coeffiecients to the beginning of the series.
        Cor.insert(Cor.begin(), final_acf[1]);
        Cor.insert(Cor.begin(), final_acf[0]);
        // Print results to screen
        vector<Double>::const_iterator first = Cor.begin();
        vector<Double>::const_iterator last = Cor.begin() + nBin;
        vector<Double> corvec(first, last);
        LOG_FINEST() << "Check the result";
        rhovec = corvec;

      }

    }
  }
  LOG_FINEST() << " Print rho";
  for (auto num : rhovec)
    LOG_FINEST()  << num << " ";*/
return rhovec;
}

vector<Double> LogisticNormal::RecursiveFilter(vector<Double>& ar_coef, unsigned nBins, vector<Double>& initial_vals) {
  LOG_TRACE();
  vector<Double> store_vec(nBins + 1,0.0);
/*
  if (ar_coef.size() > 2) {
    LOG_CODE_ERROR() <<  "RecursiveFilter(): has not been coded for more than 2 AR coeffiecients, ar_coef.size() > 2" << endl;
  }
  store_vec[0] = initial_vals[1];
  store_vec[1] = initial_vals[0];
  for (unsigned i = 1; i < nBins + 1; ++i) {
    if (i == 1) {
      store_vec[i] =   store_vec[i - 1] *ar_coef[0]  + store_vec[i] *  ar_coef[1];
    } else {
      store_vec[i] = store_vec[i - 1] *  ar_coef[0] + store_vec[i - 2] * ar_coef[1];
    }
    LOG_FINEST() << "value = " << store_vec[i];
  }
  // remove the first value
  store_vec.erase(store_vec.begin());*/
  return store_vec;
}

/**
 * Perform cholesky decomposition on our covariance
 * matrix before it's used in the MCMC.
 *
 * @return true on success, false on failure
 */
bool LogisticNormal::DoCholeskyDecmposition() {
  if (covariance_matrix_.size1() != covariance_matrix_.size2() )
      LOG_ERROR() << "Invalid covariance matrix (size1!=size2)";
    unsigned matrix_size1 = covariance_matrix_.size1();
    covariance_matrix_lt = covariance_matrix_;

    for (unsigned i = 0; i < matrix_size1; ++i) {
      for (unsigned j = 0; j < matrix_size1; ++j) {
        covariance_matrix_lt(i,j) = 0.0;
      }
    }

    for (unsigned i = 0; i < matrix_size1; ++i) {
      covariance_matrix_lt(i,i) = 1.0;
    }

    if (covariance_matrix_(0,0) < 0)
      return false;
    Double sum = 0.0;

    covariance_matrix_lt(0,0) = sqrt(covariance_matrix_(0,0));

    for (unsigned i = 1; i < matrix_size1; ++i)
      covariance_matrix_lt(i,0) = covariance_matrix_(i,0)/covariance_matrix_lt(0,0);

    for (unsigned i = 1; i < matrix_size1; ++i) {
      sum = 0.0;
      for (unsigned j = 0; j < i; ++j)
        sum += covariance_matrix_lt(i,j) * covariance_matrix_lt(i,j);

      if (covariance_matrix_(i,i) <= sum)
        return false;
      covariance_matrix_lt(i,i) = sqrt(covariance_matrix_(i,i) - sum);
      for (unsigned j = i+1; j < matrix_size1; ++j) {
        sum = 0.0;
        for (unsigned k = 0; k < i; ++k)
          sum += covariance_matrix_lt(j,k) * covariance_matrix_lt(i,k);
        covariance_matrix_lt(j,i) = (covariance_matrix_(j,i) - sum) / covariance_matrix_lt(i,i);
      }
    }
    sum = 0.0;
    for (unsigned i = 0; i < (matrix_size1 - 1); ++i)
      sum += covariance_matrix_lt(matrix_size1 - 1,i) * covariance_matrix_lt(matrix_size1-1,i);
    if (covariance_matrix_(matrix_size1 - 1, matrix_size1 - 1) <= sum)
      return false;
    covariance_matrix_lt(matrix_size1 - 1, matrix_size1 - 1) = sqrt(covariance_matrix_(matrix_size1 - 1, matrix_size1 - 1) - sum);

   return true;
}

} /* namespace likelihoods */
} /* namespace niwa */
