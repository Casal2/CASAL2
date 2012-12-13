/*
 * ConstantRecruitment.h
 *
 *  Created on: 18/09/2012
 *      Author: Admin
 */

#ifndef CONSTANTRECRUITMENT_H_
#define CONSTANTRECRUITMENT_H_

#include "Processes/Process.h"

namespace isam {
namespace processes {

class ConstantRecruitment : public isam::Process {
public:
  ConstantRecruitment();
  virtual ~ConstantRecruitment();
};

} /* namespace processes */
} /* namespace isam */
#endif /* CONSTANTRECRUITMENT_H_ */
