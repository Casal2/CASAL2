/**
 * @file Model.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the primary representation of our model and it's states
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MODEL_H_
#define MODEL_H_

// Headers
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Executor.h"
#include "BaseClasses/Object.h"
#include "Catchabilities/Manager.h"
#include "Utilities/RunMode.h"

// Namespaces
namespace niwa {

using boost::shared_ptr;
using niwa::catchabilities::CatchabilityManagerPtr;

namespace State {
enum Type {
  kStartUp,
  kValidate,
  kBuild,
  kVerify,
  kInitialise,
  kPreExecute,
  kExecute,
  kPostExecute,
  kIterationComplete,
  kFinalise,
  kReset
};
}

enum class PartitionStructure {
  kAge = 1,
  kLength = 2,
  kHybrid = 4,
  kInvalid = 4096
};

inline PartitionStructure operator&(PartitionStructure a, PartitionStructure b) {
  return static_cast<PartitionStructure>(static_cast<int>(a) & static_cast<int>(b));
}

/**
 * Class definition
 */
class Model : public base::Object {
public:
  // Methods
  static shared_ptr<Model>    Instance(bool force_new = false);
  virtual                     ~Model() = default;
  void                        Start(RunMode::Type run_mode);
  void                        FullIteration();
  void                        Subscribe(State::Type state, ExecutorPtr executor) { executors_[state].push_back(executor); }

  // Accessors
  RunMode::Type               run_mode() const { return run_mode_; }
  State::Type                 state() const { return state_; }
  unsigned                    start_year() const { return start_year_; }
  unsigned                    final_year() const { return final_year_; }
  unsigned                    projection_final_year() const { return projection_final_year_; }
  vector<unsigned>            years() const;
  unsigned                    year_spread() const;
  unsigned                    current_year() const { return current_year_; }
  virtual unsigned            min_age() const { return min_age_; }
  virtual unsigned            max_age() const { return max_age_; }
  virtual unsigned            age_spread() const { return (max_age_ - min_age_) + 1; }
  bool                        age_plus() const { return age_plus_; }
  const vector<string>&       time_steps() const { return time_steps_; }
  const vector<string>&       initialisation_phases() const { return initialisation_phases_; }
  PartitionStructure          partition_structure() const { return partition_structure_; }

protected:
  // Methods
  Model();
  void                        Validate();
  void                        Build();
  void                        Verify();
  void                        Iterate();
  void                        Reset();
  void                        RunBasic();
  void                        RunEstimation();
  void                        RunMCMC();
  void                        RunProfiling();
  void                        RunSimulation();
  void                        RunProjection();

  // Members
  RunMode::Type               run_mode_ = RunMode::kInvalid;
  State::Type                 state_    = State::kStartUp;
  unsigned                    start_year_ = 0;
  unsigned                    final_year_ = 0;
  unsigned                    projection_final_year_ = 0;
  unsigned                    current_year_ = 0;
  unsigned                    current_year_index_ = 0;
  unsigned                    min_age_ = 0;
  unsigned                    max_age_ = 0;
  bool                        age_plus_ = true;
  vector<string>              initialisation_phases_;
  vector<string>              time_steps_;
  vector<Double>              length_bins_;
  CatchabilityManagerPtr      catchability_manager_;
  bool                        estimable_values_file_ = false;
  unsigned                    estimable_values_count_ = 1;
  PartitionStructure          partition_structure_ = PartitionStructure::kInvalid;

  map<State::Type, vector<ExecutorPtr>> executors_;
};

/**
 * Typedef
 */
typedef boost::shared_ptr<Model> ModelPtr;

} /* namespace niwa */
#endif /* MODEL_H_ */
























