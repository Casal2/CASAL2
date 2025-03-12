/**
 * @file Orthogonal.h
 * @author C.Marsh
 * @date 13/04/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Transfomration parameters x1 and x2 to estimate y1 = x1/x2 and y2 x1 * x2 see the manual for more infomration.
 */
#ifndef SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_ORTHOGONAL_H_
#define SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_ORTHOGONAL_H_

// headers
#include "../AddressableTransformation.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

// Function to apply the orthogonal transformation
inline void orthogonalTransform(const Double& x1, const Double& x2, Double& y1, Double& y2) {
  y1 = x1 / x2;
  y2 = x1 * x2;
}

// Function to recover original x1 and x2
inline void inverseOrthogonalTransform(const Double& y1, const Double& y2, Double& x1, Double& x2) {
  x2 = sqrt(fabs(y2) / fabs(y1));  // Ensure we get the magnitude
  if (y2 < 0)
    x2 = -x2;  // Adjust sign
  x1 = y1 * x2;
}

/**
 *
 */
class Orthogonal : public AddressableTransformation {
public:
  Orthogonal() = delete;
  explicit Orthogonal(shared_ptr<Model> model);
  virtual ~Orthogonal() = default;
  Double GetScore() override final;
  void   FillReportCache(ostringstream& cache) override final;
  void   FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  void   PrepareForObjectiveFunction() override final;
  void   RestoreForObjectiveFunction() override final;

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoRestore() override final;

private:
  Double quotient_parameter_;
  Double product_parameter_;
};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMATETRANSFORMATIONS_CHILDREN_ORTHOGONAL_H_ */
