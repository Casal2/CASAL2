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
#include "DerivedQuantities/DerivedQuantity.h"
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"

// namespaces
namespace isam {
namespace processes {

namespace accessor = isam::partition::accessors;

/**
 * class definition
 */
class RecruitmentBevertonHolt : public isam::Process {
public:
  // methods
  RecruitmentBevertonHolt();
  virtual                     ~RecruitmentBevertonHolt() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoPreExecute() override final { };
  void                        DoReset() override final;
  void                        Execute() override final;

private:
  // members
  ModelPtr                    model_;
  accessor::CategoriesPtr     partition_;
  vector<string>              category_labels_;
  Double                      r0_;
  string                      phase_b0_label_;
  unsigned                    phase_b0_;
  Double                      b0_;
  vector<Double>              proportions_;
  unsigned                    age_;
  Double                      steepness_;
  string                      ssb_;
  unsigned                    ssb_offset_;
  vector<Double>              ssb_values_;
  vector<Double>              ycs_years_;
  vector<Double>              ycs_values_;
  vector<Double>              true_ycs_values_;
  vector<Double>              recruitment_values_;
  vector<unsigned>            standardise_ycs_;
  DerivedQuantityPtr          derived_quantity_;
};

} /* namespace processes */
} /* namespace isam */

#endif /* PROCESSES_RECRUITMENTBEVERTONHOLT_H_ */
