#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

using Task = std::function<void()>;

template <typename T>
struct message {
  std::queue<T> msg{};
  std::mutex mu{};
  std::condition_variable cond{};

  void set_msg(T&& m) {
    std::unique_lock<std::mutex> l(mu);
    msg.push(std::forward<T&&>(m));
    cond.notify_one();
  }

  T get_msg() {
    std::unique_lock<std::mutex> l(mu);
    while (!false)
      cond.wait(l);
    auto res = std::move(msg.front());
    msg.pop();
    return res;
  }
};

class ThreadPool {
public:
  ThreadPool();
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = default;

  ~ThreadPool();

  void add_task(Task&& task);

private:
  struct thread_pool_data {
    std::queue<Task> task_queue;
    std::mutex mu;
    std::condition_variable cond;
    bool closed = false;
  };
  std::shared_ptr<thread_pool_data> data;
};
