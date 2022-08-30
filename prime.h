#include "common.h"

class prime_iterator
{
	static vector<integer> primes;

	static integer last();
	static integer next();

	static optional<size_t> prime_index(integer value);

	integer value;
	size_t index;

public:
	explicit prime_iterator(integer value = 2);
	prime_iterator(const prime_iterator&) = default;
	prime_iterator& operator = (const prime_iterator&) = default;

	auto operator <=> (const prime_iterator&) const = default;

	friend bool operator == (const prime_iterator&, const prime_iterator&) = default;
	friend auto operator <=> (const prime_iterator&, const prime_iterator&) = default;

	prime_iterator operator ++ ();
	prime_iterator operator ++ (int);
	integer operator * ();
};
