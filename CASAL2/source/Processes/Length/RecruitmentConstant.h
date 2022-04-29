/**
 * @file ConstantRecruitment.h
 * @author  C.Marsh
 * @version 1.0
 * @date 12/18/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class represents a constant recruitment class. Every year
 * fish will be recruited (bred/born etc) in to some user defined length bins.
 * of categories
 *
 */
#ifndef LENGTH_CONSTANTRECRUITMENT_H_
#define LENGTH_CONSTANTRECRUITMENT_H_

// Headers
#include "../../Partition/Accessors/Categories.h"
#include "../../Processes/Process.h"
#include "../../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {

using niwa::utilities::Double;
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

/**
 * Class definition
 */
class RecruitmentConstant : public niwa::Process {
public:
  // Methods
  explicit RecruitmentConstant(shared_ptr<Model> model);
  virtual ~RecruitmentConstant() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void DoExecute() override final;

  // accessor
  const vector<string>& category_labels() const { return category_labels_; }

private:
  // Members
  vector<string>             category_labels_;
  vector<Double>             proportions_;
  OrderedMap<string, Double> proportions_categories_;
  Double                     r0_;
  vector<Double>             length_bins_;
  accessor::Categories       partition_;
  Double                     r0_by_length_bin_;
};

} /* namespace length */
}  // namespace processes
} /* namespace niwa */
#endif /* LENGTH_CONSTANTRECRUITMENT_H_ */
