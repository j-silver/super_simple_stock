//
// Created by giuseppe on 17/04/19.
//
#include <vector>
#include <algorithm>


std::vector<int> merge_arrays(std::vector<int> const& a1, std::vector<int> const& a2)
{
	std::vector<int> merged;
	merged.reserve(a1.size() + a2.size());
	auto first1 {a1.begin()};
	auto last1 {a1.end()};
	auto first2 {a2.begin()};
	auto last2 {a2.end()};
	auto dFirst {merged.begin()};
	for (; first1 != last1; ++dFirst) {
		if (first2 == last2) {
			std::copy(first1, last1, dFirst);
		}
		if (*first2 < *first1) {
			*dFirst = *first2;
			++first2;
		} else {
			*dFirst = *first1;
			++first1;
		}
	}
	std::copy(first2, last2, dFirst);

	return merged;
}