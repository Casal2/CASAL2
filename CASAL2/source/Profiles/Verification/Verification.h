// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa::profiles::verification {

// Function List
void VerifySingleProfileBlock(shared_ptr<Model> model);

// Execute the functions
void DoVerification(shared_ptr<Model> model) {
  VerifySingleProfileBlock(model);
}

}  // namespace niwa::reports::verification