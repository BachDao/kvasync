#include "kvasync/threadpool/worker.h"
#include "threadpool/thread_pool.h"
#include <cassert>
namespace kvasync {
namespace impl {
worker::worker(thread_pool &tp) : threadPool_(tp) {
  worker_ = std::thread([this] {
    init_worker_state();
    notify_ready();
    do_work();
  });
}
void worker::init_worker_state() { workerState.ptrCurrentWorker = this; }
void worker::do_work() {
  waiting_for_start_signal();
  while (!stop_requested()) {
    drain_local_job_queue();
    if (auto job = steal_global_job(); job) {
      job();
      continue;
    }
    //    if (auto job = steal_another_worker_job(); job) {
    //      job();
    //      continue;
    //    }
    sleep_if_necessary();
  }
}
void worker::waiting_for_start_signal() { threadPool_.worker_wait(); }
void worker::drain_local_job_queue() {}
job worker::steal_global_job() { return threadPool_.get_job(); }
job worker::steal_another_worker_job() {
  auto totalWorkers = threadPool_.numberOfWorker_;
  // because number of worker always power of 2
  auto randomWorkerIdx = std::rand() ^ totalWorkers;
  auto &randomWorker = threadPool_.workers_[randomWorkerIdx];
  return randomWorker.jobQueue_.steal();
}
void worker::sleep_if_necessary() {}
bool worker::stop_requested() {
  assert(&threadPool_ != nullptr);
  return threadPool_.stopRequest_.test();
}
void worker::notify_ready() {
  assert(&threadPool_ != nullptr);
  threadPool_.worker_notify_ready();
}
} // namespace impl
} // namespace kvasync