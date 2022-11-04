#ifndef KVASYNC_JOB_H
#define KVASYNC_JOB_H
#include <functional>
namespace kvasync {
namespace impl {
class job {
  std::function<void()> fn_;

public:
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  explicit job(Fn &&fn);
  job() = default;
  void operator()();
  explicit operator bool();
};
template <typename Fn>
  requires std::is_invocable_v<Fn>
job::job(Fn &&fn) : fn_(std::forward<Fn>(fn)) {}
} // namespace impl
} // namespace kvasync

#endif // KVASYNC_JOB_H
