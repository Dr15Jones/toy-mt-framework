#include "TimeMonitor.h"
#include <fstream>

using namespace demo;

namespace {
  std::ofstream f;
}

namespace demo {
   auto now = clock_t::now;
}

decltype(now()) TimeMonitor::beginTime_{};
std::atomic<bool> TimeMonitor::outputAcquired_{false};

template<typename... Args>
void
formatOutput(std::atomic<bool>&lock, Args&&... a) {
  bool expected = false;
  while(not lock.compare_exchange_strong(expected,true) ) {expected = false;}
  ( f <<... <<a )<<'\n';
  lock = false;
}

void
TimeMonitor::registerModule(std::string const& name, unsigned int iID) {
  formatOutput(outputAcquired_,"#M ",iID," ",name);
}

void
TimeMonitor::openFile() {
  f.open("stall.log");
  f<<"# Transition\n";
}

void 
TimeMonitor::startTimer(unsigned int iNConcurrentEvents) {
  formatOutput(outputAcquired_,"M ",iNConcurrentEvents," 0 4 0");
  beginTime_ = now();
}

using namespace std::chrono;
template<typename T>
auto deltaTime( T const& begin) -> decltype(std::chrono::duration_cast<milliseconds>(now()-begin).count()) {
  return std::chrono::duration_cast<milliseconds>(now()-begin).count();
}

void 
TimeMonitor::postPrefetching(unsigned int iStreamID, unsigned int iModuleID) {
  formatOutput(outputAcquired_,"p ",iStreamID, " ", iModuleID," ",deltaTime(beginTime_) );
}

void 
TimeMonitor::preModuleCalled(unsigned int iStreamID, unsigned int iModuleID) {
  formatOutput(outputAcquired_,"M ",iStreamID, " ", iModuleID," 0 ",deltaTime(beginTime_) );
}

void 
TimeMonitor::postModuleCalled(unsigned int iStreamID, unsigned int iModuleID) {
  formatOutput(outputAcquired_,"m ",iStreamID, " ", iModuleID," 0 ",deltaTime(beginTime_) );
}
