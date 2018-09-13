#if !defined(TimeMonitor_h)
#define TimeMonitor_h

#include <string>
#include <atomic>
#include <chrono>

namespace demo {

    using clock_t = std::chrono::steady_clock;
 
  class TimeMonitor {
  public:

    static void openFile();
    static void startTimer(unsigned int iNConcurrentEvents);
    static void registerModule(std::string const& name, unsigned int iID);
    static void postPrefetching(unsigned int iStreamID, unsigned int iModuleID);
    static void preModuleCalled(unsigned int iStreamID, unsigned int iModuleID);
    static void postModuleCalled(unsigned int iStreamID, unsigned int iModuleID);
  private:
    TimeMonitor() = default;
    static decltype(clock_t::now()) beginTime_;
    static std::atomic<bool> outputAcquired_;
  };
}

#endif
