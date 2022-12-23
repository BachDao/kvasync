#ifndef KV_ASYNC_JOB_H
#define KV_ASYNC_JOB_H
#include <functional>
namespace kv_async {
namespace detail {
class job {
  std::function<void()> fn_;

public:
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  explicit job(Fn &&fn);
  job() = default;
  job(const job &) = default;
  job(job &&) = default;
  job& operator=(const job& rhs);
  job& operator=(job&& rhs);

  void operator()();
  explicit operator bool();
};
template <typename Fn>
  requires std::is_invocable_v<Fn>
job::job(Fn &&fn) : fn_(std::forward<Fn>(fn)) {}
} // namespace impl
} // namespace kv_async

#endif // KV_ASYNC_JOB_H
