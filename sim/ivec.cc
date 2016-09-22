/* sim/ivec.cc
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
#include "ivec.hh"
#include <iostream>

using namespace std;

IVec::IVec() :
	min_i(1),
	max_i(0)
{
}

IVec::IVec(const IVec & d)
{
	min_i = d.min_i;
	max_i = d.max_i;
	if (min_i <= max_i) {
		va = new double [max_i - min_i + 1];
		for (int i = min_i; i <= max_i; i ++) va[i - min_i] = d.va[i - d.min_i];
	}
}

IVec::~IVec()
{
	if (min_i <= max_i) delete[] va;
}

IVec & IVec::operator = (const IVec & d)
{
	if (min_i <= max_i) delete[] va;
	min_i = d.min_i;
	max_i = d.max_i;
	if (min_i <= max_i) {
		va = new double [max_i - min_i + 1];
		for (int i = min_i; i <= max_i; i ++) va[i - min_i] = d.va[i - min_i];
	}
	return * this;
}

IVec & IVec::operator += (const IVec & d)
{
	int mn = min_i < d.min_i ? min_i : d.min_i;
	int mx = max_i > d.max_i ? max_i : d.max_i;
	if (min_i != mn || max_i != mx) { // resize the array
		double * nv = new double [mx - mn + 1];
		for (int i = mn; i <= mx; i ++) nv[i - mn] = 0;
		for (int i = min_i; i < max_i; i ++) nv[i - mn] = va[i - min_i];
		min_i = mn;
		max_i = mx;
		delete [] va;
		va = nv;
	}
	for (int i = d.min_i; i <= d.max_i; i ++) va[i - min_i] += d.va[i - d.min_i];
	return * this;
}

IVec & IVec::operator -= (const IVec & d)
{
	int mn = min_i < d.min_i ? min_i : d.min_i;
	int mx = max_i > d.max_i ? max_i : d.max_i;
	if (min_i != mn || max_i != mx) { // resize the array
		double * nv = new double [mx - mn + 1];
		for (int i = mn; i <= mx; i ++) nv[i - mn] = 0;
		for (int i = min_i; i < max_i; i ++) nv[i - mn] = va[i - min_i];
		min_i = mn;
		max_i = mx;
		delete [] va;
		va = nv;
	}
	for (int i = d.min_i; i <= d.max_i; i ++) va[i - min_i] -= d.va[i - d.min_i];
	return * this;
}

IVec & IVec::operator *= (const IVec & d)
{
	int mn = min_i < d.min_i ? d.min_i : min_i;
	int mx = max_i > d.max_i ? d.max_i : max_i;
	for (int i = mn; i <= mx; i ++) va[i - min_i] *= d.va[i - d.min_i];
	if (min_i != mn || max_i != mx) { // shrink the array
		if (mn <= mx) {
			double * nv = new double [mx - mn + 1];
			for (int i = mn; i <= mx; i ++) nv[i - mn] = va[i - min_i];
			min_i = mn;
			max_i = mx;
			delete [] va;
			va = nv;
		}
		else {
			min_i = 1;
			max_i = 0;
			delete [] va;
		}
	}
	return * this;
}

IVec & IVec::operator *= (double v)
{
	if (v) for (int i = min_i; i <= max_i; i ++) va[i - min_i] *= v;
	else if (min_i <= max_i) {
		min_i = 1;
		max_i = 0;
		delete [] va;
	}
	return * this;
}

IVec & IVec::operator /= (double v)
{
	for (int i = min_i; i <= max_i; i ++) va[i - min_i] /= v;
	return * this;
}

double IVec::operator [] (int i) const
{
	if (i >= min_i && i <= max_i) return va[i - min_i];
	return 0;
}

double & IVec::operator [] (int i)
{
	if (i >= min_i && i <= max_i) return va[i - min_i];

	int mn = (i < min_i || min_i > max_i) ? i : min_i;
	int mx = (i > max_i || min_i > max_i) ? i : max_i;
	double * nv = new double [mx - mn + 1];
	for (int j = mn; j <= mx; j ++) nv[j - mn] = 0;
	for (int j = min_i; j < max_i; j ++) nv[j - mn] = va[j - min_i];
	if (min_i <= max_i) delete [] va;
	min_i = mn;
	max_i = mx;
	va = nv;

	return va[i - min_i];
}

void IVec::clear()
{
	if (min_i <= max_i) delete [] va;
	min_i = 1;
	max_i = 0;
}

int IVec::min() const
{
	return min_i;
}

int IVec::max() const
{
	return max_i;
}

#ifdef HAVE_HDF5
hid_t IVec::h5_type()
{
	hid_t vtype = H5Tvlen_create(H5T_NATIVE_DOUBLE);
	hid_t type = H5Tcreate(H5T_COMPOUND, sizeof(H5Data));
	H5Tinsert(type, "min", HOFFSET(H5Data, min), H5T_NATIVE_INT);
	H5Tinsert(type, "arr", HOFFSET(H5Data, arr), vtype);
	H5Tclose(vtype);
	return type;
}

void IVec::h5_make(H5Data * buf) const
{
	buf->min = min_i;
	buf->arr.p = va;
	buf->arr.len = max_i - min_i + 1;
}

void IVec::h5_free(H5Data *) const
{
	// do nothing
}

void IVec::h5_read(H5Data * buf)
{
	if (min_i <= max_i) delete [] va;
	min_i = buf->min;
	max_i = buf->arr.len + buf->min - 1;
	if (min_i <= max_i) {
		va = new double [max_i - min_i + 1];
		double * p = static_cast<double *>(buf->arr.p);
		for (int i = min_i; i <=max_i; i ++) va[i - min_i] = p[i - min_i];
	}
}
#endif

void IVec::st_save(std::ostream & ou) const
{
	ou.write((char *) & min_i, sizeof(min_i));
	size_t len = max_i - min_i + 1;
	ou.write((char *) & len, sizeof(len));
	ou.write((char *) va, sizeof(double) * len);
}

void IVec::st_load(std::istream & in)
{
	if (min_i <= max_i) delete [] va;
	in.read((char *) & min_i, sizeof(min_i));
	size_t len;
	in.read((char *) & len, sizeof(len));
	max_i = len + min_i - 1;
	if (min_i <= max_i) {
		va = new double[max_i - min_i + 1];
		in.read((char *) va, sizeof(double) * len);
	}
}
