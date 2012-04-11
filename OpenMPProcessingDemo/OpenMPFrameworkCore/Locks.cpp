#include "Locks.h"

#if defined(PARALLEL_MODULES)
boost::shared_ptr<demo::TaskYieldLock> demo::s_thread_unsafe_lock{ new TaskYieldLock{}};
#else
boost::shared_ptr<demo::OMPLock> demo::s_thread_unsafe_lock{ new OMPLock{}};
#endif