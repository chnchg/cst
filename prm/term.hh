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
#include <val.hh>
#include <string>
#include <map>
class Term
{
	std::vector<std::string, std::string> descs;
public:
	void add(const std::string & term, std::string const & desc = "")
	{
		descs[term] = desc;
	}

	std::string const & get_desc(std::string const & t) const
	{
		return descs[t];
	}

	std::vector<std::string> get_terms() const
	{
		std::vector<std::string> n;
		for (auto const & p: descs) n.push_back(p.first);
		return n;
	}

	size_t size() const {return descs.size();}

	arg::SetValue * make_set_value(int & key_value)
	{
		arg::SetValue * sv = new arg::SetValue(key_value);
		for (unsigned i = 0; i < names.size(); i ++) {
			Name & n = names[i];
			if (n.name == "help") sv->add_help(n.desc, n.key);
			else sv->add(n.name, n.key, n.desc);
		}
		return sv;
	}
};
