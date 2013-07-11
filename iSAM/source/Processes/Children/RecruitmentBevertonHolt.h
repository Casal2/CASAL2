/**
 * @file RecruitmentBevertonHolt.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the Beverton-Holt recruitment process
 */
#ifndef PROCESSES_RECRUITMENTBEVERTONHOLT_H_
#define PROCESSES_RECRUITMENTBEVERTONHOLT_H_

// namespaces
namespace isam {
namespace processes {

/**
 * class definition
 */
class RecruitmentBevertonHolt {
public:
  RecruitmentBevertonHolt();
  virtual ~RecruitmentBevertonHolt();
};

} /* namespace processes */
} /* namespace isam */

#endif /* PROCESSES_RECRUITMENTBEVERTONHOLT_H_ */
