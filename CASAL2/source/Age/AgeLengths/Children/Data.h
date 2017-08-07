/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Data AgeLength is a user defined table that describes the mean length for each age group in every year. The tables dimensions are years by ages
 */
#ifndef AGELENGTHS_DATA_H_
#define AGELENGTHS_DATA_H_

// headers
#include "Age/AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
class LengthWeight;

namespace agelengths {

// classes
class Data : public AgeLength {
public:
  // methods
  explicit Data(Model* model);
  virtual                     ~Data();
  void                        DoValidate() override final {};
  void                        DoBuild() override final;
  void                        DoReset() override final {};
  virtual Double              mean_length(unsigned time_step, unsigned age) override final {return 0.0;};
  Double                      mean_weight(unsigned time_step, unsigned age) override final;

protected:
  //methods
private:
  // methods
  void                        FillExternalGaps();
  void                        FillInternalGaps();
  void                        InterpolateTimeStepsForInitialConditions();
  void                        InterpolateTimeStepsForAllYears();

  // members
  parameters::Table*            data_table_ = nullptr;
  string                        external_gaps_;
  string                        internal_gaps_;
  map<unsigned, vector<Double>> data_by_year_;
  map<unsigned,map<unsigned,Double>> data_by_age_time_step_; // used in the initialisation
  map<unsigned,map<unsigned,Double>> mean_data_by_year_and_step_;
  string                        step_data_supplied_;
  unsigned                      step_index_data_supplied_;
  unsigned                      ageing_index_;
  vector<Double>                means_;
  string                        length_weight_label_;
  const LengthWeight*           length_weight_ = nullptr;
  vector<unsigned>              steps_to_figure_;
  unsigned                      number_time_steps_;
};

} /* namespace agelengths */
} /* namespace niwa */

#endif /* AGELENGTHS_DATA_H_ */
