/* sim/random.hh
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
 *  Random number generator
 */

#ifndef RANDOM_HH
#define RANDOM_HH
#include <hdf5.h>
#include <iostream>
#include <cmath>
class Random
{
	bool gaussian_avail;
	double gaussian_saved;
public:
	Random() : gaussian_avail(false) {}
	virtual ~Random() {};
	virtual double uniform() = 0;
	double gaussian() // from uniform random numbers
	{
		if (gaussian_avail) {
			gaussian_avail = false;
			return gaussian_saved;
		}
		double x1, x2, w;
		do {
			x1 = 2.0 * uniform() - 1.0;
			x2 = 2.0 * uniform() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0);
		w = sqrt((- 2.0 * log(w)) / w);
		gaussian_saved = x2 * w;
		gaussian_avail = true;
		return x1 * w;
	}
#ifdef HAVE_HDF5
	virtual void h5_save(hid_t group, const std::string & name = "random") const = 0;
	virtual void h5_load(hid_t group, const std::string & name = "random") = 0;
#endif
	virtual void st_save(std::ostream & /*output*/) const {}
	virtual void st_load(std::istream & /*input*/) {}
};
#endif // RANDOM_HH
