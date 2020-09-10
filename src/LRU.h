#pragma once

#include <map>
#include <string>
#include <mutex>

/*
 * 实现LRU算法
 * 实际运行中，这块缓存有可能会被置换出去，由于其他的磁盘寻址等操作。
 * 可能的优化方法是锁定这块内存。大概思路应该是申请一大块内存，自行管理和分配
 * 由于涉及多线程的问题，这块不太会实现。
 */

class Node {
public:
  Node(std::string k, std::string v)
      : key(std::move(k)), value(std::move(v)), prev(nullptr), next(nullptr) {}
  std::string key;
  std::string value;
  Node* prev;
  Node* next;
};

class List {
public:
  List() : front(nullptr), rear(nullptr) {}

  Node* add_kv_to_head(std::string key, std::string value);
  void move_kv_to_head(Node* n);
  void remove_rear_kv();
  Node* get_rear_kv() { return rear; }

private:
  Node* front;
  Node* rear;
  bool isEmpty() { return rear == nullptr; }
};

class LRU {
public:
  LRU(int capacity);
  ~LRU();

  std::string get(std::string key);
  void put(std::string key, std::string value);

private:
  int capacity;
  int size;
  List* kv_list;
  std::map<std::string, Node*> kv_map;
  std::mutex mu;
};
