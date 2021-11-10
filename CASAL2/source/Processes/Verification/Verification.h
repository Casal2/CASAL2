// headers
#include "../../Model/Model.h"

// namespaces
namespace niwa::processes::verification {

// Function List
void AgeingProcessExists(shared_ptr<Model> model);
void RecruitmentNoDupelicateCategories(shared_ptr<Model> model);

// Execute the functions
void DoVerification(shared_ptr<Model> model) {
  AgeingProcessExists(model);
  RecruitmentNoDupelicateCategories(model);
}

}  // namespace niwa::processes::verification