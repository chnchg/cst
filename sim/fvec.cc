/* sim/fvec.cc
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
#include "fvec.hh"
#include <iostream>
#include <cassert>

using namespace std;

FVec::FVec(size_t sz) : sz(sz), va(new double [sz])
{
	for (size_t i = 0; i < sz; i ++) va[i] = 0;
}

FVec::FVec(const FVec & d) : sz(d.sz), va(new double[d.sz])
{
	for (size_t i = 0; i < sz; i ++) va[i] = d.va[i];
}

FVec::~FVec() {delete[] va;}

FVec & FVec::operator = (const FVec & d)
{
	assert(sz == d.sz);
	for (size_t i = 0; i < sz; i ++) va[i] = d.va[i];
	return * this;
}

FVec & FVec::operator += (const FVec & d)
{
	assert(sz == d.sz);
	for (size_t i = 0; i < d.sz; i ++) va[i] += d.va[i];
	return * this;
}

FVec & FVec::operator -= (const FVec & d)
{
	assert(sz == d.sz);
	for (size_t i = 0; i < d.sz; i ++) va[i] -= d.va[i];
	return * this;
}

FVec & FVec::operator *= (const FVec & d)
{
	assert(sz == d.sz);
	for (size_t i = 0; i < d.sz; i ++) va[i] *= d.va[i];
	return * this;
}

FVec & FVec::operator *= (double v)
{
	for (size_t i = 0; i < sz; i ++) va[i] *= v;
	return * this;
}

FVec & FVec::operator /= (double v)
{
	for (size_t i = 0; i < sz; i ++) va[i] /= v;
	return * this;
}

void FVec::clear()
{
	for (size_t i = 0; i < sz; i ++) va[i] = 0;
}

void FVec::resize(size_t s)
{
	delete [] va;
	sz = s;
	if (sz) {
		va = new double [sz];
		for (size_t i = 0; i < sz; i ++) va[i] = 0;
	}
	else va = 0;
}

#ifdef HAVE_HDF5
hid_t FVec::h5_type()
{
	hsize_t s = sz;
	return H5Tarray_create(H5T_NATIVE_DOUBLE, 1, & s);
}

FVec::H5Data * FVec::h5_buff(size_t n) const
{
	return new double [n * sz];
}

void FVec::h5_make(H5Data * buf, size_t i) const
{
	double * a = buf + i * sz;
	for (size_t j = 0; j < sz; j ++) a[j] = va[j];
}

void FVec::h5_free(H5Data * buf) const
{
	delete [] buf;
}

void FVec::h5_read(H5Data * buf, size_t i)
{
	double * a = buf + i * sz;
	for (size_t j = 0; j < sz; j ++) va[j] = a[j];
}
#endif

void FVec::st_save(std::ostream & ou) const
{
	ou.write((char *) va, sizeof(double) * sz);
}

void FVec::st_load(std::istream & in)
{
	in.read((char *) va, sizeof(double) * sz);
}
