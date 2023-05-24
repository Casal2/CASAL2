/**
 * @file TagLossEmpirical.h
 * @author  A. Dunn
 * @version 1.0
 * @date 20/05/2023
 * @section LICENSE
 *
 * @section DESCRIPTION
 *
 * This class is an instantaneous removal of individuals from the tagging partition. Currently we delete them from the partition.
 * In reality and in the future we will want category transition from tagged to untagged. This is similar to the TagLoss process,
 * except annual rates for times at liberty are not calculated, but instead supplied by the user
 *
 */
#ifndef TAGLOSSEMPIRICAL_H_
#define TAGLOSSEMPIRICAL_H_

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
class TagLossEmpirical : public niwa::Process {
public:
  // Methods
  TagLossEmpirical(shared_ptr<Model> model);
  virtual ~TagLossEmpirical() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final{};
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final{};

private:
  // Members
  vector<string>        category_labels_;
  vector<Double>        tag_loss_input_;
  vector<Double>        ratios_;
  map<unsigned, Double> time_step_ratios_;
  vector<string>        selectivity_names_;
  accessor::Categories  partition_;
  vector<Selectivity*>  selectivities_;
  unsigned              year_;
  vector<unsigned>      years_at_liberty_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TAGLOSSEMPIRICAL_H_ */
