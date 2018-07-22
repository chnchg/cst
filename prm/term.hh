/* prm/term.hh
Copyright (C) 2018 Chun-Chung Chen <cjj@u.washington.edu>

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
 *  A term with description
 */
#pragma once
#include <prm/prm.hh>
#include <string>
#include <map>

class Term
{
	std::string title;
	std::map<std::string, std::string> descs;
	bool do_help;
public:
	Term();
	void set_title(std::string const & text);
	void add(const std::string & term, std::string const & desc = "");
	void add_help();
	bool has_key(std::string const & str) const;
	std::string const & get_desc(std::string const & t) const;
	bool set_var(std::string & var, std::string const & str) const;

	std::shared_ptr<prm::Sable> make_var(std::string & var);
};
