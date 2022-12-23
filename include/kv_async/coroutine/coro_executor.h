//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_CORO_EXECUTOR_H
#define KV_ASYNC_CORO_EXECUTOR_H
namespace kv_async {
namespace detail {
class coro_executor {
public:
  template <typename Fn> void submit(Fn &&fn);
};

} // namespace detail
} // namespace kv_async

#endif // KV_ASYNC_CORO_EXECUTOR_H
