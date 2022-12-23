//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_TASK_H
#define KV_ASYNC_TASK_H
#include "coro_platform.h"
#include "kv_async/forward_declarations.h"
namespace kv_async {
class task_base {};
template <typename T> class task : public task_base {
  detail::coro_promise<T> &promise_;

public:
  explicit task(detail::coro_promise<T> &promise);
};
template <typename T>
task<T>::task(detail::coro_promise<T> &promise) : promise_(promise) {}
} // namespace kv_async

#endif // KV_ASYNC_TASK_H
