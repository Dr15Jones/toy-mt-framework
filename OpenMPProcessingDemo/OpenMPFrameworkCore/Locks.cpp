#include "Locks.h"

boost::shared_ptr<demo::TaskYieldLock> demo::s_thread_unsafe_lock{ new TaskYieldLock{}};
