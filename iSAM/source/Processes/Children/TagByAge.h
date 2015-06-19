/**
 * @file TagByAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 26/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROCESSES_TAGBYAGE_H_
#define PROCESSES_TAGBYAGE_H_

// headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"
#include "Penalties/Penalty.h"

// namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class TagByAge : public niwa::Process {
public:
  // methods
  TagByAge();
  virtual                     ~TagByAge() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // members
  ModelPtr                      model_;
  vector<string>                from_category_labels_;
  vector<string>                to_category_labels_;
  accessor::Categories          to_partition_;
  accessor::Categories          from_partition_;
  vector<unsigned>              years_;
  unsigned                      min_age_ = 0;
  unsigned                      max_age_ = 0;
  vector<string>                selectivity_labels_;
  map<string, SelectivityPtr>   selectivities_;
  string                        penalty_label_ = "";
  PenaltyPtr                    penalty_;
  Double                        u_max_ = 0;
  Double                        initial_mortality_ = 0;
  string                        initial_mortality_selectivity_label_ = "";
  SelectivityPtr                initial_mortality_selectivity_;
  vector<Double>                loss_rate_;
  map<string, Double>           loss_rate_by_category_;
  vector<string>                loss_rate_selectivity_labels_;
  map<string, SelectivityPtr>   loss_rate_selectivity_by_category_;
  vector<Double>                n_;
  parameters::TablePtr          numbers_table_;
  parameters::TablePtr          proportions_table_;
  unsigned                      first_year_ = 0;
  map<unsigned, vector<Double>> numbers_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* PROCESSES_TAGBYAGE_H_ */
