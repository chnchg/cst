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
	// Tags for labelling properties of variables/parameters
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
		void copy_tags(Taggable const & tgb); // deep copy
	public:
		Taggable() {}
		Taggable(Taggable const & t);
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

	// Streamable (can be serialized, into a string)
	class Sable
	{
	public:
		virtual ~Sable() {}
		virtual std::string to_str() const {return "";} // convert to string
		virtual std::string a_str() const {return "";}  // a default string
		virtual bool read_str(const std::string & s) {return s == "";} // convert from string
		virtual bool test_str(const std::string & s) const {return s == "";} // check string
		virtual void copy_val(Sable const & s) {read_str(s.to_str());} // copy value
	};

	// A single streamable value with a type
	template <typename T>
	class Var :
		virtual public Sable
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
		void copy_val(Sable const & s) override
		{
			if (Var<T> const * v = dynamic_cast<Var<T> const *>(& s)) set(v->get());
		}
	public:
		typedef T VarType;
		virtual void set(T v) = 0;
		virtual T get() const = 0;
	};

	// An addressed value, this is terminal variable
	template <typename T>
	class Adr :
		virtual public Var<T>
	{
		T & va;
	public:
		Adr(T & v) : va(v) {}

		void set(T v) override
		{
			va = v;
		}

		T get() const override
		{
			return va;
		}
	};

	// A value with member function access
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

	// An index refering to an element of an array/list
	class Index
	{
	public:
		virtual ~Index() {}
		virtual size_t get() const = 0;
	};

	// Simple index that is just a number
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

	// Indirect index refering to a variable
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

	// Index that get the actual value by calling a member function
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

	// An interface to get and set the size of arrays
	class Sizer
	{
	public:
		virtual ~Sizer() {}
		virtual void set_size(size_t /* new_size */) {}
		virtual size_t get_size() const {return 0;}
	};

	// Read-only sizer referencing a size_t variable
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

	// Using member functions to get/set size
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
			obj(c), ss_f(sf), gs_f(gf) {}
	};

	// Sizer for std::vector
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
	class Array
	{
		std::shared_ptr<Sizer> szr;
	public:
		Array() : szr(0) {}
		virtual ~Array() {}

		virtual void set_sizer(std::shared_ptr<Sizer> sizer) {szr = sizer;}
		void set_sizer(size_t & sz) {szr = std::make_shared<SzrRef>(sz);}
		template <typename S> void set_sizer(std::vector<S> & v) {szr = std::make_shared<SzrVec<S>>(v);}
		virtual void set_size(size_t new_size) {if (szr) szr->set_size(new_size);}
		virtual size_t get_size() const {return szr ? szr->get_size() : 0;}
	};

	// Array of sable values
	class ArrayS :
		virtual public Array
	{
	public:
		virtual std::shared_ptr<Sable> make_var(std::shared_ptr<Index> idx = 0) = 0;
	};

	// Array of values with a type
	template <typename T>
	class Arr :
		virtual public ArrayS
	{
		std::shared_ptr<Sable> make_var(std::shared_ptr<Index> idx) override;
	public:
		typedef T Type;
		virtual void set(size_t i, T val) = 0;
		virtual T get(size_t i) const = 0;
	};

	// Linear array
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
		ArrLin(Type * & arr_addr) : arr(arr_addr) {}
	};

	// Array accessed through member functions
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
			obj(c), set_f(sf), get_f(gf) {}

		~ArrFun() {}

		void set_sizer(void (C::* sf)(size_t), size_t (C::* gf)() const) override
		{
			Arr<T>::set_sizer(new SzrFun<C>(obj, sf, gf));
		}
	};

	// Array that is a vector
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

	// Abstraction of some type or struct
	struct Strt
	{
		virtual ~Strt() {}
	};

	// The actual type
	template <typename S>
	struct Stt :
		public Strt
	{
		S & s;
		Stt(S & ns) : s(ns) {}
	};

	// Resolver for certain type or struct, it resolves to a pointer to the Strt
	class Res
	{
	public:
		virtual ~Res() {}
		virtual std::shared_ptr<Strt> get(size_t) = 0;
	};

	// Resolver for a C array
	template <typename S>
	class ARes :
		public Res
	{
		S * & p;
		std::shared_ptr<Strt> get(size_t i) override {return std::make_shared<Stt<S>>(p[i]);}
	public:
		ARes(S * & p) : p(p) {}
	};

	// Resolver implemented with member functions
	template <typename C>
	class CRes :
		public Res
	{
		C & cls;
		std::shared_ptr<Strt> (C::* ptr)(size_t);

		std::shared_ptr<Strt> get(size_t i) override
		{
			return (cls.* ptr)(i);
		}
	public:
		CRes(C & c, Strt * (C::* p)(size_t)) :
			cls(c), ptr(p) {}
	};

	// Resolver for a std::vector
	template <typename S>
	class VRes :
		public Res
	{
		std::vector<S> & v;
		std::shared_ptr<Strt> get(size_t i) override {return std::make_shared<Stt<S>>(v[i]);}
	public:
		VRes(std::vector<S> & v) : v(v) {}
	};

	// Array with resolver access
	template <typename T>
	class ArrRes :
		virtual public Arr<T>
	{
		std::shared_ptr<Res> res;

		void set(size_t i, T val)
		{
			if (! res) return;
			auto st = res->get(i);
			std::dynamic_pointer_cast<Stt<T>>(st)->s = val;
		}

		T get(size_t i) const
		{
			if (! res) return T();
			auto st = res->get(i);
			return std::dynamic_pointer_cast<Stt<T> const>(st)->s;
		}
	public:
		ArrRes() : res(0) {}
		~ArrRes() {}

		void set_resolver(std::shared_ptr<Res> re)
		{
			res = re;
		}
	};

	// Indexed item in an array, the index can vary
	template <typename T>
	class Idx :
		virtual public Var<T>
	{
		Arr<T> & arr;
		std::shared_ptr<Index> idx;

		void set(T v) override
		{
			arr.set(idx ? idx->get() : 0, v);
		}

		T get() const override
		{
			return arr.get(idx ? idx->get() : 0);
		}
	public:
		Idx(Arr<T> & a, std::shared_ptr<Index> i = 0) :
			arr(a), idx(i)
		{}
		virtual ~Idx() {}
	};

	// Parameter manager
	struct Param
	{
		struct VarEntry
		{
			std::string name;
			std::shared_ptr<Sable> var;
			Taggable tags;
			VarEntry(std::string const & n, std::shared_ptr<Sable> v) :
				name(n),
				var(v)
			{}
		};
		std::vector<VarEntry> vars;
		~Param();
		Taggable & add_var(std::string const & name, std::shared_ptr<Sable> v);
		// overloaded add_var
		template <typename Type>
		Taggable & add_var(std::string const & name, Type & v)
		{
			return add_var(name, std::make_shared<Adr<Type>>(v));
		}
		// access
		std::vector<std::string> get_names() const;
		std::shared_ptr<Sable const> get_var(std::string const & name) const;
		std::shared_ptr<Sable> get_var(std::string const & name);
		void delete_var(std::string const & name);
		// conversion to/from text stream
		void read(std::istream & input);
		void write(std::ostream & output) const;
		// copy values from another Param
		void copy(Param const & p);
		// merge in another Param
		void append(Param const & p);
	};

	// Array of structures
	struct Struct :
		virtual public Array
	{
		// Member of the Struct
		struct Member
		{
			virtual ~Member() {}
			virtual std::shared_ptr<Sable> make_var(std::shared_ptr<Res> /* res */, std::shared_ptr<Index> /* idx */ = 0) {return 0;}
			virtual std::shared_ptr<Sable const> make_var(std::shared_ptr<Res> /* res */, std::shared_ptr<Index> /* idx */ = 0) const {return 0;}
		};

		// Member with specific type
		template <typename T>
		class MemT :
			virtual public Member
		{
			std::shared_ptr<Sable> make_var(std::shared_ptr<Res> res, std::shared_ptr<Index> idx) override;
			std::shared_ptr<Sable const> make_var(std::shared_ptr<Res> res, std::shared_ptr<Index> idx) const override;
		public:
			typedef T Type;
			virtual void set(std::shared_ptr<Strt> s, T v) = 0;
			virtual T get(std::shared_ptr<Strt const> s) const = 0;
		};

		// Member accessed through member pointer
		template <typename S, typename T>
		class Mem :
			virtual public MemT<T>
		{
			T S::* ptr;

			void set(std::shared_ptr<Strt> s, T v) override
			{
				std::dynamic_pointer_cast<Stt<S>>(s)->s.* ptr = v;
			}

			T get(std::shared_ptr<Strt const> s) const override
			{
				return std::dynamic_pointer_cast<Stt<S> const>(s)->s.* ptr;
			}
		public:
			Mem(T S::* p) : ptr(p) {}
		};

		struct MemEntry
		{
			std::string name;
			std::shared_ptr<Member> mem;
			Taggable tags;
		};

		std::vector<MemEntry> list;
		std::shared_ptr<Res> res;

		Struct();
		~Struct();

		template <typename S, typename T>
		Taggable & add_member(std::string const & n, T S::* ptr)
		{
			list.push_back(MemEntry{n,std::make_shared<Mem<S,T>>(ptr),Taggable()});
			return list.back().tags;
		}

		MemEntry * find_member(std::string const & n);
		MemEntry const * find_member(std::string const & n) const;
		void set_resolver(std::shared_ptr<Res> r);

		template <typename C>
		void set_resolver(C & c, Strt * (C::* p)(size_t))
		{
			res = std::make_shared<CRes<C>>(c, p);
		}

		template <typename S>
		void set_resolver(S * & p)
		{
			res = std::make_shared<ARes<S>>(p);
		}

		template <typename S>
		void set_resolver(std::vector<S> & v)
		{
			res = std::make_shared<VRes<S>>(v);
		}

		std::vector<std::string> get_names() const;
	};

	// Sable with type that is a Member of a struct
	template <typename T>
	class StrM :
		virtual public Var<T>
	{
		Struct::MemT<T> & mem;

		void set(T v) override
		{
			if (auto s = get_struct()) mem.set(s, v);
		}

		T get() const override
		{
			if (auto s = get_struct()) return mem.get(s);
			else return T();
		}

	protected:
		virtual std::shared_ptr<Strt> get_struct() const = 0;

	public:
		StrM(Struct::MemT<T> & m) : mem(m) {}
		virtual ~StrM() {}
	};

	// StrM with member function access
	template <typename C, typename T>
	class CStrM :
		virtual public StrM<T>
	{
		C & cls;
		std::shared_ptr<Strt> (C::* ptr)() const;

		std::shared_ptr<Strt> get_struct() const override
		{
			return (cls.* ptr)();
		}
	public:
		CStrM(Struct::MemT<T> & m, C & c, std::shared_ptr<Strt> (C::* p)() const) :
			StrM<T>(m), cls(c), ptr(p)
		{}
	};

	// With resolver and member function index
	template <typename C, typename T>
	class CStrMRes :
		virtual public StrM<T>
	{
		C & cls;
		size_t (C::* ptr)() const;
		std::shared_ptr<Res> res;
		std::shared_ptr<Strt> get_struct() const override
		{
			return res->get((cls.* ptr)());
		}
	public:
		CStrMRes(Struct::MemT<T> & m, C & c, size_t (C::* p)() const, std::shared_ptr<Res> r) :
			StrM<T>(m), cls(c), ptr(p),	res(r)
		{}
	};

	// With resolver and dynamic index
	template <typename T>
	class IStrMRes :
		virtual public StrM<T>
	{
		std::shared_ptr<Res> res;
		std::shared_ptr<Index> idx;
		std::shared_ptr<Strt> get_struct() const override
		{
			return res->get(idx ? idx->get() : 0);
		}
	public:
		IStrMRes(Struct::MemT<T> & m, std::shared_ptr<Res> r, std::shared_ptr<Index> i) :
			StrM<T>(m), res(r), idx(i)
		{}
		~IStrMRes() {}
	};

	// Compound Array, combine arrays to make struct array
	struct Compound :
		virtual public Array
	{
		struct Entry
		{
			std::string name;
			std::shared_ptr<Array> elm;
		};

		std::vector<Entry> list;
		~Compound();
		ArrayS & add_arrays(std::string const & name, std::shared_ptr<ArrayS> elm);
		Struct & add_struct(std::shared_ptr<Struct> elm); // have names already

		// Add linear array
		template <typename T>
		ArrLin<T> & add_linear(std::string const & name, T * & arr_addr)
		{
			auto a = std::make_shared<ArrLin<T>>(arr_addr);
			add_array(name, a);
			return * a;
		}

		std::shared_ptr<Sable> make_var(std::string const & name, std::shared_ptr<Index> idx = 0);
		std::shared_ptr<Sable const> make_var(std::string const & name, std::shared_ptr<Index> idx = 0) const;
		std::vector<std::string> get_names() const;
	};
}

template <typename T>
std::shared_ptr<prm::Sable> prm::Arr<T>::make_var(std::shared_ptr<Index> idx)
{
	return std::make_shared<Idx<T>>(* this, idx);
}

template <typename T>
std::shared_ptr<prm::Sable> prm::Struct::MemT<T>::make_var(std::shared_ptr<Res> res, std::shared_ptr<Index> idx)
{
	return std::make_shared<IStrMRes<T>>(* this, res, idx);
}

template <typename T>
std::shared_ptr<prm::Sable const> prm::Struct::MemT<T>::make_var(std::shared_ptr<Res> res, std::shared_ptr<Index> idx) const
{
	return std::make_shared<IStrMRes<T>>(* const_cast<MemT<T> *>(this), res, idx);
}
#endif // PRM_HH
