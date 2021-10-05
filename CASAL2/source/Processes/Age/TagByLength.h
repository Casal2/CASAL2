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
#include "Penalties/Penalty.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class TagByLength : public Process {
public:
  // method
  explicit TagByLength(shared_ptr<Model> model);
  virtual ~TagByLength();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;

private:
  // members
  vector<string>                from_category_labels_;
  vector<string>                to_category_labels_;
  accessor::Categories          to_partition_;
  accessor::Categories          from_partition_;
  vector<unsigned>              years_;
  vector<string>                selectivity_labels_;
  vector<string>                split_from_category_labels_;
  map<string, Selectivity*>     selectivities_;
  string                        penalty_label_                       = "";
  Penalty*                      penalty_                             = nullptr;
  double                        u_max_                               = 0;
  double                        tolerance_                           = 0;
  Double                        initial_mortality_                   = 0;
  string                        initial_mortality_selectivity_label_ = "";
  Selectivity*                  initial_mortality_selectivity_       = nullptr;
  vector<Double>                n_;
  parameters::Table*            numbers_table_     = nullptr;
  parameters::Table*            proportions_table_ = nullptr;
  unsigned                      first_year_        = 0;
  unsigned                      n_length_bins_;
  map<unsigned, vector<Double>> numbers_;
  map<unsigned, Double>         n_by_year_;
  vector<vector<Double>>        numbers_at_length_by_category_; // dims category x length bins
  vector<vector<Double>>        numbers_at_age_by_category_; // dims category x length bins

  unsigned                      min_age_;
  unsigned                      max_age_;
  unsigned                      age_spread_;

  // Containers for reporting
  vector<vector<vector<Double>>> actual_tagged_fish_from_;  // n_years x n_from_categories x n_ages
  vector<vector<vector<Double>>> actual_tagged_fish_to_;    // n_years x n_to_categories x n_ages
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_TAGBYLENGTH_H_ */
