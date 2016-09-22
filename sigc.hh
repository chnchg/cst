/* sigc.hh
 * 
 * Copyright (C) 2011 Chun-Chung Chen <cjj@u.washington.edu>
 * 
 * This file is part of gltk.
 * 
 * gltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with gltk.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
/**
   \file sigc.hh a simple signal-slot template library.
   This header file provides a very limited signal-slot API that is
   compatible with <a href="http://libsigc.sourceforge.net/">libsigc++</a>.
   The real libsigc++ can be used when available to improve flexibility and
   performance.

   Notable Limitations:
   - no aggregator for return values
   - argument number is limited to 0 or 1.
*/
#ifndef SIGC_HH
#define SIGC_HH
#include <list>
#include <iostream>
namespace sigc {
	struct nil;

	// function object with all information to call
	struct functor_base
	{
		virtual functor_base * dup() const = 0;
		virtual ~functor_base() {}
	};
	// / function object that can be copied around, it enforces the signature of the call
	struct slot_base
	{
		functor_base * fun;
		slot_base() : fun(0) {}
		slot_base(slot_base const & s) : fun(s.fun->dup()) {}
		virtual ~slot_base() {delete fun;}
	};
	// / a caller that keeps a list of function objects to call
	struct signal_base
	{
		unsigned hcnt;
		struct handle
		{
			unsigned hid;
			functor_base * fun;
			handle(unsigned h, functor_base const * f) : hid(h), fun(f->dup()) {}
		};
		std::list<handle> handle_list;
		signal_base() : hcnt(0) {}
		virtual unsigned add_handle(slot_base const & slot)
		{
			handle_list.push_back(handle(hcnt, slot.fun));
			return hcnt ++;
		}
		virtual void remove_handle(unsigned hid)
		{
			for (std::list<handle>::iterator i = handle_list.begin(); i != handle_list.end(); i ++)
			if (hid == i->hid) {
				delete i->fun;
				handle_list.erase(i);
				return;
			}
		}
		size_t size() {return handle_list.size();}
		virtual ~signal_base()
		{
			for (std::list<handle>::iterator i = handle_list.begin(); i != handle_list.end(); i ++) {
				delete i->fun;
			}
		}
	};
	/// keep track of the connection
	struct connection
	{
		signal_base * signal;
		unsigned hid;
		connection() : signal(0) {}
		connection(signal_base * s, unsigned h) : signal(s), hid(h) {}
		/// check connection
		bool is_connected() {return signal;}
		/// sever the connection
		void disconnect()
		{
			if (signal) {
				signal->remove_handle(hid);
				signal = 0;
			}
		}
	};

	// Zero argument

