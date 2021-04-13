/**
 * @file VectorSmoothing.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * vector smoothing = a penalty on the between element differences of a
 * vector of values (or map). Optionally in log space,. Will also work
 * on allvalues ogives. Equal to sum of squares of ith differences,
 * where i is a user defined option that defaults to 3. Has an arbitrary
 * multiplier (default=1) as well.
 */
#ifndef PENALTIES_VECTORSMOOTHING_H_
#define PENALTIES_VECTORSMOOTHING_H_

// headers
#include "../../AdditionalPriors/AdditionalPrior.h"
#include "../../Estimates/Estimate.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Class definition
 */
class VectorSmoothing : public niwa::AdditionalPrior {
public:
  // methods
  VectorSmoothing(shared_ptr<Model> model);
  virtual                     ~VectorSmoothing() = default;
  Double                      GetScore() final;

protected:
  // methods
  void                        DoValidate() final {};
  void                        DoBuild() final;
  // members
  map<unsigned, Double>*      addressable_map_ = nullptr;
  vector<Double>*             addressable_vector_ = nullptr;
  bool                        log_scale_ = false;
  Double                      multiplier_ = 0.0;
  unsigned                    r_ = 0;
  unsigned                    upper_ = 0;
  unsigned                    lower_ = 0;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* VECTORSMOOTHING_H_ */
