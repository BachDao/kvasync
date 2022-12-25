#include "kv_async/threadpool/work.h"
#include <cassert>
namespace kv_async {
namespace detail {

void work::operator()() {
  assert(fn_);
  fn_();
}
work::operator bool() { return bool(fn_); }
work &work::operator=(const work &rhs) {
  fn_ = rhs.fn_;
  return *this;
}
work &work::operator=(work &&rhs) {
  fn_ = std::move(rhs.fn_);
  return *this;
}
} // namespace detail
} // namespace kv_async
