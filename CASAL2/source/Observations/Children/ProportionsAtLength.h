/**
 * @file ProportionsAtLength.h
 * @author  C.Marsh
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * Date 12.8/15
 */
#ifndef OBSERVATIONS_PROPORTIONSATLENGTH_H_
#define OBSERVATIONS_PROPORTIONSATLENGTH_H_

// Headers
#include "Observations/Observation.h"

#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"

// Namespace
namespace niwa {
class Selectivity;
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class Definition
 */
class ProportionsAtLength : public niwa::Observation {
public:
  // Methods
  explicit ProportionsAtLength(Model* model);
  virtual                     ~ProportionsAtLength();
  void                        DoValidate() override final;
  virtual void                DoBuild() override;
  void                        DoReset() override final { };
  void                        PreExecute() override final;
  void                        Execute() override final;
  void                        CalculateScore() override final;
  bool                        HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>              years_;
  vector<Double>                length_bins_;
  bool                          length_plus_ = false;
  unsigned                      number_bins_ = 0;
  parameters::Table*            obs_table_ = nullptr;
  Double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  parameters::Table*            error_values_table_ = nullptr;
  CachedCombinedCategoriesPtr   cached_partition_;
  CombinedCategoriesPtr         partition_;
  vector<Double>                length_results_;
  vector<string>                selectivity_labels_;
  vector<Selectivity*>          selectivities_;
  string                        time_step_label_ = "";

  map<unsigned, map<string, vector<Double>>> proportions_;
  map<unsigned, map<string, vector<Double>>> error_values_;
};

} /* namespace observations */
} /* namespace niwa */
#endif /* OBSERVATIONS_PROPORTIONSATLENGTH_H_ */
