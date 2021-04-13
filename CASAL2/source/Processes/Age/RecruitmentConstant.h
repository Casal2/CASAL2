/**
 * @file ConstantRecruitment.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents a constant recruitment class. Every year
 * fish will be recruited (bred/born etc) in to 1 age for a collection
 * of categories
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef CONSTANTRECRUITMENT_H_
#define CONSTANTRECRUITMENT_H_

// Headers
#include "Partition/Accessors/CategoriesWithAge.h"
#include "Processes/Process.h"
#include "Utilities/Types.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

using niwa::utilities::Double;
using niwa::partition::accessors::CategoriesWithAgePtr;
using utilities::OrderedMap;

/**
 * Class definition
 */
class RecruitmentConstant : public niwa::Process {
public:
  // Methods
  explicit RecruitmentConstant(shared_ptr<Model> model);
  virtual                     ~RecruitmentConstant() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;
  void                        FillReportCache(ostringstream& cache) override final;
  void                        FillTabularReportCache(ostringstream& cache, bool first_run) override final;
private:
  // Members
  vector<string>              category_labels_;
  vector<Double>              proportions_;
  OrderedMap<string, Double>  proportions_categories_;
  Double                      r0_;
  unsigned                    age_;
  CategoriesWithAgePtr        partition_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* CONSTANTRECRUITMENT_H_ */
