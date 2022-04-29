/**
 * @file ProportionsByCategory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_PROPORTIONSBYCATEGORY_H_
#define AGE_OBSERVATIONS_PROPORTIONSBYCATEGORY_H_

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
class ProportionsByCategory : public niwa::Observation {
public:
  // methods
  ProportionsByCategory(shared_ptr<Model> model);
  virtual ~ProportionsByCategory();
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
  unsigned                    min_age_    = 0;
  unsigned                    max_age_    = 0;
  bool                        plus_group_ = false;
  unsigned                    age_spread_ = 0;
  vector<string>              target_category_labels_;
  vector<string>              target_selectivity_labels_;
  parameters::Table*          obs_table_ = nullptr;
  vector<Double>              process_error_values_;
  map<unsigned, Double>       process_errors_by_year_;
  string                      ageing_error_label_;
  parameters::Table*          error_values_table_ = nullptr;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  CachedCombinedCategoriesPtr target_cached_partition_;
  CombinedCategoriesPtr       target_partition_;
  vector<Double>              age_results_;
  vector<Selectivity*>        target_selectivities_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  string                      time_step_label_ = "";

  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROPORTIONSBYCATEGORY_H_ */
