#pragma once

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const unsigned short *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const unsigned char *)(d))[1])) << 8)\
                       +(uint32_t)(((const unsigned char *)(d))[0]) )
#endif

// From http://www.azillionmonkeys.com/qed/hash.html
static inline unsigned int SuperFastHash(const char* data, int len)
{
	unsigned int hash = len, tmp;
	int rem;

	if (len <= 0 || data == nullptr)
		return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--) {
		hash += get16bits(data);
		tmp   = (get16bits(data + 2) << 11) ^ hash;
		hash  = (hash << 16) ^ tmp;
		data += 2 * sizeof(unsigned short);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
	case 3: hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= ((signed char)data[sizeof(unsigned short)]) << 18;
		hash += hash >> 11;
		break;
	case 2: hash += get16bits(data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: hash += (signed char)* data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

#undef get16bits