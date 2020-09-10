#pragma once

#include "Index.h"
#include "LRU.h"
#include "RowData.h"
#include "ThreadPool.h"
#include <atomic>
#include <string>

/*
 * DB 抽象出了对数据的读写，写暂时未要求，没有实现
 */
class DB {
public:
  /// s 是无序数据的文件名
  DB(const char* s);
  ~DB();

  /// 载入数据、索引。如果是索引不存在，则调用 setup_index 建立索引
  void open();
  
  std::string get(const char* key);

  /// nonimplemented
  int put(std::string key, std::string value);

private:
  void setup_index();

  /// 线程池实现任务队列，没有写入，只有并发请求，直接加入任务队列无需同步
  ThreadPool pool;
  /// 原始数据
  RowData row_data;
  /// hash table
  Index index;
  /// 请求数据符合齐夫分布，将之前请求缓存在内存中加速后续读取
  LRU cache;
  
  /// 简化了请求管理，对于这个实现，直接将请求结果输出到 console
  /// 后续应该加入请求管理与进程通信
  message<std::string> msg;
  std::thread* handle_result;
  std::atomic_int32_t closed;
};
