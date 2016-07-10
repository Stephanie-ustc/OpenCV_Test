// fbc_cv is free software and uses the same licence as OpenCV
// Email: fengbingchun@163.com

/* reference: modules/core/src/mathfuncs.cpp
              modules/core/src/copy.cpp
*/

#include "core/core.hpp"
#include "core/fbcdef.hpp"
#include "core/base.hpp"

namespace fbc {

/* ************************************************************************** *\
Fast cube root by Ken Turkowski
(http://www.worldserver.com/turk/computergraphics/papers.html)
\* ************************************************************************** */
static float  cubeRoot(float value)
{
	float fr;
	Cv32suf v, m;
	int ix, s;
	int ex, shx;

	v.f = value;
	ix = v.i & 0x7fffffff;
	s = v.i & 0x80000000;
	ex = (ix >> 23) - 127;
	shx = ex % 3;
	shx -= shx >= 0 ? 3 : 0;
	ex = (ex - shx) / 3; /* exponent of cube root */
	v.i = (ix & ((1 << 23) - 1)) | ((shx + 127) << 23);
	fr = v.f;

	/* 0.125 <= fr < 1.0 */
	/* Use quartic rational polynomial with error < 2^(-24) */
	fr = (float)(((((45.2548339756803022511987494 * fr +
		192.2798368355061050458134625) * fr +
		119.1654824285581628956914143) * fr +
		13.43250139086239872172837314) * fr +
		0.1636161226585754240958355063) /
		((((14.80884093219134573786480845 * fr +
		151.9714051044435648658557668) * fr +
		168.5254414101568283957668343) * fr +
		33.9905941350215598754191872) * fr +
		1.0));

	/* fr *= 2^ex * sign */
	m.f = value;
	v.f = fr;
	v.i = (v.i + (ex << 23) + s) & (m.i * 2 != 0 ? -1 : 0);
	return v.f;
}

float fbcCbrt(float value)
{
	return cubeRoot(value);
}

/* Various border types, image boundaries are denoted with '|'

* BORDER_REPLICATE:     aaaaaa|abcdefgh|hhhhhhh
* BORDER_REFLECT:       fedcba|abcdefgh|hgfedcb
* BORDER_REFLECT_101:   gfedcb|abcdefgh|gfedcba
* BORDER_WRAP:          cdefgh|abcdefgh|abcdefg
* BORDER_CONSTANT:      iiiiii|abcdefgh|iiiiiii  with some specified 'i'
*/
int borderInterpolate(int p, int len, int borderType)
{
	if ((unsigned)p < (unsigned)len) {
		;
	} else if (borderType == BORDER_REPLICATE) {
		p = p < 0 ? 0 : len - 1;
	} else if (borderType == BORDER_REFLECT || borderType == BORDER_REFLECT_101) {
		int delta = borderType == BORDER_REFLECT_101;
		if (len == 1)
			return 0;
		do {
			if (p < 0)
				p = -p - 1 + delta;
			else
				p = len - 1 - (p - len) - delta;
		} while ((unsigned)p >= (unsigned)len);
	} else if (borderType == BORDER_WRAP) {
		FBC_Assert(len > 0);
		if (p < 0)
			p -= ((p - len + 1) / len)*len;
		if (p >= len)
			p %= len;
	} else if (borderType == BORDER_CONSTANT) {
		p = -1;
	} else {
		FBC_Error("Unknown/unsupported border type");
	}

	return p;
}

} // namespace fbc

