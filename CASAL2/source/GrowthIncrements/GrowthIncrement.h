/**
 * @file GrowthIncrement.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 *
 * @section DESCRIPTION
 * For length based models - this class builds the growth transition matrix
 * which describes the probability of moving from length bin i -> length j.
 * based on one of the children expected growth functions.
 */
#ifndef GROWTH_INCREMENT_MODELS_H_
#define GROWTH_INCREMENT_MODELS_H_

// headers
#include "../BaseClasses/Object.h"
#include "../Model/Model.h"
#include "../Utilities/Compatibility.h"
#include "../Utilities/Distribution.h"

// namespaces
namespace niwa {
class LengthWeight;

/**
 * class definition
 */
class GrowthIncrement : public niwa::base::Object {
public:
  // methods
  GrowthIncrement() = delete;
  explicit GrowthIncrement(shared_ptr<Model> model);
  virtual ~GrowthIncrement(){};
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model){};
  void Reset();
  void RebuildCache();  // should only be called by time-varying class
  void FillReportCache(ostringstream& cache);

  const Double&                 get_mean_weight(unsigned length_bin_index) { return mean_weight_by_length_bin_index_[length_bin_index]; };
  const vector<vector<Double>>& get_transition_matrix() { return growth_transition_matrix_; };

  void apply_growth();

  // accessors

protected:
  virtual void DoValidate() = 0;
  virtual void DoBuild()    = 0;
  virtual void DoReset()    = 0;

  virtual Double get_mean_increment(double length) = 0;

  void populate_growth_transition_matrix();

  // accessors

  // members

  shared_ptr<Model>      model_ = nullptr;
  string                 distribution_label_;
  Distribution           distribution_;
  string                 length_weight_label_;
  LengthWeight*          length_weight_ = nullptr;
  vector<vector<Double>> growth_transition_matrix_;  // length_bins x length_bins
  Double                 cv_;
  Double                 min_sigma_;
  unsigned               number_of_model_length_bins_;
  vector<double>         model_min_length_bins_;
  vector<double>         model_length_midpoints_;
  bool                   plus_group_;
  CompatibilityType      compatibility_type_ = CompatibilityType::kUnknown;
  string                 compatibility_      = "";
  // vector<double>              time_step_proportions_;
  vector<Double> mean_weight_by_length_bin_index_;
  bool           is_growth_type_none_ = false;
};
} /* namespace niwa */
#endif /* GROWTH_INCREMENT_MODELS_H_ */
