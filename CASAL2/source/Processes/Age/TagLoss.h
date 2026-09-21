/**
 * @file TagLoss.h
 * @author  C.Marsh
 * @version 1.0
 * @date 20/04/2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is an instantaneous removal of individuals from the tagging partition. Currently we delete them from the partition.
 * In reality and in the future we will want category transition from tagged to untagged.
 *
 * This single class implements both tag_loss and tag_loss_empirical (formerly the separate
 * TagLossEmpirical class). Every variant applies the same RemovalFormulation::kExponentialDecay
 * removal via MortalityRateBase::ApplyRemovalLoop(); they differ only in where the rate comes from:
 *
 *  - kSingle     (tag_loss, tag_loss_type single): the per-category tag_loss_rate
 *  - kDoubleTag  (tag_loss, tag_loss_type double): a per-category rate derived from years since tagging
 *  - kEmpirical  (tag_loss_empirical): the tag_loss_rate supplied for the matching years_at_liberty,
 *                applied to every category
 *
 * Deliberate behaviour normalisations made by the merge (all only affect invalid configurations):
 *  - An unrecognised tag_loss_type is now an error. Previously no tag loss was applied, silently.
 *  - Duplicate years_at_liberty values are now an error. Previously they indexed past the end of
 *    the selectivities (undefined behaviour).
 *  - A time_step_proportions count that does not match the assigned time steps is reported as an
 *    error rather than a fatal error, via the shared MortalityRateBase::BuildTimeStepRatios().
 */
#ifndef TAGLOSS_H_
#define TAGLOSS_H_

// Headers
#include "Processes/Common/Mortality/MortalityRateBase.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {
using utilities::OrderedMap;

/**
 * Where the tag loss rate applied each year comes from. tag_loss is constructed as kSingle and
 * resolved to kSingle or kDoubleTag from tag_loss_type in DoValidate().
 */
enum class TagLossRateSource {
  kSingle,     // tag_loss, tag_loss_type single
  kDoubleTag,  // tag_loss, tag_loss_type double
  kEmpirical,  // tag_loss_empirical
};

/**
 * Class Definition
 */
class TagLoss : public common::MortalityRateBase {
public:
  // Methods
  TagLoss(shared_ptr<Model> model, TagLossRateSource rate_source);
  virtual ~TagLoss() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final{};
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final{};

private:
  // Members
  TagLossRateSource          rate_source_;
  vector<Double>             tag_loss_input_;
  OrderedMap<string, Double> tag_loss_;
  string                     tag_loss_type_;
  vector<double>             ratios_;
  map<unsigned, double>      time_step_ratios_;
  unsigned                   year_;
  vector<unsigned>           years_at_liberty_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TAGLOSS_H_ */
