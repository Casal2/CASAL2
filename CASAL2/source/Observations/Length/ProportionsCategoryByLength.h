/**
 * @file ProportionsCategoryByLength.h
 * @author  C. Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section This Observation is the at age maturation class, where users can give proportions mature at age to help estimate maturity ogives
 *
 * The time class represents a moment of time.
 *
 */
#ifndef LENGTH_OBSERVATIONS_PROPORTIONS_CATEGORY_H_
#define LENGTH_OBSERVATIONS_PROPORTIONS_CATEGORY_H_

// Headers
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"

// Namespace
namespace niwa {
  class Selectivity;
namespace observations {
namespace length {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class Definition
 */
class ProportionsCategoryByLength : public niwa::Observation {
public:
  // Methods
  explicit ProportionsCategoryByLength(shared_ptr<Model> model);
  virtual ~ProportionsCategoryByLength();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void PreExecute() override final;
  void Execute() override final;
  void CalculateScore() override final;
  bool HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>            years_;
  vector<double>              length_bins_;
  bool                        length_plus_;  
  unsigned                    number_bins_ = 0;
  bool                        using_model_length_bins = true;
  vector<int>                 map_local_length_bins_to_global_length_bins_;

  parameters::Table*          obs_table_  = nullptr;
  vector<Double>              process_error_values_;
  map<unsigned, Double>       process_errors_by_year_;
  parameters::Table*          error_values_table_ = nullptr;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  vector<string>              total_category_labels_;
  CachedCombinedCategoriesPtr total_cached_partition_;
  CombinedCategoriesPtr       total_partition_;
  Double                      time_step_proportion_;
  vector<Double>              total_numbers_at_length_;
  vector<Double>              cached_total_numbers_at_length_;
  vector<Double>              numbers_at_length_;
  vector<Double>              cached_numbers_at_length_;
  vector<string>              total_selectivity_labels_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  vector<Selectivity*>        total_selectivities_;

  vector<Double>              length_results_;
  vector<string>              categories_for_comparison_;
  vector<double>              length_bins_for_comparison_;
  string                      time_step_label_ = "";
  bool                        simulated_data_sum_to_one_;
  bool                        sum_to_one_;

  map<unsigned, vector<double>> proportions_;
  map<unsigned, vector<double>> error_values_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* LENGTH_OBSERVATIONS_PROPORTIONS_CATEGORY_H_ */
