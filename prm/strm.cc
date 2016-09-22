/* prm/strm.cc
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
#include "strm.hh"
#include <set>
#include <cstring>
#include <cassert>
using namespace prm;

namespace { // no need to export all these implementation
	// saving with native types
	template <typename T>
	bool st_save_bin(std::ostream & s, const TagSable * ts)
	{
		if (const Var<T> * v = dynamic_cast<const Var<T> *>(ts)) {
			T b = v->get();
			s.write((char *) & b, sizeof(T));
			return true;
		}
		return false;
	}

	template <typename T>
	bool st_load_bin(std::istream & s, TagSable * ts)
	{
		if (Var<T> * v = dynamic_cast<Var<T> *>(ts)) {
			T b;
			s.read((char *) & b, sizeof(T));
			v->set(b);
			return true;
		}
		return false;
	}

	// buffer for array data type
	template <typename A>
	class BfArr
	{
	protected:
		typename A::Type * bf;
		BfArr() {}
	public:
		char * buf() {return (char *) bf;}
	};

	template <typename A>
	class BfoArr :
		public BfArr<A>
	{
		size_t sz;
	public:
		BfoArr(const A & a)
		{
			sz = a.get_size();
			this->bf = new typename A::Type [sz];
			for (size_t i = 0; i < sz; i ++) this->bf[i] = a.get(i);
		}
		~BfoArr() {delete [] this->bf;}
		size_t bfsize() const {return sz * sizeof(typename A::Type);}
	};

	template <typename A>
	class BfiArr :
		public BfArr<A>
	{
		A & a;
		size_t sz;
	public:
		BfiArr(A & a, size_t sz) : a(a), sz(sz) {this->bf = new typename A::Type [sz];}
		~BfiArr() {delete [] this->bf;}
		void read()
		{
			a.set_size(sz);
			assert(a.get_size() == sz);
			if (! sz) return;
			for (size_t i = 0; i < sz; i ++) a.set(i, this->bf[i]);
		}
		size_t bfsize() const {return sz * sizeof(typename A::Type);}
	};


	// buffer for compounds
	template <>
	class BfArr<prm::Compound>
	{
		template <typename T>
		bool push_type(const TagSable * v)
		{
			if (dynamic_cast<const prm::Var<T> *>(v)) {
				es += sizeof(T);
				return true;
			}
			return false;
		}
	protected:
		char * bf;
		size_t es; // element size
	public:
		BfArr(const prm::Compound & c) :
			es(0)
		{
			std::vector<size_t> zs; // offsets
			std::vector<std::string const *> ns = c.get_names();
			for (std::vector<std::string const *>::iterator i = ns.begin(); i != ns.end(); i ++) {
				zs.push_back(es);
				std::unique_ptr<TagSable const> v(c.make_var(** i));
				if (push_type<bool>(v.get())) continue;
				if (push_type<int>(v.get())) continue;
				if (push_type<size_t>(v.get())) continue;
				if (push_type<double>(v.get())) continue;
			}
		}
		char * buf() {return bf;}
	};

	template <>
	class BfoArr<prm::Compound> :
		public BfArr<prm::Compound>
	{
		size_t sz;

		template <typename T>
		bool make_arr(const Array * elm, size_t & oft)
		{
			if (const Arr<T> * a = dynamic_cast<const Arr<T> *>(elm)) {
				for (size_t n = 0; n < sz; n ++) {
					T v = a->get(n);
					memcpy(bf + oft + n * es, & v, sizeof(T));
				}
				oft += sizeof(T);
				return true;
			}
			return false;
		}

		template <typename T>
		bool make_mem(const Struct::Member * mem, Strt ** st, size_t & oft)
		{
			if (const Struct::MemT<T> * m = dynamic_cast<const Struct::MemT<T> *>(mem)) {
				for (size_t n = 0; n < sz; n ++) {
					T v = m->get(st[n]);
					memcpy(bf + oft + n * es, & v, sizeof(T));
				}
				oft += sizeof(T);
				return true;
			}
			return false;
		}
	public:
		BfoArr(const prm::Compound & c) :
			BfArr<prm::Compound>(c)
		{
			sz = c.get_size();
			bf = new char [es * sz];
			// make buffer
			size_t oft = 0;
			for (std::vector<prm::Compound::Entry>::const_iterator i = c.list.begin(); i != c.list.end(); i ++) {
				if (make_arr<bool>(i->elm, oft)) continue;
				if (make_arr<int>(i->elm, oft)) continue;
				if (make_arr<size_t>(i->elm, oft)) continue;
				if (make_arr<double>(i->elm, oft)) continue;
				if (const Struct * s = dynamic_cast<const Struct *>(i->elm)) {
					Strt ** st = new Strt * [sz];
					for (size_t n = 0; n < sz; n ++) st[n] = s->res->get(n);
					for (std::vector<prm::Struct::Entry>::const_iterator j = s->list.begin(); j != s->list.end(); j ++) {
						if (make_mem<bool>(j->mem, st, oft)) continue;
						if (make_mem<int>(j->mem, st, oft)) continue;
						if (make_mem<size_t>(j->mem, st, oft)) continue;
						if (make_mem<double>(j->mem, st, oft)) continue;
						throw strm::TypeUnknownError("member of a struct");
					}
					for (size_t n = 0; n < sz; n ++) delete st[n];
					delete [] st;
					continue;
				}
				throw strm::TypeUnknownError("compound entry");
			}
		}
		~BfoArr()
		{
			delete [] bf;
		}
		size_t bfsize() const {return es * sz;}
	};

	template <>
	class BfiArr<prm::Compound> :
		public BfArr<prm::Compound>
	{
		prm::Compound & c;
		size_t sz;
		template <typename T>
		bool read_arr(Array * arr, size_t & oft)
		{
			if (Arr<T> * a = dynamic_cast<Arr<T> *>(arr)) {
				for (size_t n = 0; n < sz; n ++) {
					T v;
					memcpy(& v, bf + oft + n * es, sizeof(T));
					a->set(n, v);
				}
				oft += sizeof(T);
				return true;
			}
			return false;
		}
		template <typename T>
		bool read_mem(Struct::Member * mem, Strt ** st, size_t & oft)
		{
			if (Struct::MemT<T> * m = dynamic_cast<Struct::MemT<T> *>(mem)) {
				for (size_t n = 0; n < sz; n ++) {
					T v;
					memcpy(& v, bf + oft + n * es, sizeof(T));
					m->set(st[n], v);
				}
				oft += sizeof(T);
				return true;
			}
			return false;
		}
	public:
		BfiArr(prm::Compound & cp, size_t s) :
			BfArr<prm::Compound>(cp),
			c(cp),
			sz(s)
		{
			bf = new char [sz * es];
			memset(bf, 0, sz * es); // in case of missing members, this defaults the values to zero
		}
 		~BfiArr() {delete [] bf;}
		void read()
		{
			c.set_size(sz);
			assert(c.get_size() == sz);
			if (! sz) return;
			size_t oft = 0;
			for (std::vector<prm::Compound::Entry>::iterator i = c.list.begin(); i != c.list.end(); i ++) {
				if (read_arr<bool>(i->elm, oft)) continue;
				if (read_arr<int>(i->elm, oft)) continue;
				if (read_arr<size_t>(i->elm, oft)) continue;
				if (read_arr<double>(i->elm, oft)) continue;
				if (Struct * s = dynamic_cast<Struct *>(i->elm)) {
					Strt ** st = new Strt * [sz];
					for (size_t n = 0; n < sz; n ++) st[n] = s->res->get(n);
					for (std::vector<prm::Struct::Entry>::iterator j = s->list.begin(); j != s->list.end(); j ++) {
						if (read_mem<bool>(j->mem, st, oft)) continue;
						if (read_mem<int>(j->mem, st, oft)) continue;
						if (read_mem<size_t>(j->mem, st, oft)) continue;
						if (read_mem<double>(j->mem, st, oft)) continue;
						throw strm::TypeUnknownError("member of a struct");
					}
					for (size_t n = 0; n < sz; n ++) delete st[n];
					delete [] st;
					continue;
				}
				throw strm::TypeUnknownError("compound entry");
			}
		}
		size_t bfsize() const {return es * sz;}
	};

	// Try saving the array if its of type A, else return false
	template <typename A>
	bool st_save_array(std::ostream & s, const Array & array)
	{
		if (const A * a = dynamic_cast<const A *>(& array)) {
			BfoArr<A> ha(* a);
			s.write(ha.buf(), ha.bfsize());
			return true;
		}
		return false;
	}

	template <typename A>
	bool st_load_array(std::istream & s, Array & array, size_t sz)
	{
		if (A * a = dynamic_cast<A *>(& array)) {
			BfiArr<A> ha(* a, sz);
			s.read(ha.buf(), ha.bfsize());
			ha.read();
			return true;
		}
		return false;
	}
}

prm::strm::TypeUnknownError::TypeUnknownError(const std::string & i)
{
	info = i;
}

std::string prm::strm::TypeUnknownError::get_info()
{
	return info;
}

prm::strm::DataMissingError::DataMissingError(const std::string & n)
{
	name = n;
}

std::string prm::strm::DataMissingError::get_name()
{
	return name;
}

void prm::st_save(std::ostream & s, const Param & p)
{
	std::vector<std::string const *> nl = p.get_names();
	for (std::vector<std::string const *>::iterator i = nl.begin(); i != nl.end(); i ++) {
		const std::string & n = ** i;
		const TagSable * ts = & p.get_var(n);
		if (! ts->find<tag::Save>()) continue;
		unsigned l = n.size();
		s.write((char *) & l, sizeof(unsigned));
		s.write(& n[0], l);
		if (st_save_bin<bool>(s, ts)) continue;
		if (st_save_bin<int>(s, ts)) continue;
		if (st_save_bin<size_t>(s, ts)) continue;
		if (st_save_bin<double>(s, ts)) continue;
		throw strm::TypeUnknownError("for " + n);
	}
	unsigned zero = 0;
	s.write((char *) & zero, sizeof(unsigned));
}

size_t prm::st_load(std::istream & s, Param & p)
{
	size_t cnt = 0;
	std::vector<std::string const *> nl = p.get_names();
	std::set<std::string> nms;
	for (std::vector<std::string const *>::iterator n = nl.begin(); n != nl.end(); n ++) nms.insert(** n);

	while (true) {
		unsigned l;
		s.read((char *) & l, sizeof(unsigned));
		if (! l) break;
		std::string n(l, ' ');
		s.read(& n[0], l);
		if (nms.find(n) == nms.end()) continue;
		TagSable * ts = & p.get_var(n);
		if (! ts->find<tag::Save>()) continue;
		cnt ++;
		if (st_load_bin<bool>(s, ts)) continue;
		if (st_load_bin<int>(s, ts)) continue;
		if (st_load_bin<size_t>(s, ts)) continue;
		if (st_load_bin<double>(s, ts)) continue;
		throw strm::TypeUnknownError("for " + n);
	}
	return cnt;
}

void prm::st_save(std::ostream & s, const Array & a, const std::string & n)
{
	unsigned l = n.size();
	s.write((char *) & l, sizeof(unsigned));
	s.write(& n[0], l);
	size_t sz = a.get_size();
	s.write((char *) & sz, sizeof(size_t));
	if (st_save_array<Compound>(s, a)) return;
	if (st_save_array<Arr<bool> >(s, a)) return;
	if (st_save_array<Arr<int> >(s, a)) return;
	if (st_save_array<Arr<size_t> >(s, a)) return;
	if (st_save_array<Arr<double> >(s, a)) return;
	throw strm::TypeUnknownError("for Array " + n);
}

bool prm::st_load(std::istream & s, Array & a, const std::string & n)
{
	unsigned l;
	s.read((char *) & l, sizeof(unsigned));
	std::string nn(l, ' ');
	s.read(& nn[0], l);
	assert(nn == n);
	size_t sz;
	s.read((char *) & sz, sizeof(size_t));
	if (st_load_array<Compound>(s, a, sz)) return true;
	if (st_load_array<Arr<bool> >(s, a, sz)) return true;
	if (st_load_array<Arr<int> >(s, a, sz)) return true;
	if (st_load_array<Arr<size_t> >(s, a, sz)) return true;
	if (st_load_array<Arr<double> >(s, a, sz)) return true;
	throw strm::TypeUnknownError("for Array " + n);
	return false;
}
