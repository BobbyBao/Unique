#include "Precompiled.h"
#include "ByteArray.h"


namespace Unique
{


	ByteArray&& toBase64(ByteArray& v)
	{
		const char alphabet[] = "ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"
			"ghijklmn" "opqrstuv" "wxyz0123" "456789+/";
		const char padchar = '=';
		int padlen = 0;

		ByteArray tmp;
		tmp.resize((v.size() * 4) / 3 + 3);

		int i = 0;
		char *out = tmp.data();
		while (i < v.size()) {
			int chunk = 0;
			chunk |= int(byte(v[i++])) << 16;
			if (i == v.size()) {
				padlen = 2;
			}
			else {
				chunk |= int(byte(v[i++])) << 8;
				if (i == v.size()) padlen = 1;
				else chunk |= int(byte(v[i++]));
			}

			int j = (chunk & 0x00fc0000) >> 18;
			int k = (chunk & 0x0003f000) >> 12;
			int l = (chunk & 0x00000fc0) >> 6;
			int m = (chunk & 0x0000003f);
			*out++ = alphabet[j];
			*out++ = alphabet[k];
			if (padlen > 1) *out++ = padchar;
			else *out++ = alphabet[l];
			if (padlen > 0) *out++ = padchar;
			else *out++ = alphabet[m];
		}

		tmp.resize(out - tmp.data());
		return std::move(tmp);
	}

	ByteArray&& fromBase64(const char* base64, size_t size)
	{
		unsigned int buf = 0;
		int nbits = 0;
		ByteArray tmp;
		tmp.reserve((size * 3) / 4);

		int offset = 0;
		for (int i = 0; i < size; ++i) {
			int ch = base64[i];
			int d;

			if (ch >= 'A' && ch <= 'Z')
				d = ch - 'A';
			else if (ch >= 'a' && ch <= 'z')
				d = ch - 'a' + 26;
			else if (ch >= '0' && ch <= '9')
				d = ch - '0' + 52;
			else if (ch == '+')
				d = 62;
			else if (ch == '/')
				d = 63;
			else
				d = -1;

			if (d != -1) {
				buf = (buf << 6) | d;
				nbits += 6;
				if (nbits >= 8) {
					nbits -= 8;
					tmp.push_back(buf >> nbits);///*[offset++]*/ = buf >> nbits;
					buf &= (1 << nbits) - 1;
				}
			}
		}

		tmp.shrink_to_fit();
		return std::move(tmp);
	}

}