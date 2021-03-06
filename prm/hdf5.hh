/* prm/hdf5.hh
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
#ifndef HDF5_HH
#define HDF5_HH 1
#include "prm.hh"
extern "C" {
#include <hdf5.h>
}
namespace prm {
	class H5Error
	{
	};

	class TypeUnknownError :
		public H5Error
	{
	protected:
		std::string info;
	public:
		TypeUnknownError(const std::string & info);
		std::string get_info();
	};

	class DataMissingError :
		public H5Error
	{
	protected:
		std::string name;
	public:
		DataMissingError(const std::string & name);
		std::string get_name();
	};

	// for Param
	void h5_save(hid_t g, const Param & p, const std::string & n = std::string());
	size_t h5_load(hid_t g, Param & p, const std::string & n = std::string());
	// for Array
	void h5_save(hid_t g, const Array & a, const std::string & n);
	bool h5_load(hid_t g, Array & a, const std::string & n);
}
#endif // HDF5_HH