	// a functor that needs no argument
	template <typename Tr>
	struct functor0 :
		public functor_base
	{
		virtual Tr call() = 0;
	};
	// a functor0 that is made of a pointer to function
	template <typename Tr>
	struct functor0_ptr :
		functor0<Tr>
	{
		Tr (* ptr)();
		Tr call() {return (* ptr)();}
		functor_base * dup() const {return new functor0_ptr<Tr>(* this);}
	};
	// specialized functor0_ptr for void return
	template <>
	struct functor0_ptr<void> :
		functor0<void>
	{
		void (* ptr)();
		void call() {(* ptr)();}
		functor_base * dup() const {return new functor0_ptr<void>(* this);}
	};
	// a functor0 that is made of a member function pointer
	template <typename C, typename Cp, typename Tr>
	struct functor0_mem :
		functor0<Tr>
	{
		C * cls;
		Tr (Cp::* ptr)();
		Tr call() {return (cls->* ptr)();}
		functor_base * dup() const {return new functor0_mem<C, Cp, Tr>(* this);}
	};
	// specialized functor0_mem for void return
	template <typename C, typename Cp>
	struct functor0_mem<C, Cp, void> :
		functor0<void>
	{
		C * cls;
		void (Cp::* ptr)();
		void call() {(cls->* ptr)();}
		functor_base * dup() const {return new functor0_mem<C, Cp, void>(* this);}
	};
	/// slot with zero argument
	template <typename Tr>
	struct slot0 :
		public slot_base
	{
		/// call the slot
		Tr call() {return dynamic_cast<functor0<Tr> *>(fun)->call();}
		slot0(functor0<Tr> const & f) {fun = f.dup();}
	};
	// specialized slot0 with void return
	template <>
	struct slot0<void> :
		public slot_base
	{
		void call() {dynamic_cast<functor0<void> *>(fun)->call();}
		slot0(functor0<void> const & f) {fun = f.dup();}
	};
	/// function to convert a pointer to slot0
	template <typename Tr>
	slot0<Tr> ptr_fun(Tr (* ptr)())
	{
		functor0_ptr<Tr> f;
		f.ptr = ptr;
		return f;
	}
	// / function to convert a member pointer to slot0
	template <typename C, typename Cp, typename Tr>
	slot0<Tr> mem_fun(C * cls, Tr (Cp::* ptr)())
	{
		functor0_mem<C, Cp, Tr> f;
		f.cls = cls;
		f.ptr = ptr;
		return f;
	}
	/// function to convert a member pointer to slot0
	template <typename C, typename Cp, typename Tr>
	slot0<Tr> mem_fun(C & cls, Tr (Cp::* ptr)())
	{
		functor0_mem<C, Cp, Tr> f;
		f.cls = & cls;
		f.ptr = ptr;
		return f;
	}
	/// signal with zero argument
	template <typename Tr>
	struct signal0 :
		public signal_base
	{
		/// make connection
		connection connect(slot0<Tr> const & s)
		{
			return connection(this, add_handle(s));
		}
		Tr emit() /// emit the signal
		{
			Tr v;
			for (typename std::list<handle>::iterator i = handle_list.begin(); i != handle_list.end(); i ++) {
				v = dynamic_cast<functor0<Tr> *>(i->fun)->call();
			}
			return v;
		}
		Tr operator()() {return emit();}
		slot0<Tr> make_slot()
		{
			return mem_fun(this, & signal0<Tr>::emit);
		}
	};
	// specialized signal0 for void return
	template <>
	struct signal0<void> :
		public signal_base
	{
		connection connect(slot0<void> const & s) ///< make connection
		{
			return connection(this, add_handle(s));
		}
		void emit() ///< emit the signal
		{
			for (std::list<handle>::iterator i = handle_list.begin(); i != handle_list.end(); i ++) {
				dynamic_cast<functor0<void> *>(i->fun)->call();
		        }
		}
		void operator()(){emit();}
		slot0<void> make_slot()
		{
			return mem_fun(this, & signal0<void>::emit);
		}
	};

	// One argument

	// a functor that needs one argument
	template <typename Tr, typename T1>
	struct functor1 :
		public functor_base
	{
		virtual Tr call(T1) = 0;
	};
	// a functor1 that is made of a pointer to function
	template <typename Tr, typename T1>
	struct functor1_ptr :
		functor1<Tr, T1>
	{
		Tr (* ptr)(T1);
		Tr call(T1 t1) {return (* ptr)(t1);}
		functor_base * dup() const {return new functor1_ptr<Tr, T1>(* this);}
	};
	// specialized functor1_ptr for void return
	template <typename T1>
	struct functor1_ptr<void, T1> :
		functor1<void, T1>
	{
		void (* ptr)(T1);
		void call(T1 t1) {(* ptr)(t1);}
		functor_base * dup() const {return new functor1_ptr<void, T1>(* this);}
	};
	// a functor1 that is made of a member function pointer
	template <typename C, typename Cp, typename Tr, typename T1>
	struct functor1_mem :
		functor1<Tr, T1>
	{
		C * cls;
		Tr (C::* ptr)(T1);
		Tr call(T1 t1) {return (cls->* ptr)(t1);}
		functor_base * dup() const {return new functor1_mem<C, Cp, Tr, T1>(* this);}
	};
	// specialized functor1_mem for void return
	template <typename C, typename Cp, typename T1>
	struct functor1_mem<C, Cp, void, T1> :
		functor1<void, T1>
	{
		C * cls;
		void (C::* ptr)(T1);
		void call(T1 t1) {(cls->* ptr)(t1);}
		functor_base * dup() const {return new functor1_mem<C, Cp, void, T1>(* this);}
	};
	/// slot with one argument
	template <typename Tr, typename T1>
	struct slot1 :
		public slot_base
	{
		/// call the slot
		Tr call(T1 t1) {return dynamic_cast<functor1<Tr, T1> *>(fun)->call(t1);}
		slot1(functor1<Tr, T1> const & f) {fun = f.dup();}
	};
	// specialized slot1 with void return
	template <typename T1>
	struct slot1<void, T1> :
		public slot_base
	{
		void call(T1 t1) {dynamic_cast<functor1<void, T1> *>(fun)->call(t1);}
		slot1(functor1<void, T1> const & f) {fun = f.dup();}
	};
	/// function to convert a pointer to slot1
	template <typename Tr, typename T1>
	slot1<Tr, T1> ptr_fun(Tr (* ptr)(T1))
	{
		functor1_ptr<Tr, T1> f;
		f.ptr = ptr;
		return f;
	}
	// / function to convert a member pointer to slot1
	template <typename C, typename Cp, typename Tr, typename T1>
	slot1<Tr, T1> mem_fun(C * cls, Tr (Cp::* ptr)(T1))
	{
		functor1_mem<C, Cp, Tr, T1> f;
		f.cls = cls;
		f.ptr = ptr;
		return f;
	}
	// / function to convert a member pointer to slot1
	template <typename C, typename Cp, typename Tr, typename T1>
	slot1<Tr, T1> mem_fun(C & cls, Tr (Cp::* ptr)(T1))
	{
		functor1_mem<C, Cp, Tr, T1> f;
		f.cls = & cls;
		f.ptr = ptr;
		return f;
	}
	/// signal with one argument
	template <typename Tr, typename T1>
	struct signal1 :
		public signal_base
	{
		/// make connection
		connection connect(slot1<Tr, T1> const & s)
		{
			return connection(this, add_handle(s));
		}
		/// emit the signal
		Tr emit(T1 t1)
		{
			Tr v = Tr();
			for (typename std::list<handle>::iterator i = handle_list.begin(); i != handle_list.end(); i ++) {
				v = dynamic_cast<functor1<Tr, T1> *>(i->fun)->call(t1);
			}
                        return v;
		}
		Tr operator()(T1 t1) {return emit(t1);}
		slot1<Tr, T1> make_slot()
		{
			return mem_fun(this, & signal1<Tr, T1>::emit);
		}
	};
	// specialized signal1 with void return
	template <typename T1>
	struct signal1<void, T1> :
		public signal_base
	{
		connection connect(slot1<void, T1> const & s) ///< make connection
		{
			return connection(this, add_handle(s));
		}
		void emit(T1 t1) ///< emit the signal
		{
			for (typename std::list<handle>::iterator i = handle_list.begin(); i != handle_list.end(); i ++) {
				(* dynamic_cast<functor1<void, T1> *>(i->fun)).call(t1);
			}
		}
		void operator()(T1 t1) {emit(t1);}
		slot1<void, T1> make_slot()
		{
			return mem_fun(this, & signal1<void, T1>::emit);
		}
	};

