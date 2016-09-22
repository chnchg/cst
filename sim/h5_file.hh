/* sim/h5_file.hh
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
/* Take care of openning and backing up HDF5 files */
#include <hdf5.h>
#include <string>
class H5File
{
	std::string name;
	bool is_open;
	hid_t file;
	hid_t group;
public:
	struct Error
	{
		std::string info;
	};
	H5File(std::string name = "") : name(name), is_open(false) {}
	~H5File();
	void set_name(std::string n) {name = n;}
	hid_t open_save(std::string path = "/");
	void done_save();
	hid_t open_load(std::string path = "/");
	void close();
};
