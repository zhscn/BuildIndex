#include "LRU.h"

Node* List::add_kv_to_head(std::string key, std::string value) {
  Node* n = new Node(key, value);
  if (!front && !rear) {
    front = rear = n;
  } else {
    n->next = front;
    front->prev = n;
    front = n;
  }
  return n;
}

void List::move_kv_to_head(Node* n) {
  if (n == front) {
    return;
  }
  if (n == rear) {
    rear = rear->prev;
    rear->next = nullptr;
  } else {
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }

  n->next = front;
  n->prev = nullptr;
  front->prev = n;
  front = n;
}

void List::remove_rear_kv() {
  if (isEmpty()) {
    return;
  }

  if (front == rear) {
    delete rear;
    front = rear = nullptr;
  } else {
    Node* temp = rear;
    rear = rear->prev;
    rear->next = nullptr;
    delete temp;
  }
}

/// capacity 只记录了数量，但是kv的大小不确定，仅根据capacity无法
/// 准确的计算内存，应该另作记录，未完成
LRU::LRU(int capacity)
    : capacity(capacity), size(0), kv_list(nullptr), kv_map() {
  kv_list = new List();
}

/// 应该另作错误处理，比如 std::optional ?
std::string LRU::get(std::string key) {
  if (kv_map.find(key) == kv_map.end()) {
    return "";
  }
  std::string val = kv_map[key]->value;
  kv_list->move_kv_to_head(kv_map[key]);
  return val;
}

void LRU::put(std::string key, std::string value) {
  if (kv_map.find(key) != kv_map.end()) {
    kv_map[key]->value = value;
    kv_list->move_kv_to_head(kv_map[key]);
    return;
  }

  if (size == capacity) {
    std::string k = kv_list->get_rear_kv()->key;
    kv_map.erase(k);
    kv_list->remove_rear_kv();
    size--;
  }

  Node* n = kv_list->add_kv_to_head(key, value);
  size++;
  kv_map[key] = n;
}

LRU::~LRU() {
  for (auto i = kv_map.begin(); i != kv_map.end(); i++) {
    delete i->second;
  }
  delete kv_list;
}
