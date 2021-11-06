/**
 * @file Simplex.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Jan 8, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Follows unit simplex theory, I used chapter 55.6 in the STAN reference map for the jacobian calculation.
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SIMPLEX_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SIMPLEX_H_

// headers
#include "../AddressableTransformation.h"
// namespaces
namespace niwa {
class Estimate;
namespace addressabletransformations {

/**
 *
 */
class Simplex : public AddressableTransformation {
public:
  Simplex() = delete;
  explicit Simplex(shared_ptr<Model> model);
  virtual ~Simplex() = default;
  Double           GetScore() override final;
  void             FillReportCache(ostringstream& cache) override final;
  void             PrepareForObjectiveFunction() override final;
  void             RestoreForObjectiveFunction() override final;
protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoRestore() override final;

private:
  vector<Double>    zk_;
  vector<Double>    cumulative_simplex_k_;
  vector<Double>    unit_vector_;
  vector<Double>    simplex_parameter_;
  vector<Double>    cache_log_k_value_;
  Double            total_ = 0.0;
  bool              sum_to_one_ = true;
  Double            n_param_double_;
  Double            sub_total_;
  vector<Double>    cached_simplex_values_for_objective_function_restore_;
  bool              simplex_input_supplied_ = false;
  Estimate*         simplex_estimate_;
};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_SIMPLEX_H_ */
