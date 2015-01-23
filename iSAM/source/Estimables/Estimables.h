/**
 * @file Estimables.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ESTIMABLES_H_
#define ESTIMABLES_H_

// headers
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "Utilities/Map.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {

using boost::shared_ptr;
using utilities::Double;
using utilities::Map;
using std::string;
using std::vector;
using std::map;

// Enumerated Types
enum class EstimableType {
  kInvalid      = 0,
  kSingle       = 1,
  kVector       = 2,
  kStringMap    = 3,
  kUnsignedMap  = 4
};


/**
 * Class definition
 */
class Estimables {
public:
  // methods
  virtual                       ~Estimables() = default;
  static shared_ptr<Estimables> Instance();
  void                          AddValue(const string& estimable_label, Double value);
  vector<string>                GetEstimables() const;
  unsigned                      GetValueCount() const;
  map<string, Double>           GetValues(unsigned index) const;
  void                          LoadValues(unsigned index);

private:
  // methods
  Estimables();

  // members
  map<string, vector<Double>>   estimable_values_;
  map<string, Double*>          estimables_;

};

typedef boost::shared_ptr<Estimables> EstimablesPtr;

} /* namespace niwa */

#endif /* ESTIMABLES_H_ */
