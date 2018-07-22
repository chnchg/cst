/* prm/term.cc
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
#include "term.hh"
#include <iostream>

namespace {
	class TSable :
		virtual public prm::Sable
	{
		Term const * t;
		std::string & s;
		std::string to_str() const override {return s;}
		bool read_str(std::string const & str) override
		{
			return t->set_var(s, str);
		}
		bool test_str(std::string const & str) const override
		{
			return t->has_key(str);
		}
	public:
		TSable(Term const * t, std::string & s) : t(t), s(s) {}
	};
}

Term::Term() :
	do_help(false)
{}

void Term::set_title(std::string const & text)
{
	title = text;
}

void Term::add(const std::string & term, std::string const & desc)
{
	descs[term] = desc;
}

void Term::add_help()
{
	do_help = true;
}

bool Term::has_key(std::string const & str) const
{
	if (do_help && str == "help") {
		std::cout << '\n' << title << ":\n\n";
		for (auto const & p: descs) {
			std::cout << "    " << p.first;
			std::cout << std::string(25-p.first.size(),' ');
			std::cout << p.second << '\n';
		}
		std::cout << '\n';
		exit(0);
	}
	return descs.find(str) != descs.end();
}

std::string const & Term::get_desc(std::string const & t) const
{
	return descs.find(t)->second;
}

bool Term::set_var(std::string & var, std::string const & str) const
{
	if (has_key(str)) {
		var = str;
		return true;
	}
	return false;
}

std::shared_ptr<prm::Sable> Term::make_var(std::string & str)
{
	return std::make_shared<TSable>(this, str);
}
