/* prm/argu.cc
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
#include "argu.hh"
#include <iostream>

using namespace prm;

tag::Base * tag::Key::dup() const
{
	return new Key(key);
}

tag::Key::Key(int a_key)
{
	key = a_key;
}

argu::Base::Base(Param::VarEntry * v) :
	v(v),
	set_altering(0)
{
}

void argu::Base::dump(std::ostream & output) const
{
	if (auto n = v->tags.find<tag::Name>()) output << n->text;
	else output << v->name;
	output << '=' << v->var->to_str();
}

void argu::Base::set(const std::string & str)
{
	v->var->read_str(str);
}

void argu::Base::addto_parser(arg::Parser & parser)
{
	int key = 0;
	if (auto k = v->tags.find<tag::Key>()) key = k->key;
	arg::Option & opt = parser.add_opt(key, get_name())
		.set(& set_altering, true)
		.show_default();
	if (auto d = v->tags.find<tag::Desc>()) opt.help(d->text);
	addto_option(opt);
}

bool argu::Base::alter()
{
	if (set_altering) do_altering();
	return set_altering;
}

template <> std::string argu::type_word<bool>() {return "BOOL";}
template <> std::string argu::type_word<int>() {return "INT";}
template <> std::string argu::type_word<unsigned>() {return "UINT";}
template <> std::string argu::type_word<size_t>() {return "SIZE";}
template <> std::string argu::type_word<double>() {return "DOUBLE";}

Argu::~Argu()
{
	clear();
}

void Argu::add(Param & p)
{
	if (argus.size()) argus.push_back(0);
	for (auto i: p.vars) if (i.tags.find<tag::CmdLine>()) {
		argu::Base * c = 0;
		// The Type infomation for Var<Type> can not be recovered.
		// So, we are making tabulated translations:
		if (std::dynamic_pointer_cast<Var<bool>>(i.var)) c = new argu::Arg<bool>(& i);
		else if (std::dynamic_pointer_cast<Var<int>>(i.var)) c = new argu::Arg<int>(& i);
		else if (std::dynamic_pointer_cast<Var<unsigned>>(i.var)) c = new argu::Arg<unsigned>(& i);
		else if (std::dynamic_pointer_cast<Var<size_t>>(i.var)) c = new argu::Arg<size_t>(& i);
		else if (std::dynamic_pointer_cast<Var<double>>(i.var)) c = new argu::Arg<double>(& i);
		else if (std::dynamic_pointer_cast<Var<std::string>>(i.var)) c = new argu::Arg<std::string>(& i);
		if (c) argus.push_back(c);
		else std::cerr << "can not make command-line for " << i.name << '\n';
	}
}

void Argu::clear()
{
	// This invalidates the SubParser created by make_parser
	while (argus.size()) {
		if (argu::Base * b = argus.back()) delete b;
		argus.pop_back();
	}
}

arg::SubParser * Argu::make_parser(std::string title)
{
	arg::SubParser * sp = new arg::SubParser;
	if (title.size()) sp->add_help(title + '\n');
	sp->add_help("Available parameters:");
	sp->add_help("");
	for (std::vector<argu::Base *>::iterator i = argus.begin(); i != argus.end(); i ++) {
		if (argu::Base * b = * i) b->addto_parser(* sp);
		else sp->add_help("");
	}
	sp->add_help("");
	sp->add_opt_help();
	return sp;
}

void Argu::addto_parser(arg::Parser & parser)
{
	for (std::vector<argu::Base *>::iterator i = argus.begin(); i != argus.end(); i ++) {
		if (argu::Base * b = * i) b->addto_parser(parser);
		else parser.add_help("");
	}
}

bool Argu::alter()
{
	bool any_change = false;
	for (std::vector<argu::Base *>::iterator i = argus.begin(); i != argus.end(); i ++) {
		if (argu::Base * b = * i) any_change = b->alter() || any_change;
	}
	return any_change;
}

size_t Argu::size() const
{
	return argus.size();
}
