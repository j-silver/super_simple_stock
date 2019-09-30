//
// Created by giuseppe on 17/04/19.
//

#include "merge_arrays.cpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <random>


TEST(Merge, RandomArray)
{
	std::random_device randomDevice {"/dev/random"};
	std::uniform_int_distribution<int> distribution;
	std::mt19937_64 generator {randomDevice()};
	std::vector<int> a1(100);
	std::vector<int> a2(100);

	for (auto& x : a1)
		x = distribution(generator);
	for (auto& y : a2)
		y = distribution(generator);

	std::vector<int> expected(200);

	std::merge(a1.begin(), a1.end(), a2.begin(), a2.end(), expected.begin());
	auto merged {merge_arrays(a1, a2)};
	for (size_t i {0}; i < 200; ++i)
		ASSERT_EQ(merged[i], expected[i]);
}


int main(int argc, char* argv[])
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}