	// convenience struct
	// / convenience template for slots
	template <typename Tr, typename T1 = nil>
	struct slot :
		public slot1<Tr, T1>
	{
		slot() {}
		slot(slot1<Tr, T1> const & s) : slot1<Tr, T1>(s) {}
	};

	template <typename Tr>
	struct slot<Tr, nil> :
		public slot0<Tr>
	{
		slot() {}
		slot(slot0<Tr> const & s) : slot0<Tr>(s) {}
	};
	/// convenience template for signal of any number of arguments
	template <typename Tr, typename T1 = nil>
	struct signal :
		public signal1<Tr, T1>
	{
		signal() {}
	};

	template <typename Tr>
	struct signal<Tr, nil> :
		public signal0<Tr>
	{
		signal() {}
	};

	// adaptors
	// a functor0 that is made from a functor1
	template <typename Tr, typename D1>
	struct adaptor0 :
		public functor0<Tr>
	{
		functor1<Tr, D1> * fun;
		D1 d1;
		adaptor0(functor1<Tr, D1> * f, D1 v1) : fun(f->dup()), d1(v1) {}
		Tr call() {return dynamic_cast<functor1<Tr, D1> *>(fun)->call(d1);}
		functor_base * dup() {return new adaptor0<Tr, D1>(fun, d1);}
	};
	// specialized adaptor0 for void return
	template <typename D1>
	struct adaptor0<void, D1> :
		public functor0<void>
	{
		functor1<void, D1> * fun;
		D1 d1;
		adaptor0(functor1<void, D1> const * f, D1 v1) : fun(dynamic_cast<functor1<void, D1> *>(f->dup())), d1(v1) {}
		void call() {dynamic_cast<functor1<void, D1> *>(fun)->call(d1);}
		functor_base * dup() const {return new adaptor0<void, D1>(fun, d1);}
	};
	// / make a slot0 from a slot1 and a pre-given parameter
	template <typename Tr, typename T1>
	slot0<Tr> bind(slot1<Tr, T1> const & s, T1 v)
	{
		return slot0<Tr>(adaptor0<Tr, T1>(dynamic_cast<functor1<Tr, T1> *>(s.fun), v));
	}
}
#endif
