#pragma once

#include <random>

class Rand {
public:
  Rand(int min = 0, int max = 0);

  int next();

  /// return random number on the range [min, max]
  int next(int min, int max);

  /// return n random numbers on the range [min, max]
  std::vector<int> generate(int min, int max, int n);

  /// return a shuffled vector, within [min, max]
  std::vector<int> shuffle(int min, int max);
  static void shuffle(std::vector<int>& input);

  int operator()() { return next(); }

private:
  std::random_device rd_;
  std::mt19937::result_type seed_;
  std::mt19937 gen_;
  std::uniform_int_distribution<std::mt19937::result_type> distribute_;

  int min_;
  int max_;
};
