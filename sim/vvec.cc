/* sim/vvec.cc
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
#include "vvec.hh"
#include <iostream>

using namespace std;

VVec::VVec() :
	sz(8),  // set minimal size to 8
	va(new double[8]),
	last(0)
{
	for (size_t i = 0; i < sz; i ++) va[i] = 0;
}

VVec::VVec(const VVec & d) :
	sz(d.sz),
	va(new double[d.sz]),
	last(d.last)
{
	for (size_t i = 0; i < sz; i ++) va[i] = d.va[i];
}

VVec::~VVec()
{
	delete[] va;
}

VVec & VVec::operator = (const VVec & d)
{
	if (sz != d.sz) {
		delete[] va;
		sz = d.sz;
		va = new double[sz];
	}
	for (size_t i = 0; i < sz; i ++) va[i] = d.va[i];
	last = d.last;
	return * this;
}

VVec & VVec::operator += (const VVec & d)
{
	if (sz < d.sz) {
		size_t nz = sz * 2;
		while (nz < d.sz) nz *= 2;
		double * nv = new double[nz];
		for (size_t j = 0; j < sz; j ++) nv[j] = va[j];
		for (size_t j = sz; j < nz; j ++) nv[j] = 0;
		delete[] va;
		va = nv;
		sz = nz;
	}
	for (size_t i = 0; i < d.sz; i ++) va[i] += d.va[i];
	if (last < d.last) last = d.last;
	return * this;
}

VVec & VVec::operator -= (const VVec & d)
{
	if (sz < d.sz) {
		size_t nz = sz * 2;
		while (nz < d.sz) nz *= 2;
		double * nv = new double[nz];
		for (size_t j = 0; j < sz; j ++) nv[j] = va[j];
		for (size_t j = sz; j < nz; j ++) nv[j] = 0;
		delete [] va;
		va = nv;
		sz = nz;
	}
	for (size_t i = 0; i < d.sz; i ++) va[i] -= d.va[i];
	if (last < d.last) last = d.last;
	return * this;
}

VVec & VVec::operator *= (const VVec & d)
{
	size_t i;
	for (i = 0; i < d.sz && i < sz; i ++) va[i] *= d.va[i];
	while (i < sz) {
		va[i] = 0;
		i ++;
	}
	if (last > d.last) last = d.last;
	return * this;
}

VVec & VVec::operator *= (double v)
{
	for (size_t i = 0; i < sz; i ++) va[i] *= v;
	return * this;
}

VVec & VVec::operator /= (double v)
{
	for (size_t i = 0; i < sz; i ++) va[i] /= v;
	return * this;
}

const double & VVec::operator [] (size_t i) const
{
	return va[i];
}

double & VVec::operator [] (size_t i)
{
	if (i <= last) return va[i];
	last = i;
	if (last < sz) return va[i];

	size_t nz = sz * 2; // new size
	while (nz <= last) nz *= 2;
	double * nv = new double[nz];

	for (size_t j = 0; j < sz; j ++) nv[j] = va[j];
	for (size_t j = sz; j < nz; j ++) nv[j] = 0;

	delete[] va;
	va = nv;
	sz = nz;

	return va[i];
}

void VVec::clear()
{
	delete [] va;
	sz = 8;
	va = new double[sz];
	for (size_t i = 0; i < sz; i ++) va[i] = 0;
	last = 0;
}

size_t VVec::size() const
{
	return last + 1;
}

#ifdef HAVE_HDF5
hid_t VVec::h5_type()
{
	hid_t vtype = H5Tvlen_create(H5T_NATIVE_DOUBLE);
	return vtype;
}

void VVec::h5_make(H5Data * buf) const
{
	buf->v.p = va;
	buf->v.len = last + 1;
}

void VVec::h5_free(H5Data *) const
{
	// do nothing
}

void VVec::h5_read(H5Data * buf)
{
	delete [] va;
	last = buf->v.len;
	sz = 8;
	while (sz < last) sz *= 2;
	va = new double[sz];

	double * p = static_cast<double *>(buf->v.p);
	for (size_t i = 0; i < last; i ++) va[i] = * (p ++);
	for (size_t i = last; i < sz; i ++) va[i] = 0;
	last --;
}
#endif

void VVec::st_save(std::ostream & ou) const
{
	size_t len = last + 1;
	ou.write((char *) & len, sizeof(len));
	ou.write((char *) va, sizeof(double) * len);
}

void VVec::st_load(std::istream & in)
{
	size_t len;
	in.read((char *) & len, sizeof(len));
	delete [] va;
	for (sz = 8; sz < len; sz *= 2);
	va = new double[sz];
	in.read((char *) va, sizeof(double) * len);
	for (size_t i = len; i < sz; i ++) va[i] = 0;
	last = len - 1;
}
