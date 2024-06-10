/**
 * @file RecruitmentBevertonHolt.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is the Beverton-Holt recruitment process
 */
#ifndef PROCESSES_RECRUITMENTBEVERTONHOLT_LENGTH_H_
#define PROCESSES_RECRUITMENTBEVERTONHOLT_LENGTH_H_

// headers
#include "../../DerivedQuantities/DerivedQuantity.h"
#include "../../Partition/Accessors/Categories.h"
#include "../Process.h"

// namespaces
namespace niwa {
namespace processes {
namespace length {

namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

/**
 * class definition
 */
class RecruitmentBevertonHolt : public niwa::Process {
public:
  // methods
  RecruitmentBevertonHolt(shared_ptr<Model> model);
  virtual ~RecruitmentBevertonHolt() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void DoVerify(shared_ptr<Model> model) override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  void ScalePartition();
  void RebuildCache() override final {DoReset();}
  bool has_partition_been_scaled() {return have_scaled_partition;};

  // accessor
  unsigned              ssb_offset() { return ssb_offset_; };
  string                ssb_label() { return ssb_label_; };
  bool                  b0_initialised() { return b0_initialised_; };
  const vector<string>& category_labels() const { return category_labels_; }

private:
  // members
  accessor::Categories       partition_;
  vector<string>             category_labels_;
  Double                     r0_ = 0.0;
  string                     phase_b0_label_;
  unsigned                   phase_b0_;
  Double                     b0_             = 0.0;
  bool                       b0_initialised_ = false;
  vector<Double>             proportions_;
  Double                     steepness_ = 0.0;
  string                     ssb_label_;
  unsigned                   ssb_offset_;
  vector<Double>             ssb_values_;
  vector<Double>             true_ycs_values_;
  vector<Double>             recruitment_values_;


  // 
  vector<unsigned>           spawn_event_years_;
  vector<Double>             recruitment_multipliers_;
  map<unsigned, Double>      recruitment_multipliers_by_year_;
  map<unsigned, Double>      standardised_recruitment_multipliers_by_year_;

  bool                       have_scaled_partition = false;
  vector<unsigned>           standardise_years_;
  DerivedQuantity*           derived_quantity_ = nullptr;
  unsigned                   year_counter_     = 0;
  OrderedMap<string, Double> proportions_by_category_;
  bool                       standardise_recruitment_multipliers_ = true;
  vector<Double>             initial_mean_length_;
  vector<Double>             initial_length_cv_;
  vector<vector<Double>>     initial_length_distribution_; // category x length bins            
  vector<unsigned>           years_;

  // deprecated objects
  vector<unsigned>           ycs_years_;
  vector<unsigned>           standardise_ycs_years_;
  vector<Double>             ycs_values_;
  Double                     mean_ycs_ = 1.0;
  bool                       project_standardised_ycs_ = true;


};

} /* namespace length */
} /* namespace processes */
} /* namespace niwa */

#endif /* PROCESSES_RECRUITMENTBEVERTONHOLT_LENGTH_H_ */
