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
  vector<unsigned>              years_;
  vector<string>                from_category_labels_;
  vector<string>                to_category_labels_;
  vector<string>                selectivity_labels_;
  vector<SelectivityPtr>        selectivities_;
  string                        penalty_label_;
  PenaltyPtr                    penalty_;
  unsigned                      min_age_;
  unsigned                      max_age_;
  bool                          age_plus_;
  Double                        u_max_;
  Double                        initial_mortality_;
  Double                        constant_loss_rate_;
  vector<unsigned>              tag_loss_years_;
  vector<Double>                tag_loss_rate_;
  parameters::TablePtr          n_table_;
  map<unsigned, vector<Double>> n_;
  accessor::Categories          to_partition_;
  accessor::Categories          from_partition_;
  ModelPtr                      model_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* PROCESSES_TAGBYAGE_H_ */
