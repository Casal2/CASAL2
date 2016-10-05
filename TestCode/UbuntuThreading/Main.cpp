#include <iostream>
#include <string>
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>

using std::cout;
using std::endl;
using std::string;

typedef int(*RUNPROC)();

void* LoadLibraryFunction(void* library, string method) {
 return dlsym(library, method.c_str());
}

void CloseLibrary(void* library) {
  dlclose(library);
}

int main(int argc, char * argv[]) {

  auto library = dlopen("./libshared_library.so", RTLD_LAZY);
  if (library == nullptr) {
    cout << "Failed to Load Shared Library" << endl;
    return -1;
  }

  auto main_method = (RUNPROC)LoadLibraryFunction(library, "Run");
  if (main_method == nullptr) {
      cout << "Error: Failed to get the main method address" << endl;
      CloseLibrary(library);
      return -1;
    }

  int return_code_ = (main_method)();
  return return_code_;
}
