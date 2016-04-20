/**
 * @file TagByLength.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_PROCESSES_CHILDREN_TAGBYLENGTH_H_
#define SOURCE_PROCESSES_CHILDREN_TAGBYLENGTH_H_

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
class TagByLength : public Process {
public:
  // method
  explicit TagByLength(Model* model);
  virtual                     ~TagByLength();
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // members
  vector<string>                from_category_labels_;
  vector<string>                to_category_labels_;
  accessor::Categories          to_partition_;
  accessor::Categories          from_partition_;
  vector<unsigned>              years_;
  vector<Double>                length_bins_;
  bool                          plus_group_ = false;
  Double                        max_length_ = 0.0;
  vector<string>                selectivity_labels_;
  map<string, Selectivity*>     selectivities_;
  string                        penalty_label_ = "";
  Penalty*                      penalty_ = nullptr;
  Double                        u_max_ = 0;
  Double                        initial_mortality_ = 0;
  string                        initial_mortality_selectivity_label_ = "";
  Selectivity*                  initial_mortality_selectivity_ = nullptr;
  vector<Double>                n_;
  parameters::Table*            numbers_table_ = nullptr;
  parameters::Table*            proportions_table_ = nullptr;
  unsigned                      first_year_ = 0;
  map<unsigned, vector<Double>> numbers_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_TAGBYLENGTH_H_ */
