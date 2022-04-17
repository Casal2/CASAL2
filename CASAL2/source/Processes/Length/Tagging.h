/**
 * @file Tagging.h
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
#ifndef LENGTH_TAGGING_H_
#define LENGTH_TAGGING_H_

// Headers
#include "../../Partition/Accessors/Categories.h"
#include "../../Processes/Process.h"
#include "../../Utilities/Types.h"
#include "Penalties/Penalty.h"

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
class Tagging : public niwa::Process {
public:
  // Methods
  explicit Tagging(shared_ptr<Model> model);
  virtual ~Tagging();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void DoExecute() override final;

  // accessor

private:
  // Members
  parameters::Table*            proportions_table_ = nullptr;
  // members
  vector<string>                from_category_labels_;
  vector<string>                to_category_labels_;
  accessor::Categories          to_partition_;
  accessor::Categories          from_partition_;
  vector<unsigned>              years_;
  vector<string>                selectivity_labels_;
  map<string, Selectivity*>     selectivities_;
  string                        penalty_label_                       = "";
  Penalty*                      penalty_                             = nullptr;
  double                        u_max_                               = 0;
  Double                        initial_mortality_                   = 0;
  string                        initial_mortality_selectivity_label_ = "";
  Selectivity*                  initial_mortality_selectivity_       = nullptr;
  vector<Double>                n_;
  unsigned                      first_year_        = 0;
  map<unsigned, vector<Double>> numbers_;
};

} /* namespace length */
}  // namespace processes
} /* namespace niwa */
#endif /* LENGTH_CONSTANTRECRUITMENT_H_ */
