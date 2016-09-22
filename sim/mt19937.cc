/* sim/mt19937.cc
Copyright (C) 2016 Chun-Chung Chen <cjj@u.washington.edu>

This file is part of cst.

cst is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with cst.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mt19937.hh"
/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u, v) (((u) & UMASK) | ((v) & LMASK))
#define TWIST(u, v) ((MIXBITS(u, v) >> 1) ^ ((v) & 1UL ? MATRIX_A : 0UL))
void MT19937::next_state()
{
	unsigned long * p = state;

	if (! initf) init(5489UL);

	left = N;
	next = state;

	for (int j = N - M + 1; -- j; p ++) * p = p[M] ^ TWIST(p[0], p[1]);
	for (int j = M; -- j; p ++) * p = p[M - N] ^ TWIST(p[0], p[1]);
	* p = p[M - N] ^ TWIST(p[0], state[0]);
}
MT19937::MT19937() :
	state(new unsigned long [N]),
	left(1),
	initf(false)
{
	for (int i = N; i--;) state[i] = 0;
}
MT19937::MT19937(const MT19937 & rng) :
	Random(rng), // intel compiler work around
	state(new unsigned long [N]),
	left(rng.left),
	initf(rng.initf)
{
	for (int i = N; i --;) state[i] = rng.state[i];
	next = state + N - left + 1;
}
MT19937::~MT19937()
{
	delete[] state;
}
void MT19937::init(unsigned long s)
{
	state[0] = s & 0xffffffffUL;
	for (int j = 1; j < N; j ++) {
		state[j] = (1812433253UL * (state[j - 1] ^ (state[j - 1] >> 30)) + j);
		state[j] &= 0xffffffffUL;
	}
	left = 1;
	initf = true;
}
double MT19937::uniform()
{
    unsigned long y;

    if (! -- left) next_state();
    y = * next ++;

    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y * (1.0 / 4294967296.0);
}
#ifdef HAVE_HDF5
void MT19937::h5_save(hid_t g, const std::string & name) const
{
	hid_t gg = H5Gcreate(g, name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	hsize_t d = N;
	hid_t space = H5Screate_simple(1, & d, 0);
	hid_t data = H5Dcreate(gg, "state", H5T_NATIVE_ULONG, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	H5Dwrite(data, H5T_NATIVE_ULONG, H5S_ALL, H5S_ALL, H5P_DEFAULT, state);
	H5Dclose(data);
	H5Sclose(space);

	space = H5Screate(H5S_SCALAR);

	hid_t attr = H5Acreate(gg, "left", H5T_NATIVE_INT, space, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(attr, H5T_NATIVE_INT, & left);
	H5Aclose(attr);

	unsigned char i = initf ? 1 : 0;
	attr = H5Acreate(gg, "initf", H5T_NATIVE_UCHAR, space, H5P_DEFAULT, H5P_DEFAULT);
	H5Awrite(attr, H5T_NATIVE_UCHAR, & i);
	H5Aclose(attr);
	H5Sclose(space);
	H5Gclose(gg);
}
void MT19937::h5_load(hid_t g, const std::string & name)
{
	if (! H5Lexists(g, name.c_str(), H5P_DEFAULT)) {
		init();
		return;
	}
	hid_t sg = H5Gopen(g, name.c_str(), H5P_DEFAULT);
	hid_t data = H5Dopen(sg, "state", H5P_DEFAULT);
	H5Dread(data, H5T_NATIVE_ULONG, H5S_ALL, H5S_ALL, H5P_DEFAULT, state);
	H5Dclose(data);
	hid_t attr = H5Aopen_name(sg, "left");
	H5Aread(attr, H5T_NATIVE_INT, & left);
	H5Aclose(attr);
	unsigned char i;
	attr = H5Aopen_name(sg, "initf");
	H5Aread(attr, H5T_NATIVE_UCHAR, & i);
	H5Aclose(attr);
	initf = i == 1;
	H5Gclose(sg);

	next = state + N - left + 1;
}
#endif
