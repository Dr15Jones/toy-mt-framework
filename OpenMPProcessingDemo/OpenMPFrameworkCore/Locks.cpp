#include "Locks.h"

boost::shared_ptr<demo::OMPLock> demo::s_thread_unsafe_lock{ new OMPLock{}};
