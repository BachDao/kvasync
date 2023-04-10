#ifndef KV_ASYNC_THREAD_UTIL_H
#define KV_ASYNC_THREAD_UTIL_H
#include "kv_async/forward_declarations.h"
namespace kv_async {
namespace detail {
struct worker_state {
  worker *ptrCurrentWorker = nullptr;
  inline bool valid() { return ptrCurrentWorker != nullptr; }
};
thread_local static worker_state workerState;
} // namespace impl
} // namespace kv_async
#endif // KV_ASYNC_THREAD_UTIL_H
