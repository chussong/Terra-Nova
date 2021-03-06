#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <cstdint>
#include <random>
#include <iostream>

namespace TerraNova {
namespace Random {

void Initialize();

uint64_t XorShiftPlus(uint64_t& source1, uint64_t& source2);

double Percentage(); // return something between 0.0 and 100.0
bool Bool();
int Int(const int min, const int max);

} // namespace Random
} // namespace TerraNova

#endif
