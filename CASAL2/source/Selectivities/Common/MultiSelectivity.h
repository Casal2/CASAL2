/**
 * @file MultiSelectivity.h
 * @author  C.Marsh
 * @version 1.0
 * @date 16/08/2023
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 *
 */
#ifndef MULTISELECTIVITY_H_
#define MULTISELECTIVITY_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespacaes
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class MultiSelectivity : public niwa::Selectivity {
public:
  // Methods
  explicit MultiSelectivity(shared_ptr<Model> model);
  virtual ~MultiSelectivity() = default;
  void   DoValidate() override final;
  void   DoBuild() override final;
  Double GetAgeResult(unsigned age, AgeLength* age_length) override final;
  Double GetLengthResult(unsigned length_bin) override final;

  Double get_value(Double value) override final;
  Double get_value(unsigned value) override final;

private:
  // Members
  vector<unsigned>            years_;
  vector<string>              selectivity_labels_;
  map<unsigned, Selectivity*> selectivities_;
  string                      default_selectivity_label_;
  string                      projection_selectivity_label_;
  Selectivity*                default_selectivity_;
  Selectivity*                projection_selectivity_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* MULTISELECTIVITY_H_ */
