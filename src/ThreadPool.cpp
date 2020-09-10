#include "ThreadPool.h"

ThreadPool::ThreadPool() : data(std::make_shared<thread_pool_data>()) {
  int thread_count = std::thread::hardware_concurrency();
  for (int i = 0; i < thread_count; i++)

    std::thread([=]() {
      std::unique_lock<std::mutex> l(data->mu);
      while (true) {
        if (!data->task_queue.empty()) {
          auto task = std::move(data->task_queue.front());
          data->task_queue.pop();

          l.unlock();
          task();
          l.lock();
        } else if (data->closed) {
          break;
        } else {
          data->cond.wait(l);
        }
      }
    }).detach();
}

ThreadPool::~ThreadPool() {
  if (data) {
    {
      std::lock_guard<std::mutex> l(data->mu);
      data->closed = true;
    }
    data->cond.notify_all();
  }
}

void ThreadPool::add_task(Task&& task) {
  {
    std::lock_guard<std::mutex> l(data->mu);
    data->task_queue.emplace(task);
  }
  data->cond.notify_one();
}
