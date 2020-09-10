#include "Index.h"
#include "MurmurHash2.h"

#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

Index::Index(RowData& row_data)
    : row_data(row_data), index_file_length(row_data.get_file_length()),
      begin(nullptr), buckets(nullptr), buckets_mask(0x3FFFFFFFU), // 1 GiB
      entries(nullptr), entry_size(0),
      index_file_name(row_data.get_file_name()) {

  index_file_name.append(".index");
  if (index_file_length < 2 * (1LU << 30)) {
    index_file_length = 2 * (1LU << 30);
  }
}

Index::~Index() {
  msync((void*)begin, index_file_length, MS_SYNC);
  *(uint64_t*)begin = entry_next_index.load();
  munmap((void*)begin, index_file_length);
  close(fd);
}

int Index::load() {
  int return_num = 0;
  if (access(index_file_name.c_str(), F_OK) == 0) {
    entry_next_index.store(*(uint64_t*)begin);
    fd = open(index_file_name.c_str(), 0644);
  } else {
    return_num = -1;
    /// creat index file
    fd = creat(index_file_name.c_str(), 0644);
    ftruncate(fd, index_file_length);
  }

  begin = (const char*)mmap(NULL, index_file_length, PROT_WRITE | PROT_READ,
                            MAP_SHARED, fd, 0);
  buckets = (uint64_t*)(begin + 8);

  entries = (Entry*)(begin + 8 + 8 * (buckets_mask + 1));
  entry_size = (index_file_length - 8 * (buckets_mask + 2)) / 16;
  return return_num;
}

uint32_t Index::hash(const char* key, int len) {
  return MurmurHash64A(key, len, 0x240FACD6'DE170684) & buckets_mask;
}

uint64_t Index::alloc_entry() {
  auto new_entry_index = std::atomic_fetch_add(&entry_next_index, 1);
  if (new_entry_index < entry_size) {
    return new_entry_index;
  } else {
    return -1LU;
  }
}

int Index::put(const std::string& key, uint64_t offset) {
  auto new_entry_index = alloc_entry();
  if (new_entry_index == -1LU) {
    return -1;
  }
  auto entry = &entries[new_entry_index];
  entry->offset = offset;

  int bucket_index = hash(key.c_str(), key.length());
  uint64_t bucket_next;
  do {
    bucket_next = buckets[bucket_index];
    entry->next = bucket_next;
  } while (!__atomic_compare_exchange_n(&buckets[bucket_index], &bucket_next,
                                        new_entry_index, false,
                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
  return 0;
}

std::string Index::get(const std::string& key) {
  auto ety = &entries[hash(key.c_str(), key.length())];
  KeyValue kv;
  while (true) {
    row_data.read_key_value(ety->offset, kv);
    if (kv.key_size == key.length() &&
        std::strncmp(key.c_str(), kv.key, kv.key_size) == 0) {
      // kv.value
    } else if (ety->next == -1LU) {
      return "";
    } else {
      ety = &entries[ety->next];
    }
  }
}
