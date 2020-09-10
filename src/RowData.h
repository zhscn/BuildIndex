#pragma once

#include <string>

struct KeyValue {
  uint64_t key_size;
  const char* key;
  uint64_t value_size;
  const char* value;
};

class RowData {
public:
  class Cursor {
  public:
    Cursor(RowData& data) : data(data), location(data.begin) {}
    uint64_t next(KeyValue& kv) {
      auto off = location - data.begin;
      location = data.read_key_value(location, kv);
      return off;
    }
    bool eof() { return location - data.begin > data.file_length; }

  private:
    RowData& data;
    const char* location;
  };

  RowData(std::string file_name);
  ~RowData();
  int map_to_memory();
  std::string get_file_name() { return file_name; }
  size_t get_file_length();
  const char* read_key_value(const char* location, KeyValue& kv);
  const char* read_key_value(uint64_t offset, KeyValue& kv);

private:
  std::string file_name;
  uint64_t file_length;
  char* begin;
  int fd;
};
