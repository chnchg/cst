/* sim/ivec.hh
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
/*
 * Auto growing array with negative to positive index
 */
#include <iostream>
extern "C" {
#include <hdf5.h>
}
class IVec
{
public:
	IVec();
	IVec(const IVec & vec);
	~IVec();

	IVec & operator = (const IVec & vec);
	IVec & operator += (const IVec & vec);
	IVec & operator -= (const IVec & vec);
	IVec & operator *= (const IVec & vec);
	IVec & operator *= (double value);
	IVec & operator /= (double value);
	double operator [] (int index) const;
	double & operator [] (int index);
	void clear();
	int min() const;
	int max() const;
private:
	double * va;
	int min_i;
	int max_i;
public:
#ifdef HAVE_HDF5
	static hid_t h5_type();
	struct H5Data
	{
		int min;
		hvl_t arr;
	};
	void h5_make(H5Data * buf) const;
	void h5_free(H5Data * buf) const;
	void h5_read(H5Data * buf);
#endif
	void st_save(std::ostream & output) const;
	void st_load(std::istream & input);
};
