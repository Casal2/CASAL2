#include <iostream>
#include <thread>

#include "SharedLibrary.h"

using std::cout;
using std::endl;

int Run() {

  cout << "Starting Thread" << endl;
  std::thread report_thread([]() {
    cout << "Thead Started..." << endl;
  });

  cout << "Joining Thread" << endl;
  report_thread.join();
  cout << "Thread Finished" << endl;
  cout << endl;

  return 0;
}
