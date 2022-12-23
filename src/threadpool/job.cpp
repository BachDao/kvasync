#include "kv_async/threadpool/job.h"
#include <cassert>
namespace kv_async {
namespace detail {

void job::operator()() {
  assert(fn_);
  fn_();
}
job::operator bool() { return bool(fn_); }
job &job::operator=(const job &rhs) {
  fn_ = rhs.fn_;
  return *this;
}
job &job::operator=(job &&rhs) {
  fn_ = std::move(rhs.fn_);
  return *this;
}
} // namespace detail
} // namespace kv_async
