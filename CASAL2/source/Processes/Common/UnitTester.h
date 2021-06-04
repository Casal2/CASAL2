/**
 * @file UnitTester.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 29/01/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This process does nothing. It's used for unit testing objects
 */
#ifndef SOURCE_PROCESSES_COMMON_UNITTESTER_H_
#define SOURCE_PROCESSES_COMMON_UNITTESTER_H_
#ifdef TESTMODE

// headers
#include "../../Processes/Process.h"
#include "../../Utilities/Map.h"

// namespaces
namespace niwa {
namespace processes {
using niwa::utilities::OrderedMap;

/**
 * Class definition
 */
class UnitTester : public Process {
public:
  // methods
  explicit UnitTester(shared_ptr<Model> model) : Process(model) {
    process_type_        = ProcessType::kNullProcess;
    partition_structure_ = PartitionType::kAge | PartitionType::kLength;

    RegisterAsAddressable(PARAM_A, &addressable_, addressable::kEstimate);
    RegisterAsAddressable(PARAM_B, &addressable_v_, addressable::kEstimate);
    RegisterAsAddressable(PARAM_C, &addressable_um_, addressable::kEstimate);
    RegisterAsAddressable(PARAM_D, &addressable_sm_, addressable::kEstimate);

    addressable_v_.push_back(1.0);
    addressable_v_.push_back(1.5);
    addressable_v_.push_back(2.0);

    addressable_um_[1990] = 1.0;
    addressable_um_[1991] = 1.5;
    addressable_um_[1992] = 2.0;

    addressable_sm_["a"] = 1.0;
    addressable_sm_["b"] = 1.5;
    addressable_sm_["c"] = 2.0;
  }
  virtual ~UnitTester() = default;
  void DoValidate() override final{};
  void DoBuild() override final{};
  void DoReset() override final{};
  void DoExecute() override final{};

public:
  // These addressables are used for unit testing
  // They can be addressed in a model, but will do nothing
  Double                     addressable_ = 1.0;
  vector<Double>             addressable_v_;
  map<unsigned, Double>      addressable_um_;
  OrderedMap<string, Double> addressable_sm_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* TESTMODE */
#endif /* SOURCE_PROCESSES_COMMON_UNITTESTER_H_ */
