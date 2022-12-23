#ifndef KV_ASYNC_FORWARD_DECLARATIONS_H
#define KV_ASYNC_FORWARD_DECLARATIONS_H
namespace kv_async {
class thread_pool;
namespace detail {
class worker;
template <typename T> class coro_promise;
} // namespace detail
template <typename T> class task;

} // namespace kv_async
#endif // KV_ASYNC_FORWARD_DECLARATIONS_H
