/**
 * @file Abundance.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTH_OBSERVATIONS_ABUNDANCE_H_
#define LENGTH_OBSERVATIONS_ABUNDANCE_H_

// Headers
#include "../../Catchabilities/Catchability.h"
#include "../../Catchabilities/Common/Nuisance.h"
#include "../../Partition/Accessors/Cached/CombinedCategories.h"
#include "../../Partition/Accessors/CombinedCategories.h"
#include "../Observation.h"


// Namespaces
namespace niwa {
class Selectivity;
namespace observations {
namespace length {

using catchabilities::Nuisance;
using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class definition
 */
class Abundance : public niwa::Observation {
public:
  // Methods
  Abundance(shared_ptr<Model> model);
  virtual ~Abundance();
  void         DoValidate() override final;
  virtual void DoBuild() override;
  void         DoReset() override final;
  void         PreExecute() override final;
  void         Execute() override final;
  void         CalculateScore() override final;
  bool         HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>            years_;
  string                      catchability_label_  = "";
  Catchability*               catchability_        = nullptr;
  Double                      process_error_value_ = 0;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  Double                      proportion_of_time_ = 0;
  parameters::Table*          obs_table_          = nullptr;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  string                      time_step_label_       = "";
  Nuisance*                   nuisance_catchability_ = nullptr;
  bool                        nuisance_q_            = false;

  map<unsigned, vector<double> > proportions_by_year_;
  map<unsigned, double>          error_values_by_year_;
  bool                         calculate_nuisance_q_ = true; // more for simualtions, if nuisance Q we don't want to keep applying it to expected values

};

}  // namespace length
} /* namespace observations */
} /* namespace niwa */
#endif /* AGE_OBSERVATIONS_ABUNDANCE_H_ */
