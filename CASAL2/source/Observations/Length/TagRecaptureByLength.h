/**
 * @file TagRecaptureByLength.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTH_OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_
#define LENGTH_OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_

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
 * Class definition
 */
class TagRecaptureByLength : public niwa::Observation {
public:
  // methods
  TagRecaptureByLength(shared_ptr<Model> model);
  virtual ~TagRecaptureByLength() = default;
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
  double                      detection_;
  vector<string>              tagged_selectivity_labels_;
  parameters::Table*          recaptures_table_ = nullptr;
  vector<Double>              process_error_values_;
  map<unsigned, Double>       process_errors_by_year_;
  parameters::Table*          scanned_table_ = nullptr;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  CachedCombinedCategoriesPtr tagged_cached_partition_;
  CombinedCategoriesPtr       tagged_partition_;
  vector<Selectivity*>        tagged_selectivities_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  string                      time_step_label_ = "";
  Double                      time_step_proportion_;

  unsigned                    number_bins_ = 0;
  bool                        using_model_length_bins = true;
  vector<int>                 map_local_length_bins_to_global_length_bins_;
  vector<double>              length_bins_;
  bool                        length_plus_;  // have to be assigned a value to be optional, note in the constructpr the default is model_->length_plus()
  bool                        simulated_data_sum_to_one_;
  bool                        sum_to_one_;

  vector<vector<Double>>         numbers_at_length_;
  vector<vector<Double>>         tagged_cached_numbers_at_length_;
  vector<vector<Double>>         cached_numbers_at_length_;
  vector<vector<Double>>         tagged_numbers_at_length_;
  vector<Double>                 length_results_;
  vector<Double>                 tagged_length_results_;

  vector<string>                 tagged_category_labels_;
  vector<vector<string>>         tagged_category_split_labels_;
  vector<vector<string>>         category_split_labels_;

  map<unsigned, map<string, vector<double>>> recaptures_;
  map<unsigned, map<string, vector<double>>> scanned_;
};

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */

#endif /* LENGTH_OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_ */
