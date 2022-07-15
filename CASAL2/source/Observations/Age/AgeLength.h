/**
 * @file AgeLength.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 */
#ifndef AGE_LENGTH_OBSERVATIONS_H_
#define AGE_LENGTH_OBSERVATIONS_H_

// Headers
#include "AgeingErrors/AgeingError.h"
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"

// Namespace
namespace niwa {
class Selectivity;
namespace observations {
namespace age {

// Enumerated Types
enum class SampleType {
  kAge        = 0,
  kLength     = 1,
  kRandom     = 2,
};

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class Definition
 */
class AgeLength : public niwa::Observation {
public:
  // Methods
  explicit AgeLength(shared_ptr<Model> model);
  virtual ~AgeLength();
  void         DoValidate() override final;
  virtual void DoBuild() override;
  void         DoReset() override final{};
  void         PreExecute() override final;
  void         Execute() override final;
  void         CalculateScore() override final;
  bool         HasYear(unsigned year) const override final { return year == year_; }

protected:
  // Members
  unsigned                    year_;
  vector<unsigned>            individual_ages_;
  vector<double>              individual_lengths_;
  vector<string>              individual_categories_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  string                      time_step_label_;   
  bool                        plus_group_ = false;
  unsigned                    age_spread_; 
  string                      sample_type_;
  SampleType                  actual_sample_type_;
  vector<vector<Double>>      ageing_error_matrix_;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  vector<Double>              process_error_values_;
  string                      ageing_error_label_;
  AgeingError*                ageing_error_ = nullptr;
  bool                        selectivity_for_combined_categories_ = false;
  Double                      time_step_proportion_;
  vector<Double>              quantiles_;
  vector<Double>              quantile_breaks_;             
  vector<double>              unique_lengths_;
  vector<double>              unique_ages_;
  vector<vector<Double>>      pdf_by_age_and_length_;
  vector<vector<Double>>      pre_numbers_at_age_;
  vector<vector<Double>>      numbers_at_age_;
  unsigned                    n_fish_;
  bool                        combined_categories_provided_ = false;
  vector<string>              split_category_labels_; // only used in validation
  bool                        apply_ageing_error_ = false;
  vector<Double>              numbers_by_unique_size_;


  // vectors to do lookups on combined categories populated during DoBuild()
  vector<unsigned>            category_group_ndx_;
  vector<unsigned>            category_combined_ndx_;
  bool                        supplied_individual_categories_ = false;

};

} /* namespace age */
}  // namespace observations
}  // namespace niwa
#endif /* AGE_LENGTH_OBSERVATIONS_H_ */