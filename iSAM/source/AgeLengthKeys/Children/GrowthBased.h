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
  Data();
  virtual                     ~Data() = default;
  void                        DoValidate() override final {};
  void                        DoBuild() override final;
  void                        DoReset() override final {};
  
  // accessors Is this a double function or a void function? find the difference
  Double					  Cum_normal(Double mu, Double sigma, string distribution,class_mins_t *class mins, vector<Double> *vprop_in_length);
  
private:
  // members
  vector<Double>              Class_mins_;		// Class min lengths
  //vector<Double>              Class_mins_log_;
  bool                        Plus_grp_;		// Whether a plus group exists
  string                      Distribution_;    // What distribution is used
  Int						  Im2_;				// How many Length bins there are
  TablePtr                    data_table_;		// Store the AKL
 

};

} /* namespace agelengthkeys */
} /* namespace niwa */

#endif /* SOURCE_AGELENGTHKEYS_CHILDREN_DATA_H_ */
