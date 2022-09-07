/*
 * Selectivity.h
 *
 *  Created on: 21/12/2012
 *      Author: Admin
 */

#ifndef SELECTIVITY_H_
#define SELECTIVITY_H_

// Headers
#include <map>

#include "../BaseClasses/Object.h"
#include "../Model/Model.h"
#include "../Utilities/PartitionType.h"
#include "../Utilities/Types.h"

// Namespaces
namespace niwa {
class AgeLength;

// Using
using niwa::utilities::Double;
using std::map;
using utilities::Vector3;

/**
 * Class Definition
 */
class Selectivity : public niwa::base::Object {
public:
  // Methods
  Selectivity() = delete;
  explicit Selectivity(shared_ptr<Model> model);
  virtual ~Selectivity() = default;
  void           Validate();
  virtual void   Build() {Reset(); RebuildCache();};
  void           Verify(shared_ptr<Model> model){};
  void           Reset();
  virtual Double GetAgeResult(unsigned age, AgeLength* age_length);
  virtual Double GetLengthResult(unsigned length_bin_index);
  virtual Double GetResult(Double x);
  bool           IsSelectivityLengthBased() const { return length_based_; }
  void           RebuildCache();

  Vector3*       GetCache(AgeLength* age_length);



  bool length_based() const { return length_based_; }
  // now we can print length based selectivities in age-based models
  virtual Double get_value(Double value)                                                                                = 0;
  virtual Double get_value(unsigned value)                                                                              = 0;
protected:
  // pure methods
  virtual Double GetLengthBasedResult(unsigned age, AgeLength* age_length);
  virtual void   DoValidate()                                                                                           = 0;
  virtual void   DoReset();


  // Members
  shared_ptr<Model> model_   = nullptr;
  unsigned          n_quant_ = 5;
  vector<Double>    quantiles_;
  vector<Double>    quantiles_at_;
  bool              length_based_ = false;
  //  map<unsigned, Double>       values_;
  vector<Double>       values_;
  vector<Double>       length_values_;
  string               partition_type_label_ = "";
  PartitionType        partition_type_       = PartitionType::kInvalid;
  unsigned             age_index_;
  bool                 allowed_length_based_in_age_based_model_ = false;
  map<string, Vector3> cached_age_length_values_;  // map<age_length_label, cache(year x time_step x age)>
};
} /* namespace niwa */
#endif /* SELECTIVITY_H_ */
