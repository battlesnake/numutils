#include "prime.h"

vector<integer> prime_iterator::primes { 2 };

optional<size_t> prime_iterator::prime_index(integer value)
{
	while (last() < value) {
		next();
	}
	auto it = std::find(primes.cbegin(), primes.cend(), value);
	return it != primes.cend() ? make_optional(it - primes.cbegin()) : nullopt;
}

integer prime_iterator::last()
{
	return *primes.crbegin();
}

/* Generate the next prime */
integer prime_iterator::next()
{
	/* Dynamic */
	for (auto i = last() + 1; ; ++i) {
		const auto si = integer(sqrt(i));
		bool is_prime = true;
		for (auto p : primes) {
			/* No potential factors remaining */
			if (p > si) {
				break;
			}
			/* Has factor: Not prime */
			if (i % p == 0) {
				is_prime = false;
				break;
			}
		}
		/* Prime */
		if (is_prime) {
			primes.emplace_back(i);
			return i;
		}
	}
}

prime_iterator::prime_iterator(integer value) :
	value(value)
{
	while (last() < value) {
		next();
	}
	auto _index = prime_index(value);
	if (value <= 1 || !_index) {
		throw domain_error("Invalid parameter, should be prime");
	}
	index = *_index;
}

prime_iterator prime_iterator::operator ++ ()
{
	if (value == last()) {
		next();
	}
	++index;
	value = primes[index];
	return *this;
}

prime_iterator prime_iterator::operator ++ (int)
{
	prime_iterator prev = *this;
	++*this;
	return prev;
}

integer prime_iterator::operator * ()
{
	return value;
}
