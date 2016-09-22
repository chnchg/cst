/* sim/vvec.hh
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
// auto growing array

#include <iostream>
extern "C" {
#include <hdf5.h>
}
class VVec
{
public:
	VVec();
	VVec(const VVec & vec);
	~VVec();

	VVec & operator = (const VVec & vec);
	VVec & operator += (const VVec & vec);
	VVec & operator -= (const VVec & vec);
	VVec & operator *= (const VVec & vec);
	VVec & operator *= (double value);
	VVec & operator /= (double value);
	const double & operator [] (size_t index) const;
	double & operator [] (size_t index);
	void clear();
	size_t size() const;
private:
	size_t sz;
	double * va;
	size_t last; // last accessed index
public:
#ifdef HAVE_HDF5
	static hid_t h5_type();
	struct H5Data
	{
		hvl_t v;
	};
	void h5_make(H5Data * buf) const;
	void h5_free(H5Data * buf) const;
	void h5_read(H5Data * buf);
#endif
	void st_save(std::ostream & output) const;
	void st_load(std::istream & input);
};
