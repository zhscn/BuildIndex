#include "RowData.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

RowData::RowData(std::string file_name)
    : fd(-1), file_name(std::move(file_name)), file_length(-1), begin(nullptr) {
}

RowData::~RowData() {
  if (begin != nullptr) {
    msync((void*)begin, file_length, MS_SYNC);
    munmap((void*)begin, file_length);
  }
}

size_t RowData::get_file_length() {
  if (__builtin_expect(file_length == -1, 0)) {
    struct stat st;
    if (stat(file_name.c_str(), &st) < 0) {
      exit(EXIT_FAILURE);
    }
    file_length = st.st_size;
  }

  return file_length;
}

int RowData::map_to_memory() {
  fd = open(file_name.c_str(), O_RDONLY);

  if (fd == -1) {
    exit(EXIT_FAILURE);
  }

  begin = (char*)mmap(NULL, get_file_length(), PROT_READ, MAP_SHARED, fd, 0);
  if (begin == (void*)-1) {
    exit(EXIT_FAILURE);
  }

  close(fd);
  return 0;
}

const char* RowData::read_key_value(const char* location, KeyValue& kv) {
  kv.key_size = *(uint64_t*)location;
  kv.key = location + 8;

  kv.value_size = *(uint64_t*)(kv.key + kv.key_size);
  kv.value = location + kv.key_size + 16;
  return kv.value + kv.value_size;
}

const char* RowData::read_key_value(uint64_t offset, KeyValue& kv) {
  return read_key_value(begin + offset, kv);
}
