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

#include "BaseClasses/Object.h"

// Namespaces
namespace isam {

using boost::shared_ptr;

// Enumerated Types
namespace RunMode {
enum Type {
  kInvalid,
  kLicense,
  kVersion,
  kHelp,
  kBasic,
  kEstimation
};}

namespace State {
enum Type {
  kInitialise,
  kVerify,
  kValidate,
  kBuild,
  kPreExecute,
  kExecute,
  kFinalise,
  kReset
};
}

/**
 * Class definition
 */
class Model : public base::Object {
public:
  // Methods
  static shared_ptr<Model>    Instance();
  virtual                     ~Model() = default;
  void                        Run();

  // Accessors
  void                        set_run_mode(RunMode::Type new_mode) { run_mode_ = new_mode; }
  RunMode::Type               run_mode() { return run_mode_; }

private:
  // Methods
  Model();

  // Members
  RunMode::Type               run_mode_ = RunMode::kInvalid;
  State::Type                 state_    = State::kInitialise;
};

/**
 * Typedef
 */
typedef boost::shared_ptr<Model> ModelPtr;

} /* namespace isam */
#endif /* MODEL_H_ */
























