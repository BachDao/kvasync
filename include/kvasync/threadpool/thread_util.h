#ifndef KVASYNC_THREAD_UTIL_H
#define KVASYNC_THREAD_UTIL_H
#include "kvasync/forward_declarations.h"
namespace kvasync {
namespace impl {
struct worker_state {
  worker *ptrCurrentWorker = nullptr;
  inline bool valid() { return ptrCurrentWorker != nullptr; }
};
thread_local static worker_state workerState;
} // namespace impl
} // namespace kvasync
#endif // KVASYNC_THREAD_UTIL_H
