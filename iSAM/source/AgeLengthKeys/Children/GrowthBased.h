/**
 * @file GrowthBased.h
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef SOURCE_AGELENGTHKEYS_CHILDREN_GROWTHBASED_H_
#define SOURCE_AGELENGTHKEYS_CHILDREN_GROWTHBASED_H_

// headers
#include "AgeLengthKeys/AgeLengthKey.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

// classes
class GrowthBased : public AgeLengthKey {
public:
  // methods
  GrowthBased();
  virtual                     ~GrowthBased() = default;
  void                        DoValidate() override final {};
  void                        DoBuild() override final {};
  void                        DoReset() override final {};

  void                        DoAgeToLengthConversion(std::shared_ptr<partition::Category> category) override final;

private:
  // methods
//  Double                      CummulativeNormal(Double mu, Double sigma, string distribution,class_mins_t *class mins, vector<Double> *vprop_in_length);

  // members
  vector<Double>              class_mins_;		  // Class min lengths
  //vector<Double>              Class_mins_log_;
  string                      distribution_;    // What distribution is used
};

} /* namespace agelengthkeys */
} /* namespace niwa */

#endif /* SOURCE_AGELENGTHKEYS_CHILDREN_DATA_H_ */
