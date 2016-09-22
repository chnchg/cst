/* prm/prm.hh
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
 *  Parameter utilities
 *
 *    Var/Parameter should be tagged.
 */
#ifndef PRM_HH
#define PRM_HH 1
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#if __cplusplus < 201103L
#define override
#define final
#endif
namespace prm {
	namespace tag {
		class Base
		{
		public:
			virtual ~Base() {}
			virtual Base * dup() const {return new Base;}
		};

		class Text :
			virtual public Base
		{
			Base * dup() const override {return new Text(text);}
		public:
			const std::string text;
			Text(const std::string & text) : text(text) {}
		};

		class Name : // a readable name
			virtual public Text
		{
			Base * dup() const override {return new Name(text);}
		public:
			Name(const std::string & name) : Text(name) {}
		};

		class Desc : // description
			virtual public Text
		{
			Base * dup() const override {return new Desc(text);}
		public:
			Desc(const std::string & desc) : Text(desc) {}
		};

		class Word : // an expressive word to short for
			virtual public Text
		{
			Base * dup() const override {return new Word(text);}
		public:
			Word(const std::string & word) : Text(word) {}
		};

		class Range :
			virtual public Base
		{
			Base * dup() const override {return new Range(min, max);}
		public:
			const double min;
			const double max;
			Range(double min, double max) : min(min), max(max) {}
		};

		class File :
			virtual public Base
		{
			Base * dup() const override {return new File;}
		};

		class Save : // to be saved
			virtual public Base
		{
			Base * dup() const override {return new Save;}
		};

		class CmdLine : // should generate command line parameter
			virtual public Base
		{
			Base * dup() const override {return new CmdLine;}
		};
	}

	class Taggable
	{
		std::vector<tag::Base *> tag_list;
	protected:
		void copy_tags(const Taggable & tgb); // deep copy
	public:
		virtual ~Taggable();
		template <typename TG>
		Taggable & operator << (const TG & tg)
		{
			tag_list.push_back(new TG(tg));
			return * this;
		}
		template <typename TG>
		const TG * find() const
		{
			const TG * t = 0;
			for (std::vector<tag::Base *>::const_iterator i = tag_list.begin(); i != tag_list.end(); i ++) {
				t = dynamic_cast<const TG *>(* i);
				if (t) break;
			}
			return t;
		}
	};

	// Streamable (can be serialized)
	class Sable
	{
	public:
		virtual ~Sable() {}
		virtual std::string to_str() const {return "";} // convert to string
		virtual std::string a_str() const {return "";}  // a default string
		virtual bool read_str(const std::string & s) {return s == "";} // convert from string
		virtual bool test_str(const std::string & s) const {return s == "";} // check string
	};

	class TagSable :
		virtual public Taggable,
		virtual public Sable
	{
	public:
		virtual void copy_value(TagSable const & /*ts*/) {}
	};

	// single value
	template <typename T>
	class Var :
		virtual public TagSable
	{
		std::string to_str() const override
		{
			std::ostringstream s;
			s.precision(16);
			s << get();
			return s.str();
		}
		std::string a_str() const override
		{
			std::ostringstream s;
			s << T();
			return s.str();
		}
		bool read_str(const std::string & str) override
		{
			std::istringstream s(str);
			T v;
			s >> v;
			if (s.fail() || ! s.eof()) return false;
			set(v);
			return true;
		}
		bool test_str(const std::string & str) const override
		{
			std::istringstream s(str);
			T v;
			s >> v;
			if (s.fail() || ! s.eof()) return false;
			return true;
		}
		void copy_value(TagSable const & ts) override
		{
			if (Var<T> const * v = dynamic_cast<Var<T> const *>(& ts)) set(v->get());
		}
	public:
		typedef T VarType;
		virtual void set(T v) = 0;
		virtual T get() const = 0;
	};
	// addressed value, this is terminal variable
	template <typename T>
	class Adr :
		virtual public Var<T>
	{
		T & va;
	public:
		Adr(T & v) :
			va(v)
		{
		}

		void set(T v) override
		{
			va = v;
		}

		T get() const override
		{
			return va;
		}
	};
	// value with member function access
	template <typename Cls, typename Type>
	class Fun :
		virtual public Var<Type>
	{
		Cls & obj;
		void (Cls::* set_f)(Type);
		Type (Cls::* get_f)() const;
	public:
		Fun(Cls & p, void (Cls::* sf)(Type), Type (Cls::* gf)() const) :
			obj(p),
			set_f(sf),
			get_f(gf)
		{}

		void set(Type val) override
		{
			(obj.* set_f)(val);
		}

		Type get() const override
		{
			return (obj.* get_f)();
		}
	};
	class Index
	{
	public:
		virtual ~Index() {}
		virtual size_t get() const = 0;
	};

