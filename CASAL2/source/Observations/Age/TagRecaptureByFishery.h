/**
 * @file TagRecaptureByFishery.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2023
 * @section LICENSE
 *
 * Copyright NIWA Science 2023 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_TAGRECAPTURE_BY_FISHERY_H_
#define AGE_OBSERVATIONS_TAGRECAPTURE_BY_FISHERY_H_

// Headers
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Processes/Age/Mortality.h"

// Namespace
namespace niwa {
class Selectivity;
namespace observations {
namespace age {
using processes::age::Mortality;
using partition::accessors::CombinedCategoriesPtr;

/**
 * Class definition
 */
class TagRecaptureByFishery : public niwa::Observation {
public:
  // methods
  TagRecaptureByFishery(shared_ptr<Model> model);
  virtual ~TagRecaptureByFishery() = default;
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
  Double                      reporting_rate_;
  vector<string>              tagged_category_labels_;
  parameters::Table*          recaptures_table_ = nullptr;
  CombinedCategoriesPtr       partition_;
  vector<string>              time_step_label_;
  vector<Double>              expected_values_;
  vector<Double>              accumulated_expected_values_;
  map<unsigned, vector<double>> observed_recaptures_; // year x category x ages
  map<unsigned, vector<Double>> expected_recaptures_; // year x category x ages

  age::Mortality*             mortality_process_ = nullptr;
  vector<string>              method_;
  string                      process_label_;
  unsigned                    time_step_to_execute_;

  vector<unsigned>        fishery_ndx_to_get_catch_at_info_;
  vector<unsigned>        year_ndx_to_get_catch_at_info_;
  map<string, unsigned >  category_lookup_for_ndx_to_get_catch_at_info_;

  vector<string>          allowed_mortality_types_;

};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_TAGRECAPTURE_BY_FISHERY_H_ */
