#include "random.hpp"

namespace TerraNova {
namespace Random {

// anonymous namespace containing PRNG variables
namespace {
	uint64_t boolSources[2];
	uint64_t currentBoolSource;
	uint64_t currentBoolBit;

	std::mt19937 intGenerator;
}

void Initialize() {
	std::random_device randomDevice;

	// bools
	boolSources[0] = (uint64_t(randomDevice()) << 32) ^ randomDevice();
	boolSources[1] = (uint64_t(randomDevice()) << 32) ^ randomDevice();
	currentBoolSource = 0; // this is irrelevant because of the next line
	currentBoolBit = 63;   // the generator is reset when first called
	
	// ints
	intGenerator = std::mt19937(randomDevice());
}

uint64_t XorShiftPlus(uint64_t& source1, uint64_t& source2) {
	uint64_t x = source1;
	const uint64_t y = source2;
	source1 = y;
	x ^= x << 23;
	source2 = x ^ y ^ (x >> 17) ^ (y >> 26);
	return source2 + y;
}

bool Bool() {
	if(currentBoolBit >= 63){
		currentBoolSource = XorShiftPlus(boolSources[0], boolSources[1]);
		currentBoolBit = 0;
	} else {
		++currentBoolBit;
	}
	return currentBoolSource & (1 << currentBoolBit);
}

int Int(const int min, const int max) {
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(intGenerator);
}

} // namespace Random
} // namespace TerraNova