	class IndexConst :
		virtual public Index
	{
		size_t idx;
		size_t get() const override {return idx;}
	public:
		IndexConst(size_t i) :
			idx(i)
		{}
	};
	class IndexRef :
		virtual public Index
	{
		size_t & idx;
		size_t get() const override {return idx;}
	public:
		IndexRef(size_t & i) :
			idx(i)
		{}
	};
	template <typename T>
	class IndexMemFun :
		virtual public Index
	{
		T & obj;
		size_t (T::* ptr)() const;
		size_t get() const override
		{
			return (obj.* ptr)();
		}
	public:
		IndexMemFun(T & o, size_t (T::* p)() const) :
			obj(o),
			ptr(p)
		{}
	};
	// method to get and set the size of arrays
	class Sizer
	{
	public:
		virtual ~Sizer() {}
		virtual void set_size(size_t /* new_size */) {}
		virtual size_t get_size() const {return 0;}
	};

	// read-only sizer referencing a size_t variable
	class SzrRef :
		public Sizer
	{
		size_t & sz;
		void set_size(size_t new_size) override
		{
			if (new_size != sz) throw;
		}
		size_t get_size() const override {return sz;}
	public:
		SzrRef(size_t & s) :
			sz(s)
		{}
	};

	// using member functions to get/set size
	template <typename Cls>
	class SzrFun :
		public Sizer
	{
		Cls & obj;
		void (Cls::* ss_f)(size_t);
		size_t (Cls::* gs_f)() const;

		void set_size(size_t new_size) override
		{
			if (! ss_f) {
				if (gs_f && (obj.* gs_f)() != new_size) throw;
				return;
			}
			(obj.* ss_f)(new_size);
		}

		size_t get_size() const override
		{
			if (! gs_f) return 0;
			return (obj.* gs_f)();
		}
	public:
		SzrFun(Cls & c, void (Cls::* sf)(size_t), size_t (Cls::* gf)() const) :
			obj(c),
			ss_f(sf),
			gs_f(gf)
		{}
	};

	// sizer for std::vector
	template <typename S>
	class SzrVec :
		public Sizer
	{
		std::vector<S> & v;
		void set_size(size_t new_size) override
		{
			v.resize(new_size);
		}
		size_t get_size() const override
		{
			return v.size();
		}
	public:
		SzrVec(std::vector<S> & v) : v(v) {}
	};
	// Array
	class Array :
		virtual public Taggable
	{
		Sizer * szr;
	public:
		Array() : szr(0) {}
		virtual ~Array() {delete szr;}

		virtual void set_sizer(Sizer * sizer) {delete szr; szr = sizer;}
		void set_sizer(size_t & sz) {delete szr; szr = new SzrRef(sz);}
		template <typename S> void set_sizer(std::vector<S> & v) {delete szr; szr = new SzrVec<S>(v);}
		virtual void set_size(size_t new_size) {if (szr) szr->set_size(new_size);}
		virtual size_t get_size() const {return szr ? szr->get_size() : 0;}
		virtual TagSable * make_var(Index * /*idx*/ = 0) {return 0;}
	};

