/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef AGELENGTHS_DATA_H_
#define AGELENGTHS_DATA_H_

// headers
#include "AgeLengths/AgeLength.h"
#include "LengthWeights/LengthWeight.h"
#include "Model/Model.h"

// namespaces
namespace niwa {
namespace agelengths {

// classes
class Data : public AgeLength {
public:
  // methods
  Data();
  explicit Data(ModelPtr model);
  virtual                     ~Data() = default;
  void                        DoValidate() override final {};
  void                        DoBuild() override final;
  void                        DoReset() override final {};
  Double                      mean_length(unsigned year, unsigned age) override final;
  Double                      mean_weight(unsigned year, unsigned age) override final;
  void                        BuildCV(unsigned year) override final;
  void                        DoAgeToLengthConversion(std::shared_ptr<partition::Category> category, const vector<Double>& length_bins, bool plus_grp) override final { };

private:
  // methods
  void                        FillExternalGaps();
  void                        FillInternalGaps();

  // members
  parameters::TablePtr          data_table_;
  string                        external_gaps_;
  string                        internal_gaps_;
  map<unsigned, vector<Double>> data_by_year_;
  vector<Double>                means_;
  string                        distribution_;
  string                        length_weight_label_;
  LengthWeightPtr               length_weight_;
};

} /* namespace agelengths */
} /* namespace niwa */

#endif /* AGELENGTHS_DATA_H_ */
