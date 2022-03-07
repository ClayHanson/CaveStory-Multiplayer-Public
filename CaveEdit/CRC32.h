#pragma once

// https://rosettacode.org/wiki/CRC-32#C
static unsigned int rc_crc32(unsigned int crc, const char* buf, size_t len)
{
	static unsigned int table[256];
	static bool have_table = false;
	unsigned int rem;
	unsigned char octet;
	int i, j;
	const char* p, * q;

	if (!have_table)
	{
		for (i = 0; i < 256; i++)
		{
			rem = i;
			for (j = 0; j < 8; j++)
			{
				if (rem & 1)
				{
					rem >>= 1;
					rem  ^= 0xedb88320;
				}
				else
				{
					rem >>= 1;
				}
			}

			table[i] = rem;
		}

		have_table = true;
	}

	crc = ~crc;
	q   = buf + len;

	for (p = buf; p < q; p++)
	{
		octet = *p;
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}

	return ~crc;
}