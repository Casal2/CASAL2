/**
 * @file TagRecaptureByLength.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 28/10/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_
#define AGE_OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_

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
 * Class definition
 */
class TagRecaptureByLength : public niwa::Observation {
public:
  // methods
  TagRecaptureByLength(shared_ptr<Model> model);
  virtual ~TagRecaptureByLength() = default;
  void         DoValidate() override final;
  virtual void DoBuild() override;
  void         DoReset() override final{};
  void         PreExecute() override final;
  void         Execute() override final;
  void         CalculateScore() override final;
  bool         HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>   years_;
  string             time_step_label_ = "";
  Double             detection_;
  vector<double>     length_bins_;
  bool               length_plus_ = false;
  string             ageing_error_label_;
  parameters::Table* recaptures_table_ = nullptr;
  parameters::Table* scanned_table_    = nullptr;
  Double             time_step_proportion_;
  unsigned           number_bins_;
  bool               using_model_length_bins = true;
  vector<int>        map_local_length_bins_to_global_length_bins_;

  // Category members
  vector<string>         tagged_category_labels_;
  vector<vector<string>> tagged_category_split_labels_;
  vector<vector<string>> category_split_labels_;

  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  CachedCombinedCategoriesPtr tagged_cached_partition_;
  CombinedCategoriesPtr       tagged_partition_;

  // Selectivity members
  vector<Selectivity*> tagged_selectivities_;
  vector<string>       selectivity_labels_;
  vector<Selectivity*> selectivities_;
  vector<string>       tagged_selectivity_labels_;

  //
  vector<Double>         length_results_;
  vector<Double>         tagged_length_results_;
  vector<vector<Double>> numbers_at_length_;
  vector<vector<Double>> tagged_cached_numbers_at_length_;
  vector<vector<Double>> cached_numbers_at_length_;
  vector<vector<Double>> tagged_numbers_at_length_;

  map<unsigned, map<string, vector<double>>> recaptures_;
  map<unsigned, map<string, vector<double>>> scanned_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */
#endif /* AGE_OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_ */
