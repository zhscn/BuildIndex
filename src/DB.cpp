#include "DB.h"

#include <iostream>
#include <vector>

DB::DB(const char* s)
    : pool(), row_data(s), index(row_data), cache(2048), msg(),
      handle_result(nullptr), closed(0) {}

DB::~DB() {
  closed.store(-1);
  /// 唤醒该线程，防止阻塞可能导致的死锁？
  msg.set_msg("");
  handle_result->join();
  delete handle_result;
}

void DB::open() {
  row_data.map_to_memory();
  if (index.load() != -1) {
    setup_index();
  }
  handle_result = new std::thread([this]() {
    while (true) {
      if (closed.load() == 1) {
        return;
      }
      std::cout << msg.get_msg();
    }
  });
}

void DB::setup_index() {
  pool.add_task([this]() {
    RowData::Cursor cursor(row_data);
    KeyValue kv;
    while (!cursor.eof()) {
      /// 这里如果向前跑的太快，会导致其他线程跟不上Cursor线程
      /// 期间从文件读取到的内容会频繁的换入换出
      /// 可能需要加一个滑动窗口来控制读入的速度
      /// 未完成
      uint64_t offset = cursor.next(kv);
      pool.add_task([this, kv, offset]() {
        index.put(std::string(kv.key, kv.key_size), offset);
      });
    }
  });
}

void DB::get(const char* key) {
  pool.add_task([this, key]() {
    auto res = cache.get(key);
    if (res.length() != 0) {
      msg.set_msg(std::move(res));
    } else {
      /// 访问磁盘获取value时应该对每个请求做批处理，每个线程集中访问部分内存
      /// 需要修改线程池的实现，未完成。
      res = std::move(index.get(key));
      cache.put(key, res);
      msg.set_msg(std::move(res));
    }
  });
}
