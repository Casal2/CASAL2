/**
 * @file nuisance.h
 * @author C.Marsh
 * @github https://github.com/NIWAFisheriesModelling
 * @date 16/08/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Nuisance catchabilities are an analytical solutions for solving catchabilities given the data. They can only be derived under certain scenerios's see the manual for more detail.
 * They can still have a prior on them even though they are not estimated as a free parameter. This is useful functionality for reducing correlations in your
 * free parameters, thus generating more stable MCMC chains.
 */
#ifndef CATCHABILITIES_NUISANCE_H_
#define CATCHABILITIES_NUISANCE_H_

// headers
#include "Catchabilities/Catchability.h"
#include "Observations/Comparison.h"

// namespaces
namespace niwa {
class Estimate;
namespace catchabilities {

// classes
class Nuisance : public Catchability {
public:
  Nuisance() = delete;
  explicit                    Nuisance(Model* model);
  virtual                     ~Nuisance()  { };
  Double                      q() const override final { return q_; }
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        CalculateQ(map<unsigned, vector<observations::Comparison> >& comparisons, const string& likelihood);


private:
  // members
  Double                      q_;
  Double                      lower_bound_ = 0.0;
  Double                      upper_bound_ = 100.0;
  string                      prior_type_ = PARAM_NONE;
  Double                      mu_ = 0.0;
  Double                      cv_ = 1.0;




};

} /* namespace catchabilities */
} /* namespace niwa */

#endif /* CATCHABILITIES_NUISANCE_H_ */
