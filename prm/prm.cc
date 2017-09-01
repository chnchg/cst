/* prm/prm.cc
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
#include "prm.hh"
#include <iostream>
#include <cstdlib>

using namespace prm;

// Taggable
void Taggable::copy_tags(const Taggable & tgb)
{
	while (tag_list.size()) {
		delete tag_list.back();
		tag_list.pop_back();
	}
	for (std::vector<tag::Base *>::const_iterator i = tgb.tag_list.begin(); i != tgb.tag_list.end(); i ++) {
		tag_list.push_back((* i)->dup());
	}
}

Taggable::~Taggable()
{
	while (tag_list.size()) {
		delete tag_list.back();
		tag_list.pop_back();
	}
}

// Param
Param::~Param() {}

Taggable & Param::add_var(std::string const & name, std::shared_ptr<Sable> v)
{
	for (auto & i: vars) if (i.name == name) {
		std::cerr << "Duplicate var!\n";
		abort();
	}
	vars.push_back(VarEntry(name, v));
	return vars.back().tags;
}

std::vector<std::string> Param::get_names() const
{
	std::vector<std::string> nl;
	for (auto & i: vars) nl.push_back(i.name);
	return nl;
}

std::shared_ptr<const Sable> Param::get_var(std::string const & name) const
{
	for (auto & i: vars) if (i.name == name) return i.var;
	std::cerr << "Unknown var!\n";
	throw;
}

std::shared_ptr<Sable> Param::get_var(std::string const & name)
{
	for (auto & i: vars) if (i.name == name) return i.var;
	std::cerr << "Unknown var!\n";
	throw;
}

void Param::delete_var(const std::string & name)
{
	for (auto i = vars.begin(); i != vars.end(); i ++) if (i->name == name) {
		vars.erase(i);
		return;
	}
	std::cerr << "Unknown var!\n";
	throw;
}

void Param::read(std::istream & input)
{
	// line by line
	char buf[128];
	while (input.good()) {
		input.getline(buf, 128);
		if (buf[0] == '#') continue; // skip comments
		std::string s(buf);
		std::string::size_type ep = s.find('=', 0);
		if (ep == std::string::npos) continue; // skip mal formed lines
		std::string name = s.substr(0, ep);
		std::string val = s.substr(ep + 1);
		for (auto & i: vars) if (i.name == name) {
			i.var->read_str(val);
			break;
		}
	}
}

void Param::write(std::ostream & output) const
{
	for (auto & i: vars) {
		output << i.name << '=';
		if (auto ts = std::static_pointer_cast<Sable>(i.var)) output << ts->to_str();
		output << '\n';
	}
}

void Param::copy(Param const & p)
{
	for (auto & i: vars) for (auto & j: p.vars) if (i.name == j.name) i.var->copy_val(* j.var);
}

void Param::append(Param const & p)
{
	for (auto v: p.vars) vars.push_back(v);
}

Struct::Struct() : res(0) {}

Struct::~Struct() {}

Struct::MemEntry * Struct::find_member(std::string const & n)
{
	for (auto & i: list) if (n == i.name) return & i;
	return 0;
}

Struct::MemEntry const * Struct::find_member(std::string const & n) const
{
	for (auto & i: list) if (n == i.name) return & i;
	return 0;
}

void Struct::set_resolver(std::shared_ptr<Res> r)
{
	res = r;
}

std::vector<std::string> Struct::get_names() const
{
	std::vector<std::string> ns;
	for (auto & i: list) ns.push_back(i.name);
	return ns;
}

// prm::Compound
Compound::~Compound() {}

Array & Compound::add_array(const std::string & name, std::shared_ptr<Array> elm)
{
	list.push_back(Entry{name, elm});
	return * list.back().elm;
}

Struct & Compound::add_struct(std::shared_ptr<Struct> elm)
{
	add_array("", elm);
	return * elm;
}

std::shared_ptr<Sable> Compound::make_var(const std::string & name, Index * idx)
{
	for (auto & i: list) {
		if (auto s = std::dynamic_pointer_cast<Struct>(i.elm)) {
			if (auto m = s->find_member(name)) {
				return m->mem->make_var(* s->res, idx);
			}
		}
		else if (i.name == name) return i.elm->make_var(idx);
	}
	return 0;
}

std::shared_ptr<Sable const> Compound::make_var(std::string const & name, Index * idx) const
{
	for (auto i: list) {
		if (auto s = std::dynamic_pointer_cast<Struct const>(i.elm)) {
			if (auto m = s->find_member(name)) {
				return m->mem->make_var(* s->res, idx);
			}
		}
		else if (i.name == name) return i.elm->make_var(idx);
	}
	return 0;
}

std::vector<std::string> Compound::get_names() const
{
	std::vector<std::string> ns;
	for (auto i: list) {
		if (auto s = std::dynamic_pointer_cast<Struct const>(i.elm)) {
			std::vector<std::string> sn = s->get_names();
			ns.insert(ns.end(), sn.begin(), sn.end());
		}
		else ns.push_back(i.name);
	}
	return ns;
}
