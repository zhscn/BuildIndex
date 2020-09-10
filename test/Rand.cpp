#include "Rand.h"

#include <chrono>

Rand::Rand(int min, int max)
    : rd_{}, seed_{rd_() ^
                   ((std::mt19937::result_type)
                        std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count() +
                    (std::mt19937::result_type)
                        std::chrono::duration_cast<std::chrono::microseconds>(
                            std::chrono::high_resolution_clock::now()
                                .time_since_epoch())
                            .count())},
      gen_{seed_}, distribute_(min, max), min_{min}, max_{max} {}

int Rand::next() {
  if (max_ == min_)
    return max_;
  else
    return distribute_(gen_);
}

int Rand::next(int min, int max) {
  std::uniform_int_distribution<decltype(seed_)> distrib(min, max);
  return distrib(gen_);
}

std::vector<int> Rand::generate(int min, int max, int n) {
  std::uniform_int_distribution<decltype(seed_)> distrib(min, max);
  std::vector<int> result;

  for (int i = 0; i < n; i++) {
    auto r = distrib(gen_);
    result.push_back(r);
  }

  return result;
}

std::vector<int> Rand::shuffle(int min, int max) {
  std::vector<int> result;
  auto length = max - min + 1;

  for (int i = min; i <= max; i++)
    result.push_back(i);

  for (int i = 0; i < length; i++)
    std::swap(result[i], result[next(i, length - 1)]);

  return result;
}

void Rand::shuffle(std::vector<int>& input) {
  Rand rand;
  int len = input.size();
  for (int i = 0; i < len; i++)
    std::swap(input[i], input[rand.next(i, len - 1)]);
}
