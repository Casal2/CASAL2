/**
 * @file RecruitmentBevertonHolt.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the Beverton-Holt recruitment process
 */
#ifndef PROCESSES_RECRUITMENTBEVERTONHOLT_H_
#define PROCESSES_RECRUITMENTBEVERTONHOLT_H_

// headers
#include "Common/DerivedQuantities/DerivedQuantity.h"
#include "Common/Partition/Accessors/Categories.h"
#include "Common/Processes/Process.h"

// namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;

/**
 * class definition
 */
class RecruitmentBevertonHolt : public niwa::Process {
public:
  // methods
  RecruitmentBevertonHolt(Model* model);
  virtual                     ~RecruitmentBevertonHolt() = default;
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
  string                      ssb_;
  unsigned                    ssb_offset_;
  vector<Double>              ssb_values_;
  vector<unsigned>            ycs_years_;
  vector<Double>              ycs_values_;
  map<unsigned, Double>       ycs_value_by_year_;
  vector<Double>              true_ycs_values_;
  map<unsigned, Double>       stand_ycs_value_by_year_;
  vector<Double>              recruitment_values_;
  bool                        have_scaled_partition = false;
  vector<unsigned>            standardise_ycs_;
  DerivedQuantity*            derived_quantity_ = nullptr;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* PROCESSES_RECRUITMENTBEVERTONHOLT_H_ */
