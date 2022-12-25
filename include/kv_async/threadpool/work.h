#ifndef KV_ASYNC_WORK_H
#define KV_ASYNC_WORK_H
#include <functional>
namespace kv_async {
namespace detail {
class work {
  std::function<void()> fn_;

public:
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  work(Fn &&fn);
  work() = default;
  work(const work &) = default;
  work(work &&) = default;
  work &operator=(const work &rhs);
  work &operator=(work &&rhs);

  void operator()();
  explicit operator bool();
};
template <typename Fn>
  requires std::is_invocable_v<Fn>
work::work(Fn &&fn) : fn_(std::forward<Fn>(fn)) {}
} // namespace detail
} // namespace kv_async

#endif // KV_ASYNC_WORK_H
