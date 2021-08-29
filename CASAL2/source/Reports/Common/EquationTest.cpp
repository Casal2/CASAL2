/**
 * @file fEquationTest.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "EquationTest.h"

#include <boost/algorithm/string/join.hpp>

#include "../../EquationParser/EquationParser.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */

EquationTest::EquationTest() {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kFinalise;

  parameters_.Bind<string>(PARAM_EQUATION, &equation_input_, "The equation to do a test run of", "");
}

/**
 * Execute the report
 */
void EquationTest::DoExecute(shared_ptr<Model> model) {
  equation_ = boost::algorithm::join(equation_input_, " ");

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "equation: " << equation_ << REPORT_EOL;
  try {
    cache_ << "result: " << model->equation_parser().Parse(equation_) << REPORT_EOL;
  } catch (std::runtime_error& ex) {
    cache_ << "result_exception: " << ex.what() << REPORT_EOL;
  } catch (...) {
    cache_ << "result: equation failed" << REPORT_EOL;
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
