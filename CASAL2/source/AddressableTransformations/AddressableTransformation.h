/**
 * @file AddressableTransformation.h
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief As estimable transformation is a way to manipulate an addressable in the system and create a new transformed addressable
 *
 *
 *
 * @version 0.1
 * @date 2015-12-07
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SOURCE_ADDRESSABLETRANSFORMATIONS_H_
#define SOURCE_ADDRESSABLETRANSFORMATIONS_H_

// headers
#include "../BaseClasses/Object.h"
#include "../Model/Model.h"

// namespaces
namespace niwa {
class Model;
// classes
class AddressableTransformation : public niwa::base::Object {
    typedef void (AddressableTransformation::*RestoreFunction)(vector<Double>);

public:
  // methods
  AddressableTransformation() = delete;
  explicit AddressableTransformation(shared_ptr<Model> model);
  virtual ~AddressableTransformation() = default;
  void         Validate();
  void         Build();
  void         Reset(){Restore();}; // Restore in the reset
  void         Restore();

  // pure virtual
  vector<string>           GetParameterLabels() {return parameter_labels_;} // so managers can check business rules to catch duplicate addressables in multiple transformation blocks
  virtual Double           GetScore()                      = 0;

  // For reporting EstimableTransformation
  virtual void             FillReportCache(ostringstream& cache){};
  virtual void             PrepareForObjectiveFunction() = 0;
  virtual void             RestoreForObjectiveFunction() = 0;
protected:
  // methods
  virtual void DoValidate()  = 0;
  virtual void DoBuild()     = 0;
  virtual void DoRestore()   = 0;

  // settors
  // for univarite, or multiple univariate transformations this will be used.
  void set_single_values(vector<Double> values);
  void set_single_values_for_multiple_maps(vector<Double> values);
  void set_single_values_for_multiple_vectors(vector<Double> values);
  void set_single_values_for_multiple_string_maps(vector<Double> values);
  // how is process[Recruit].ycs_values{2013} treated? is this a single value or a map?
  // and process[Instan_mort].m{male} treated? is this a single value or a map?
  // I want these to be single values is that easy?

  // These should only be used for special transformations which operate on an entire map/subset or vector/subset vector
  // i.e only on process[Recruit].ycs_values or process[Recruit].ycs_values{1990:2013}
  // or 
  // selectivity[all_values_bounded].v or selectivity[all_values_bounded].v{1:5}
  // We can catch these from the above seperate case as if we 
  void set_vector_values(vector<Double> value);
  void set_map_values(vector<Double> value);
  void set_map_string_values(vector<Double> value);

  RestoreFunction restore_function_ = 0;

  // members
  shared_ptr<Model> model_                        = nullptr;
  bool              prior_applies_to_restored_parameters_ = false;
  Double            jacobian_                     = 0.0;
  vector<string>    parameter_labels_;

  vector<map<unsigned, Double>*> addressable_maps_;
  vector<OrderedMap<string, Double>*>   addressable_string_maps_;
  vector<vector<Double>*>        addressable_vectors_;
  vector<Double*>                addressables_;
  vector<base::Object*>          target_objects_;
  vector<vector<unsigned>>       vector_and_u_map_indicies_; // n_params x n_indicies
  vector<vector<string>>         string_map_indicies_; // n_params x n_indicies

  // each child will populate this.
  vector<Double>  restored_values_;
  // If there is no 
  vector<Double>  init_values_;
  // number of parameters
  unsigned        n_params_;




};

} /* namespace niwa */

#endif /* SOURCE_ADDRESSABLETRANSFORMATIONS_H_ */
