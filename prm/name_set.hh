/* prm/name_set.hh
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
 *  A mapping between keys and names with some descriptions
 */
#ifndef NAME_SET_HH
#define NAME_SET_HH
#include <val.hh>
#include <string>
#include <vector>
class NameSet
{
public:
	class Name
	{
		std::string name;
		int key;
		std::string desc;

		friend class NameSet;
	public:
		Name(std::string const & name, int key, std::string const & desc = "") :
			name(name), key(key), desc(desc) {}

		// utilities
		const std::string & get_name() const {return name;}

		int get_key() const {return key;}

		const std::string & get_desc() const {return desc;}
	};

private:
	std::vector<Name> names;

public:
	void add(const std::string & name, int key, std::string const & desc = "")
	{
		names.push_back(Name(name, key, desc));
	}

	void add(const std::string & name, std::string const & desc = "")
	{
		names.push_back(Name(name, new_key(), desc));
	}

	bool check(int key, const std::string & name) const
	{
		for (unsigned i = 0; i < names.size(); i ++) if (key == names[i].key) return name == names[i].name;
		return false;
	}

	int get_key(const std::string & name) const
	{
		for (unsigned i = 0; i < names.size(); i ++) if (name == names[i].name) return names[i].key;
		throw;
	}

	std::string const & get_name(int key) const
	{
		for (unsigned i = 0; i < names.size(); i ++) if (key == names[i].key) return names[i].name;
		throw;
	}

	std::string const & get_desc(int key) const
	{
		for (unsigned i = 0; i < names.size(); i ++) if (key == names[i].key) return names[i].desc;
		throw;
	}

	std::vector<std::string> get_names() const
	{
		std::vector<std::string> n;
		for (unsigned i = 0; i < names.size(); i ++) n.push_back(names[i].name);
		return n;
	}

	int new_key() const
	{
		int k = 0;
		for (unsigned i = 0; i < names.size(); i ++) if (k <= names[i].key) k = names[i].key + 1;
		return k;
	}

	size_t size() const {return names.size();}

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
#endif // NAME_SET_HH
