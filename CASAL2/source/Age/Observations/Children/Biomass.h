/**
 * @file Biomass.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_BIOMASS_H_
#define AGE_OBSERVATIONS_BIOMASS_H_

// headers
#include "Common/Observations/Observation.h"

#include "Common/Catchabilities/Catchability.h"
#include "Common/Partition/Accessors/CombinedCategories.h"
#include "Common/Partition/Accessors/Cached/CombinedCategories.h"
#include "Common/Catchabilities/Children/Nuisance.h"
#include "Age/AgeWeights/AgeWeight.h"

// namespaces
namespace niwa {
class Selectivity;
namespace age {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using catchabilities::Nuisance;

/**
 * class definition
 */
class Biomass : public niwa::Observation {
public:
  // methods
  Biomass(Model* model);
  virtual                     ~Biomass() = default;
  void                        DoValidate() override final;
  virtual void                DoBuild() override;
  void                        DoReset() override final { };
  void                        PreExecute() override final;
  void                        Execute() override final;
  void                        CalculateScore() override final;
  bool                        HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // members
  vector<unsigned>                years_;
  map<unsigned, vector<Double> >  proportions_by_year_;
  map<unsigned, Double>           error_values_by_year_;
  vector<Double>                  error_values_;
  string                          catchability_label_;
  Catchability*                   catchability_ = nullptr;
  Double                          process_error_value_;
  CachedCombinedCategoriesPtr     cached_partition_;
  CombinedCategoriesPtr           partition_;
  vector<string>                  obs_;
  vector<string>                  selectivity_labels_;
  vector<Selectivity*>            selectivities_;
  string                          time_step_label_ = "";
  Nuisance*                       nuisance_catchability_ = nullptr;
  bool                            nuisance_q_ = false;
  vector<string>                  age_weight_labels_;
  vector<AgeWeight*>              age_weights_;
};

} /* namespace observations */
} /* namespace niwa */
} /* namespace age */

#endif /* AGE_OBSERVATIONS_BIOMASS_H_ */
