/* sim/runnable.hh
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
 * A runnable system
 */
#ifndef RUNNABLE_HH
#define RUNNABLE_HH
#include "with_param.hh"
#include <prm/name_set.hh>
#include <hdf5.h>
class Runnable :
	virtual public WithParam // running parameter manager
{
public:
	virtual ~Runnable() {}
	// essentials
	virtual void init() = 0; // initialize system
	virtual void dash() = 0; // short run
	// HDF5
	virtual void h5_save(hid_t group) const = 0;
	virtual void h5_load(hid_t group) = 0;
	// run meter
	virtual double count() const = 0;
	// optional abilities
	NameSet dump_set;
	virtual void dump(int /*what*/, std::ostream & /*output*/) const {} // dump states to a stream
	std::string description;
};
#endif // RUNNABLE_HH
