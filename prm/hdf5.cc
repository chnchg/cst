/* prm/hdf5.cc
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
#include "hdf5.hh"
#include <cstring>
#include <cassert>
using namespace prm;

namespace { // no need to export all these implementation
	// Type conversion template
	template <typename T> struct h5t {typedef T Type;}; // same type by default
	// Special types
	template <> struct h5t<bool> {typedef hbool_t Type;};
	template <> struct h5t<size_t> {typedef hsize_t Type;};

	// ids for HDF5 native types
	template <typename T> hid_t h5_nat() {return - 1;}
	template <> hid_t h5_nat<bool>() {return H5T_NATIVE_HBOOL;}
	template <> hid_t h5_nat<int>() {return H5T_NATIVE_INT;}
	template <> hid_t h5_nat<unsigned>() {return H5T_NATIVE_UINT;}
	template <> hid_t h5_nat<size_t>() {return H5T_NATIVE_HSIZE;}
	template <> hid_t h5_nat<double>() {return H5T_NATIVE_DOUBLE;}

	// string
	hid_t _h5t_std_string = - 1;
	template <> hid_t h5_nat<std::string>()
	{
		if (_h5t_std_string < 0) {
			_h5t_std_string = H5Tcopy(H5T_C_S1); // Where can I clean this up?
			H5Tset_size(_h5t_std_string, H5T_VARIABLE);
		}
		return _h5t_std_string;
	}

	// saving with native types
	template <typename T>
	bool h5_save_nat(hid_t group, std::shared_ptr<Sable const> s, std::string const & name)
	{
		if (auto v = std::dynamic_pointer_cast<Var<T> const>(s)) {
			typename h5t<T>::Type b;
			b = v->get();
			hid_t space = H5Screate(H5S_SCALAR);
			hid_t data = H5Dcreate(group, name.c_str(), h5_nat<T>(), space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			H5Dwrite(data, h5_nat<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, & b);
			H5Dclose(data);
			H5Sclose(space);
			return true;
		}
		return false;
	}

	template <typename T>
	bool h5_load_nat(hid_t group, std::shared_ptr<Sable> s, const std::string & name, size_t * cnt = 0)
	{
		if (auto v = std::dynamic_pointer_cast<Var<T>>(s)) {
			if (! H5Lexists(group, name.c_str(), H5P_DEFAULT)) return true;
			hid_t data = H5Dopen(group, name.c_str(), H5P_DEFAULT);
			hid_t space = H5Dget_space(data);
			if (H5Sget_simple_extent_ndims(space) != 0) { // need to be a scalar
				H5Sclose(space);
				H5Dclose(data);
				return true;
			}
			typename h5t<T>::Type b;
			H5Dread(data, h5_nat<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, & b);
			v->set(b);
			if (cnt) (* cnt) ++;
			H5Sclose(space);
			H5Dclose(data);
			return true;
		}
		return false;
	}

	// specialize for std::string
	template <>
	bool h5_save_nat<std::string>(hid_t group, std::shared_ptr<Sable const> s, const std::string & name)
	{
		if (auto v = std::dynamic_pointer_cast<Var<std::string> const>(s)) {
			std::string b;
			b = v->get();
			hid_t space = H5Screate(H5S_SCALAR);
			hid_t data = H5Dcreate(group, name.c_str(), h5_nat<std::string>(), space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			char const * t = b.c_str();
			H5Dwrite(data, h5_nat<std::string>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, & t);
			H5Dclose(data);
			H5Sclose(space);
			return true;
		}
		return false;
	}

	template <>
	bool h5_load_nat<std::string>(hid_t group, std::shared_ptr<Sable> s, const std::string & name, size_t * cnt)
	{
		if (auto v = std::dynamic_pointer_cast<Var<std::string>>(s)) {
			if (! H5Lexists(group, name.c_str(), H5P_DEFAULT)) return true;
			hid_t data = H5Dopen(group, name.c_str(), H5P_DEFAULT);
			hid_t space = H5Dget_space(data);
			if (H5Sget_simple_extent_ndims(space) != 0) { // need to be a scalar
				H5Sclose(space);
				H5Dclose(data);
				return true;
			}
			char * t;
			H5Dread(data, h5_nat<std::string>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, & t);
			v->set(t);
			H5Dvlen_reclaim(h5_nat<std::string>(), space, H5P_DEFAULT, & t);
			if (cnt) (* cnt) ++;
			H5Sclose(space);
			H5Dclose(data);
			return true;
		}
		return false;
	}

	// specialize for Sable
	template <>
	bool h5_save_nat<Sable>(hid_t group, std::shared_ptr<Sable const> s, std::string const & name)
	{
		std::string b = s->to_str();
		hid_t space = H5Screate(H5S_SCALAR);
		hid_t data = H5Dcreate(group, name.c_str(), h5_nat<std::string>(), space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		char const * t = b.c_str();
		H5Dwrite(data, h5_nat<std::string>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, & t);
		H5Dclose(data);
		H5Sclose(space);
		return true;
	}

	template <>
	bool h5_load_nat<Sable>(hid_t group, std::shared_ptr<Sable> s, const std::string & name, size_t * cnt)
	{
		if (! H5Lexists(group, name.c_str(), H5P_DEFAULT)) return true;
		hid_t data = H5Dopen(group, name.c_str(), H5P_DEFAULT);
		hid_t space = H5Dget_space(data);
		if (H5Sget_simple_extent_ndims(space) != 0) { // need to be a scalar
			H5Sclose(space);
			H5Dclose(data);
			return true;
		}
		char * t;
		H5Dread(data, h5_nat<std::string>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, & t);
		if (! s->test_str(t)) return true;
		s->read_str(t);
		H5Dvlen_reclaim(h5_nat<std::string>(), space, H5P_DEFAULT, & t);
		if (cnt) (*cnt) ++;
		H5Sclose(space);
		H5Dclose(data);
		return true;
	}

	// Buffer for array data type
	template <typename A>
	class H5Arr
	{
	protected:
		typedef typename A::Type Type;
		typedef typename h5t<Type>::Type HType;
		HType * bf;
		H5Arr() {}
	public:
		static hid_t type() {return h5_nat<Type>();}
		void * buf() {return bf;}
	};

	// Output buffer for array data type
	template <typename A>
	class H5oArr :
		public H5Arr<A>
	{
		hid_t spc;
	public:
		H5oArr(A const & a)
		{
			hsize_t sz = a.get_size();
			this->bf = new typename H5Arr<A>::HType [sz];
			for (size_t i = 0; i < sz; i ++) this->bf[i] = a.get(i);
			spc = H5Screate_simple(1, & sz, 0);
		}
		~H5oArr()
		{
			H5Sclose(spc);
			delete [] this->bf;
		}
		hid_t space() {return spc;}
	};

	// Input buffer for array data type
	template <typename A>
	class H5iArr :
		public H5Arr<A>
	{
		A & a;
		size_t sz;
	public:
		H5iArr(A & ar, size_t s) : a(ar), sz(s) {this->bf = new typename H5Arr<A>::HType [sz];}
		~H5iArr() {delete [] this->bf;}
		void read()
		{
			a.set_size(sz);
			assert(a.get_size() == sz);
			if (! sz) return;
			for (size_t i = 0; i < sz; i ++) a.set(i, this->bf[i]);
		}
	};

	// Buffer for compounds
	template <>
	class H5Arr<prm::Compound>
	{
		// adding member type for constuction 
		template <typename T>
		bool push_type(std::shared_ptr<Sable const> v, std::vector<hid_t> & ts)
		{
			if (std::dynamic_pointer_cast<prm::Var<T> const>(v)) {
				es += sizeof(typename h5t<T>::Type);
				ts.push_back(h5_nat<T>());
				return true;
			}
			return false;
		}
	protected:
		char * bf;
		hid_t htype;
		size_t es; // element size
	public:
		H5Arr(prm::Compound const & c) :
			es(0)
		{
			auto ns = c.get_names(); // all names in compound
			std::vector<size_t> zs; // offsets
			std::vector<hid_t> ts;  // H5 types
			size_t nn = ns.size();
			for (size_t i = 0; i < nn; i ++) {
				zs.push_back(es);
				auto v = c.make_var(ns[i]);
				if (push_type<bool>(v, ts)) continue;
				if (push_type<int>(v, ts)) continue;
				if (push_type<unsigned>(v, ts)) continue;
				if (push_type<size_t>(v, ts)) continue;
				if (push_type<double>(v, ts)) continue;
			}
			htype = H5Tcreate(H5T_COMPOUND, es);
			for (size_t i = 0; i < nn; i ++) H5Tinsert(htype, ns[i].c_str(), zs[i], ts[i]);
		}
		~H5Arr() {H5Tclose(htype);}
		void * buf() {return bf;}
		hid_t type() {return htype;}
	};

	// Output buffer for compound
	template <>
	class H5oArr<prm::Compound> :
		public H5Arr<prm::Compound>
	{
		size_t sz;
		hid_t spc;

		template <typename T>
		bool make_arr(std::shared_ptr<Array const> elm, size_t & oft)
		{
			if (auto a = std::dynamic_pointer_cast<Arr<T> const>(elm)) {
				for (size_t n = 0; n < sz; n ++) {
					typename h5t<T>::Type v = a->get(n);
					memcpy(bf + oft + n * es, & v, sizeof(v));
				}
				oft += sizeof(typename h5t<T>::Type);
				return true;
			}
			return false;
		}

		template <typename T>
		bool make_mem(std::shared_ptr<Struct::Member const> mem, std::shared_ptr<Strt> * st, size_t & oft)
		{
			if (auto m = std::dynamic_pointer_cast<Struct::MemT<T> const>(mem)) {
				for (size_t n = 0; n < sz; n ++) {
					typename h5t<T>::Type v = m->get(st[n]);
					memcpy(bf + oft + n * es, & v, sizeof(v));
				}
				oft += sizeof(typename h5t<T>::Type);
				return true;
			}
			return false;
		}
	public:
		H5oArr(prm::Compound const & c) :
			H5Arr<prm::Compound>(c)
		{
			sz = c.get_size();
			bf = new char [es * sz];
			// make buffer
			size_t oft = 0;
			for (auto i: c.list) {
				if (make_arr<bool>(i.elm, oft)) continue;
				if (make_arr<int>(i.elm, oft)) continue;
				if (make_arr<unsigned>(i.elm, oft)) continue;
				if (make_arr<size_t>(i.elm, oft)) continue;
				if (make_arr<double>(i.elm, oft)) continue;
				if (auto s = std::dynamic_pointer_cast<Struct const>(i.elm)) {
					std::shared_ptr<Strt> * st = new std::shared_ptr<Strt> [sz];
					for (size_t n = 0; n < sz; n ++) st[n] = s->res->get(n);
					for (auto & j: s->list) {
						if (make_mem<bool>(j.mem, st, oft)) continue;
						if (make_mem<int>(j.mem, st, oft)) continue;
						if (make_mem<unsigned>(j.mem, st, oft)) continue;
						if (make_mem<size_t>(j.mem, st, oft)) continue;
						if (make_mem<double>(j.mem, st, oft)) continue;
						throw TypeUnknownError("member of a struct");
					}
					delete [] st;
					continue;
				}
				throw TypeUnknownError("compound entry");
			}
			hsize_t hsz = sz;
			spc = H5Screate_simple(1, & hsz, 0);
		}
		~H5oArr()
		{
			H5Sclose(spc);
			delete [] bf;
		}
		hid_t space() {return spc;}
	};

	template <>
	class H5iArr<prm::Compound> :
		public H5Arr<prm::Compound>
	{
		prm::Compound & c;
		size_t sz;
		template <typename T>
		bool read_arr(std::shared_ptr<Array> arr, size_t & oft)
		{
			if (auto a = std::dynamic_pointer_cast<Arr<T>>(arr)) {
				for (size_t n = 0; n < sz; n ++) {
					typename h5t<T>::Type v;
					memcpy(& v, bf + oft + n * es, sizeof(v));
					a->set(n, v);
				}
				oft += sizeof(typename h5t<T>::Type);
				return true;
			}
			return false;
		}
		template <typename T>
		bool read_mem(std::shared_ptr<Struct::Member> mem, std::shared_ptr<Strt> * st, size_t & oft)
		{
			if (auto m = std::dynamic_pointer_cast<Struct::MemT<T>>(mem)) {
				for (size_t n = 0; n < sz; n ++) {
					typename h5t<T>::Type v;
					memcpy(& v, bf + oft + n * es, sizeof(v));
					m->set(st[n], v);
				}
				oft += sizeof(typename h5t<T>::Type);
				return true;
			}
			return false;
		}
	public:
		H5iArr(prm::Compound & cp, size_t s) :
			H5Arr<prm::Compound>(cp),
			c(cp),
			sz(s)
		{
			bf = new char [sz * es];
			memset(bf, 0, sz * es); // in case of missing members, this defaults the values to zero
		}
 		~H5iArr() {delete [] bf;}
		void read()
		{
			c.set_size(sz);
			assert(c.get_size() == sz);
			if (! sz) return;
			size_t oft = 0;
			for (auto & i: c.list) {
				if (read_arr<bool>(i.elm, oft)) continue;
				if (read_arr<int>(i.elm, oft)) continue;
				if (read_arr<unsigned>(i.elm, oft)) continue;
				if (read_arr<size_t>(i.elm, oft)) continue;
				if (read_arr<double>(i.elm, oft)) continue;
				if (auto s = std::dynamic_pointer_cast<Struct>(i.elm)) {
					std::shared_ptr<Strt> * st = new std::shared_ptr<Strt> [sz];
					for (size_t n = 0; n < sz; n ++) st[n] = s->res->get(n);
					for (auto & j: s->list) {
						if (read_mem<bool>(j.mem, st, oft)) continue;
						if (read_mem<int>(j.mem, st, oft)) continue;
						if (read_mem<unsigned>(j.mem, st, oft)) continue;
						if (read_mem<size_t>(j.mem, st, oft)) continue;
						if (read_mem<double>(j.mem, st, oft)) continue;
						throw TypeUnknownError("member of a struct");
					}
					delete [] st;
					continue;
				}
				throw TypeUnknownError("compound entry");
			}
		}
	};

	// Try saving the array if its of type A, else return false
	template <typename A>
	bool h5_save_array(hid_t group, const Array & array, const std::string & name)
	{
		if (const A * a = dynamic_cast<const A *>(& array)) {
			H5oArr<A> ha(* a);
			hid_t data = H5Dcreate(group, name.c_str(), ha.type(), ha.space(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			H5Dwrite(data, ha.type(), H5S_ALL, H5S_ALL, H5P_DEFAULT, ha.buf());
			H5Dclose(data);
			return true;
		}
		return false;
	}

	template <typename A>
	bool h5_load_array(hid_t group, Array & array, const std::string & name)
	{
		if (A * a = dynamic_cast<A *>(& array)) {
			if (! H5Lexists(group, name.c_str(), H5P_DEFAULT)) throw DataMissingError(name);
			hid_t data = H5Dopen(group, name.c_str(), H5P_DEFAULT);
			hid_t space = H5Dget_space(data);
			if (H5Sget_simple_extent_ndims(space) != 1) { // need to be an array
				H5Sclose(space);
				H5Dclose(data);
				return true;
			}
			hsize_t sz;
			H5Sget_simple_extent_dims(space, & sz, 0);
			H5Sclose(space);
			H5iArr<A> ha(* a, sz);
			H5Dread(data, ha.type(), H5S_ALL, H5S_ALL, H5P_DEFAULT, ha.buf());
			ha.read();
			H5Dclose(data);
			return true;
		}
		return false;
	}
}

prm::TypeUnknownError::TypeUnknownError(const std::string & i)
{
	info = i;
}

std::string prm::TypeUnknownError::get_info()
{
	return info;
}

prm::DataMissingError::DataMissingError(const std::string & n)
{
	name = n;
}

std::string prm::DataMissingError::get_name()
{
	return name;
}

void prm::h5_save(hid_t group, const Param & p, const std::string & n)
{
	hid_t g = n.size() ? H5Gcreate(group, n.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT) : group;
	for (auto & i: p.vars) {
		if (! i.tags.find<tag::Save>()) continue;
		if (h5_save_nat<bool>(g, i.var, i.name)) continue;
		if (h5_save_nat<int>(g, i.var, i.name)) continue;
		if (h5_save_nat<unsigned>(g, i.var, i.name)) continue;
		if (h5_save_nat<size_t>(g, i.var, i.name)) continue;
		if (h5_save_nat<double>(g, i.var, i.name)) continue;
		if (h5_save_nat<std::string>(g, i.var, i.name)) continue;
		h5_save_nat<Sable>(g, i.var, i.name); // final resort
	}
	if (g != group) H5Gclose(g);
}

size_t prm::h5_load(hid_t group, Param & p, const std::string & n)
{
	hid_t g = n.size() ? H5Gopen(group, n.c_str(), H5P_DEFAULT) : group;
	size_t cnt = 0;
	for (auto i: p.vars) {
		if (! i.tags.find<tag::Save>()) continue;
		if (h5_load_nat<bool>(g, i.var, i.name, & cnt)) continue;
		if (h5_load_nat<int>(g, i.var, i.name, & cnt)) continue;
		if (h5_load_nat<unsigned>(g, i.var, i.name, & cnt)) continue;
		if (h5_load_nat<size_t>(g, i.var, i.name, & cnt)) continue;
		if (h5_load_nat<double>(g, i.var, i.name, & cnt)) continue;
		if (h5_load_nat<std::string>(g, i.var, i.name, & cnt)) continue;
		h5_load_nat<Sable>(g, i.var, i.name, & cnt); // final resort
	}
	if (g != group) H5Gclose(g);
	return cnt;
}

void prm::h5_save(hid_t g, const Array & a, const std::string & n)
{
	if (h5_save_array<Compound>(g, a, n)) return;
	if (h5_save_array<Arr<bool> >(g, a, n)) return;
	if (h5_save_array<Arr<int> >(g, a, n)) return;
	if (h5_save_array<Arr<unsigned> >(g, a, n)) return;
	if (h5_save_array<Arr<size_t> >(g, a, n)) return;
	if (h5_save_array<Arr<double> >(g, a, n)) return;
	throw TypeUnknownError("for Array " + n);
}

bool prm::h5_load(hid_t g, Array & a, const std::string & n)
{
	if (h5_load_array<Compound>(g, a, n)) return true;
	if (h5_load_array<Arr<bool> >(g, a, n)) return true;
	if (h5_load_array<Arr<int> >(g, a, n)) return true;
	if (h5_load_array<Arr<unsigned> >(g, a, n)) return true;
	if (h5_load_array<Arr<size_t> >(g, a, n)) return true;
	if (h5_load_array<Arr<double> >(g, a, n)) return true;
	throw TypeUnknownError("for Array " + n);
	return false;
}
