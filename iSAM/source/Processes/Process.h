/**
 * @file Process.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the parent process for all process objects in the model.
 * Anything that acts on the partition during a time step is considered
 * a process (e.g mortality, recruitment, ageing etc)
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PROCESS_H_
#define PROCESS_H_

// Headers
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Object.h"

namespace isam {

/**
 * Class Definition
 */
class Process : public isam::base::Object {
public:
  // methods
  Process();
  virtual                     ~Process() { };
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  void                        PreExecute() { };
  void                        Print();

  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;
  virtual void                DoPreExecute() = 0;
  virtual void                DoReset() = 0;
  virtual void                Execute() = 0;

protected:
  // members
  string                      type_ = "";
  bool                        print_report_ = false;
  map<string, vector<string>> print_values_;

};

/**
 * Typedef
 */
typedef boost::shared_ptr<Process> ProcessPtr;

} /* namespace isam */
#endif /* PROCESS_H_ */
