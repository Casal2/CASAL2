/**
 * @file TagByAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 26/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROCESSES_TAGBYAGE_H_
#define PROCESSES_TAGBYAGE_H_

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
class TagByAge : public niwa::Process {
public:
  // methods
  explicit TagByAge(shared_ptr<Model> model);
  virtual ~TagByAge();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;

private:
  // members
  vector<string>                from_category_labels_;
  vector<string>                to_category_labels_;
  unsigned                      number_categories_;
  accessor::Categories          to_partition_;
  accessor::Categories          from_partition_;
  vector<unsigned>              years_;
  unsigned                      min_age_    = 0.0;
  unsigned                      max_age_    = 0.0;
  unsigned                      age_spread_ = 0.0;
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
  map<unsigned, vector<Double>> numbers_;
  map<unsigned, Double>         n_by_year_;
  vector<vector<Double>>        numbers_at_age_by_category_;           // dims category x ages
  vector<vector<Double>>        selected_numbers_at_age_by_category_;  // dims category x ages
  vector<vector<Double>>        exploitation_by_age_category_;         // category x age bins
  vector<Double>                exploitation_by_age_;
  vector<Double>                final_exploitation_by_age_;
  vector<vector<Double>>        proportion_by_age_;            // year x length bins
  vector<Double>                tagged_fish_by_year_;          // year bins
  vector<Double>                tag_to_fish_by_age_;           // age bins
  vector<vector<Double>>        tag_to_fish_by_category_age_;  // category x age bins

  vector<Double>                vulnerable_fish_by_age_;                      // age bins
  unsigned                      min_age_offset_ = 0;
  // Containers for reporting
  vector<vector<vector<Double>>> tagged_fish_after_init_mort_;  // n_years x n_from_categories x n_ages
  vector<vector<vector<Double>>> actual_tagged_fish_to_;        // n_years x n_to_categories x n_ages
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* PROCESSES_TAGBYAGE_H_ */
