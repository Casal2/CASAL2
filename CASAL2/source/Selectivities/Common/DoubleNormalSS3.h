/**
 * @file DoubleNormalSS3.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2020 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef DOUBLENORMALSS3_H_
#define DOUBLENORMALSS3_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class DoubleNormalSS3 : public niwa::Selectivity {
public:
  // Methods
  explicit DoubleNormalSS3(shared_ptr<Model> model);
  virtual ~DoubleNormalSS3() = default;
  void DoValidate() override final;
  void RebuildCache() override final;

protected:
  // Methods
  Double GetLengthBasedResult(unsigned age, AgeLength* age_length, unsigned year = 0, int time_step_index = -1) override final;

private:
  // Members
  Double peak_;
  Double first_sel_val_;
  Double last_sel_val_;
  Double asc_width_;
  Double desc_width_;
  Double min_x_val_;
  Double max_x_val_;
  Double top_width_;
  Double alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* DOUBLENORMALPLATEAUSS3_H_ */
