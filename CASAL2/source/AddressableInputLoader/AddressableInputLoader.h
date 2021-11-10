/**
 * @file Estimables.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Because we handle the values loaded by an input file differently depending on what
 * run mode we are in this class is responsible for loading the values from the input
 * file and making them available to the sub-systems that require them.
 */
#ifndef ADDRESSABLEINPUTLOADER_H_
#define ADDRESSABLEINPUTLOADER_H_

// headers
#include <map>
#include <memory>
#include <string>

#include "../Model/Managers.h"
#include "../Utilities/Map.h"
#include "../Utilities/Types.h"

// namespaces
namespace niwa {

using std::map;
using std::shared_ptr;
using std::string;
using std::vector;
using utilities::Double;
class Model;

// Enumerated Types
enum class AddressableType { kInvalid = 0, kSingle = 1, kVector = 2, kStringMap = 3, kUnsignedMap = 4 };

/**
 * Class definition
 */
class AddressableInputLoader {
public:
  // methods
  AddressableInputLoader(shared_ptr<Model> model) : model_(model){};
  virtual ~AddressableInputLoader() = default;
  void                Validate(){};
  void                Build(){};
  void                Verify(shared_ptr<Model> model){};
  void                AddValue(const string& addressable_label, Double value);
  vector<string>      GetAddressableLabels() const;
  unsigned            GetValueCount() const;
  map<string, Double> GetValues(unsigned index) const;
  void                LoadValues(unsigned index);

private:
  // members
  shared_ptr<Model>           model_ = nullptr;
  map<string, vector<Double>> addressable_values_;
  map<string, Double*>        addressables_;
};
} /* namespace niwa */

#endif /* ADDRESSABLEINPUTLOADER_H_ */
