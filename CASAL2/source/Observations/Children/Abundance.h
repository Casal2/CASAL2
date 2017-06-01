/**
 * @file Abundance.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_ABUNDANCE_H_
#define OBSERVATIONS_ABUNDANCE_H_

// Headers
#include "Observations/Observation.h"

#include "Catchabilities/Catchability.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Catchabilities/Children/Nuisance.h"

// Namespaces
namespace niwa {
class Selectivity;
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using catchabilities::Nuisance;

/**
 * Class definition
 */
class Abundance : public niwa::Observation {
public:
  // Methods
  //Abundance() = delete;
  Abundance(Model* model);
  virtual                     ~Abundance() = default;
  void                        DoValidate() override final;
  virtual void                DoBuild() override;
  void                        DoReset() override final { };
  void                        PreExecute() override final;
  void                        Execute() override final;
  void                        CalculateScore() override final;
  bool                        HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>                years_;
  map<unsigned, vector<Double> >  proportions_by_year_;
  map<unsigned, Double>           error_values_by_year_;
  vector<Double>                  error_values_;
  string                          catchability_label_ = "";
  Catchability*                   catchability_ = nullptr;
  Double                          process_error_value_ = 0;
  CachedCombinedCategoriesPtr     cached_partition_;
  CombinedCategoriesPtr           partition_;
  vector<string>                  obs_;
  Double                          proportion_of_time_ = 0;
  vector<string>                  selectivity_labels_;
  vector<Selectivity*>            selectivities_;
  string                          time_step_label_ = "";
  Nuisance*                       nuisance_catchability_ = nullptr;
  bool                            nuisance_q_ = false;
};

} /* namespace observation */
} /* namespace niwa */
#endif /* OBSERVATIONS_ABUNDANCE_H_ */
