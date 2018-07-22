/* prm/argu.cc
Copyright (C) 2016,2018 Chun-Chung Chen <cjj@u.washington.edu>

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

argu::Base::Base(Param::VarEntry const & v) :
	name(v.name),
	tags(v.tags),
	set_altering(0)
{
}

void argu::Base::dump(std::ostream & output) const
{
	if (auto n = tags.find<tag::Name>()) output << n->text;
	else output << name;
	output << '=' << get_var()->to_str();
}

void argu::Base::set(const std::string & str)
{
	get_var()->read_str(str);
}

void argu::Base::addto_parser(arg::Parser & parser)
{
	int key = 0;
	if (auto k = tags.find<tag::Key>()) key = k->key;
	arg::Option & opt = parser.add_opt(key, get_name())
		.set(& set_altering, true)
		.show_default();
	if (auto d = tags.find<tag::Desc>()) opt.help(d->text);
	addto_option(opt);
}

bool argu::Base::alter()
{
	if (set_altering) do_altering();
	return set_altering;
}

std::string const & argu::Base::get_name() const
{
	return name;
}

template <> std::string argu::type_word<bool>() {return "BOOL";}
template <> std::string argu::type_word<int>() {return "INT";}
template <> std::string argu::type_word<unsigned>() {return "UINT";}
template <> std::string argu::type_word<size_t>() {return "SIZE";}
template <> std::string argu::type_word<double>() {return "DOUBLE";}
template <> std::string argu::type_word<std::string>() {return "STRING";}

namespace {
	bool sable_callback(int, std::string const & str, void * data)
	{
		argu::Sab * s = static_cast<argu::Sab *>(data);
		return s->parse(str);
	}
}

void argu::Sab::addto_option(arg::Option & opt)
{
	dft = sb->to_str();
	opt.stow(dft);
	opt.call(& sable_callback, this);
	if (auto w = tags.find<tag::Word>()) opt.help_word(w->text);
	else opt.help_word("DATA");
}

void argu::Sab::do_altering()
{
	sb->read_str(altering);
}

argu::Sab::Sab(Param::VarEntry const & v) :
	Base(v),
	sb(v.var)
{
	if (! sb) throw;
	altering = sb->to_str();
}

bool argu::Sab::parse(std::string const & str)
{
	if (sb->test_str(str)) {
		altering = str;
		return true;
	}
	return false;
}

std::shared_ptr<argu::Sab> argu::Sab::make(Param::VarEntry const & v)
{
	return std::make_shared<Sab>(v);
}

Argu::~Argu()
{
	clear();
}

void Argu::add(Param & p)
{
	if (argus.size()) argus.push_back(0); // add separator...
	for (auto & i: p.vars) if (i.tags.find<tag::CmdLine>()) {
		// The Type infomation for Var<Type> can not be recovered.
		// So, we are making tabulated translations:
		if (auto c = argu::Arg<bool>::make(i)) argus.push_back(c);
		else if (auto c = argu::Arg<int>::make(i)) argus.push_back(c);
		else if (auto c = argu::Arg<unsigned>::make(i)) argus.push_back(c);
		else if (auto c = argu::Arg<size_t>::make(i)) argus.push_back(c);
		else if (auto c = argu::Arg<double>::make(i)) argus.push_back(c);
		else if (auto c = argu::Arg<std::string>::make(i)) argus.push_back(c);
		else argus.push_back(argu::Sab::make(i));
	}
}

void Argu::clear()
{
	// This invalidates the SubParser created by make_parser
	argus.clear();
}

std::shared_ptr<arg::SubParser> Argu::make_parser(std::string title)
{
	auto sp = std::make_shared<arg::SubParser>();
	if (title.size()) sp->add_help(title + '\n');
	sp->add_help("Available parameters:");
	sp->add_help("");
	for (auto & i: argus) {
		if (i) i->addto_parser(* sp);
		else sp->add_help("");
	}
	sp->add_help("");
	sp->add_opt_help();
	return sp;
}

void Argu::addto_parser(arg::Parser & parser)
{
	for (auto & i: argus) {
		if (i) i->addto_parser(parser);
		else parser.add_help(""); // separator
	}
}

bool Argu::alter()
{
	bool any_change = false;
	for (auto & i: argus) if (i && i->alter()) any_change = true;
	return any_change;
}

size_t Argu::size() const
{
	return argus.size();
}
