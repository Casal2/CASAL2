/**
 * @file Beta.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class along with the the uniformLog class were specifically implemented so that
 * Casal2 could emulate the nuisance Q functionality that CASAL does.
 */
#ifndef ADDITIONALPRIORS_LOGNORMAL_H_
#define ADDITIONALPRIORS_LOGNORMAL_H_

// headers
#include "../../AdditionalPriors/AdditionalPrior.h"

// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class LogNormal : public AdditionalPrior {
public:
  // methods
  LogNormal(shared_ptr<Model> model);
  virtual ~LogNormal() = default;
  void   DoValidate() final;
  void   DoBuild() final;
  Double GetScore() final;

protected:
  // members
  Double*                addressable_            = nullptr;
  map<unsigned, Double>* addressable_map_        = nullptr;
  vector<Double>*        addressable_vector_     = nullptr;
  vector<Double*>*       addressable_ptr_vector_ = nullptr;
  Double                 score_                  = 0.0;
  Double                 mu_;
  Double                 cv_;
  Double                 sigma_;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONALPRIORS_LOGNORMAL_H_ */
