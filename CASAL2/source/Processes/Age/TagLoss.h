/**
 * @file TagLoss.h
 * @author  C.Marsh
 * @version 1.0
 * @date 20/04/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is an instantaneous removal of individuals from the tagging partition. Currently we delete them from the partition.
 * In reality and in the future we will want category transition from tagged to untagged.
 *
 */
#ifndef TAGLOSS_H_
#define TAGLOSS_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
class Selectivity;
namespace processes {
namespace age {
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;
/**
 * Class Definition
 */
class TagLoss : public niwa::Process {
public:
  // Methods
  TagLoss(shared_ptr<Model> model);
  virtual                     ~TagLoss() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;
  void                        FillReportCache(ostringstream& cache) override final { };
  void                        FillTabularReportCache(ostringstream& cache, bool first_run) override final { };

private:
  // Members
  vector<string>              category_labels_;
  vector<Double>              tag_loss_input_;
  OrderedMap<string, Double>  tag_loss_;
  string                      tag_loss_type_ = "single";
  vector<Double>              ratios_;
  map<unsigned, Double>       time_step_ratios_;
  vector<string>              selectivity_names_;
  accessor::Categories        partition_;
  vector<Selectivity*>        selectivities_;
  unsigned                    year_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TAGLOSS_H_ */
