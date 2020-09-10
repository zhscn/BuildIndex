#include "Rand.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <unistd.h>

template <typename To, typename From>
typename std::enable_if<(sizeof(To) == sizeof(From)) &&
                            std::is_trivially_copyable<From>::value &&
                            std::is_trivial<To>::value,
                        To>::value
bit_cast(const From& src) noexcept {
  To dst;
  std::memcpy(&dst, &src, sizeof(To));
  return dst;
}

template <typename T>
char* tob(T& t) {
  auto s = new char[szieof(t)];
  std::memcpy(s, &t, sizeof(t));
  return s;
}

struct KV {
  uint64_t key_size;
  char* key;
  uint64_t value_size;
  char* value;
};

int main() {
  remove("output");
  int fd = open("output", O_CREAT | O_RDWR, 0644);

  Rand rand(32, 126);

  // 512 KiB on average, 1 TiB data needs 2^21 round
  auto round = rand.next(100, 400);

  for (int i = 0; i < round; i++) {
    KV kv;
    kv.key_size = rand.next(1, 1 << 10);   // 1 KiB
    kv.value_size = rand.next(1, 1 << 20); // 1 MiB
    kv.key = new char[kv.key_size]{0};
    kv.value = new char[kv.value_size]{0};

    for (int j = 0; j < kv.key_size; j++) {
      kv.key[j] = rand.next();
    }

    for (int j = 0; j < kv.value_size; j++) {
      kv.value[j] = rand.next();
    }

    write(fd, &kv.key_size, 8);
    write(fd, kv.key, kv.key_size);
    write(fd, &kv.value_size, 8);
    write(fd, kv.value, kv.value_size);
    delete[] kv.key;
    delete[] kv.value;
  }
  return 0;
}
