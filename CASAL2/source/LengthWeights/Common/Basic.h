/**
 * @file Basic.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTHWEIGHTS_BASIC_H_
#define LENGTHWEIGHTS_BASIC_H_

// headers
#include "LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
namespace lengthweights {

/**
 * class definition
 */
class Basic : public niwa::LengthWeight {
public:
  // methods
  explicit                    Basic(Model* model);
  virtual                     ~Basic() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoInitialise() override final;
  void                        DoReset() override final { };

  // accessors
  Double                      mean_weight(Double size, Distribution distribution, Double cv) const override final;
  vector<unsigned>&           GetTimeVaryingYears() override final { return time_varying_years_; }

private:
  // members
  Double                      a_ = 0;
  Double                      b_ = 0;
  Double                      cv_ = 0;
  Double                      unit_multipier_ = 1.0;
  string                      units_;
  vector<unsigned>            time_varying_years_;

};

} /* namespace lengthweights */
} /* namespace niwa */
#endif /* LENGTHWEIGHTS_BASIC_H_ */
