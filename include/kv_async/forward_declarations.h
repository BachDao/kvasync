#ifndef KV_ASYNC_FORWARD_DECLARATIONS_H
#define KV_ASYNC_FORWARD_DECLARATIONS_H
namespace kv_async {
class thread_pool;
class coro_scope;
namespace detail {
class worker;
class launch_promise;
class coro_executor;
template <typename T> class coro_promise;
class promise_base;
class launch_promise;
} // namespace detail
template <typename T> class task;

} // namespace kv_async
#endif // KV_ASYNC_FORWARD_DECLARATIONS_H
