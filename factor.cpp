#include "common.h"
#include "prime.h"

list<integer> find_primes(integer until)
{
	prime_iterator it;
	integer value;
	list<integer> result;
	while ((value = *it++) <= until) {
		result.emplace_back(value);
	}
	return result;
}

map<integer, integer> find_factors(integer value)
{
	map<integer, integer> factors;
	if (value <= 0) {
		throw domain_error("Parameter must be positive");
	}
	for (prime_iterator it; value > 1; ++it) {
		auto prime = *it;
		while (value % prime == 0) {
			value /= prime;
			factors[prime]++;
		}
	}
	return factors;
}

template <typename Reductor>
static map<integer, integer> merge_factors(const map<integer, integer>& a, const map<integer, integer>& b, Reductor reductor)
{
	set<integer> factors;
	map<integer, integer> result;
	for (const auto& kv : a) {
		factors.emplace(kv.first);
	}
	for (const auto& kv : b) {
		factors.emplace(kv.first);
	}
	for (const auto f : factors) {
		auto ai = a.find(f);
		auto bi = b.find(f);
		optional<integer> av = ai != a.end() ? make_optional(ai->second) : nullopt;
		optional<integer> bv = bi != b.end() ? make_optional(bi->second) : nullopt;
		if (integer degree = reductor(f, av, bv); degree) {
			result[f] = degree;
		}
	}
	return result;
}

integer find_lowest_common_factor(integer x, integer y)
{
	auto factors = merge_factors(find_factors(x), find_factors(y), [] (auto, auto fx , auto fy) {
		return fx && fy ? 1 : 0;
	});
	return factors.empty() ? 1 : factors.begin()->first;
}

integer find_greatest_common_factor(integer x, integer y)
{
	auto factors = merge_factors(find_factors(x), find_factors(y), [] (auto, auto fx, auto fy) {
		return min(fx.value_or(0), fy.value_or(0));
	});
	integer result = 1;
	for (auto [factor, count] : factors) {
		for (int i = 0; i < count; ++i) {
			result *= factor;
		}
	}
	return result;
}

integer find_lowest_common_multiple(integer x, integer y)
{
	auto factors = merge_factors(find_factors(x), find_factors(y), [] (auto, auto fx, auto fy) {
		return max(fx.value_or(*fy), fy.value_or(*fx));
	});
	integer result = 1;
	for (auto [factor, count] : factors) {
		for (int i = 0; i < count; ++i) {
			result *= factor;
		}
	}
	return result;
}

integer find_greatest_common_factor_euclid(integer x, integer y)
{
	/* Euclid's algorithm */
	if (x <= 0 || y <= 0) {
		throw domain_error("Parameters must be positive");
	}
	int log2;
	for (log2 = 0; ((x | y) & 1) == 0; ++log2) {
		x >>= 1;
		y >>= 1;
	}
	while ((x & 1) == 0) {
		x >>= 1;
	}
	do {
		while ((y & 1) == 0) {
			y >>= 1;
		}
		if (x > y) {
			swap(x, y);
		}
	} while (y > 0);
	return x << log2;
}
