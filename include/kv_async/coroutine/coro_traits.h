//
// Created by Bach Dao.
//
#include "kv_async/coroutine/coro_platform.h"
#include "kv_async/coroutine/coro_promise.h"
#include "kv_async/coroutine/coro_scope.h"
#ifndef KV_ASYNC_CORO_TRAITS_H
#define KV_ASYNC_CORO_TRAITS_H
template <typename T, typename... Args>
struct kv_async::detail::coroutine::coroutine_traits<kv_async::task<T>,
                                                     Args...> {
  using promise_type = kv_async::detail::coro_promise<T>;
};
template <typename... Args>
struct kv_async::detail::coroutine::coroutine_traits<
    kv_async::detail::launch_task, Args...> {
  using promise_type = kv_async::detail::launch_promise;
};
#endif // KV_ASYNC_CORO_TRAITS_H
