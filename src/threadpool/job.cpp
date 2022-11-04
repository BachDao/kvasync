#include "kvasync/threadpool/job.h"
#include <cassert>
namespace kvasync {
namespace impl {

void job::operator()() {
  assert(fn_);
  fn_();
}
job::operator bool() { return bool(fn_); }
} // namespace impl
} // namespace kvasync
