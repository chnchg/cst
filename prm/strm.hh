/* prm/strm.hh
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
  storage using binary c++ iostream
 */
#pragma once
#include "prm.hh"
#include <iostream>
namespace prm {
	namespace strm {
		class Error
		{
		};
		class TypeUnknownError :
			public Error
		{
		protected:
			std::string info;
		public:
			TypeUnknownError(const std::string & info);
			std::string get_info();
		};
		class DataMissingError :
			public Error
		{
		protected:
			std::string name;
		public:
			DataMissingError(const std::string & name);
			std::string get_name();
		};
	}
	// for Param
	void st_save(std::ostream & s, const Param & p);
	size_t st_load(std::istream & s, Param & p);
	// for Array
	void st_save(std::ostream & s, const Array & a, const std::string & n);
	bool st_load(std::istream & s, Array & a, const std::string & n);
}
