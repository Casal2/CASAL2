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

#include "AgeLengthKeys/AgeLengthKey.h"
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
  void                        UpdateMeanLengthData();
  void                        UpdateMeanWeightData();
  void                        ConvertAgeDataToLength();
  void                        ConvertLengthDataToAge();

  // members
  string                      name_ = "";
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  vector<unsigned>            years_;
  vector<Double>              data_;
  map<unsigned,map<unsigned, Double>> length_data_; // map<age, map<length, amount>>
  map<unsigned, Double>       length_per_; // map<bin, number per>
  map<unsigned, Double>       weight_per_; // map<age, weight per>


  // accessors
  void                        set_age_length(AgeLengthPtr value) { age_length_ = value; }
  void                        set_age_length_key(AgeLengthKeyPtr value) { age_length_key_ = value; }

private:
  // members
  AgeLengthPtr                age_length_ = AgeLengthPtr();
  AgeLengthKeyPtr             age_length_key_ = AgeLengthKeyPtr();
};

} /* namespace partitions */
} /* namespace niwa */

#endif /* PARTITION_CATEGORY_H_ */
