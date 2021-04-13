/**
 * @file fEquationParser.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the interface to the equation parser
 * third-party library. We need this to ensure we can have a single
 * instance managed by the model that handles registration of symbols/addressables
 * to be used within an equation.
 */
#ifndef SOURCE_EQUATIONPARSER_EQUATIONPARSER_H_
#define SOURCE_EQUATIONPARSER_EQUATIONPARSER_H_

// headers
#include <string>
#include <memory>

#include "../Utilities/NoCopy.h"
#include "../Utilities/Types.h"

#include <parser.h>

using niwa::utilities::Double;

// namespaces
namespace niwa {
class Model;
using std::string;
using std::shared_ptr;

/**
 * Class definition
 */
class EquationParser : public LookupObject<Double> {
public:
  // methods
  EquationParser();
  explicit EquationParser(shared_ptr<Model> model);
  virtual                     ~EquationParser();
  Double                      Parse(string equation);

  // Callback method for the equation parser to lookup addressables
  Double&                     LookupValue(const std::string& name);

private:
    // members
  shared_ptr<Model>                      model_ = nullptr;
#ifdef USE_ADOLC
  Parser<adouble, adub, const badouble&>* parser_ = nullptr;
#elif USE_BETADIFF
  Parser<adouble, adub, const badouble&>* parser_ = nullptr;
#elif USE_CPPAD
  Parser<CppAD::AD<double>, CppAD::AD<double>, const CppAD::AD<double>&>* parser_ = nullptr;
#else
  Parser<Double, Double, Double>* parser_ = nullptr;
#endif

  DISALLOW_COPY_AND_ASSIGN(EquationParser);
};

} /* namespace niwa */

#endif /* SOURCE_EQUATIONPARSER_EQUATIONPARSER_H_ */
