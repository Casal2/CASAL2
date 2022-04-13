/**
 * @file Basic.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTHWEIGHTS_BASIC_H_
#define LENGTHWEIGHTS_BASIC_H_

// headers
#include "../../LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
namespace lengthweights {

/**
 * class definition
 */
class Basic : public niwa::LengthWeight {
public:
  // methods
  explicit Basic(shared_ptr<Model> model);
  virtual ~Basic() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};

  // accessors
  Double mean_weight(Double size, Distribution distribution, Double cv) const override final;
  Double mean_weight(Double size) const override final;

  string weight_units() const override final { return units_; };

private:
  // members
  Double a_              = 0;
  Double b_              = 0;
  Double cv_             = 0;
  Double unit_multipier_ = 1.0;
  string units_;
  Double length_bin_proportion_ = 0.5;
};

} /* namespace lengthweights */
} /* namespace niwa */
#endif /* LENGTHWEIGHTS_BASIC_H_ */
