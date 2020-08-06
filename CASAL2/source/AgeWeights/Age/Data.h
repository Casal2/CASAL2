/**
 * @file Data.h
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright NIWA Science 2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Data Ageweight is a user defined table that describes the mean weight for each age group in every year.
 */
#ifndef AGEWEIGHT_DATA_H_
#define AGEWEIGHT_DATA_H_

// headers
#include "AgeWeights/AgeWeight.h"

// namespaces
namespace niwa {

namespace ageweights {

// classes
class Data : public AgeWeight {
public:
  // methods
  explicit Data(Model* model);
  virtual                     ~Data();
  void                        DoValidate() override final {};
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoRebuildCache() override final { }; // This should never happen. i.e time vary data type.

  Double                      mean_weight_at_age_by_year(unsigned year, unsigned age) override final;


protected:
  //methods
private:
  // methods
  // members
  parameters::Table*            data_table_ = nullptr;
  map<unsigned,Double>          initial_; // For initial state
  vector<unsigned>              steps_to_figure_;
  unsigned                      number_time_steps_;
  unsigned                      final_year_;
  vector<unsigned>              years_;
  vector<unsigned>              age_;
  string                        units_;
  string                        equilibrium_method_;
  Double                        unit_multipier_ =  1.0;

  map<unsigned, vector<Double>> data_by_year_; // To initially store inputs
  map<unsigned,map<unsigned,Double>> mean_data_by_year_and_age_; // during execute and projection
};

} /* namespace ageweights */
} /* namespace niwa */

#endif /* AGEWEIGHT_DATA_H_ */
