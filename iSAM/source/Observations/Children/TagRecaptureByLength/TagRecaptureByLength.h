/**
 * @file TagRecaptureByLength.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 28/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_
#define OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_

// Headers
#include "Observations/Observation.h"

#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"

// Namespace
namespace niwa {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class definition
 */
class TagRecaptureByLength : public niwa::Observation {
public:
  // methods
  TagRecaptureByLength(Model* model);
  virtual                     ~TagRecaptureByLength() = default;
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
  bool                          plus_group_ = false;
  unsigned                      length_spread_ = 0;
  Double                        detection_;
  vector<string>                target_category_labels_;
  vector<string>                target_selectivity_labels_;
  Double                        delta_ = 0.0;
  parameters::Table*            recaptures_table_ = nullptr;
  Double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        ageing_error_label_;
  parameters::Table*            scanned_table_ = nullptr;
  CachedCombinedCategoriesPtr   cached_partition_;
  CombinedCategoriesPtr         partition_;
  CachedCombinedCategoriesPtr   target_cached_partition_;
  CombinedCategoriesPtr         target_partition_;
  vector<Double>                age_results_;
  vector<Selectivity*>          target_selectivities_;

  map<unsigned, map<string, vector<Double>>> recaptures_;
  map<unsigned, map<string, vector<Double>>> scanned_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVATIONS_TAGRECAPTUREBYLENGTH_H_ */
