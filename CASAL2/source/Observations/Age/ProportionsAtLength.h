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
#ifndef AGE_OBSERVATIONS_PROPORTIONSATLENGTH_H_
#define AGE_OBSERVATIONS_PROPORTIONSATLENGTH_H_

// Headers
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"

// Namespace
namespace niwa {
class Selectivity;
namespace observations {
namespace age {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class Definition
 */
class ProportionsAtLength : public niwa::Observation {
public:
  // Methods
  explicit ProportionsAtLength(shared_ptr<Model> model);
  virtual ~ProportionsAtLength();
  void         DoValidate() override final;
  virtual void DoBuild() override;
  void         DoReset() override final{};
  void         PreExecute() override final;
  void         Execute() override final;
  void         CalculateScore() override final;
  bool         HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>            years_;
  parameters::Table*          obs_table_ = nullptr;
  Double                      tolerance_ = 0.0;
  vector<Double>              process_error_values_;
  map<unsigned, Double>       process_errors_by_year_;
  parameters::Table*          error_values_table_ = nullptr;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  string                      time_step_label_ = "";
  vector<Double>              expected_values_;
  unsigned                    number_bins_ = 0;
  vector<Double>              numbers_at_length_;
  vector<Double>              cached_numbers_at_length_;

  vector<double> length_bins_;
  bool           length_plus_     = false;
  unsigned       mlb_index_first_ = 0;  // index of model length bin for length_bins_[0]

  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROPORTIONSATLENGTH_H_ */
