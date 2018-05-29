/**
 * @file GrowthBasic.h
 * @author  C.Marsh
 * @version 1.0
 * @date 27/07/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Growth is the class responsible for moving numbers at length to other length bins within a category, Can be thought of as analagous to ageing in an age based model
 *
 *
 */
#ifndef LENGTH_GROWTH_BASIC_H_
#define LENGTH_GROWTH_BASIC_H_

// Headers
#include "Common/Partition/Accessors/Categories.h"
#include "Common/Processes/Process.h"

// Namespaces
namespace niwa {
namespace length {
namespace processes {

namespace accessor = niwa::partition::accessors;

/**
 * Class Definition
 */
class GrowthBasic : public niwa::Process {
public:
  // Methods
  explicit GrowthBasic(Model* model);
  virtual                     ~GrowthBasic() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;

private:
  // Members
  accessor::Categories        partition_;
  vector<Double>              g_;
  vector<Double>              l_;
  vector<Double>              length_bin_mid_points_;
  vector<unsigned>            length_bins_;
  Model*                      model_ = nullptr;
  vector<string>              category_labels_;
  Double                      cv_ = 0.0;
  vector<string>              growth_time_steps_;
  Double                      min_sigma_ = 0.0;
  unsigned                    n_growth_episodes_ = 1;
  vector<vector<Double>>      transition_matrix_; // explains how each length bin moves to others.
  vector<vector<Double>>      numbers_transitioning_matrix_; // explains how each length bin moves to others.

};

} /* namespace processes */
} /* namespace length */
} /* namespace niwa */
#endif /* LENGTH_GROWTH_BASIC_H_ */