	// Array of values
	template <typename T>
	class Arr :
		virtual public Array
	{
		virtual TagSable * make_var(Index * idx);
	public:
		typedef T Type;
		virtual void set(size_t i, T val) = 0;
		virtual T get(size_t i) const = 0;
	};
	// linear array
	template <typename Type>
	class ArrLin :
		virtual public Arr<Type>
	{
		Type * & arr;

		void set(size_t i, Type val) override
		{
			arr[i] = val;
		}

		Type get(size_t i) const override
		{
			return arr[i];
		}
	public:
		ArrLin(Type * & arr_addr) :
			arr(arr_addr)
		{}
	};
	// array with functional access
	template <typename C, typename T>
	class ArrFun :
		virtual public Arr<T>
	{
		C & obj;
		void (C::* set_f)(size_t, T);
		T (C::* get_f)(size_t) const;

		void set(size_t i, T val) override
		{
			(obj.* set_f)(i, val);
		}

		T get(size_t i) const override
		{
			return (obj.* get_f)(i);
		}
	public:
		ArrFun(C & c, void (C::* sf)(size_t, T), T (C::* gf)(size_t) const) :
			obj(c),
			set_f(sf),
			get_f(gf)
		{}

		~ArrFun()
		{
		}

		void set_sizer(void (C::* sf)(size_t), size_t (C::* gf)() const) override
		{
			Arr<T>::set_sizer(new SzrFun<C>(obj, sf, gf));
		}
	};
	// array that is a vector
	template <typename T>
	class ArrVec :
		virtual public Arr<T>
	{
		std::vector<T> & vec;
		void set(size_t i, T val) override
		{
			vec[i] = val;
		}
		T get(size_t i) const override
		{
			return vec[i];
		}
	public:
		ArrVec(std::vector<T> & v) :
			vec(v)
		{
			Array::set_sizer(v);
		}
	};
	// some type or struct
	struct Strt
	{
		virtual ~Strt() {}
	};
	// the actual type
	template <typename S>
	struct Stt :
		public Strt
	{
		S & s;
		Stt(S & ns) :
			s(ns)
		{}
	};
	// resolver for certain type or struct
	class Res
	{
	public:
		virtual ~Res() {}
		virtual Strt * get(size_t) = 0;
	};
	// resolver for a C array
	template <typename S>
	class ARes :
		public Res
	{
		S * & p;
		Strt * get(size_t i) {return new Stt<S>(p[i]);}
	public:
		ARes(S * & p) : p(p) {}
	};
	// resolver implemented with member functions
	template <typename C>
	class CRes :
		public Res
	{
		C & cls;
		Strt * (C::* ptr)(size_t);

		Strt * get(size_t i)
		{
			return (cls.* ptr)(i);
		}
	public:
		CRes(C & c, Strt * (C::* p)(size_t)) :
			cls(c),
			ptr(p)
		{}
	};
	// resolver for a std::vector
	template <typename S>
	class VRes :
		public Res
	{
		std::vector<S> & v;
		Strt * get(size_t i) {return new Stt<S>(v[i]);}
	public:
		VRes(std::vector<S> & v) : v(v) {}
	};
	// array with resolver access
	template <typename T>
	class ArrRes :
		virtual public Arr<T>
	{
		Res * res;

		void set(size_t i, T val)
		{
			if (! res) return;
			Strt * st = res->get(i);
			dynamic_cast<Stt<T> *>(st)->s = val;
			delete st;
		}

		T get(size_t i) const
		{
			if (! res) return T();
			std::unique_ptr<Strt> st(res->get(i));
			return dynamic_cast<const Stt<T> *>(st.get())->s;
		}
	public:
		ArrRes() :
			res(0)
		{}

		~ArrRes()
		{
			delete res;
		}

		void set_resolver(Res * re)
		{
			delete res;
			res = re;
		}
	};
	// indexed item in an array
	template <typename T>
	class Idx :
		virtual public Var<T>
	{
		Arr<T> & arr;
		Index * idx;

		void set(T v)
		{
			size_t i = idx ? idx->get() : 0;
			arr.set(i, v);
		}

		T get() const
		{
			size_t i = idx ? idx->get() : 0;
			return arr.get(i);
		}
	public:
		Idx(Arr<T> & a, Index * i = 0) :
			arr(a),
			idx(i)
		{
			this->copy_tags(a);
		}

		virtual ~Idx()
		{
			delete idx;
		}
	};
	struct Param
	{
		struct VarEntry
		{
			std::string name;
			TagSable * var;
			VarEntry(const std::string & n, TagSable * v) :
				name(n),
				var(v)
			{}
		};
		std::vector<VarEntry> vars;
		~Param();
		TagSable & add_var(const std::string & name, TagSable * v); // * v is taken over
		// overloaded add_var
		template <typename Type>
		TagSable & add_var(const std::string & name, Type & v)
		{
			return add_var(name, new Adr<Type>(v));
		}
		// access
		std::vector<const std::string *> get_names() const;
		const TagSable & get_var(const std::string & name) const;
		TagSable & get_var(const std::string & name);
		void delete_var(const std::string & name);
		// conversion to/from text stream
		void read(std::istream & input);
		void write(std::ostream & output) const;
		// copy values from another Param
		void copy(Param const & p);
	};
	struct Struct :
		virtual public Array
	{
		class Member :
			virtual public Taggable
		{
		public:
			virtual ~Member() {}
			virtual TagSable * make_var(Res & /* res */, Index * /* idx */ = 0) {return 0;}
			virtual const TagSable * make_var(Res & /* res */, Index * /* idx */ = 0) const {return 0;}
		};
		// member with specific type
		template <typename T>
		class MemT :
			virtual public Member
		{
			TagSable * make_var(Res & res, Index * idx);
			const TagSable * make_var(Res & res, Index * idx) const;
		public:
			typedef T Type;
			virtual void set(Strt * s, T v) = 0;
			virtual T get(const Strt * s) const = 0;
		};
		template <typename S, typename T>
		class Mem :
			virtual public MemT<T> // access through member pointer
		{
			T S::* ptr;

			void set(Strt * s, T v)
			{
				dynamic_cast<Stt<S> *>(s)->s.* ptr = v;
			}

			T get(const Strt * s) const
			{
				return dynamic_cast<const Stt<S> *>(s)->s.* ptr;
			}
		public:
			Mem(T S::* p) :
				ptr(p)
			{}
		};
		struct Entry
		{
			std::string name;
			Member * mem;
		};
		std::vector<Entry> list;
		Res * res;
		Struct();
		~Struct();
		template <typename S, typename T>
		Member & add_member(const std::string & n, T S::* ptr)
		{
			Entry e;
			e.name = n;
			e.mem = new Mem<S, T>(ptr);
			list.push_back(e);
			return * e.mem;
		}
		Member * find_member(const std::string & n);
		const Member * find_member(const std::string & n) const;
		void set_resolver(Res * r);

		template <typename C>
		void set_resolver(C & c, Strt * (C::* p)(size_t))
		{
			delete res;
			res = new CRes<C>(c, p);
		}
		template <typename S>
		void set_resolver(S * & p)
		{
			delete res;
			res = new ARes<S>(p);
		}
		template <typename S>
		void set_resolver(std::vector<S> & v)
		{
			delete res;
			res = new VRes<S>(v);
		}
		std::vector<const std::string *> get_names() const;
	};
	// member of a struct
	template <typename T>
	class StrM :
		virtual public Var<T>
	{
		Struct::MemT<T> & mem;

		void set(T v)
		{
			if (Strt * s = get_struct()) {
				mem.set(s, v);
				delete s;
			}
		}
		T get() const
		{
			if (Strt * s = get_struct()) {
				T t = mem.get(s);
				delete s;
				return t;
			}
			return T();
		}
	protected:
		virtual Strt * get_struct() const = 0;
	public:
		StrM(Struct::MemT<T> & m) :
			mem(m)
		{
			this->copy_tags(m);
		}
		virtual ~StrM() {}
	};
	template <typename C, typename T>
	class CStrM :
		virtual public StrM<T>
	{
		C & cls;
		Strt * (C::* ptr)() const;

		Strt * get_struct() const
		{
			return (cls.* ptr)();
		}
	public:
		CStrM(Struct::MemT<T> & m, C & c, Strt * (C::* p)() const) :
			StrM<T>(m),
			cls(c),
			ptr(p)
		{}
	};
	// with Resolver
	template <typename C, typename T>
	class CStrMRes :
		virtual public StrM<T>
	{
		C & cls;
		size_t (C::* ptr)() const;
		Res & res;
		Strt * get_struct() const
		{
			return res.get((cls.* ptr)());
		}
	public:
		CStrMRes(Struct::MemT<T> & m, C & c, size_t (C::* p)() const, Res & r) :
			StrM<T>(m),
			cls(c),
			ptr(p),
			res(r)
		{}
	};
	// with resolver and fixed index
	template <typename T>
	class IStrMRes :
		virtual public StrM<T>
	{
		Res & res;
		Index * idx;
		Strt * get_struct() const
		{
			size_t i = idx ? idx->get() : 0;
			return res.get(i);
		}
	public:
		IStrMRes(Struct::MemT<T> & m, Res & r, Index * i) :
			StrM<T>(m),
			res(r),
			idx(i)
		{}
		~IStrMRes()
		{
			delete idx;
		}
	};
	struct Compound :
		virtual public Array
	{
		struct Entry
		{
			std::string name;
			Array * elm;
		};
		std::vector<Entry> list;
		~Compound();
		Array & add_array(const std::string & name, Array * elm);
		Struct & add_struct(Struct * elm);
		template <typename T>
		ArrLin<T> & add_linear(const std::string & name, T * & arr_addr)
		{
			ArrLin<T> * a = new ArrLin<T>(arr_addr);
			add_array(name, a);
			return * a;
		}
		TagSable * make_var(const std::string & name, Index * idx = 0);
		const TagSable * make_var(const std::string & name, Index * idx = 0) const;
		std::vector<const std::string *> get_names() const;
	};
}
template <typename T>
prm::TagSable * prm::Arr<T>::make_var(Index * idx)
{
	return new Idx<T>(* this, idx);
}
template <typename T>
prm::TagSable * prm::Struct::MemT<T>::make_var(Res & res, Index * idx)
{
	return new IStrMRes<T>(* this, res, idx);
}
template <typename T>
const prm::TagSable * prm::Struct::MemT<T>::make_var(Res & res, Index * idx) const
{
	return new IStrMRes<T>(* const_cast<MemT<T> *>(this), res, idx);
}
#endif // PRM_HH
