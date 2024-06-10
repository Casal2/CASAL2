/**
 * @file TagRecaptureByAge.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 23/10/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_TAGRECAPTUREBYAGE_H_
#define AGE_OBSERVATIONS_TAGRECAPTUREBYAGE_H_

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
class TagRecaptureByAge : public niwa::Observation {
public:
  // methods
  TagRecaptureByAge(shared_ptr<Model> model);
  virtual ~TagRecaptureByAge() = default;
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
  unsigned                    min_age_    = 0;
  unsigned                    max_age_    = 0;
  bool                        plus_group_ = false;
  unsigned                    age_spread_ = 0;
  Double                      detection_;
  vector<string>              tagged_category_labels_;
  vector<string>              tagged_selectivity_labels_;
  parameters::Table*          recaptures_table_ = nullptr;
  string                      ageing_error_label_;
  parameters::Table*          scanned_table_ = nullptr;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  CachedCombinedCategoriesPtr tagged_cached_partition_;
  CombinedCategoriesPtr       tagged_partition_;
  vector<Double>              age_results_;
  vector<Selectivity*>        tagged_selectivities_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  string                      time_step_label_ = "";
  Double                      time_step_proportion_;

  map<unsigned, map<string, vector<double>>> recaptures_;
  map<unsigned, map<string, vector<double>>> scanned_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_TAGRECAPTUREBYAGE_H_ */
