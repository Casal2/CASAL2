/**
 * @file RecruitmentBevertonHoltWithDeviations.h
 * @author C.Marsh
 * @date 11/7/17
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the Beverton-Holt recruitment process, that is parameterised with deviations instead of raw YCSs as is done in CASAL
 */
#ifndef PROCESSES_RECRUITMENTBEVERTONHOLT_WITHDEVIATIONS_H_
#define PROCESSES_RECRUITMENTBEVERTONHOLT_WITHDEVIATIONS_H_

// headers
#include "DerivedQuantities/DerivedQuantity.h"
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"

#include "Estimates/Estimate.h"
// namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;

/**
 * class definition
 */
class RecruitmentBevertonHoltWithDeviations : public niwa::Process {
public:
  // methods
  RecruitmentBevertonHoltWithDeviations(Model* model);
  virtual                     ~RecruitmentBevertonHoltWithDeviations() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;
  void                        ScalePartition();
  //accessor
  unsigned                    ssb_offset() {return ssb_offset_;};
  string                      ssb_label() {return ssb_;};
  bool                        bo_initialised() {return bo_initialised_;};

private:
  // members
  accessor::Categories        partition_;
  vector<string>              category_labels_;
  Double                      r0_ = 0.0;
  string                      phase_b0_label_;
  unsigned                    phase_b0_;
  Double                      b0_ = 0.0;
  bool                        bo_initialised_ = false;
  vector<Double>              proportions_;
  unsigned                    age_ = 0;
  Double                      steepness_ = 0.0;
  Double                      sigma_r_;
  string                      ssb_;
  unsigned                    ssb_offset_;
  vector<Double>              ssb_values_;
  vector<unsigned>            recruit_dev_years_;
  vector<Double>              recruit_dev_values_;
  map<unsigned, Double>       recruit_dev_value_by_year_;
  vector<Double>              true_ycs_values_;
  vector<Double>              recruitment_values_;
  bool                        have_scaled_partition = false;
  DerivedQuantity*            derived_quantity_ = nullptr;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* PROCESSES_RECRUITMENTBEVERTONHOLT_WITHDEVIATIONS_H_ */
