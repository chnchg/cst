/* sim/fvec.hh
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
// fixed array

#include <iostream>
extern "C" {
#include <hdf5.h>
}
class FVec
{
public:
	FVec() : sz(0), va(0) {}
	FVec(size_t sz);
	FVec(const FVec & vec);
	~FVec();

	FVec & operator = (const FVec & vec);
	FVec & operator += (const FVec & vec);
	FVec & operator -= (const FVec & vec);
	FVec & operator *= (const FVec & vec);
	FVec & operator *= (double value);
	FVec & operator /= (double value);
	double const & operator [] (size_t index) const {return va[index];}
	double & operator [] (size_t index) {return va[index];}
	void clear();
	size_t size() const {return sz;}
	void resize(size_t sz);
private:
	size_t sz;
	double * va;
public:
#ifdef HAVE_HDF5
	hid_t h5_type();
	typedef double H5Data;
	H5Data * h5_buff(size_t n = 1) const; // alloc buffer
	void h5_make(H5Data * buf, size_t i = 0) const;
	void h5_free(H5Data * buf) const;
	void h5_read(H5Data * buf, size_t i = 0);
#endif
	void st_save(std::ostream & output) const;
	void st_load(std::istream & input);
};
