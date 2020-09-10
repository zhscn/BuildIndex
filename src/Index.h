#pragma once

#include "RowData.h"
#include <atomic>

/*
 * Entry 存储了偏移量与下一个Entry
 * 可能的优化思路：将小于一定长度的key直接存在Entry中。
 * 在该范围可以减少一半的磁盘寻址。
 */
struct Entry {
  void set(uint64_t off, uint64_t n) {
    offset = off;
    next = n;
  }
  uint64_t offset;
  uint64_t next;
};

class Index {
public:
  Index(RowData& row_data);
  ~Index();
  ///读取文件
  int load();
  int put(const std::string& key, uint64_t offset);
  std::string get(const std::string& key);

private:
  uint32_t hash(const char* key, int len);
  uint64_t alloc_entry();

  RowData& row_data;
  uint64_t index_file_length;
  const char* begin;

  uint64_t* buckets;
  uint64_t buckets_mask; // 2的整数次幂减一

  Entry* entries;
  uint64_t entry_size;
  std::atomic_uint64_t entry_next_index; // 分配新的Entry时+1
  std::string index_file_name;
  int fd;
};
