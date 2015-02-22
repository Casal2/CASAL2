/**
 * @file Category.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This object represents one category in the model.
 * Because stuff in this is built by the partition there is no need
 * to hide the members behind accessors.
 */
#ifndef PARTITION_CATEGORY_H_
#define PARTITION_CATEGORY_H_

// headers
#include <map>
#include <vector>
#include <string>

#include "AgeLengths/AgeLength.h"
#include "Utilities/Types.h"


// namespaces
namespace niwa {
namespace partition {

using std::string;
using std::map;
using std::vector;
using std::pair;
using niwa::utilities::Double;

/**
 * Class definition
 */
class Category {
public:
  // methods
  Category() = default;
  virtual                     ~Category() = default;
  void                        UpdateLengthData();
  void                        UpdateAgeData();
  void                        UpdateWeightData();

  // members
  string                      name_ = "";
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  vector<unsigned>            years_;
  vector<Double>              data_;
  map<unsigned, Double>       length_per_; // map<bin, number per>
  map<unsigned, Double>       weight_per_; // map<age, weight per>

  // accessors
  void                        set_age_length(AgeLengthPtr age_length) { age_length_ = age_length; }

private:
  // members
  AgeLengthPtr                age_length_ = AgeLengthPtr();
};

} /* namespace partitions */
} /* namespace niwa */

#endif /* PARTITION_CATEGORY_H_ */
