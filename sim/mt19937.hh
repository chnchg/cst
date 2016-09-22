/* sim/mt19937.hh
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
 *  Random number generator adapted from:
 *
 *    http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/MTARCOK/mt19937ar-cok.c
 *
 *  See http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 */
#ifndef MT19937_HH
#define MT19937_HH
#include "random.hh"
#include <hdf5.h>
#include <string>
class MT19937 :
	virtual public Random
{
	unsigned long * state;
	int left;
	bool initf;
	unsigned long * next;
	void next_state();
public:
	MT19937();
	MT19937(const MT19937 & rng);
	~MT19937();
	void init(unsigned long seed = 0UL);
	double uniform();
#ifdef HAVE_HDF5
	void h5_save(hid_t group, const std::string & name = "mt19937") const;
	void h5_load(hid_t group, const std::string & name = "mt19937");
#endif
};
#endif
